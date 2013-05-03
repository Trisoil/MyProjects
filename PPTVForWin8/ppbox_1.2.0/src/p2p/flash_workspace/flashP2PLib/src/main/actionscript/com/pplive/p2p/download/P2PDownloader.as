package com.pplive.p2p.download
{
	import com.pplive.ds.Pair;
	import com.pplive.mx.ObjectUtil;
	import com.pplive.p2p.P2PModule;
	import com.pplive.p2p.events.GetPeersEvent;
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.network.ISocket;
	import com.pplive.p2p.network.TcpStream;
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.p2p.BootStrapConfig;
	
	import com.pplive.profile.FunctionProfiler;
	import com.pplive.util.MathUtil;
	
	
	import de.polygonal.ds.ArrayUtil;
	import de.polygonal.ds.HashMap;
	import de.polygonal.ds.Itr;
	
	import flash.events.TimerEvent;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	import org.as3commons.concurrency.thread.FramePseudoThread;
	import org.as3commons.concurrency.thread.IRunnable;
	import org.as3commons.concurrency.thread.IThread;
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class P2PDownloader extends Downloader implements IP2PDownloader, IConnectorListener, IRunnable
	{
		private static var logger:ILogger = getLogger(P2PDownloader);
		private static const MAX_CONNECTION_SIZE:uint = 10;
		private static const MAX_CONNECT_COUNT_ONE_SECOND:uint = 10;
		private static const SUBPIECE_REQUEST_TIMEOUT_IN_MILLSECONDS:uint = 5000;
		private static const MAX_ADD_CANDIDATE_PEERS_PER_FRAME:uint = 10;
		
		private static const DEFAULT_SPEED_LIMIT_IN_KBPS:uint = 250;
		
		private var _rid:RID;
		private var p2pTimer:Timer;
		
		private var peerConnector:PeerConnector;
		private var ipPool:IPPool = new IPPool;
		private var connections:Vector.<PeerConnection> = new Vector.<PeerConnection>;
		private var backupConnections:Vector.<PeerConnection> = new Vector.<PeerConnection>;
		private var speedLimitInKBps:uint = DEFAULT_SPEED_LIMIT_IN_KBPS;
		private var requestCountInSeconds:uint = 0;
		private var listCount:uint = 0;
		
		private var pieceArray:Array = new Array;
		private var nextSubPiece:SubPiece;
		private var requestingSubPieces:HashMap = new HashMap;
		
		private var connectStartTime:int;
		private var connectCompleteTime:uint;
		private var frameThread:IThread;
		private var connectCountInSeconds:uint;		
		
		private var candidatePeers:Vector.<PeerInfo> = new Vector.<PeerInfo>;
		
		public function P2PDownloader(rid:RID)
		{
			_rid = rid;
			peerConnector = new PeerConnector(this);
			P2PModule.instance().addEventListener(GetPeersEvent.GET_PEERS, onGetPeers, false, 0, true);
			P2PModule.instance().listPeers(rid);
			p2pTimer = new Timer(500);
			p2pTimer.addEventListener(TimerEvent.TIMER, onP2PTimer, false, 0, true); 
			p2pTimer.start();
			
			frameThread = new FramePseudoThread(this);
			frameThread.start();
		}

		public function get rid():RID { return _rid;}
		public function get priority():uint 
		{ 
			// TODO(herain):flash p2p download priority always the highest.
			return 10;
		}
		
		// downloadDriver update, reset connections.
		public function reset():void
		{
			logger.info("reset p2pdownloader");
			var oldPeers:Vector.<Endpoint> = new Vector.<Endpoint>;
			var connectionsLength:uint = connections.length;
			for (var i:uint = 0; i < connectionsLength; ++i)
			{
				oldPeers.push(connections[i].endpoint);
				connections[i].stop();
			}
			
			connections = new Vector.<PeerConnection>;
			resetDownload();
			useBackupConnections();
			var oldPeersLength:uint = oldPeers.length;
			for (i = 0; i < oldPeersLength; ++i)
			{
				peerConnector.connect(oldPeers[i]);
			}
		}
		
		public function connectionCount():uint
		{
			return connections.length;
		}
		
		public function connectTime():uint
		{
			return connectCompleteTime;
		}
		
		public function queriedPeerCount():uint
		{
			return ipPool.getPeerCount();
		}
		
		private function resetDownload():void
		{
			pieceArray = new Array;
			nextSubPiece = null;
			requestingSubPieces.clear(true);
		}
		
		private function onP2PTimer(event:TimerEvent):void
		{
			// list peer
			var profiler:FunctionProfiler = new FunctionProfiler(logger);					
			if (
				(ipPool.getUnTriedPeerCount() < BootStrapConfig.leastUntriedPeerCountForList)
				&& 
				(passedSeconds(BootStrapConfig.frequentTimeIntervalPeerList))
				)
			{
				if( listCount < BootStrapConfig.frequentPeerListTimes || 
					passedSeconds(BootStrapConfig.nofrequentTimeIntervalPeerList))
				{
					logger.info("doList peers"+BootStrapConfig.leastUntriedPeerCountForList+" "+BootStrapConfig.frequentTimeIntervalPeerList
					+" "+BootStrapConfig.frequentPeerListTimes+" "+BootStrapConfig.nofrequentTimeIntervalPeerList);
					P2PModule.instance().listPeers(rid);
					++listCount;
				}
			}
			
			profiler.makeSection();
			if (isRunning)
			{
				logger.info("onP2PTimer connections:" + connections.length + 
					", speed:" + speedMeter.getSecondSpeedInKBPS() + " KB/S");
				
				if (passedSeconds(1))
				{
					requestCountInSeconds = 0;
					connectCountInSeconds = 0;
				}
				
				kickPeers();
				checkSubPieceTimeOut();
				callConnectionsP2PTimer();
			}
			
			profiler.end();
		}
		
		private function callConnectionsP2PTimer():void
		{
			var connectionLength:uint = connections.length;
			for(var i:uint = 0; i < connectionLength; ++i)
			{
				connections[i].onP2PTimer(p2pTimer.currentCount);
			}
		}
		
		private function passedSeconds(seconds:uint):Boolean
		{
			return p2pTimer.currentCount % (2 * seconds) == 0;
		}
		
		public function getNextSubPiece():SubPiece
		{
			logger.debug("getNextSubPiece");
			
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			if (!isRunning)
			{
				logger.debug("P2PDownloader is paused");
				return null;
			}
			
			if (requestCountInSeconds > speedLimitInKBps)
			{
				logger.debug("getNextSubPiece speed to fast.");
				return null;
			}
			
			if (downloadDriver == null)
			{
				logger.error("no downloadDriver");
				return null;
			}
			
			if (nextSubPiece == null)
			{
				if(!requestNextPiece())
				{
					// TODO(herain):Download to file end. Need redudant strategy.
					logger.debug("no more piece");
					return null;
				}
			}
			
			profiler.makeSection();
			
			var firstSubPieceIndex:uint = nextSubPiece.subPieceIndex % Constants.SUBPIECE_NUM_PER_PIECE;
			var indexInPieceArray:uint = ArrayUtil.bsearchComparator(pieceArray, nextSubPiece.getPiece(), 
				0, pieceArray.length - 1, pieceCompare);
			if (!isDownloadingPiece(nextSubPiece.getPiece()))
			{
				logger.error("piece of nextSubPiece is not downloading");
				indexInPieceArray = 0;
				firstSubPieceIndex = 0;
			}
			
			profiler.makeSection();
			var pieceArrayLength:uint = pieceArray.length;
			for (var i:uint = indexInPieceArray; i < pieceArrayLength; ++i)
			{
				var piece:Piece = pieceArray[i] as Piece;
				if (i != indexInPieceArray)
				{
					firstSubPieceIndex = piece.subPieceIndex;
				}
				else
				{
					firstSubPieceIndex = firstSubPieceIndex < piece.subPieceIndex ? piece.subPieceIndex : firstSubPieceIndex;
				}
				
				var subpieceCount:uint = StructUtil.getSubPieceCountInPiece(downloadDriver.getFileLength(), piece);
				for (var j:uint = firstSubPieceIndex; j < subpieceCount; ++j)
				{
					var subpiece:SubPiece = piece.getNthSubPiece(j);
					if (!isRequestingSubPiece(subpiece) && !downloadDriver.hasSubPiece(subpiece))
					{
						updateNextSubPiece(subpiece);
						requestCountInSeconds++;
						profiler.makeSection();
						addAssignedSubPiece(subpiece);
						profiler.end();
						return subpiece;
					}
				}
			}
			
			logger.debug("has no more task in p2pDownloader");
			nextSubPiece = null;
			return null;
		}
		
		private function requestNextPiece():Boolean
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "requestNextPiece");
			var nextPiece:Piece = downloadDriver.requestNextPiece(this);
			if (nextPiece != null)
			{
				pieceArray.push(nextPiece);
				pieceArray.sort(pieceCompare);
				nextSubPiece = nextPiece.getFirstSubPiece();
				logger.debug("request next piece " + nextPiece);
				profiler.end();
				return true;
			}
			else
			{
				logger.debug("has no more task in downloadDriver");
				profiler.end();
				return false;
			}
		}
		
		private function updateNextSubPiece(lastSubPiece:SubPiece):void
		{
			var tempNextSubPiece:SubPiece = com.pplive.mx.ObjectUtil.copy(lastSubPiece) as SubPiece;
			tempNextSubPiece.moveToNextSubPiece();
			
			if (isDownloadingPiece(tempNextSubPiece.getPiece()))
			{
				nextSubPiece = tempNextSubPiece;
			}
			else
			{
				nextSubPiece = null;				
			}
		}
		
		private function isDownloadingPiece(piece:Piece):Boolean
		{
			return ArrayUtil.bsearchComparator(pieceArray, piece, 0, pieceArray.length - 1, pieceCompare) >= 0;
		}
		
		private function isRequestingSubPiece(subpiece:SubPiece):Boolean
		{
			return requestingSubPieces.hasKey(subpiece.offset);
		}
		
		private function pieceCompare(piece1:Piece, piece2:Piece):int
		{
			if (piece1.blockIndex < piece2.blockIndex)
				return -1;
			else if (piece1.blockIndex > piece2.blockIndex)
				return 1;
			else
			{
				if (piece1.pieceIndex < piece2.pieceIndex)
					return -1;
				else if (piece1.pieceIndex > piece2.pieceIndex)
					return 1;
				else
					return 0;
			}
		}
		
		public function onSubPiece(subpiece:SubPiece, data:ByteArray):void
		{
			logger.debug("onSubPiece " + subpiece);
			requestingSubPieces.clr(subpiece.offset);
			speedMeter.submitBytes(data.length);
			
			if (downloadDriver != null)
			{
				// may receive subpiece after downoadDriver stop.
				downloadDriver.onRecvSubPiece(subpiece, data, this);	
			}
		}
		
		override public function pause():void
		{
			if (isRunning)
			{
				logger.info("pause. isRunning:" + isRunning);
				super.pause();
			}
		}
		
		override public function resume():void
		{
			if(!isRunning)
			{
				logger.info("resume. isRunning:" + isRunning);
				super.resume();
				resetDownload();
			}
		}
		
		public function stop():void
		{
			logger.info("stop");
			pause();
			deleteAllPeers();
		}
		
		override public function destory():void
		{
			super.destory();
			_rid = null;
			p2pTimer.stop();
			p2pTimer.removeEventListener(TimerEvent.TIMER, onP2PTimer);
			p2pTimer = null;
			
			peerConnector.destory();
			peerConnector = null;
			ipPool.destory();
			ipPool = null;
			connections.length = 0;
			connections = null;
			backupConnections.length = 0;
			backupConnections = null;
			
			pieceArray.length = 0;
			pieceArray = null;
			nextSubPiece = null;
			requestingSubPieces.clear(true);
			requestingSubPieces = null;

			frameThread.destroy();
			frameThread = null;
			candidatePeers.length = 0;
			candidatePeers = null;
		}
		
		private function deleteAllPeers():void
		{
			while(connections.length > 0)
			{
				deletePeer(connections[0]);
			}
			
			while(backupConnections.length > 0)
			{
				deletePeer(backupConnections[0]);
			}
		}
		
		private function useBackupConnections():void
		{
			while(backupConnections.length > 0 && connections.length < MAX_CONNECTION_SIZE)
			{
				var connection:PeerConnection = backupConnections.shift();
				logger.debug("use backup connection " + connection.endpoint);
				connections.push(connection);
				connection.start();
			}
		}
		
		private function connectOnePeer():void
		{
			if (connections.length < MAX_CONNECTION_SIZE && 
				connectCountInSeconds < MAX_CONNECT_COUNT_ONE_SECOND)
			{
				var profiler:FunctionProfiler = new FunctionProfiler(logger, "connectOnePeer");
				if (connectStartTime == 0)
				{
					connectStartTime = getTimer();
				}
				
				if (connections.length < MAX_CONNECTION_SIZE && backupConnections.length > 0)
				{
					//herain:use backupConnections first
					useBackupConnections();
				}
				
				profiler.makeSection();
				var peer:PeerInfo = ipPool.getNextPeer();
				profiler.makeSection();
				if (peer == null)
				{
					logger.debug("no more peer in ippool");
				}
				else
				{
					++connectCountInSeconds;
					peerConnector.connect(new Endpoint(peer.detectIp, peer.detectPort));
				}
			
				profiler.end();
			}
		}
		
		private function kickPeers():void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "kickPeers");
			var connectionsLength:uint = connections.length; 
			if (connectionsLength == MAX_CONNECTION_SIZE)
			{
				var connectionSpeedArray:Array = new Array;
				for(var i:uint = 0; i < connectionsLength; ++i)
				{
					var connection:PeerConnection = connections[i];
					if (connection.canKick())
					{
						var speedObject:Object = new Object;
						speedObject.speed = connection.getCurrentSpeedInKBps();
						speedObject.connection = connection;
						connectionSpeedArray.push(speedObject);	
					}
				}
				
				connectionSpeedArray.sortOn("speed");
				var connectionSpeedArrayLength:uint = connectionSpeedArray.length; 
				for(i = 0; i < 3 && i < connectionSpeedArrayLength; ++i)
				{
					deletePeer(connectionSpeedArray[i].connection);
				}
			}
			
			profiler.end();
		}
		
		private function deletePeer(connection:PeerConnection):void
		{
			logger.debug("deltePeer " + connection.endpoint);
			var index:int = connections.indexOf(connection);
			if(index >= 0)
			{
				ipPool.onPeerDisconnect(connections[index].endpoint);
				connections[index].stop();
				connections.splice(index, 1);
			}
			else
			{
				index = backupConnections.indexOf(connection);
				if(index >= 0)
				{
					ipPool.onPeerDisconnect(backupConnections[index].endpoint);
					backupConnections[index].stop();
					backupConnections.splice(index, 1);
				}
			}
		}
		
		public function onPeerConnected(endpoint:Endpoint, socket:ISocket):void
		{
			logger.debug("onPeerConnected " + endpoint);
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			
			if (isRunning)
			{
				var connection:PeerConnection = new PeerConnection(socket, endpoint, this);
				if (connections.length < MAX_CONNECTION_SIZE)
				{
					connections.push(connection);
					connection.start();
				}
				else
				{
					backupConnections.push(connection);
				}
				
				if (connections.length == MAX_CONNECTION_SIZE && connectCompleteTime == 0)
				{
					connectCompleteTime = getTimer() - connectStartTime;
				}
				
				profiler.end();
			}
			else
			{
				socket.close();
				logger.debug("onPeerConnected " + endpoint + " when isRunning is false.");
			}
		}
		
		public function onPeerConnectFailed(endpoint:Endpoint):void
		{
			ipPool.onConnectFailed(endpoint);
		}
		
		private function onGetPeers(event:GetPeersEvent):void
		{
			logger.info("onGetPeers get " + event.peers.length + " peers");
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			
			if (ObjectUtil.compare(event.rid, _rid) == 0)
			{
				candidatePeers = candidatePeers.concat(event.peers);
			}
			
			profiler.end();
		}
		
		private function addCandidatePeers():void
		{
			var addPeersCount:uint = candidatePeers.length;
			if (addPeersCount > 0)
			{
				var profiler:FunctionProfiler = new FunctionProfiler(logger, "addCandidatePeers");
				MathUtil.limitMaxUint(addPeersCount, MAX_ADD_CANDIDATE_PEERS_PER_FRAME);
				ipPool.addCandidatePeers(candidatePeers.splice(0, addPeersCount));
				profiler.end();
			}
		}
		
		private function addAssignedSubPiece(subpiece:SubPiece):void
		{
			logger.debug("addAssignedSubPiece " + subpiece);
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			requestingSubPieces.set(subpiece.offset, getTimer());	
			profiler.end();
		}
		
		private function setSubPieceTimeOut(subpiece:SubPiece):void
		{
			logger.debug("setSubPieceTimeOut " + subpiece);
			if (isDownloadingPiece(subpiece.getPiece()) && 
				(nextSubPiece == null || subpiece.compare(nextSubPiece) < 0))
			{
				nextSubPiece = subpiece;
			}
		}
		
		private function checkSubPieceTimeOut():void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "checkSubPieceTimeOut");
			var iter:Itr = requestingSubPieces.keys();
			var now:int = getTimer();
			while(iter.hasNext())
			{
				var key:int = iter.next() as uint;
				var requestTime:uint = requestingSubPieces.get(key) as uint;
				if (now - requestTime >= SUBPIECE_REQUEST_TIMEOUT_IN_MILLSECONDS)
				{
					requestingSubPieces.clr(key);
					setSubPieceTimeOut(SubPiece.createSubPieceFromOffset(key));
				}
			}
			
			profiler.end();
		}
		
		public function setSpeedLimitInKBps(speedLimitInKBps:uint):void
		{
			this.speedLimitInKBps = speedLimitInKBps;
		}
		
		public function onPeerDisconnected(peer:PeerConnection):void
		{
			deletePeer(peer);
		}
		
		public function process():void
		{
			logger.debug("process,is running:"+isRunning);
			if (isRunning)
			{
				connectOnePeer();
			}
			
			addCandidatePeers();
		}
		
		public function cleanup():void
		{
		}
	}
}