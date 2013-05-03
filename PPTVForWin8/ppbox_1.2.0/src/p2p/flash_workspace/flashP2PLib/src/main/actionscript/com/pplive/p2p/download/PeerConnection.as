package com.pplive.p2p.download
{
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.network.ISocket;
	import com.pplive.p2p.network.ISocketListener;
	import com.pplive.p2p.network.TcpStream;
	import com.pplive.p2p.network.protocol.AnnounceRequestPacket;
	import com.pplive.p2p.network.protocol.AnnounceResponsePacket;
	import com.pplive.p2p.network.protocol.Packet;
	import com.pplive.p2p.network.protocol.ReportSpeedPacket;
	import com.pplive.p2p.network.protocol.SubPiecePacket;
	import com.pplive.p2p.network.protocol.SubPieceRequestPacket;
	import com.pplive.p2p.struct.BlockMap;
	import com.pplive.p2p.struct.PeerDownloadInfo;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.profile.FunctionProfiler;
	import com.pplive.util.MathUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class PeerConnection implements ISocketListener
	{
		private static const MIN_WINDOW_SIZE:uint = 4;
		private static const MAX_WINDOW_SIZE:uint = 20;
		private static var logger:ILogger = getLogger(PeerConnection);
		
		private var stream:TcpStream;
		private var p2pDownloader:IP2PDownloader;
		private var isRunning:Boolean;
		private var startTime:Number;
		private var windowSize:uint;
		private var requestingSize:uint;
		
		private var isAnnouceResponsed:Boolean = false;
		private var peerDownloadInfo:PeerDownloadInfo;
		private var blockMap:BlockMap;
		
		private var speedMeter:SpeedMeter = new SpeedMeter;
		
		public function PeerConnection(socket:ISocket, endpoint:Endpoint, p2pDownloader:IP2PDownloader)
		{
			logger.debug("PeerConnection constuctor.");
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			
			stream = new TcpStream(socket, endpoint, this);
			this.p2pDownloader = p2pDownloader;
			isRunning = false;
			windowSize = MIN_WINDOW_SIZE;
			requestingSize = 0;
			speedMeter.resume();
			
			doAnnounce();
			profiler.end();
		}
		
		public function start():void
		{
			isRunning = true;
			startTime = new Date().getTime();
			doRequest();
		}
		
		public function stop():void
		{
			isRunning  = false;
			stream.destory();
			stream = null;		
			p2pDownloader = null;
			peerDownloadInfo = null;
			blockMap = null;
			speedMeter.destory();
			speedMeter = null;
		}
		
		public function canKick():Boolean
		{
			// protect peer in first 5 seconds.
			var now:Number = new Date().getTime();
			return (now - startTime > 5000) && (getCurrentSpeedInKBps() < 5);
		}
		
		public function get endpoint():Endpoint
		{
			return stream.endpoint;
		}
		
		public function getCurrentSpeedInKBps():uint
		{
			return speedMeter.getRecentSpeedInKBPS();
		}
		
		private function doAnnounce():void
		{
			logger.debug("" + stream.endpoint + " doAnnounce");
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			var packet:AnnounceRequestPacket = new AnnounceRequestPacket(
				Packet.NewTransactionID(), p2pDownloader.rid);
			stream.sendPacket(packet);
			profiler.end();
		}
		
		public function haveSubPiece(subPiece:SubPiece):Boolean
		{
			// TODO(herain):call haveSubPiece.
			return blockMap.hasBlock(subPiece.blockIndex);
		}
		
		public function onP2PTimer(times:uint):void
		{
			logger.debug("onP2PTimer " + isRunning + ", times:" + times);
			if (isRunning)
			{
				doRequest();
				
				if (!isAnnouceResponsed && passedSeconds(times, 2))
				{
					doAnnounce();
				}
				
				if (blockMap != null && peerDownloadInfo != null)
				{
					// herain:DoAnnounce every 20 seconds when connected peer is downloading the smae resource.
					if (!blockMap.isFull() && peerDownloadInfo.isDownloading && passedSeconds(times, 20))
						doAnnounce();	
				}
				
				// herain:ReportSpeed
				if (passedSeconds(times, 5))
				{
					doReportSpeed();
				}
				
				windowSize = MathUtil.limitMinMaxUint(speedMeter.getRecentSpeedInKBPS(), 
					MIN_WINDOW_SIZE, MAX_WINDOW_SIZE);
			}
		}
		
		public function onPacket(packet:Packet):void
		{
			switch(packet.action)
			{
				case SubPiecePacket.ACTION:
					onSubPiece(packet as SubPiecePacket);
					break;
				case AnnounceResponsePacket.ACTION:
					onAnnounce(packet as AnnounceResponsePacket);
					break;
			}
		}
		
		public function onSocketLost():void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "onSocketLost");
			logger.error("socketLost " + stream.endpoint);
			p2pDownloader.onPeerDisconnected(this);
			profiler.end();
		}
		
		private function passedSeconds(times:uint, seconds:uint):Boolean
		{
			return times % (2 * seconds) == 0;
		}
		
		private function doRequest():void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "doRequest");
			if (isRunning)
			{
				var subpieces:Vector.<SubPiece> = new Vector.<SubPiece>;
				while(requestingSize < windowSize)
				{
					var subpiece:SubPiece = p2pDownloader.getNextSubPiece();
					profiler.makeSection();
					if (subpiece != null)
					{
						subpieces.push(subpiece);
						++requestingSize;
						logger.debug("" + stream.endpoint + " request " + subpiece);
					}
					else
					{
						break;
					}					
				}
				
				profiler.makeSection();
				if (subpieces.length != 0)
				{
					var packet:SubPieceRequestPacket = new SubPieceRequestPacket(Packet.NewTransactionID(), 
						p2pDownloader.rid, subpieces, p2pDownloader.priority);
					stream.sendPacket(packet);
				}
			}
			
			profiler.end();
		}
		
		private function doReportSpeed():void
		{
			logger.debug("doReportSpeed");
			var packet:ReportSpeedPacket = new ReportSpeedPacket(Packet.NewTransactionID(), 
				speedMeter.getRecentSpeedInKBPS());
			stream.sendPacket(packet);
		}
		
		private function onSubPiece(packet:SubPiecePacket):void
		{
			logger.debug("" + stream.endpoint + " onSubPiece " + packet.subpiece);
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			
			p2pDownloader.onSubPiece(packet.subpiece, packet.data);
			
			profiler.makeSection();
			speedMeter.submitBytes(packet.subpieceLength);
			--requestingSize;
			doRequest();
			profiler.end();
		}
		
		private function onAnnounce(packet:AnnounceResponsePacket):void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "onAnnounce");
			isAnnouceResponsed = true;
			blockMap = packet.blockMap;
			peerDownloadInfo = packet.peerDownloadInfo;
			logger.debug("" + stream.endpoint + " onAnnounce blockMap:" + packet.blockMap);
			profiler.end();
		}
	}
}