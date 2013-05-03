package
{
	import com.pplive.util.URI;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLRequest;
	import flash.net.URLRequestHeader;
	import flash.net.URLStream;
	import flash.utils.ByteArray;
	
	import mx.utils.ObjectUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class HttpDownloader extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(HttpDownloader);
		private var uri:URI;
			
		private var stream:URLStream;
		private var currentSample:uint;
		private var currentPieceSize:uint;
		private var currentPiece:Piece;
		private var isRunning:Boolean;
		private var isDone:Boolean = false;
		private var mp4Download:Mp4Download;
		private var segmentInfo:SegmentInfo;
		private var _offset:uint;
		private var offsetDiscarded:Boolean = false;
		
		public static const HTTP_IO_ERROR:uint = 1000;
		public static const HTTP_SECURITY_ERROR:uint = 1001;
		public static const HTTP_URL_ERROR:uint = 1002;

		public function HttpDownloader(downloader:Mp4Download, url:String, segInfo:SegmentInfo, offset:uint)
		{
			logger.info("new HttpDownloader url:" + url);
			this.uri = new URI(url);
			segmentInfo = segInfo;
			_offset = offset;
			isRunning = false;
			mp4Download = downloader;
		}
		
		public function get isDownloadDone():Boolean { return isDone; }
		
		public function pause():void
		{
			if (isRunning)
			{
				logger.info("pause. isRunning:" + isRunning);
				//super.pause();
				if (stream != null)
				{
					// stream will be null when pause called before resume
					stream.close();
					removeEventListeners(stream);
					stream = null;
				}
				isRunning = false;
			}
		}
		
		public function resume():void
		{
			if (!isRunning)
			{
				logger.info("resume. isRunning:" + isRunning);
				//super.resume();

				currentPiece = Piece.createPieceFromOffset(0, _offset, segmentInfo.fileLength);
				if (currentPiece != null)
				{
					currentPieceSize = currentPiece.size;
					//var requestUrl:String = constructUrl(currentPiece.offset, currentPiece.offset+currentPiece.size);
					var requestUrl:String = constructUrl(_offset, segmentInfo.offset + segmentInfo.fileLength);
					if (requestUrl != null)
					{
						stream = new URLStream;
						registerEventListeners(stream);
						logger.info("request " + requestUrl);
						var urlRequest:URLRequest = new URLRequest(requestUrl);
						//var rhArray:Array = new Array(new URLRequestHeader("Content-Type", "text/html"));
						stream.load(urlRequest);
						isRunning = true;
					}
					else
					{
						handleError(HTTP_URL_ERROR, "url construct error");
					}
				}
			}
		}
		
		public function destroy():void
		{
			//super.destory();
			pause();
			logger.info("destroy");
			uri = null;
			currentPiece = null;
		}
		
		public function constructUrl(rangeStart:uint, rangeEnd:uint = 0):String
		{
			if (uri.host == null || uri.path == null)
				return null;
			
			if (rangeStart == 0 && rangeEnd == 0)
			{
				return uri.toString();
			}
			else
			{
				//wenjiewang: range support does not exist right now.
				/*var newUri:URI = new URI(uri.toString());

				var pathArray:Array = uri.path.split("/");
				if (pathArray.length < 3)
					return null;
					
				var newPath:String = "/" + pathArray[1] + "/" + rangeStart + "/" + rangeEnd + "/" + pathArray[2] + "0range";
				var index:uint = 3;
				while (index < pathArray.length)
				{
					newPath += pathArray[index++];
				}
					
				newUri.path = newPath;
				
				return newUri.toString();*/
				return uri.toString();
			}
		}
		
		private function registerEventListeners(stream:URLStream):void
		{
			stream.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			stream.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError, false, 0, true);
			stream.addEventListener(Event.OPEN, onOpen, false, 0, true);
			stream.addEventListener(HTTPStatusEvent.HTTP_STATUS, onHttpStatus, false, 0, true);
			stream.addEventListener(ProgressEvent.PROGRESS, onProgress, false, 0, true);
			stream.addEventListener(Event.COMPLETE, onComplete, false, 0, true);
		}
		
		private function removeEventListeners(stream:URLStream):void
		{
			stream.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			stream.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
			stream.removeEventListener(Event.OPEN, onOpen);
			stream.removeEventListener(HTTPStatusEvent.HTTP_STATUS, onHttpStatus);
			stream.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			stream.removeEventListener(Event.COMPLETE, onComplete);
		}
		
		private function onIOError(event:IOErrorEvent):void
		{
			logger.error("onIOError  " + event);
			handleError(HTTP_IO_ERROR, event.toString());
		}
		
		private function onSecurityError(event:SecurityErrorEvent):void
		{
			logger.error("onSecurityError	" + event);
			handleError(HTTP_SECURITY_ERROR, event.toString());
		}
		
		private function onOpen(event:Event):void
		{
			logger.info("onOpen:" + event);
		}
		
		private function onHttpStatus(event:HTTPStatusEvent):void
		{
			logger.info("onHttpStatus:" + event);
			if (uint(event.status / 100) == 4 || uint(event.status / 100) == 5)
			{
				//handleError(event.status, "Http Status error.");
			}
		}
		
		private function onProgress(event:ProgressEvent):void
		{
			logger.info("onProgress:" + event.bytesLoaded + "|" + event.bytesTotal + " available:" + stream.bytesAvailable);

			// If isRunning is false, stream and download may be assigned to null by onRecvSubPiece.
			if (isRunning && offsetDiscarded == false) {
				if (segmentInfo.offset == 0) {
					offsetDiscarded = true;
				}else if (stream.bytesAvailable >= segmentInfo.offset) {
					var wbytes:ByteArray = new ByteArray;
					stream.readBytes(wbytes, 0, segmentInfo.offset);
					//ugly hack to get around NO RANGE support.
					offsetDiscarded = true;
					logger.info("hack: discarded data " + segmentInfo.offset);
				}
				//return;
			}
			while (isRunning && offsetDiscarded && stream.bytesAvailable >= currentPieceSize)
			{
				var bytes:ByteArray = new ByteArray;
				stream.readBytes(bytes, 0, currentPieceSize);
				logger.info("read bytes to process " + currentPieceSize);
				//speedMeter.submitBytes(bytes.length);
				
				var piece:Piece = mx.utils.ObjectUtil.copy(currentPiece) as Piece;
				currentPiece = mp4Download.cacheManager.getNextNonExistPiece(piece);
				if (currentPiece) {
					currentPieceSize = currentPiece.size;
				}else{
					//we are done with the segment. :)
					//wenjiewang: now what do we do?
					isDone = true;
					pause();
				}
				mp4Download.onRecvPiece(piece, bytes);
				if (isDone) {
					dispatchEvent(new DownloadEvent(DownloadEvent.DOWNLOAD_EVENT, 0));
				}
			}
		}
		
		private function onComplete(event:Event):void
		{
			logger.info("onComplete:" + event);
			//var tevent:ProgressEvent = new ProgressEvent("PROGRESS_EVENT", false, false, stream.bytesAvailable, segmentInfo.fileLength);
			//onProgress(tevent);
			dispatchEvent(new DownloadEvent("DOWNLOAD_EVENT", 0));
		}
		
		private function handleError(error:uint, message:String):void
		{
			logger.error("HttpDownloader::handleError1	" + message + ":" + error);
			destroy();
			dispatchEvent(new DownloadEvent(DownloadEvent.DOWNLOAD_EVENT, error));
		}
	}
}