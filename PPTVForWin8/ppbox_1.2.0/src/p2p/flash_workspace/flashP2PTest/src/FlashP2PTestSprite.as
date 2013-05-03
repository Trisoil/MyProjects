package
{
	import com.pplive.events.*;
	import com.pplive.play.PlayInfo;
	import com.pplive.play.IPlayManager;
	import com.pplive.play.P2PPlayManager;
	import com.pplive.LogTarget;
	import com.pplive.play.SegmentInfo;
	
	import flash.display.DisplayObject;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.NetStatusEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	import mx.controls.Button;
	import mx.controls.Text;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.LOGGER_FACTORY;
	import org.as3commons.logging.api.getLogger;
	import org.as3commons.logging.setup.LevelTargetSetup;
	import org.as3commons.logging.setup.LogSetupLevel;
	import org.as3commons.logging.setup.RegExpSetup;
	import org.as3commons.logging.setup.SimpleTargetSetup;
	import org.as3commons.logging.setup.target.FirebugTarget;
	import org.as3commons.logging.setup.target.TraceTarget;
	import org.as3commons.logging.util.START_TIME;
	
	public class FlashP2PTestSprite extends Sprite
	{
		private static var logger:ILogger = getLogger(FlashP2PTestSprite);
		
		private var connection:NetConnection;
		private var playManager:IPlayManager;
		private var secondTimer:Timer;
		private var progress:Progress;
		private var duration:Number;
		private var video:Video;
		private var restText:Text;
		private var dropText:Text;
		
		private var completeSegmentCount:uint = 0;
		private var dragLoader:DragLoader;
		private var dtLoader:DtLoader;
		private var hasDrag:Boolean = false;
		private var hasDt:Boolean = false;
		private var fileName:String;
		private var lastEnterFrameTime:int;
		private var fileIndex:uint = 1;
		private var isHttpOnly:Boolean = false;
		private var historyDroppedFrame:uint;
		private var lastSecond:uint;
		private var secondHistoryDroppedFrame:uint;
		
		public function FlashP2PTestSprite(rest:Text, drop:Text)
		{
			//var target:TraceTarget = new TraceTarget;
			//LOGGER_FACTORY.setup = new LevelTargetSetup(target, LogSetupLevel.INFO);
			//LOGGER_FACTORY.setup = new LevelTargetSetup(new FirebugTarget, LogSetupLevel.INFO);
//			LOGGER_FACTORY.setup = new RegExpSetup()
//				.addTargetRule(/FlashP2PTestSprite/, new TraceTarget, LogSetupLevel.INFO)
//				.addTargetRule(/PlayManager/, new TraceTarget, LogSetupLevel.INFO)
//				.addTargetRule(/TrackerConnection/, new TraceTarget, LogSetupLevel.INFO)
//				.addTargetRule(/P2PMp4Stream/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/HttpDownloader/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/P2PDownloader/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/PeerConnection/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/PeerConnector/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/IPPool/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/TcpStream/, new TraceTarget, LogSetupLevel.DEBUG)
//				.addTargetRule(/FunctionProfiler/, new TraceTarget, LogSetupLevel.DEBUG);
			
//				.addTargetRule(/PeerConnector/, new TraceTarget, LogSetupLevel.INFO)
//				.addTargetRule(/DownloadDriver/, new TraceTarget, LogSetupLevel.INFO);
			
			this.addEventListener(Event.ADDED_TO_STAGE, addToStage);
			
			restText = rest;
			dropText = drop;
			
			LogTarget.container = null;
			
			connection = new NetConnection();
			connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection.connect(null);
		}
		
		private function addToStage(event:Event):void
		{
			logger.info("addToStage");
			this.addEventListener(Event.ENTER_FRAME, onEnterFrame);
		}
		
		private function onEnterFrame(event:Event):void
		{
			var now:int = getTimer();
			if (playManager)
			{
				var frameDroppedFrame:uint = playManager.droppedFrame - historyDroppedFrame;
				logger.debug("onEnterFrame " + now + "ms, interval:" + (now - lastEnterFrameTime) + 
					", historyDroppedFrame:" + historyDroppedFrame + 
					((frameDroppedFrame != 0) ? (", frameDroppedFrame:" +  frameDroppedFrame) : ""));
				historyDroppedFrame = playManager.droppedFrame;
				
				var second:uint = uint(now / 1000);
				if (lastSecond == 0)
				{
					lastSecond = second;
				}
				else if (lastSecond != second)
				{
					lastSecond = second;
					if (playManager.droppedFrame != secondHistoryDroppedFrame)
					{
						logger.info("secondDroppedFrame:" + (playManager.droppedFrame - secondHistoryDroppedFrame));
						secondHistoryDroppedFrame = playManager.droppedFrame;
					}
				}
			}
			
			lastEnterFrameTime = now;
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
			video.x = video.x - 640;
			video.opaqueBackground = 0xaaaaaa;
			video.width = 600;
			video.height = 450;
			video.smoothing = true;
			addChild(video);
			
			logger.info("video x:" + video.x + ", y:" + video.y);
			
			restText.text = "RestPlayTime:";
			dropText.text = "DroppedFrame:";
		
			progress = new Progress(video.width);
			progress.x = video.x;
			progress.y = video.y + video.height;
			progress.addEventListener("_position_", onSeek, false, 0, true);
			addChild(progress);

			play();
		}
		
		private function play():void
		{			
			fileName = getFileName();
			hasDrag = false;
			hasDt = false;
			dragLoader = new DragLoader(fileName);
			dragLoader.addEventListener(Event.COMPLETE, onDragComplete, false, 0, true);
			dragLoader.load();
			
			dtLoader = new DtLoader(fileName);
			dtLoader.addEventListener(Event.COMPLETE, onDtComplete, false, 0, true);
			dtLoader.load();
		}
		
		private function getFileName():String
		{
			return 'cdbb4abd505bda7bffaa09d062b8f0de.mp4';
			
			//fileName = "%CC%EC%D3%EB%B5%D8%B9%FA%D3%EF%B0%E6%28%B5%DA01%BC%AF%29.mp4";
			//热片，最佳拍档
			//return "%B9%C5%BB%F3%D7%D02.mp4";
			
			//热片
			//return "%C4%C7%D0%A9%C4%EA%CE%D2%C3%C7%D2%BB%C6%F0%D7%B7%B5%C4%C5%AE%BA%A2%28%C0%B6%B9%E2%29.mp4";
			//热片倾城之泪
			//return "%5B400k%5D%C7%E3%B3%C7%D6%AE%C0%E1%28%B8%DF%C7%E5%29.mp4";
			//return "015d373ea06d919217a715c2b7f03ac4.mp4";
			
			// 三国
			return"%D0%C2%C8%FD%B9%FA%28%B5%DA01%BC%AF%29.mp4";
			
			//台北飘雪
			//return "885681dc47fa668e8c7718d7cfa004a1.mp4";
			
			//return "__C3__CD__C4__D0__B9__F6__CB__C0__B6__D3__28__B8__DF__C7__E5__29.mp4";
			
			//全球娱乐通-第411期-《魔镜魔镜》走华丽喜感路线
			//return "__C8__AB__C7__F2__D3__E9__C0__D6__CD__A8-__B5__DA411__C6__DA-__A1__B6__C4__A7__BE__B5__C4__A7__BE__B5__A1__B7__D7__DF__BB__AA__C0__F6__CF__B2__B8__D0__C2__B7__CF__DF.mp4"		
			//fileName = "%CC%C6%B9%AC%C3%C0%C8%CB%CC%EC%CF%C2%28%B5%DA12%BC%AF%29.mp4";
			
			//return "%B9%C5%BB%F3%D7%D0" + fileIndex + ".mp4";
			//return "%CE%C2%E1%B0-DISCO.mp4";
			//return "%B0%C2%CC%D8%C2%FC%C1%D0%B4%AB%28%B5%DA01%BC%AF%29.mp4";
			//return "%28%B2%C6%BE%AD%29%BF%AA%D7%C5%B3%A8%C5%F1%CD%E6%C9%A8%D1%A9%B0%A2%CB%B9%B6%D9%C2%ED%B6%A1%B6%AC%BC%BE%BC%DD%CA%BB%CC%E5%D1%E9-1%D4%C24%C8%D5.mp4";
			//西游记
			//return "%5B650k%5D%D5%C5%BC%CD%D6%D0%B0%E6%CE%F7%D3%CE%BC%C7%28%B5%DA01%BC%AF%29.mp4";
			//法证先锋
			//return "%B7%A8%D6%A4%CF%C8%B7%E63%B9%FA%D3%EF%B0%E6%28%B5%DA27%BC%AF%29.mp4";
			//热片 凰图腾
			//return "%bb%cb%cd%bc%cc%da%28%b5%da01%bc%af%29.mp4";
		}
		
		private function onDragComplete(event:Event):void
		{
			logger.info("onDragComplete"); 
			hasDrag = true;
			video.height = dragLoader.videoHeight;
			video.width = dragLoader.videoWidth + 2;
			video.x = - (video.width / 2);			
			progress.x = video.x;			
			progress.y = video.y + video.height;
			progress.width = video.width;
			onReady();
		}
		
		private function onDtComplete(event:Event):void
		{
			logger.info("onDtComplete");
			hasDt = true;
			onReady();
		}
		
		private function onReady():void
		{
			if (hasDrag && hasDt)
			{
				var backupHosts:Vector.<String> = new Vector.<String>;
				backupHosts.push("ccf.pptv.com");
				var playInfo:PlayInfo;
				if (isHttpOnly)
				{
					playInfo = new PlayInfo(dtLoader.host, fileName, dtLoader.key, 2, dragLoader.segments, backupHosts); 
				}
				else
				{
					for (var i:uint = 0; i < dragLoader.segments.length; ++i)
					{
						dragLoader.segments[i].rid = null;
					}
										
					playInfo = new PlayInfo(dtLoader.host, fileName, dtLoader.key, 0, dragLoader.segments, backupHosts);
			
					
				}
				
				
				duration = playInfo.duration;
				playManager = new P2PPlayManager(playInfo);
				playManager.addEventListener(PlayStatusEvent.PLAY_START, onPlayStatus, false, 0, true);
				playManager.addEventListener(PlayStatusEvent.PLAY_FAILED, onPlayStatus, false, 0, true);
				playManager.addEventListener(PlayStatusEvent.PLAY_COMPLETE, onPlayStatus, false, 0, true);
				playManager.addEventListener(PlayStatusEvent.BUFFER_FULL, onPlayStatus, false, 0, true);
				playManager.addEventListener(PlayStatusEvent.BUFFER_EMPTY, onPlayStatus, false, 0, true);
				playManager.addEventListener(DacLogEvent.P2P_DAC_LOG, onDacLog, false, 0, true);
				playManager.addEventListener(PlayProgressEvent.PLAY_PROGRESS, onPlayProgress, false, 0, true);
				playManager.addEventListener(PlayResultEvent.PLAY_RESULT, onPlayResult, false, 0, true);
				playManager.attachVideo(video);
				playManager.play();
			}
		}
		
		public function playNextFile():void
		{
			fileIndex++;
			reset();
		}
		
		private function onPlayStatus(event:PlayStatusEvent):void
		{
			logger.info("onPlaySatus " + event + ", info:" + event.info);	
		}
		
		private function onDacLog(event:DacLogEvent):void
		{
			logger.info("onDacLog" + event);
		}
		
		private function onPlayProgress(event:PlayProgressEvent):void
		{
			logger.info("SetPosition: " + event.timeLoaded / duration);
			progress.setPosition(event.timeLoaded / duration);
			dropText.text = "DroppedFrame:" + playManager.droppedFrame;
			restText.text = "RestPlayTime:" + event.bufferLength + " S, BufferTime:" 
				+ (event.timeLoaded + event.bufferLength);
		}
		
		private function onPlayResult(event:PlayResultEvent):void
		{
			logger.info("onPlayResult:"+" m:"+event.m + " url:"+event.url + " interval:"+event.interval);
		}
		
		private function onSeek(event:Event):void
		{
			if (playManager != null)
			{
				logger.info("SeekPosition:" + progress.percent);
				playManager.seek(progress.percent * duration);	
			}
		}
		
		public function toggleHttpOnly():void
		{
			isHttpOnly = !isHttpOnly;
		}
		
		public function reset():void
		{
			if (playManager)
			{
				playManager.removeEventListener(PlayStatusEvent.PLAY_START, onPlayStatus);
				playManager.removeEventListener(PlayStatusEvent.PLAY_FAILED, onPlayStatus);
				playManager.removeEventListener(PlayStatusEvent.PLAY_COMPLETE, onPlayStatus);
				playManager.removeEventListener(PlayStatusEvent.BUFFER_FULL, onPlayStatus);
				playManager.removeEventListener(PlayStatusEvent.BUFFER_EMPTY, onPlayStatus);
				playManager.removeEventListener(DacLogEvent.P2P_DAC_LOG, onDacLog);
				playManager.removeEventListener(PlayProgressEvent.PLAY_PROGRESS, onPlayProgress);
				playManager.removeEventListener(PlayResultEvent.PLAY_RESULT, onPlayResult);
				playManager.destroy();
				playManager = null;
			}
			
			play();
		}
	}
}