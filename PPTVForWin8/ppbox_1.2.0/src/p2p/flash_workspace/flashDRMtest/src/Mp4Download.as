package
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.ProgressEvent;
	import flash.events.TimerEvent;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	
	import mx.utils.ObjectUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class Mp4Download extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(Mp4Download);		
		
		private var _sessionId:String;
		private var _segmentInfo:SegmentInfo;
		private var _url:String;
		private var offset:uint;
		
		private var secondTimer:Timer;
		private var sendPosition:Piece;
		private var sendPieceCountInSecond:uint = 0;
		private var maxPieceCountInSecond:uint = 10;
		
		private var mp4Stream:Mp4Stream;
		public  var cacheManager:CacheManager;
		private var httpDownloader:HttpDownloader;
		
		private var needSeek:Boolean;
		private var seekTime:uint;
		private var seekOffset:uint;
		private var seekFirstPieceSended:Boolean;
		
		public function Mp4Download(sessionId:String, url:String, segmentInfo:SegmentInfo, mp4StreamGlobal:Mp4Stream)
		{
			super();
			
			_sessionId = sessionId;
			_url = url;
			_segmentInfo = segmentInfo;
			
			logger.info("new Mp4Download url:" + url + ", fileLength:" + _segmentInfo.fileLength + ", rid:" + _segmentInfo.rid);
			logger.info(" segment range: " + _segmentInfo.offset + " - " + _segmentInfo.endOffset);
			secondTimer = new Timer(1000);
			secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);

			mp4Stream = mp4StreamGlobal;
			mp4Stream.addEventListener(ProgressEvent.PROGRESS, onRecvFlvData, false, 0, true);
			mp4Stream.addEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, onComplete, false, 0, true);

			cacheManager = new CacheManager(_segmentInfo.fileLength);
			
		}
		
		public function destroy():void
		{
			stopDownload();
			
			secondTimer.stop();
			secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
			secondTimer = null;
			
			sendPosition = null;
			//mp4Stream.destory();
			//mp4Stream = null;
			cacheManager.destory();
			cacheManager = null;
		}

		private function createDownloadDriver(offset:uint):void
		{
			logger.info("createDownloadDriver offset:" + offset);

			httpDownloader = new HttpDownloader(this, _url, _segmentInfo, offset);
			httpDownloader.addEventListener(DownloadEvent.DOWNLOAD_EVENT, onDownloadEvent);
			httpDownloader.resume();

		}
		
		private function onDownloadEvent(event:DownloadEvent):void
		{
			if (event) {
				logger.info("Download Event: " + event.downloadStatus);
				if (event.downloadStatus != 0) {
					destroy();
				}
				dispatchEvent(new DownloadEvent(DownloadEvent.DOWNLOAD_EVENT, event.downloadStatus));
			}
		}
		
		public function onRecvPiece(piece:Piece, data:ByteArray):void
		{		
			logger.info("Mp4Download::onRecvPiece	" + piece + ", sendPosition:" + sendPosition);
			cacheManager.addPiece(piece, data);
			//var sentSucceed:Boolean = false;
			if (piece.compare(sendPosition) >= 0)
			{
				sendBytesToMp4Stream();
			}
			
			if (needSeek)
			{
				doSeek();
			} else if (httpDownloader.isDownloadDone == true) {
				logger.info("Download done: ");
				httpDownloader.destroy();
				httpDownloader = null;
			}
		}
		
		public function hasPiece(piece:Piece):Boolean
		{
			return cacheManager.hasPiece(piece);
		}
						
		public function start(offset:uint, completeEvent:SegmentCompleteEvent = null):void
		{
			logger.info("start offset:" + offset + ", completeEvent:" + completeEvent);
			sendPosition = new Piece(0, 0);
			needSeek = false;
			seekFirstPieceSended = true;
			
			if (completeEvent != null)
			{
				mp4Stream.setBaseTimeStamp(completeEvent);
			}
			
			if (offset == 0)
			{
				createDownloadDriver(0);
			}
			else
			{
				seek(offset);
			}
			
			secondTimer.start();
		}
		
		public function readBytes(byteArray:ByteArray):void
		{
			mp4Stream.readBytes(byteArray);
		}
		
		public function seek(time:uint):void
		{
			logger.info("seek seekTime:" + time);
			seekTime = time;
			needSeek = true;
			doSeek();
		}
		
		public function sendStopDacLog(logObject:Object):void
		{
			logObject.dt = "stop";
			logObject.rid = _segmentInfo.rid;
			logObject.url = _url;
			logObject.sz = _segmentInfo.fileLength;
			logObject.du = _segmentInfo.duration;
			
			dispatchEvent(new DacLogEvent(DacLogEvent.STOP_DAC_LOG, logObject));
		}
		
		private function stopDownload():void
		{
			if (httpDownloader)
			{
				httpDownloader.destroy();
				httpDownloader = null;
			}
		}
		
		private function doSeek():void
		{
			if (mp4Stream.hasMp4Header())
			{
				logger.info("doSeek seekTime:{0}", [seekTime]);
				
				// report real seek time.
				var realSeekTime:Number = mp4Stream.getRealSeekTime(seekTime);
				dispatchEvent(new GetSeekTimeEvent(GetSeekTimeEvent.GET_SEEK_TIME, realSeekTime));
				
				seekOffset = mp4Stream.seek(seekTime);
				sendPosition = Piece.createPieceFromOffset(0, seekOffset, _segmentInfo.fileLength);
				needSeek = false;
				seekFirstPieceSended = false;
				
				if (httpDownloader != null)
				{
					httpDownloader.destroy();
					httpDownloader = null;
				}
				
				// create new downloadDriver which download begin from the seek offset.
				createDownloadDriver(seekOffset);
			}
			else if (httpDownloader == null)
			{
				logger.info("seek without header, downloader header first");
				createDownloadDriver(0);
			}
		}
		
		public function setRestPlayTime(time:uint):void
		{
			if (httpDownloader != null)
			{
				//httpDownloader.setRestPlayTime(time);
				logger.info("setRestPlayTime " + time + " seconds.");
			}
		}
		
		private function onRecvFlvData(event:ProgressEvent):void
		{
			if (!needSeek)
			{
				// avoid send any data before call seek to mp4stream.
				dispatchEvent(event);
			}
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			logger.info("Mp4Download::onSecondTimer sendCount:" + sendPieceCountInSecond);
			sendPieceCountInSecond = 0;
			sendBytesToMp4Stream();
		}
		
		public function sendBytesToMp4Stream():void
		{
			while(!isSendTooFast() && cacheManager.hasPiece(sendPosition))
			{
				var bytes:ByteArray = cacheManager.getPiece(sendPosition);
				if (!seekFirstPieceSended)
				{
					bytes.position = seekOffset - sendPosition.offset;
					seekFirstPieceSended = true;
				}
				
				logger.info("Mp4Download::sendBytesToMp4Stream send:" + sendPosition);
				mp4Stream.appendBytes(bytes);
				sendPosition = cacheManager.getNextNonExistPiece(sendPosition);
				++sendPieceCountInSecond;
			}
		}
		
		private function isSendTooFast():Boolean
		{
			//wenjiewang: Disable this. This is buggy. 
			// when we pause to wait, it is possible that the current Segment has finished downloading.
			// then the current Mp4Download will be destoried, causing the unsent data in that Segment 
			//   to be lost. (this is because sendPosition is per Segment.
			//return sendPieceCountInSecond >= maxPieceCountInSecond;
			return false;
		}
		
		private function onComplete(event:SegmentCompleteEvent):void
		{
			logger.info("onComplete");
			dispatchEvent(new SegmentCompleteEvent(SegmentCompleteEvent.SEGMENT_COMPLETE, event.videoTimeStamp, event.audioTimeStamp));
			
			// download complete, stop download which will send dac log.
			stopDownload();
		}
	}
}