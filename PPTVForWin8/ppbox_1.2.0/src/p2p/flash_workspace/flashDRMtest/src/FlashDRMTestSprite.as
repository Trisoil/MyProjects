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
	
	public class FlashDRMTestSprite extends Sprite
	{
		private static var logger:ILogger = getLogger(FlashDRMTestSprite);
		
		private var connection:NetConnection;
		private var secondTimer:Timer;
		private var progress:Progress;
		private var duration:Number;
		private var video:Video;
		private var text:TextField = new TextField;
		private var stopText:TextField = new TextField;
		private var completeSegmentCount:uint = 0;
		private var fileURLBase:String;
		private var fileURLMeta:String;
		private var fileURL:String;
		private var stream:NetStream;
		private var streamManager:StreamManager;
		private var dragLoader:DragLoader;
		private var hasDrag:Boolean;
		
		private var metaLoader:DRMMetaLoader;
		private var drmService:DRMService;

		
		public function FlashDRMTestSprite()
		{
			//LOGGER_FACTORY.setup = new SimpleTargetSetup(new TraceTarget);
			LOGGER_FACTORY.setup = new LevelTargetSetup(new TraceTarget, LogSetupLevel.INFO);
			//LOGGER_FACTORY.setup = new LevelTargetSetup(new FirebugTarget, LogSetupLevel.INFO);
			/*LOGGER_FACTORY.setup = new RegExpSetup()
				.addTargetRule(/FlashDRMTestSprite/, new TraceTarget, LogSetupLevel.INFO)
				.addTargetRule(/P2PMp4Stream/, new TraceTarget, LogSetupLevel.DEBUG)
				.addTargetRule(/P2PDownloader/, new TraceTarget, LogSetupLevel.DEBUG)
				.addTargetRule(/PeerConnector/, new TraceTarget, LogSetupLevel.INFO)
				.addTargetRule(/PeerConnection/, new TraceTarget, LogSetupLevel.INFO)
				.addTargetRule(/DownloadDriver/, new TraceTarget, LogSetupLevel.INFO);
			*/
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
			dragLoader = new DragLoader("anonymous.play");
			dragLoader.addEventListener(Event.COMPLETE, onDragComplete, false, 0, true);
			dragLoader.load();
			
			fileURLBase = "Content/anonymous.f4v";
			fileURLMeta = fileURLBase + ".metadata";
			metaLoader = new DRMMetaLoader(fileURLMeta);
			metaLoader.addEventListener(Event.COMPLETE, onMetaComplete, false, 0, true);
			metaLoader.load();
			
		}
		
		private function onMetaComplete(event:Event):void
		{
			trace("start to load downloaded metadata");
			drmService.drmMetaData = metaLoader.data;
		}

		private function onDragComplete(event:Event):void
		{
			hasDrag = true;
			video.height = dragLoader.videoHeight;
			video.width = dragLoader.videoWidth;
			progress.x = video.x;
			progress.y = video.y + video.height;
			progress.width = video.width;
			
			onDRMComplete(null);
		}
		
		private function onDRMComplete(event:DRMEvent):void
		{
			//video.height = dragLoader.videoHeight;
			//video.width = dragLoader.videoWidth;
			//progress.x = video.x;
			//progress.y = video.y + video.height;
			//progress.width = video.width;
			if (hasDrag && drmService.drmDone ) {
				secondTimer = new Timer(1000);
				secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
				secondTimer.start();
				onReady();
			}
		}
		
		private function onReady():void
		{

			var playInfo:PlayInfo = new PlayInfo("http://59.53.73.62/", "anonymous.f4v", dragLoader.segments);
			duration = playInfo.duration;
			streamManager = new StreamManager(playInfo);
			//streamManager.addEventListener(Event.STOP_DAC_LOG, onStopDacLog, false, 0, true);
			streamManager.attachVideo(video);
			streamManager.play(0);
			/*
			stream = new NetStream(connection); 
			stream.addEventListener(DRMStatusEvent.DRM_STATUS, drmStatusHandler); 
			stream.addEventListener(DRMErrorEvent.DRM_ERROR, drmErrorHandler); 
			stream.addEventListener(NetStatusEvent.NET_STATUS, netstreamStatusHandler); 
			stream.client = new CustomClient(); 
			video.attachNetStream(stream); 
			fileURL = Utils.DRMURL_BASE + fileURLBase;
			stream.play(fileURL); 
			*/
		}
		
		public function drmStatusHandler(event:DRMStatusEvent):void
		{
			trace("netsteam drmStatusHandler");
		}

		public function drmErrorHandler(event:DRMErrorEvent):void
		{
			trace("netsteam drmErrorHandler");
		}
		
		private function netstreamStatusHandler(event:NetStatusEvent):void
		{
			trace("netstream netstreamStatusHandle");
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			if (streamManager.hasError()) {
				text.text = "Experienced Error";
				secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
				secondTimer = null;
			}else if ( drmService.drmDone )
			{
				logger.info("SetPosition: " + streamManager.time / duration);
				progress.setPosition(streamManager.time / duration);
				text.text = "RestPlayTime:" + streamManager.bufferLength + " S, BufferTime:" + (streamManager.time + streamManager.bufferLength);
			}
		}
		
		private function onSeek(event:Event):void
		{
			if ( drmService.drmDone)
			{
				logger.info("SeekPosition:" + progress.percent);
				streamManager.seek(progress.percent * duration);	
			}
		}

	}
}