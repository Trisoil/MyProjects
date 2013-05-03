package com.pplive.p2p.download
{
	import com.pplive.mx.ObjectUtil;
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.profile.FunctionProfiler;
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
	import flash.utils.getTimer;
	import flash.utils.setTimeout;

	
	import org.as3commons.concurrency.thread.FramePseudoThread;
	import org.as3commons.concurrency.thread.IRunnable;
	import org.as3commons.concurrency.thread.IThread;
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class HttpConnection extends EventDispatcher implements IRunnable
	{
		private static const logger:ILogger = getLogger(HttpConnection);
		private static const MAX_RECV_COUNT_PER_FRAME:uint = 50;
		//超过 DATA_RECV_TIMEOUT 毫秒 没有收到数据就认为是连接出错了。
		private static const DATA_RECV_TIMEOUT:uint = 15000;
		
		public static const HTTP_IO_ERROR:uint = 1000;
		public static const HTTP_SECURITY_ERROR:uint = 1001;
		public static const HTTP_URL_ERROR:uint = 1002;	
		public static const HTTP_TIMEOUT_ERROR:uint = 1003;
		

		private var _downloader:HttpDownloader;
		private var _uri:URI;
		private var _pieces:Vector.<Piece>;
		private var _fileLength:uint;
		
		private var _stream:URLStream;
		private var _currentSubPiece:SubPiece;
		private var _currentSubPieceSize:uint;
		private var _lastSubPiece:SubPiece;
		
		private var _frameThread:IThread;
		
		//发起请求的时间
		private var _requestTime:uint;
		//表明是否收到过包体里的数据，目前只是在失败回调有用，因为收到过数据的失败和没收到过数据的失败处理不太一样
		private var _hasRecvData:Boolean = false;
		//记录上一次收到数据的时间。初始值为创建连接时候的时间。
		private var _lastRecvTime:uint;
		
		public function HttpConnection(downloader:HttpDownloader, url:String, pieces:Vector.<Piece>, fileLength:uint)
		{
			logger.info("new HttpConnection url:" + url + ", download from:" + pieces[0] + ", to:" + pieces[pieces.length-1]);
			_downloader = downloader;
			_uri = new URI(url);
			_pieces = pieces;
			_fileLength = fileLength;
			_frameThread = new FramePseudoThread(this);
			_frameThread.start();
			
			doRequest();
		}
		
		public function destory():void
		{
			logger.info("destory");
			_downloader = null;
			_uri = null;
			_pieces.length = 0;
			_pieces = null;
			
			closeStream();
			_currentSubPiece = null;
			_lastSubPiece = null;
			
			_frameThread.stop();
			_frameThread = null;
		}
		
		public function stop():void
		{
			destory();
		}
		
		public function reuse(pieces:Vector.<Piece>):void
		{
			_pieces = pieces;
			if (canReuseStream(pieces[0]))
			{
				logger.info("reuse HttpConnection");
				initRequest();
			}
			else
			{
				logger.info("can not reuse HttpConnection");
				closeStream();
				doRequest();
			}
		}
		
		// 能否复用连接来下载piece
		private function canReuseStream(piece:Piece):Boolean
		{
			logger.debug("canReuseStream: _lastSubPiece:" + _lastSubPiece + ", new subpiece:" + piece.getFirstSubPiece());
			var nextSubPiece:SubPiece = com.pplive.mx.ObjectUtil.copy(_lastSubPiece) as SubPiece;
			nextSubPiece.moveToNextSubPiece();
			
			return nextSubPiece.compare(piece.getFirstSubPiece()) == 0;
		}
		
		public function cleanup():void
		{
			
		}
		
		public function process():void
		{
			var recvSubPieceCount:uint;
			if(_stream.bytesAvailable >= _currentSubPieceSize)
			{
				_hasRecvData = true;
				_lastRecvTime = getTimer();
			}
			else
			{
				//如果是刚启动，且一定时间没有收到数据，才报错。
				if(!_hasRecvData && (getTimer() - _lastRecvTime > DATA_RECV_TIMEOUT ))
				{
					logger.info("post a timer:"+this);
					//这里不能同步调用handleError，因为handleError里会destory这个connection，这样会导致调用process的_frameThread成为null
					//setTimeout(handleError,1,HTTP_TIMEOUT_ERROR, "recv timeout,last recv time:"+_lastRecvTime+" now:"+getTimer());
					handleError(HTTP_TIMEOUT_ERROR, "recv timeout,last recv time:"+_lastRecvTime+" now:"+getTimer());
					return;
				}
			}
			
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			// If isRunning is false, stream and download may be assigned to null by onRecvSubPiece.
			
			
			while (_stream.bytesAvailable >= _currentSubPieceSize
				&& recvSubPieceCount < MAX_RECV_COUNT_PER_FRAME)
			{
				++recvSubPieceCount;
				
				var bytes:ByteArray = new ByteArray;
				_stream.readBytes(bytes, 0, _currentSubPieceSize);
				//speedMeter.submitBytes(bytes.length);
				profiler.makeSection();
				
				var subpiece:SubPiece = com.pplive.mx.ObjectUtil.copy(_currentSubPiece) as SubPiece;
				logger.debug("call onRecvSubPiece,_currentSubPieceSize:"+_currentSubPieceSize);
				_downloader.onRecvSubPiece(subpiece, bytes);				
				if (_lastSubPiece.compare(_currentSubPiece) == 0)
				{
					// task finished.
					_downloader.onConnectionCompleted(this);
					break;
				}
				else
				{
					_currentSubPiece.moveToNextSubPiece();
					if(_downloader.dragHeadLength != 0)
					{
						_currentSubPieceSize = _currentSubPiece.getSize(_downloader.dragHeadLength);
					}
					else
					{	
						_currentSubPieceSize = _currentSubPiece.getSize(_fileLength);
					}
				}		
				profiler.makeSection();
			}
			profiler.end();
		}
		
		private function closeStream():void
		{
			if (_stream)
			{
				_stream.close();
				removeEventListeners(_stream);
				_stream = null;
			}
		}
		
		private function initRequest():void
		{			
			var lastPiece:Piece = _pieces[_pieces.length - 1]; 
			var subpieceCountInLastPiece:uint = StructUtil.getSubPieceCountInPiece(_fileLength, lastPiece);
			_lastSubPiece = lastPiece.getNthSubPiece(subpieceCountInLastPiece - 1);
			
			_currentSubPiece = _pieces[0].getFirstSubPiece();
			_currentSubPieceSize = _currentSubPiece.getSize(_fileLength);	
			logger.info(": download from:" + _pieces[0] + ", to:" + _pieces[_pieces.length-1]+ "_fileLength:"
				+_fileLength + " _currentSubPiece:"+_currentSubPiece+ "_currentSubPieceSize:"+_currentSubPieceSize);
		}
		
		private function doRequest():void
		{
			initRequest();
			
			var requestUrl:String = constructUrl(_currentSubPiece.offset);
			if (requestUrl != null)
			{
				_stream = new URLStream;
				registerEventListeners(_stream);
				logger.info("request " + requestUrl);
				_stream.load(new URLRequest(requestUrl));
				_hasRecvData = false;
				_requestTime = getTimer();				
				_lastRecvTime = getTimer();
			}
			else
			{
				handleError(HTTP_URL_ERROR, "url construct error");
			}
		}
		
		private function constructUrl(rangeStart:uint, rangeEnd:uint = 0):String
		{
			logger.info("constructUrl,start:"+rangeStart+" end:"+rangeEnd);
			if (_uri.host == null || _uri.path == null)
				return null;
			
			var newUri:URI = new URI(_uri.toString());
			if (_uri.port != Constants.KERNEL_MAGIC_PORT)
			{
				if (isRangeRequest(rangeStart, rangeEnd))
				{
					var pathArray:Array = _uri.path.split("/");
					var pathArrayLength:uint = pathArray.length; 
					if (pathArrayLength < 3)
						return null;
					
					var newPath:String = "/" + pathArray[1] + "/" + rangeStart + "/" + rangeEnd + "/" + pathArray[2];
					var index:uint = 3;
					while (index < pathArrayLength)
					{
						newPath += pathArray[index++];
					}
					
					newUri.path = newPath;
				}				
				
				// 将URL中的百分号替换为__，解决flash会报SecurityError的问题。
				// 为了CDN统计不同业务流量增加type字段
				newUri.variables.type = "fpp";
				return URI.transferToPPFixUrl(newUri.toString());
			}
			else
			{
				if (isRangeRequest(rangeStart, rangeEnd))
				{
					newUri.variables.rangeStart = rangeStart;
					newUri.variables.rangeEnd = rangeEnd;
				}
				
				newUri.variables.resttime = _downloader.restPlayTime;		
				
				if(_downloader.dragHeadLength != 0)
				{
					//拖动的mp4头下载，需要特殊处理
					newUri.variables.rangeStart = 0;
					newUri.variables.rangeEnd = _downloader.dragHeadLength;
					newUri.variables.headonly = "true";
					newUri.variables.drag = 1;
				}
				
				if(Constants.IS_VIP)
				{
					newUri.variables.vip = 1;
				}
				
				logger.info("constructUrl,start:"+rangeStart+" end:"+rangeEnd+" resttime:"
					+_downloader.restPlayTime+"dragHeadLength:"+_downloader.dragHeadLength);
				return newUri.toString();
			}
		}
		
		
		
		private function isRangeRequest(rangeStart:uint, rangeEnd:uint):Boolean
		{
			return (rangeStart != 0 || rangeEnd != 0);
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
				handleError(event.status, "Http Status error.");
			}			
		}
		
		private function onProgress(event:ProgressEvent):void
		{
			logger.debug("onProgress:" + event.bytesLoaded + "|" + event.bytesTotal + " available:" + _stream.bytesAvailable);
		}
		
		private function onComplete(event:Event):void
		{
			logger.info("onComplete:" + event);
		}
		
		private function handleError(error:uint, message:String):void
		{
			setTimeout(OnError,1,error, message);	
		}
		
		private function OnError(error:uint, message:String):void
		{
			if (null == _stream || _downloader == null || _frameThread==null)
			{
				//走到这里的原因是 setTimeout(handleError,1。。。） 被连续调用了2次，但是却没有得到处理，正常的情况应该是调用一次处理一次
				logger.info("may not run here,stream "+(null == _stream)+" download:"+(null == _downloader) + 
					" _frameThread:"+(_frameThread==null));
				return;
			}
			
			logger.error("HttpDownloader::handleError1	" + message + ":" + error+
				" time:"+getTimer()+" _requestTime:"+_requestTime+" _hasRecvData:"+_hasRecvData);
		
			// TODO(herain):2012-1-31:report http error
			_downloader.onHttpError(this,error+(_hasRecvData?10000:0),(uint)(getTimer())-_requestTime);
		}
		
		public function get piece():Vector.<Piece> 
		{
			return _pieces;
		}
		
	}
}