package com.pplive.p2p
{
	import com.pplive.events.DacLogEvent;
	import com.pplive.events.HttpErrorEvent;
	import com.pplive.mx.ObjectUtil;
	import com.pplive.p2p.download.DownloadDriver;
	import com.pplive.p2p.download.IDownloadTarget;
	import com.pplive.p2p.download.P2PDownloader;
	import com.pplive.p2p.events.GetSeekTimeEvent;
	import com.pplive.p2p.events.SegmentCompleteEvent;
	import com.pplive.p2p.kernel.KernelDectecter;
	import com.pplive.p2p.kernel.KernelDescription;
	import com.pplive.p2p.kernel.StatusReporter;
	import com.pplive.p2p.mp4.Mp4Stream;
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.play.SegmentInfo;
	import com.pplive.profile.FunctionProfiler;
	import com.pplive.util.URI;
	
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.ProgressEvent;
	import flash.events.TimerEvent;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	
	import org.as3commons.concurrency.thread.FramePseudoThread;
	import org.as3commons.concurrency.thread.IRunnable;
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	//P2PMp4Stream 负责创建download driver。检查内核是否存在，如果存在，再创建p2pdownloader
	public class P2PMp4Stream extends EventDispatcher implements IDownloadTarget, IP2PStream, IRunnable
	{
		private static var logger:ILogger = getLogger(P2PMp4Stream);
		private static var MAX_SEND_COUNT_PER_FRAME:uint = 50;

		private var _sessionId:String;
		
		//url可能是传给内核的时候用的url，和原始url是不同的。原始url需要记录下来，用于内核故障的时候恢复
		private var _url:String;
		private var _originalUrl:String;
		private var _bwType:uint;
		private var _segmentInfo:SegmentInfo;
		private var _backupHosts:Vector.<String>;
		
		//offset 表示创建downloaddriver时需要下载的数据的起始地址（不考虑mp4头），如果内核模式失败，这个值在重置时有用。 
		//createDownloadDriver 可能会把offset设置为0，但是下完头部之后，会再设置回到正确的offset
		private var offset:uint;
		
		private var kernelDetecter:KernelDectecter;
		private var statusReporter:StatusReporter=null;
		private var kernelDescription:KernelDescription;
		private var playMode:int = Constants.PLAY_MODE_UNKNOWN;

		private var mp4Stream:Mp4Stream;
		private var cacheManager:CacheManager;
		private var downloadDriver:DownloadDriver;
		private var p2pDownloader:P2PDownloader;
		private var frameThread:FramePseudoThread;
		
		//下一个要发送给mp4Stream的subpiece。cacheManager不是顺序填入subpiece，但是mp4stream需要顺序填入
		private var sendPosition:SubPiece;
		//拖动之后，mp4头下完了，就会设置needSeek为false
		private var needSeek:Boolean;
		private var seekTime:uint;
		private var seekOffset:uint;
		private var seekFirstSubPieceSended:Boolean;
		private var restPlayTime:uint;
		
		public function P2PMp4Stream(sessionId:String, url:String, bwType:uint, segmentInfo:SegmentInfo, backupHosts:Vector.<String>)
		{
			super();
			
			_sessionId = sessionId;
			_url = url;
			_originalUrl = url;
			_bwType = bwType;
			_segmentInfo = segmentInfo;
			_backupHosts = backupHosts;
			
			logger.info("new P2PMp4Stream url:" + url + ", rid:" + _segmentInfo.rid + ", fileLength:" + _segmentInfo.fileLength
						+ ", bwType:" + bwType);
			
			mp4Stream = new Mp4Stream();
			mp4Stream.addEventListener(ProgressEvent.PROGRESS, onRecvFlvData, false, 0, true);
			mp4Stream.addEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, onComplete, false, 0, true);
			
			cacheManager = new CacheManager(_segmentInfo.fileLength);
			kernelDetecter = new KernelDectecter(this);
			
			frameThread = new FramePseudoThread(this);
		}
		
		public function destory():void
		{
			logger.info("destory");
			stopDownload();
			_segmentInfo = null;
			_backupHosts = null;
			
			if (kernelDetecter)
			{
				kernelDetecter.destory();
				kernelDetecter = null;
			}
			
			if (statusReporter)
			{
				statusReporter.destory();
				statusReporter = null;
			}
			
			kernelDescription = null;			
			sendPosition = null;
			mp4Stream.destory();
			mp4Stream = null;
			cacheManager.destory();
			cacheManager = null;
			
			if (p2pDownloader)
			{
				p2pDownloader.destory();
				p2pDownloader = null;
			}
			
			frameThread.destroy();
			frameThread = null;
		}
		
		public function reportKernelStatus(exist:Boolean, detectTime:int, kernel:KernelDescription = null):void
		{
			logger.info("reportKernelStatus exist:" + exist + ", detectTime" + detectTime);
			
			if(exist)
			{
				sendDetectKernelLogEvent(detectTime);
			}
			
			var rid:RID = new RID(_segmentInfo.rid);
			// herain:内核从1812版本开始支持flash p2p
			if (exist && kernel.extraVersion >= 1812)
			{
				// change url to kernel mode url
				playMode = Constants.PLAY_MODE_KERNEL;
				var uri:URI = new URI(_originalUrl);
				uri.port = Constants.KERNEL_MAGIC_PORT;
				//原来是连19765端口，被钩挂到9000端口，所以不用设置host，现在直连9000，会判断只接受本机来的ip	
				uri.host = "127.0.0.1";
				uri.path = "/ppvaplaybyopen";
				uri.variables.url = _originalUrl;
				uri.variables.id = _sessionId;
				uri.variables.headlength = _segmentInfo.headLength;
				uri.variables.BWType = _bwType;

				if (_backupHosts != null && _backupHosts.length > 0)
				{
					uri.variables.bakhost = _backupHosts.join("@");
				}
				
				// TODO(herain):sourceType may change. 
				uri.variables.source = 1;
				_url = uri.toString();
				logger.info("change url to " + _url+" orginal url:"+_originalUrl);
				
				if (rid.isValid())
				{
					logger.info("rid is valid,new a StatusReporter");
					if(null == statusReporter)
					{
						statusReporter = new StatusReporter(kernel.tcpPort, rid);	
						Constants.LOCAL_KERNEL_TCP_PORT = kernel.tcpPort;
					}
				}
				else
				{
					logger.info("rid is invalid,kerneldescription:"+kernelDescription);
					kernelDescription = kernel;
				}
			}
			else
			{
				// non kernel mode, create p2pDownloader
				playMode = Constants.PLAY_MODE_DIRECT;
				if (rid.isValid())
				{
					logger.info("rid is:"+rid+" create p2pdownloader");
					p2pDownloader = new P2PDownloader(rid);	
				}
				_url = _originalUrl;
			}
			
			createDownloadDriver(offset);
			kernelDetecter.destory();
			kernelDetecter = null;
		}
		
		public function getPlayMode():int
		{
			return playMode;
		}
		
		public function onRecvSubPiece(subPiece:SubPiece, data:ByteArray):void
		{		
			logger.debug("P2PMp4Stream::onRecvSubPiece	" + subPiece + ", sendPosition:" + sendPosition);
			cacheManager.addSubPiece(subPiece, data);
		}
		
		//有插失败，重试
		public function onKernelModeError():void
		{			
			stopDownload();	
			playMode = Constants.PLAY_MODE_UNKNOWN;
			if (statusReporter)
			{
				statusReporter.destory();
				statusReporter = null;
			}
			
			if (needSeek)
			{
				logger.info("need seek,offset:"+offset+" hasmp4head:"+ mp4Stream.hasMp4Header());
			}
			
			//如果断开的时候，mp4头收了一半，需要重新来过，cachemanager里会处理mp4头是否已经收过			
			kernelDetecter = new KernelDectecter(this);
		}
		
		public function onHttpError(error:uint,interval:uint):void
		{
			if(playMode == Constants.PLAY_MODE_KERNEL)
			{
				onKernelModeError();
			}
			else
			{
				dispatchEvent(new HttpErrorEvent(HttpErrorEvent.VOD_HTTP_ERROR,interval,error));	
			}
		}
		
		public function hasPiece(piece:Piece):Boolean
		{
			return cacheManager.hasPiece(piece);
		}
		
		public function hasSubPiece(subPiece:SubPiece):Boolean
		{
			return cacheManager.hasSubPiece(subPiece);
		}
		
		public function get bwType():uint
		{
			return _bwType;
		}
		
		public function start(timeOffset:uint, restPlayTime:uint, completeEvent:SegmentCompleteEvent = null):void
		{
			logger.info("start timeOffset:" + timeOffset + ", completeEvent:" + completeEvent);
			sendPosition = new SubPiece(0, 0);
			needSeek = false;
			seekFirstSubPieceSended = true;
			this.restPlayTime = restPlayTime;
			
			if (completeEvent != null)
			{
				mp4Stream.setBaseTimeStamp(completeEvent);
			}
			
			if (timeOffset == 0)
			{
				createDownloadDriver(0);
			}
			else
			{
				seek(timeOffset);
			}
			
			frameThread.start();
		}
		
		public function readBytes():ByteArray
		{
			var bytes:ByteArray = mp4Stream.readBytes();
			offset += bytes.length;
			logger.debug("new offset:"+offset);
			return bytes;
		}
		
		//段内拖动也会走到这里
		public function seek(time:uint):void
		{
			logger.info("seek seekTime:" + time);
			seekTime = time;
			needSeek = true;
			doSeek();
		}
		
		public function setRestPlayTime(time:uint):void
		{
			if (downloadDriver != null)
			{
				downloadDriver.setRestPlayTime(time);
				logger.info("setRestPlayTime " + time + " seconds.");
			}
			
			if (statusReporter)
			{
				statusReporter.setRestPlayTime(time);
			}
		}
		
		public function sendStopDacLog(logObject:Object):void
		{
			logObject.dt = "stop";
			logObject.rid = _segmentInfo.rid;
			logObject.url = _url;
			logObject.sz = _segmentInfo.fileLength;
			logObject.du = _segmentInfo.duration;
			logObject.bwt = bwType;
			
			logger.info("dispatch DacLogEvent.P2P_DAC_LOG");
			
			dispatchEvent(new DacLogEvent(logObject));
		}
		
		private function sendDetectKernelLogEvent(detectTime:uint):void
		{
			var logObject:Object = new Object;
			
			// guid, uid, s property must be set by player
			logObject.dt = "vst";
			logObject.ctx = null;
			logObject.vst = detectTime;
			logger.info("dispatch DacLogEvent.DETECT_KERNEL_LOG");
			
			dispatchEvent(new DacLogEvent(logObject, DacLogEvent.DETECT_KERNEL_LOG));
		}
		
		private function createDownloadDriver(offset:uint):void
		{
			if (playMode == Constants.PLAY_MODE_UNKNOWN)
			{
				// record offset, createDownloadDriver will be called again after kernel status detected.
				this.offset = offset;
				logger.info("offset:"+offset+" playMode:"+playMode);
			}
			else
			{
				var kernelMode:Boolean = (playMode == Constants.PLAY_MODE_KERNEL);
				logger.info("createDownloadDriver offset:" + offset + ", kernelMode:" 
					+ kernelMode +" url:"+_url+" need seek:"+needSeek);
				if(needSeek)
				{
					//王斌的忠告：下载请以subpiece_size为单位，不要下载半个subpiece，所以这里向上取整了。
					var dragHeadLength:uint = ((int)((_segmentInfo.headLength - 1)/Constants.SUBPIECE_SIZE+1))*Constants.SUBPIECE_SIZE ;
					
					//如果是拖动的时候，仅需要下载头部。
					downloadDriver = new DownloadDriver(this, kernelMode, _url, _backupHosts, _segmentInfo.fileLength, offset,restPlayTime,dragHeadLength);					
				}
				else
				{
					downloadDriver = new DownloadDriver(this, kernelMode, _url, _backupHosts, _segmentInfo.fileLength, offset,restPlayTime,0);
				}
				if (!kernelMode && p2pDownloader)
				{
					downloadDriver.attachP2PDownloader(p2pDownloader);
					p2pDownloader.attachDownloadDriver(downloadDriver);
				}
				
				downloadDriver.start();
			}
		}
		
		private function stopDownload():void
		{
			if (downloadDriver)
			{
				downloadDriver.stop();
				downloadDriver = null;
			}
			
			if (p2pDownloader)
			{
				p2pDownloader.stop();
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
				sendPosition = SubPiece.createSubPieceFromOffset(seekOffset);
				needSeek = false;
				seekFirstSubPieceSended = false;
				
				if (downloadDriver != null)
				{
					// downloadDriver will be null when start with non zero offset and mp4Stream.hasMp4Header() is true.
					logger.info("httpFailTimes:"+httpFailTimes()+" when download header");
					downloadDriver.stop();
					if (p2pDownloader)
					{
						p2pDownloader.reset();
					}
				}				
				// create new downloadDriver which download begin from the seek offset.
				createDownloadDriver(seekOffset);
			
			}
			else if (downloadDriver == null)
			{
				logger.debug("doSeek without header, downloader header first");
				createDownloadDriver(0);
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
		
		public function cleanup():void
		{
			// IRunnable interface function cleanup, do nothing.
		}
		
		public function process():void
		{
			// IRunnable interface function process
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "process");
			sendBytesToMp4Stream();
			profiler.makeSection();
			if (needSeek)
			{
				doSeek();
			}
			profiler.end();
		}
		
		private function sendBytesToMp4Stream():void
		{
			var sendCount:uint;
			logger.debug("sendBytesToMp4Stream sendPosition:"+sendPosition);
			while(sendCount < MAX_SEND_COUNT_PER_FRAME && cacheManager.hasSubPiece(sendPosition))
			{
				var bytes:ByteArray = cacheManager.getSubPiece(sendPosition);
				if (!seekFirstSubPieceSended)
				{
					bytes.position = seekOffset - sendPosition.offset;
					seekFirstSubPieceSended = true;
					logger.debug("set seekFirstSubPieceSended，seekOffset"+seekOffset+" sendPosition.offset"+sendPosition.offset);
				}
				
				logger.debug("P2PMp4Stream::sendBytesToMp4Stream send:" + sendPosition);
				mp4Stream.appendBytes(bytes);
				sendPosition.moveToNextSubPiece();
				++sendCount;
			}
		}
		
		private function onComplete(event:SegmentCompleteEvent):void
		{
			logger.info("onComplete");
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			dispatchEvent(new SegmentCompleteEvent(SegmentCompleteEvent.SEGMENT_COMPLETE, event.videoTimeStamp, event.audioTimeStamp));
			
			// download complete, stop download which will send dac log.
			profiler.makeSection();
			stopDownload();
			profiler.end();
		}
		
		public function setRid(ridString:String):void
		{
			logger.info("set rid:"+ridString);
			_segmentInfo.rid = ridString;
			var rid:RID = new RID(ridString);
			switch(playMode)
			{
				case Constants.PLAY_MODE_KERNEL:
					if(null == statusReporter)
					{
						statusReporter = new StatusReporter(kernelDescription.tcpPort, rid);
					}
					break;
				case Constants.PLAY_MODE_DIRECT:
					p2pDownloader = new P2PDownloader(rid);
					downloadDriver.attachP2PDownloader(p2pDownloader);
					p2pDownloader.attachDownloadDriver(downloadDriver);
					break;
				case Constants.PLAY_MODE_UNKNOWN:
					break;
			}
		}

		public function httpFailTimes():int 
		{	
			var failtimes:int = 0;
			if (downloadDriver)
			{				
				failtimes = downloadDriver.httpFailTimes();
				logger.info("http failed times:"+failtimes);
				return failtimes;
			}
			return failtimes;
		}
	}
}