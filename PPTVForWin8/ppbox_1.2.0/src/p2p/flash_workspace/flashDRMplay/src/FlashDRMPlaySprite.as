package
{	
	import flash.display.Sprite;
	import flash.errors.IllegalOperationError;
	import flash.events.DRMAuthenticationCompleteEvent;
	import flash.events.DRMAuthenticationErrorEvent;
	import flash.events.DRMErrorEvent;
	import flash.events.DRMStatusEvent;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.NetStatusEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.NetStream;
	import flash.net.NetStreamPlayOptions;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.net.drm.AuthenticationMethod;
	import flash.net.drm.DRMContentData;
	import flash.net.drm.DRMManager;
	import flash.net.drm.LoadVoucherSetting;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.LOGGER_FACTORY;
	import org.as3commons.logging.api.getLogger;
	import org.as3commons.logging.setup.LevelTargetSetup;
	import org.as3commons.logging.setup.LogSetupLevel;
	import org.as3commons.logging.setup.RegExpSetup;
	import org.as3commons.logging.setup.SimpleTargetSetup;
	import org.as3commons.logging.setup.target.FirebugTarget;
	import org.as3commons.logging.setup.target.TraceTarget;
	
	public class FlashDRMPlaySprite extends Sprite
	{
		private static var logger:ILogger = getLogger(FlashDRMPlaySprite);
		
		private var connection:NetConnection;
		private var secondTimer:Timer;
		private var progress:Progress;
		private var duration:Number;
		private var video:Video;
		private var text:TextField = new TextField;
		private var stopText:TextField = new TextField;
		private var completeSegmentCount:uint = 0;
		//private var stream:NetStream;
		private var streamManager:StreamManager;
		private var dragLoader:DragLoader;
		private var hasDrag:Boolean;
		private var hasDT:Boolean;
		private var metaLoader:DRMMetaLoader;
		private var dtLoader:DtLoader;
		private var drmService:DRMService;
		
		private var fileURLMeta:String;
		private var fileURL:String;
		private var mediaFileName:String = "anonymous";
		
		public function FlashDRMPlaySprite()
		{
			//LOGGER_FACTORY.setup = new SimpleTargetSetup(new TraceTarget);
			LOGGER_FACTORY.setup = new LevelTargetSetup(new TraceTarget, LogSetupLevel.INFO);
			drmService = new DRMService();
			
			connection = new NetConnection();
			connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection.connect(null);
		}
		
		private function netStatusHandler(event:NetStatusEvent):void {
			switch (event.info.code) {
				case "NetConnection.Connect.Success":
					connectStream();
					break;
			}
		}
		
		private function securityErrorHandler(event:SecurityErrorEvent):void {
			logger.info("securityErrorHandler: " + event);
		}
		
		private function errorHandler(event:Event):void {
			logger.info("errorHandler: " + event);
		}
		
		private function connectStream():void {
			logger.info("connectStream");
			
			video = new Video();
			video.x = 0;
			video.opaqueBackground = 0xaaaaaa;
			video.width = 600;
			video.height = 450;
			addChild(video);
			
			text.x = video.x - 300;
			text.y = video.y + video.height;
			text.width = 200;
			text.text = "RestPlayTime:";
			addChild(text);
			
			stopText.x = text.x;
			stopText.y = text.y + text.height;
			stopText.width = 100;
			stopText.text = "Stop:";
			addChild(stopText);
			
			progress = new Progress(video.width);
			progress.x = video.x;
			progress.y = video.y + video.height;
			progress.addEventListener("_position_", onSeek, false, 0, true);
			addChild(progress);
			
			drmService.addEventListener(DRMEvent.DRM_STATE_CHANGE, onDRMComplete, false, 0, true);
			
			hasDrag = false;
			dragLoader = new DragLoader(mediaFileName + ".mp4");
			dragLoader.addEventListener(Event.COMPLETE, onDragComplete, false, 0, true);
			dragLoader.load();
			
			hasDT = false;
			dtLoader = new DtLoader(mediaFileName + ".mp4");
			dtLoader.addEventListener(Event.COMPLETE, onDtComplete, false, 0, true);
			dtLoader.load();
			
			//fileURLBase = "http://drmtest2.adobe.com/Content/Encrypted/";
			//fileURLMeta = fileURLBase + "Sample.f4v.metadata";

			fileURLMeta = Utils.DRMURL_BASE + mediaFileName + ".f4v.metadata";
			metaLoader = new DRMMetaLoader(fileURLMeta);
			metaLoader.addEventListener(Event.COMPLETE, onMetaComplete, false, 0, true);
			metaLoader.load();
			
		}
		
		private function onMetaComplete(event:Event):void
		{
			trace("start to load downloaded metadata");
			drmService.drmMetaData = metaLoader.data;
			onDRMComplete(null);
		}
		
		private function onDtComplete(event:Event):void
		{
			hasDT = true;
			onDRMComplete(null);
		}
		
		private function onDragComplete(event:Event):void
		{
			hasDrag = true;
			duration = dragLoader.videoDuration;
			onDRMComplete(null);
		}
		
		private function onDRMComplete(event:DRMEvent):void
		{
			if (hasDT && hasDrag && drmService.drmDone ) {
				
				video.height = dragLoader.videoHeight;
				video.width = dragLoader.videoWidth;
				progress.x = video.x;
				progress.y = video.y + video.height;
				progress.width = video.width;
				
				var playInfo:PlayInfo = new PlayInfo(mediaFileName, dtLoader, dragLoader);
				
				streamManager = new StreamManager(playInfo);
				streamManager.addEventListener(PlayStatusEvent.PLAY_COMPLETE, onPlayComplete);
				streamManager.addEventListener(PlayStatusEvent.PLAY_FAILED, onPlayFailed);
				streamManager.attachVideo(video);
				
				secondTimer = new Timer(1000);
				secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
				secondTimer.start();
				
				streamManager.play(0);
			}
		}
		
		private function onPlayComplete(event:PlayStatusEvent):void
		{
			text.text = "Play done.";
			streamManager.destroy();
		}
		
		private function onPlayFailed(event:PlayStatusEvent):void
		{
			text.text = "Play encoutered error.";
			streamManager.destroy();
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			if (streamManager.hasError()) {
				text.text = "Experienced Error";
				secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
				secondTimer = null;
			}else if ( hasDT && hasDrag && drmService.drmDone )
			{
				if (streamManager.isRunning) {
					logger.info("SetPosition: " + streamManager.time / duration);
					progress.setPosition(streamManager.time / duration);
					text.text = "RestPlayTime:" + streamManager.bufferLength + " S, BufferTime:" + (streamManager.time + streamManager.bufferLength);
				}else{
					//we don't do anything there.
				}
			
			}
		}
		
		private function onSeek(event:Event):void
		{
			if ( hasDT && hasDrag && drmService.drmDone)
			{
				logger.info("SeekPosition:" + progress.percent);
				streamManager.seek(progress.percent * duration);
				progress.setPosition(streamManager.time / duration);
			}
		}
		
	}
}