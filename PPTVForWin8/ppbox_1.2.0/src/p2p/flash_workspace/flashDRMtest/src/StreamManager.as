package
{

	import de.polygonal.ds.ArrayUtil;
	
	import flash.events.AsyncErrorEvent;
	import flash.events.DRMErrorEvent;
	import flash.events.DRMStatusEvent;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.NetStatusEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.media.SoundTransform;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.NetStream;
	import flash.net.NetStreamAppendBytesAction;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class StreamManager extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(StreamManager);
		private static var MAX_BUFFER_LENGTH:uint = 300;
		
		private var playInfo:PlayInfo;
		private var connection:NetConnection;
		private var video:Video;
		private var stream:NetStream;
		private var segmentTimeOffsetArray:Array = new Array;
		private var segmentStreamVector:Vector.<Mp4Download>;
		private var mp4HeaderDownload:Mp4Download;
		private var mp4Stream:Mp4Stream;
		private var currentSegment:int = -1;
		private var seekTime:uint = 0;
		private var secondTimer:Timer;
		private var currentSegmentCompleteEvent:SegmentCompleteEvent;
		private var sessionId:String;
		private var haveDispatchPlayStartEvent:Boolean = false;
		private var isLastSegmentComplete:Boolean = false;
		private var startCalled:Boolean = false;
		private var startHeaderReady:Boolean = false;
		private var startTimeSaved:uint = 0;
		private var headerSegmentInfo:SegmentInfo;
		private var downloadError:Boolean = false;
		private var videoAttached:Boolean = false;
		
		public function StreamManager(playInfo:PlayInfo)
		{
			this.playInfo = playInfo;
			var timeOffset:Number = 0;
			for (var i:uint = 0; i < playInfo.segments.length; ++i)
			{
				segmentTimeOffsetArray.push(timeOffset);
				timeOffset += playInfo.segments[i].duration;
			}
			headerSegmentInfo = new SegmentInfo("Null", 0, playInfo.segments[0].offset, playInfo.segments[0].offset, 0);
			
			segmentStreamVector = new Vector.<Mp4Download>(playInfo.segments.length);
			
			sessionId = new Date().valueOf().toString() + Math.random();
			
			mp4Stream = new Mp4Stream(sessionId);
			
			connection = new NetConnection;
			connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler, false, 0, true);
			connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler, false, 0, true);
			connection.connect(null);
			
			secondTimer = new Timer(1000);
			secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
			
		}
		
		public function destory():void
		{
			video.attachNetStream(null);
			video = null;
			videoAttached = false;
			
			deleteMp4Download(currentSegment);
			
			connection.close();
			connection.removeEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			connection.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection = null;
			
			stream.close();
			stream.removeEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			stream.removeEventListener(IOErrorEvent.IO_ERROR, errorHandler);
			stream.removeEventListener(AsyncErrorEvent.ASYNC_ERROR, errorHandler);
			stream = null;
			
			playInfo = null;
			segmentTimeOffsetArray.length = 0;
			segmentTimeOffsetArray = null;
			segmentStreamVector.length = 0;
			segmentStreamVector = null;
			secondTimer.stop();
			secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
			secondTimer = null;
			currentSegmentCompleteEvent = null;
			sessionId = null;
		}
		
		public function hasError():Boolean
		{
			return (downloadError);	
		}
		
		public function attachVideo(video:Video):void
		{
			this.video = video;
			if (stream != null) {
				video.attachNetStream(stream);
				videoAttached = true;
			}
		}
		
		public function get volume():Number
		{
			return stream.soundTransform.volume;
		}
		
		public function set volume(volume:Number):void
		{
			if (volume >= 0 && volume <= 1)
			{
				logger.info("set volume:" + volume);
				stream.soundTransform = new SoundTransform(volume);
			}
			else
			{
				logger.error("invalid volume:" + volume);
			}
		}
		
		public function play(startTime:uint = 0):void
		{			
			logger.info("play from " + startTime + "seconds.");
			if (videoAttached == false) {
				video.attachNetStream(stream);
			}
			if (startHeaderReady == false) {
				startCalled = true;
				startTimeSaved = startTime;
			}else{
				var segmentIndex:uint = findSegmentIndexFromTime(startTime);
				createMp4Download(segmentIndex);
				currentSegment = segmentIndex;
				var timeOffset:uint = startTime - segmentTimeOffsetArray[segmentIndex];			
				segmentStreamVector[segmentIndex].start(timeOffset);
				secondTimer.start();
			}
		}
		
		public function pause():void
		{
			logger.info("pause");
			stream.pause();
		}
		
		public function resume():void
		{
			logger.info("resume");
			stream.resume();
		}
		
		public function seek(seekTime:uint):void
		{
			if (seekTime >= 0 && seekTime <= playInfo.duration)
			{
				logger.info("seek to " + seekTime + "seconds.");
				this.seekTime = seekTime;
				currentSegmentCompleteEvent = null;
				isLastSegmentComplete = false;
				stream.seek(0);	
			}
		}
		
		public function get time():uint
		{
			return seekTime + stream.time;
		}
		
		public function get bufferLength():uint
		{
			return stream.bufferLength;
		}
		
		
		private function onSeekNotify():void
		{
			logger.info("onSeekNotify");
			stream.appendBytesAction(NetStreamAppendBytesAction.RESET_BEGIN);
			
			var segmentIndex:uint = findSegmentIndexFromTime(seekTime);
			var timeOffset:uint;
			if (segmentIndex == currentSegment)
			{
				logger.info("seek in the same segment:" + currentSegment);
				timeOffset = seekTime - segmentTimeOffsetArray[currentSegment];
				segmentStreamVector[currentSegment].seek(timeOffset);
			}
			else
			{
				logger.info("seek in the different segment: " + currentSegment + "->" + segmentIndex);
				deleteMp4Download(currentSegment);
				currentSegment = segmentIndex;
				createMp4Download(segmentIndex);
				timeOffset = seekTime - segmentTimeOffsetArray[currentSegment];
				segmentStreamVector[currentSegment].start(timeOffset);
			}
		}
		
		private function createMp4Download(segmentIndex:int):void
		{
			var url:String;
			if (segmentIndex < 0) {
				logger.info("create download for the header segment");
				url = createSegmentURL(-1);
				mp4HeaderDownload = new Mp4Download(sessionId, url, headerSegmentInfo, mp4Stream);
				mp4HeaderDownload.addEventListener(ProgressEvent.PROGRESS, flvProgrossHandler, false, 0, true);
				mp4HeaderDownload.addEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, segmentComplete, false, 0, true);
				mp4HeaderDownload.addEventListener(DownloadEvent.DOWNLOAD_EVENT, onDownloadEvent);
			} else if (segmentStreamVector[segmentIndex] == null)
			{
				url = createSegmentURL(segmentIndex);
				var mp4Download:Mp4Download = new Mp4Download(sessionId, url, playInfo.segments[segmentIndex], mp4Stream);
				segmentStreamVector[segmentIndex] = mp4Download;
				mp4Download.addEventListener(ProgressEvent.PROGRESS, flvProgrossHandler, false, 0, true);
				mp4Download.addEventListener(GetSeekTimeEvent.GET_SEEK_TIME, getSeekTime, false, 0, true);
				mp4Download.addEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, segmentComplete, false, 0, true);
				mp4Download.addEventListener(DacLogEvent.STOP_DAC_LOG, onDacStopLog, false, 0, true);
				mp4Download.addEventListener(DownloadEvent.DOWNLOAD_EVENT, onDownloadEvent);
			}
		}
		
		private function createSegmentURL(segmentIndex:int):String
		{
			var url:String;
			
			url = playInfo.host + playInfo.fileName;
			
			return url;
		}
		
		private function deleteMp4Download(segmentIndex:int):void
		{
			if (segmentIndex < 0 && mp4HeaderDownload != null) {
				logger.info("free the mp4header downloader");
				mp4HeaderDownload.destroy();
				mp4HeaderDownload.removeEventListener(ProgressEvent.PROGRESS, flvProgrossHandler);
				mp4HeaderDownload.removeEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, segmentComplete);
				mp4HeaderDownload.removeEventListener(DownloadEvent.DOWNLOAD_EVENT, onDownloadEvent);
				mp4HeaderDownload = null;
			} else if (segmentStreamVector[segmentIndex] != null)
			{
				var mp4Download:Mp4Download = segmentStreamVector[segmentIndex];
				segmentStreamVector[segmentIndex] = null;
				
				mp4Download.destroy();
				mp4Download.removeEventListener(ProgressEvent.PROGRESS, flvProgrossHandler);
				mp4Download.removeEventListener(GetSeekTimeEvent.GET_SEEK_TIME, getSeekTime);
				mp4Download.removeEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, segmentComplete);
				mp4Download.removeEventListener(DacLogEvent.STOP_DAC_LOG, onDacStopLog);	
				mp4Download.removeEventListener(DownloadEvent.DOWNLOAD_EVENT, onDownloadEvent);
			}
		}
		
		private function flvProgrossHandler(event:ProgressEvent):void 
		{
			var bytes:ByteArray = new ByteArray;
			// We must guarantee that progress event are dispatched by currentSegment stream.
			if (startHeaderReady == false) {
				mp4HeaderDownload.readBytes(bytes);
			}else{
				segmentStreamVector[currentSegment].readBytes(bytes);
			}
			logger.debug("flvProgrossHandler size:" + bytes.length);
			stream.appendBytes(bytes);
		}
		
		private function getSeekTime(event:GetSeekTimeEvent):void 
		{
			// get accurate seekTime.
			seekTime = segmentTimeOffsetArray[currentSegment] + event.seekTime;
			logger.info("getSeekTime " + seekTime);
		}
		
		private function segmentComplete(event:SegmentCompleteEvent):void
		{
			logger.info("segmentComplete: " + currentSegment);
			if (startHeaderReady == false) {
				logger.info("Header segment complete");
				if (mp4Stream.hasMp4Header() == false) {
					//something is wrong. 
					//TODO: wenjiewang: we should raise an error event here.
					logger.info("error: mp4header downloaded but not passed");
					mp4HeaderDownload.sendBytesToMp4Stream();
				}
				//Destroy the header downloader.
				deleteMp4Download(-1);
				startHeaderReady = true;
				if (startCalled == true) {
					play(startTimeSaved);
				}
			}else{
				if (currentSegment != segmentStreamVector.length - 1)
				{
					// we have more segments.	
					currentSegmentCompleteEvent = event;
					moveToNextSegment();
				}
				else
				{
					isLastSegmentComplete = true;
				}
			}
		}
		
		private function findSegmentIndexFromTime(time:uint):uint
		{
			var segmentIndex:int = ArrayUtil.bsearchFloat(segmentTimeOffsetArray, time, 0, segmentTimeOffsetArray.length - 1);
			if (segmentIndex < 0)
			{
				return ~segmentIndex - 1;
			}
			else
			{
				// Wow, you are so lucky to come this path!
				return segmentIndex;
			}
		}
		
		private function netStatusHandler(event:NetStatusEvent):void 
		{
			switch (event.info.code)
			{
				case "NetConnection.Connect.Success":
					createNetStream();
					break;
				case "NetStream.Buffer.Full":
					if (!haveDispatchPlayStartEvent)
					{
						logger.info("NetStream.Play.Start: send play start event.");
						dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_START));
						haveDispatchPlayStartEvent = true;
					}
					
					logger.info("NetStream.Buffer.Full " + new Date().toLocaleTimeString());
					dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_FULL));
					break;
				case "NetStream.Buffer.Empty":
					if (isLastSegmentComplete)
					{
						logger.info("NetStream.Buffer.Empty movie complete.");
						dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_COMPLETE));
					}
					else
					{
						logger.info("NetStream.Buffer.Empty movie buffering.");
						dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_EMPTY));
					}					
					break;
				case "NetStream.Seek.Notify":
					onSeekNotify();
					break;
			}
		}
		
		private function securityErrorHandler(event:SecurityErrorEvent):void {
			logger.info("securityErrorHandler: " + event);
		}
		
		private function errorHandler(event:Event):void {
			logger.info("errorHandler: " + event);
		}
		
		private function createNetStream():void
		{
			stream = new NetStream(connection);
			stream.inBufferSeek = false;
			stream.checkPolicyFile = true;
			stream.bufferTime = 4;
			stream.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler, false, 0, true);
			stream.addEventListener(IOErrorEvent.IO_ERROR, errorHandler, false, 0, true);
			stream.addEventListener(AsyncErrorEvent.ASYNC_ERROR, errorHandler, false, 0, true);
			stream.addEventListener(DRMErrorEvent.DRM_ERROR, onDrmError, false, 0, true);
			stream.addEventListener(DRMStatusEvent.DRM_STATUS, drmStatusHandler); 
			stream.play(null);
			
			logger.info("NetStream created");
			
			createMp4Download(-1);
			mp4HeaderDownload.start(0);
		}
		
		private function onDrmError(event:DRMErrorEvent):void
		{
			logger.info("DRM Error: " + event.errorID + " SubID " + event.subErrorID);
			logger.info(mp4Stream.debugstr);
			var terror:ByteArray;
			logger.info(terror.length);
		}
		
		private function drmStatusHandler(event:DRMStatusEvent):void
		{
			logger.info("DRM Status: " + event);
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			dispatchEvent(new PlayProgressEvent(time, stream.bufferLength));
			// update rest play time.
			segmentStreamVector[currentSegment].setRestPlayTime(stream.bufferLength);
			if (currentSegmentCompleteEvent != null)
			{
				// current segment is complete.
				moveToNextSegment();
			}
		}
		
		private function moveToNextSegment():void
		{
			if (stream.bufferLength < MAX_BUFFER_LENGTH)
			{
				deleteMp4Download(currentSegment);
				currentSegment++;
				createMp4Download(currentSegment);
				
				segmentStreamVector[currentSegment].start(0, currentSegmentCompleteEvent);
				currentSegmentCompleteEvent = null;
			}
		}
		
		private function onDacStopLog(event:DacLogEvent):void
		{
			dispatchEvent(new DacLogEvent(DacLogEvent.STOP_DAC_LOG, event.logObject));
		}
		
		private function onDownloadEvent(event:DownloadEvent):void
		{
			if (event) {
				if (event.downloadStatus != 0) {
					downloadError = true;
				}else{
					//this means we are done with the current segment.
					logger.info("We are done with current segment " + currentSegment);
					//currentSegmentCompleteEvent = null;
					//moveToNextSegment();
					segmentComplete(null);
				}
			}
		}
	}
}
