package
{
	
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
		private var currentSegment:int = -1;
		private var seekTime:uint = 0;
		private var secondTimer:Timer;
		private var newStartTime:Number;
		private var sessionId:String;
		private var haveDispatchPlayStartEvent:Boolean = false;
		private var isLastSegmentComplete:Boolean = false;
		private var startCalled:Boolean = false;
		private var startHeaderReady:Boolean = false;
		private var startTimeSaved:uint = 0;
		private var downloadError:Boolean = false;
		private var fileURL:String;
		public var isRunning:Boolean = false;
		
		public function StreamManager(playInfo:PlayInfo)
		{
			this.playInfo = playInfo;
			
			sessionId = new Date().valueOf().toString() + Math.random();

			secondTimer = new Timer(1000);
			secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
			
		}
		
		public function destroy():void
		{
			video.attachNetStream(null);
			video = null;
			
			destroyStream();			
			destroyConnection();
			
			playInfo = null;

			secondTimer.stop();
			secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
			secondTimer = null;
			sessionId = null;
			
			isRunning = false;
		}
		
		public function destroyConnection():void
		{
			logger.info("NetConnection closed");
			connection.close();
			connection.removeEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			connection.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection = null;
		}

		public function destroyStream():void
		{
			stream.close();
			stream.removeEventListener(DRMStatusEvent.DRM_STATUS, drmStatusHandler); 
			stream.removeEventListener(DRMErrorEvent.DRM_ERROR, drmErrorHandler); 
			stream.removeEventListener(NetStatusEvent.NET_STATUS, netStatusHandler); 
			haveDispatchPlayStartEvent = false;
			stream = null;
		}
		
		public function hasError():Boolean
		{
			return (downloadError);	
		}
		
		public function attachVideo(video:Video):void
		{
			this.video = video;
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
		
		public function play(startTime:Number):void
		{			
			if (stream != null) {
				destroyStream();
				//destroy connection
				//destroyConnection();
			}
			
			if (secondTimer == null) {
				secondTimer = new Timer(1000);
				secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
			}
			
			newStartTime = startTime;
			logger.info("play called: " + newStartTime);
			isRunning = true;
			
			if (connection == null) {
				connection = new NetConnection;
				connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler, false, 0, true);
				connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler, false, 0, true);
				logger.info("conn prop: " + connection.connected);
				connection.connect(null);
			}else{
				logger.info("conn already exists");
				createNetStream();
			}

		}
				
		public function drmErrorHandler(event:DRMErrorEvent):void
		{
			trace("netsteam drmErrorHandler");
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
		
		public function seek(newSeekTime:uint):void
		{
			if (newSeekTime >= 0 && newSeekTime <= playInfo.duration)
			{
				logger.info("seek to " + newSeekTime + " seconds.");
				//this.seekTime = seekTime;
				//what the hack?
				//stream.seek(0);
				if (playInfo.needSeek(newSeekTime, this.time)){
					play(newSeekTime);
					this.seekTime = playInfo.getSeekTime();
				}else{
					//seektime should not move in this case. since we are not seeking.
					logger.info("seek: tehre is no need to seek");
				}
			}
		}
		
		public function get time():uint
		{
			var nTime:Number = seekTime + stream.time;
			logger.info( "get time: seekTime = " + seekTime + " streamTime = " + stream.time);
			return nTime;
		}
		
		public function get bufferLength():uint
		{
			return stream.bufferLength;
		}
		
		
		private function onSeekNotify():void
		{
			logger.info("onSeekNotify");
		}
		
		private function constructURL(startTime:int):String
		{
			var url:String = playInfo.constructUrl(startTime);
			logger.info("URL: " + url);
			return url;
		}

		private function netStatusHandler(event:NetStatusEvent):void 
		{
			logger.info("NetStatus Triggered: " + event.info.code);
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
				case "NetStream.Buffer.Flush":
					logger.info("NetStream.Buffer.Flush.");
					//dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_COMPLETE));
					break;
				case "NetStream.Buffer.Stop":
					logger.info("NetStream.Buffer.Stop.");
					dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_COMPLETE));
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
			stream.addEventListener(DRMStatusEvent.DRM_STATUS, drmStatusHandler); 
			stream.addEventListener(DRMErrorEvent.DRM_ERROR, drmErrorHandler); 
			stream.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler); 
			stream.client = new CustomClient(); 
			video.attachNetStream(stream); 
			
			logger.info("create new netstream: " + newStartTime);
			fileURL = constructURL(newStartTime);
			//fileURL = "http://192.168.13.21:8080/0/anonymous.mp4?key=7d1b5dff99f1a1f29232eb481ec66dea&start=1981536";
			//fileURL = "http://192.168.13.21:8080/0/anonymous.mp4?key=7d1b5dff99f1a1f29232eb481ec66dea&start=3880482";
			
			stream.play(fileURL); 
		}
		
		private function onDrmError(event:DRMErrorEvent):void
		{
			logger.info("DRM Error: " + event.errorID + " SubID " + event.subErrorID);
			//logger.info(mp4Stream.debugstr);
			var terror:ByteArray;
			logger.info(terror.length);
		}
		
		private function drmStatusHandler(event:DRMStatusEvent):void
		{
			logger.info("DRM Status: " + event);
			logger.info("DRM isLocal: "+ event.isLocal);
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			dispatchEvent(new PlayProgressEvent(time, stream.bufferLength));
			// update rest play time.
			// set the rest playtime
			// wenjiewang
		}
		
		private function onDacStopLog(event:DacLogEvent):void
		{
			dispatchEvent(new DacLogEvent(DacLogEvent.STOP_DAC_LOG, event.logObject));
		}
		

	}
}
