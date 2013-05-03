package com.pplive.p2p.download
{
	import com.pplive.mx.ObjectUtil;
	import com.pplive.p2p.BootStrapConfig;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.SubPiece;
	
	import de.polygonal.ds.HashMap;
	
	import flash.utils.ByteArray;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class DownloadDriver implements IDownloadDriver
	{
		private static var logger:ILogger = getLogger(DownloadDriver);
		private var target:IDownloadTarget;
		private var fileLength:uint;
		private var currentSubPiece:SubPiece;
		private var kernelMode:Boolean;
		private var requestingPieces:HashMap = new HashMap; // key:piece offset, value:downloader
		
		private var switchController:ISwitchController;
		
		private var httpDownloaders:Vector.<HttpDownloader> = new Vector.<HttpDownloader>;
		private var p2pDownloader:IDownloader;
		private var speedMeter:SpeedMeter = new SpeedMeter;
		private var startDownloadTime:int;
		private var httpDownloadBytes:uint;
		private var p2pDownloadBytes:uint;
		private var redundantDownloadBytes:uint;
		private var p2pRedundantDownloadBytes:uint;
		
		//url上报用的
		private var url:String;		
		//统计http失败的错误码，用于上报,初始值为0，表示没有失败
		private var httpErrCode:uint = 0;
		//剩余播放时间
		private var _restPlayTime:uint = 0;
		
		//如果不为0，表明是拖动，且是仅下载头部
		private var _dragHeadLength:uint = 0;
		
		public function DownloadDriver(target:IDownloadTarget, kernelMode:Boolean, url:String, 
									   backupHosts:Vector.<String>, fileLength:uint, offset:uint,restPlayTime:uint,dragHeadLength:uint)
		{
			this.target = target;
			this.fileLength = fileLength;
			this.currentSubPiece = SubPiece.createSubPieceFromOffset(offset);
			logger.info("offset:"+offset+" current subpiece:"+currentSubPiece);
			this.kernelMode = kernelMode;
			this.url = url;
			switchController = new SwitchController(kernelMode);
			addHttpDownloader(url);
			switchController.setBWType(target.bwType);
			switchController.setRestPlayTime(restPlayTime);
			_restPlayTime = restPlayTime; 
			_dragHeadLength = dragHeadLength;
			
			startDownloadTime = getTimer();
			speedMeter.resume();
			
			// TODO(herain): Add backup httpDownloader
		}
		
		private function addHttpDownloader(url:String):void
		{
			var httpDownloader:HttpDownloader = new HttpDownloader(url);
			httpDownloader.attachDownloadDriver(this);
			httpDownloaders.push(httpDownloader);
			switchController.addHttpDownloader(httpDownloader);
		}
		
		public function start():void
		{
			switchController.start();
		}
		
		public function stop():void
		{
			sendStopLog();
			
			// destory variables
			switchController.stop();
			switchController = null;
			
			if (p2pDownloader != null)
			{
				p2pDownloader.deAttachDownloaderDriver(this);
				p2pDownloader = null;
			}
			
			var httpDownloadersLength:uint = httpDownloaders.length;
			for(var i:uint = 0; i < httpDownloadersLength; ++i)
			{
				httpDownloaders[i].destory();
			}
			httpDownloaders.length = 0;
			httpDownloaders = null;
			
			target = null;
			currentSubPiece = null;
			requestingPieces.clear(true);
			requestingPieces = null;
			speedMeter.destory();
			speedMeter = null;
		}
		
		public function getFileLength():uint
		{
			return fileLength;
		}
		
		public function requestNextPiece(downloader:IDownloader):Piece
		{
			logger.debug("requestNextPiece currentSubPiece:" + currentSubPiece);
			var currentPiece:Piece = currentSubPiece.getPiece();
			while (isPieceDownloading(currentPiece) || target.hasPiece(currentPiece))
			{
				currentPiece.moveToNextPiece();
			}
			
			if (currentPiece.compare(StructUtil.getLastPieceByFileLength(fileLength)) <= 0)
			{
				logger.info("requestNextPiece " + downloader + ":" + currentPiece);
				requestingPieces.set(currentPiece.getOffset(), downloader);
				return com.pplive.mx.ObjectUtil.copy(currentPiece) as Piece;
			}
			else
			{
				return null;
			}
		}
		
		public function reportDownloaderPaused(downloader:IDownloader):void
		{
			// TODO(herain):因为现在不会存在多个Downloader同时下载，所以只要清掉requestingPieces即可。
			// 否则需要清理掉对应downloader的任务。
			requestingPieces.clear();
		}
		
		public function stopPieceDownload(piece:Piece):void
		{
			logger.debug("stop piece:"+piece);
			requestingPieces.clr(piece.getOffset());
		}
		
		private function isPieceDownloading(piece:Piece):Boolean
		{
			var offset:uint = piece.getOffset();
			if (requestingPieces.hasKey(offset))
			{
				var downloader:IDownloader = requestingPieces.get(offset) as IDownloader;
				if (downloader.isPausing())
				{
					logger.debug("isPieceDownloading " + piece + " false, downloader " + downloader + " paused.");
					requestingPieces.clr(offset);
					return false;
				}
				else
				{
					logger.debug("isPieceDownloading " + piece + " true");
					return true;
				}
			}
			else
			{
				logger.debug("isPieceDownloading " + piece + " false");
				return false;
			}
		}
		
		public function onRecvSubPiece(subPiece:SubPiece, data:ByteArray, downloader:IDownloader):void
		{
			if (currentSubPiece.compare(subPiece) == 0)
			{
				do
				{
					currentSubPiece.moveToNextSubPiece();
				}
				while(target.hasSubPiece(currentSubPiece));
			}
			
			speedMeter.submitBytes(data.length);
			if (target.hasSubPiece(subPiece))
			{
				redundantDownloadBytes += data.length;
				if (downloader == p2pDownloader)
				{
					p2pRedundantDownloadBytes += data.length;
				}
				
				return;
			}

			if (downloader == p2pDownloader)
				p2pDownloadBytes += data.length;
			else
				httpDownloadBytes += data.length;

			target.onRecvSubPiece(subPiece, data);
			
			var piece:Piece = subPiece.getPiece();
			if (target.hasPiece(piece))
			{
				logger.debug("piece complete: " + piece);
				requestingPieces.clr(piece.getOffset());
			}
		}
		
		public function onHttpError(error:uint,interval:uint):void
		{
			// TODO(herain):onHttpError, change to p2p mode.
			logger.info("onHttpError,error:"+error+" interval:"+interval);
			target.onHttpError(error,interval);
			httpErrCode = error;
		}
		
		public function attachP2PDownloader(p2pDownloader:IDownloader):void
		{
			this.p2pDownloader = p2pDownloader;
			switchController.addP2PDownloader(p2pDownloader);
		}
		
		public function setRestPlayTime(restPlayTime:uint):void
		{
			_restPlayTime=restPlayTime;
			switchController.setRestPlayTime(restPlayTime);
		}
		
		public function get restPlayTime():uint
		{
			return _restPlayTime; 
		}
		
		public function get dragHeadLength():uint
		{
			return _dragHeadLength; 
		}
		
		public function hasSubPiece(subpiece:SubPiece):Boolean
		{
			return target.hasSubPiece(subpiece);
		}
		
		private function sendStopLog():void
		{
			var logObject:Object = new Object;
			logObject.pz = p2pDownloadBytes; //p2p下载字节数
			logObject.hz = httpDownloadBytes; //http下载字节数
			logObject.av = speedMeter.getTotalAvarageSpeedInKBPS(); //平均下载速度
			logObject.hv = httpDownloaders[0].getAverageSpeedInKBPS(); //http平均下载速度
			logObject.pv = (p2pDownloader == null) ? 0 : p2pDownloader.getAverageSpeedInKBPS(); //p2p平均下载速度
			logObject.pc = (p2pDownloader == null) ? 0 : (p2pDownloader as P2PDownloader).connectionCount(); //p2p连接节点数
			logObject.qc = (p2pDownloader == null) ? 0 : (p2pDownloader as P2PDownloader).queriedPeerCount(); //p2p查询到的节点数
			logObject.ct = (p2pDownloader == null) ? 0 : (p2pDownloader as P2PDownloader).connectTime(); //连满节点耗时
			logObject.dwt = getTimer() - startDownloadTime; //下载时长
			logObject.red = redundantDownloadBytes; //冗余字节数
			logObject.p2pred = p2pRedundantDownloadBytes; //p2p冗余字节数
			logObject.km = kernelMode?1:0;		
			
			//errcode，小于20000的表示是http的错误码
			logObject.ec = httpErrCode;			
			//上报完成了，需要清理掉errcode
			httpErrCode = 0;
			
			logObject.usep2p = BootStrapConfig.useP2p?1:0;
			logObject.url = url;
			
			target.sendStopDacLog(logObject);
			logger.info("sendStopLog:" + com.pplive.mx.ObjectUtil.toString(logObject));
		}
		
		public function httpFailTimes():int 
		{
			var failtimes:int = 0;
			if(httpDownloaders)
			{
				var httpDownloadersLength:uint = httpDownloaders.length;
				for(var i:uint = 0; i < httpDownloadersLength; ++i)
				{					
					failtimes += httpDownloaders[i].failtimes;
				}
			}
			return failtimes;
		}
	}
}