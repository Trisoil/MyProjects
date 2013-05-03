package {
	import com.pplive.play.DacLogEvent;
	import com.pplive.play.PlayInfo;
	import com.pplive.play.PlayManager;
	import com.pplive.play.PlayStatusEvent;
	import com.pplive.util.GUID;
	import com.pplive.util.StringConvert;
	
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.EventPhase;
	import flash.events.IOErrorEvent;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.external.ExternalInterface;
	import flash.media.Video;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.system.Security;
	import flash.ui.ContextMenu;
	import flash.ui.ContextMenuItem;
	import flash.ui.Keyboard;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	import mx.utils.ObjectProxy;
	import mx.utils.ObjectUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.LOGGER_FACTORY;
	import org.as3commons.logging.api.getLogger;
	import org.as3commons.logging.setup.LevelTargetSetup;
	import org.as3commons.logging.setup.LogSetupLevel;
	import org.as3commons.logging.setup.RegExpSetup;
	import org.as3commons.logging.setup.SimpleTargetSetup;
	import org.as3commons.logging.setup.target.FirebugTarget;
	import org.as3commons.logging.setup.target.TraceTarget;
	
	import pplive.Utils.ExternalPacked;
	import pplive.Utils.Guid;
	import pplive.Utils.UserData;
	import pplive.Utils.Utils;
	import pplive.skin.view.components.IkanSkin;
	import pplive.skin.view.events.SkinEvent;
	
	public class OpenIkan extends Sprite
	{	
		private static var logger:ILogger = getLogger(OpenIkan);
		private static const VERSION:String = '1.1.2.1.1129';
		private static var dacStoreKey:String = "p2p_static";
		
		private var userData:UserData = new UserData("_ikan_p2p_");		

		// flash vars
		private var structureXmlUrl:String;
		private var fileName:String;
		private var decodedFileName:String;
		private var playerVersion:String;
		
		private var newSkin:IkanSkin;
		private var playManager:PlayManager;
		private var video:Video;
		private var skinVideoWidth:uint = 0;
		private var skinVideoHeight:uint = 0;
		private var secondTimer:Timer;
		private var playInfo:PlayInfo;
		
		private var dtLoader:DtLoader;
		private var dragLoader:DragLoader;
		private var hasDtLoaded:Boolean = false;
		private var hasDragLoaded:Boolean = false;
		
		// dac variables
		private var dacLogger:DacLogHelper;
		private var startTime:int;
		private var startDelayTime:uint = 0;
		private var startPlayTime:int = 0;
		private var bufferCount:uint = 0;
		private var bufferStartTime:int = 0;
		private var bufferTime:uint = 0;
		private var seekCount:uint = 0;
		private var hasReportPlayFailed:Boolean = false;

		public function OpenIkan()
		{
			initLog();
			Security.allowDomain('*');
			newSkin = new IkanSkin();
			initCanvas();
			startTime = getTimer();
			dacLogger = new DacLogHelper(getGuid());
			sendPlayDacLog();
			
			if (parseFlashVars())
			{
				var structureLoader:URLLoader = new URLLoader();
				structureLoader.addEventListener(Event.COMPLETE, skinLoadCompleteHandler);
				structureLoader.addEventListener(IOErrorEvent.IO_ERROR, function(evt:IOErrorEvent):void { 
					logger.error(evt);
				} );
				
				try{
					structureLoader.load(new URLRequest(structureXmlUrl));
				} catch (e:Error){
					logger.error(e);
				}
				
				secondTimer = new Timer(1000);
				secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
				secondTimer.start();
			}
		}
		
		CONFIG::debug
		private function initLog():void
		{
			trace("debug");
			LOGGER_FACTORY.setup = new LevelTargetSetup(new TraceTarget, LogSetupLevel.INFO);
		}
		
		CONFIG::release
		private function initLog():void {trace("release");}
		
		private function parseFlashVars():Boolean
		{
			var paramObject:Object = LoaderInfo(this.root.loaderInfo).parameters;
			if (paramObject == null)
				return false;
			
			logger.info(mx.utils.ObjectUtil.toString(paramObject));
			structureXmlUrl = paramObject.xmlurl;
			logger.info("xmlUrl:" + structureXmlUrl);
			return true;
		}
		
		private function initCanvas():void
		{
			try
			{
				var this_menu:ContextMenu = new ContextMenu();
				var this_item:ContextMenuItem = new ContextMenuItem('iKan Player v' + VERSION,true);
				//			this_item.enabled = false;
				this_menu.customItems.push(this_item);
				this_menu.hideBuiltInItems();
				this.contextMenu = this_menu;
				
				this.stage.showDefaultContextMenu = false;
				this.stage.scaleMode = StageScaleMode.NO_SCALE;
				this.stage.align = StageAlign.TOP_LEFT;
				this.stage.doubleClickEnabled = true;
			} catch (e:Error){
				logger.error(e);
			}
		}
		
		private function loadPlayInfo():void
		{
			dtLoader = new DtLoader(fileName);
			dtLoader.addEventListener(Event.COMPLETE, onDtComplete, false, 0, true);
			dtLoader.addEventListener(PlayInfoLoadFailedEvent.PLAYINFO_LOAD_ERROR, onPlayInfoError, false, 0, true);
			dtLoader.load();
			
			dragLoader = new DragLoader(fileName);
			dragLoader.addEventListener(Event.COMPLETE, onDragComplete, false, 0, true);
			dragLoader.addEventListener(PlayInfoLoadFailedEvent.PLAYINFO_LOAD_ERROR, onPlayInfoError, false, 0, true);
			dragLoader.load();
		}
		
		private function skinLoadCompleteHandler(e:Event):void 
		{
			newSkin.containWidth = stage.stageWidth;
			newSkin.containHeight = stage.stageHeight;
			addChild(newSkin);
			newSkin.playerXscale = 1;
			newSkin.playerYscale = 1;
			
			var structureXml:XML = new XML(e.target.data);
			// delete handyUI config
			delete structureXml.handyUI;
			logger.info(structureXml);
			newSkin.setData(structureXml);
			newSkin.addEventListener(SkinEvent.LAYOUT_SUCCESS,newSkinEventHandler);
			newSkin.addEventListener(SkinEvent.LAYOUT_FAILED,newSkinEventHandler);
			newSkin.addEventListener(SkinEvent.MEDIA_RESIZE,newSkinEventHandler);
			newSkin.addEventListener(SkinEvent.MEDIA_PAUSE,newSkinEventHandler);
			newSkin.addEventListener(SkinEvent.MEDIA_PLAY,newSkinEventHandler);
			newSkin.addEventListener(SkinEvent.MEDIA_SOUND,newSkinEventHandler);
			newSkin.addEventListener(SkinEvent.MEDIA_VOD_POSITION,newSkinEventHandler);
			
			this.stage.addEventListener(KeyboardEvent.KEY_UP, videoKeyboardHandler);
			
			registerInterface();
			ExternalPacked.call('IkanPlayer.ready');
		}
		
		private function newSkinEventHandler(e:SkinEvent):void 
		{
			switch(e.type){
			case SkinEvent.LAYOUT_SUCCESS :
				logger.info('LAYOUT_SUCCESS');
				break;
			case SkinEvent.LAYOUT_FAILED :
				logger.info('LAYOUT_FAILED');
				break;
			case SkinEvent.MEDIA_RESIZE:
				skinVideoWidth = e.currObj.w;
				skinVideoHeight = e.currObj.h;
				logger.info('LAYOUT_RESIZE w:' + skinVideoWidth + ", h:" + skinVideoHeight);
				resize();
				break;
			case SkinEvent.MEDIA_PAUSE:
				logger.info("MEDIA_PAUSE");
				if (playManager != null)
					playManager.pause();
				break;
			case SkinEvent.MEDIA_PLAY:
				logger.info("MEDIA_PLAY");
				if (playManager != null)
					playManager.resume();
				break;
			case SkinEvent.MEDIA_SOUND:
				logger.info("MEDIA_SOUND volume:" + e.currObj.value);
				if (playManager != null)
					playManager.volume = e.currObj.value / 100;
				break;
			case SkinEvent.MEDIA_VOD_POSITION:
				logger.info("MEDIA_VOD_POSITION position:" + e.currObj.value);
				if (playManager != null)
				{
					++seekCount;
					playManager.seek(e.currObj.value * playManager.duration);
				}
				break;
			}
		}
		
		private function videoKeyboardHandler(event:KeyboardEvent):void
		{
			if (playManager == null)
				return;
			
			switch (event.keyCode) {
				case Keyboard.LEFT:
					var newPlayTime:int = playManager.time - 15;
					if (newPlayTime < 0)
						newPlayTime = 0;
					logger.info('后退-->'+ newPlayTime);
					playManager.seek(newPlayTime);
					break;
				case Keyboard.RIGHT:
					newPlayTime = playManager.time + 15;
					if (newPlayTime > playManager.duration)
						newPlayTime = playManager.duration;
					logger.info('前进-->'+ newPlayTime);
					playManager.seek(newPlayTime);
					break;
				case Keyboard.UP:
					var volume:uint = playManager.volume * 100; 
					volume = (volume + 5) <= 100 ? (volume + 5) : 100;
					logger.info("set volume:" + volume);
					newSkin.setSound(volume);
					break;
				case Keyboard.DOWN:
					volume = playManager.volume * 100; 
					volume = (volume - 5) >= 0 ? (volume - 5) : 0;
					logger.info("set volume:" + volume);
					newSkin.setSound(volume);
					break;
			}	
		}
		
		private function registerInterface():void
		{
			logger.info("registerInterface");
			
			ExternalPacked.add('addList',addList);
			ExternalPacked.add('startPlay',startPlay);
			ExternalPacked.add('switchMode', movieMode);
			ExternalPacked.add('setSize',videoSize);
			ExternalPacked.add('toggleState',togglevideo);
			ExternalPacked.add('setSize',videoResize);
			ExternalPacked.add('videoStop',mediaEnd);
			ExternalPacked.add('hasPlugin',hasPlugin);
			ExternalPacked.add('timeloaded',getTimeLoaded);
		}
		
		private function onPlayInfoError(event:PlayInfoLoadFailedEvent):void
		{
			logger.error("onPlayInfoError:" + event);
			reportPlayFailed(event.reason);
		}
		
		private function onDtComplete(event:Event):void
		{
			logger.info("onDtComplete:" + event);
			hasDtLoaded = true;
			
			var logObject:Object = new Object;
			logObject.dt = "dtt";
			logObject.dtt = dtLoader.requestDuration;
			logObject.n = decodedFileName;
			logObject.s = 1;
			logObject.v = VERSION;
			dacLogger.sendDacLog(logObject);
			
			createVideo();
		}
		
		private function onDragComplete(event:Event):void
		{
			logger.info("onDragComplete:" + event);
			hasDragLoaded = true;
			
			var logObject:Object = new Object;
			logObject.dt = "dgt";
			logObject.dgt = dragLoader.requestDuration;
			logObject.n = decodedFileName;
			logObject.s = 1;
			logObject.v = VERSION;
			dacLogger.sendDacLog(logObject);
			
			createVideo();
		}
		
		private function createVideo():void
		{
			if (video == null && skinVideoWidth != 0 && hasDtLoaded && hasDragLoaded)
			{
				logger.info("createVideo");
				video = new Video;
				video.smoothing = true;
				calcVideoShape();
				newSkin.movieCon.addChild(video);
				newSkin.movieCon.addEventListener(MouseEvent.CLICK, onStageClickHandler, false, 0, true);
				
				playInfo = new PlayInfo(dtLoader.host, fileName, dtLoader.key, dtLoader.bwType, dragLoader.segments, null);
				newSkin.totalDuration = playInfo.duration;
				
				playManager = new PlayManager(playInfo);
				playManager.addEventListener(PlayStatusEvent.BUFFER_EMPTY, onPlayBufferEmpty, false, 0, true);
				playManager.addEventListener(PlayStatusEvent.BUFFER_FULL, onPlayBufferFull, false, 0, true);
				playManager.addEventListener(DacLogEvent.STOP_DAC_LOG, onStopDacLog, false, 0, true);
				playManager.attachVideo(video);
				playManager.play();
				
				startPlayTime = getTimer();
			}
		}
		
		private function onStageClickHandler(evt:MouseEvent):void
		{
			logger.info("onStageClickHandler");
			newSkin.onStageClick();	
		}
		
		private function resize():void
		{
			if (video != null)
			{
				calcVideoShape();
			}
			else
			{
				createVideo();
			}
		}
		
		private function calcVideoShape():void
		{
			var skinScale:Number = skinVideoWidth / skinVideoHeight;
			var realScale:Number = dragLoader.videoWidth / dragLoader.videoHeight;
			if(skinScale > realScale)
			{
				video.height = skinVideoHeight;
				video.width = realScale * video.height;
			}
			else
			{
				video.width = skinVideoWidth;
				video.height = video.width / realScale;
			}
			
			video.x = (skinVideoWidth - video.width) / 2;
			video.y = (skinVideoHeight - video.height) / 2;
			logger.info("skinVideoWidth:" + skinVideoWidth + ", skinVideoHeight:" + skinVideoHeight
						+ "\nrealVideoWidth:" + dragLoader.videoWidth + ", realVideoHeight:" + dragLoader.videoHeight
						+ "\nvideo.width:" + video.width + ", video.height:" + video.height);
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			if (playManager != null)
			{
				newSkin.showTime(playManager.time);
				newSkin.showBuffer((playManager.time + playManager.bufferLength) / playManager.duration);
				logger.info("time:" +playManager.time + ", buffer:" + (playManager.time + playManager.bufferLength));
				
				if (playManager.time + playManager.bufferLength == 0 && (getTimer() - startPlayTime) > 15000)
				{
					reportPlayFailed(PlayFailedReason.REQUEST_DATA_FAILED);
				}
			}
			
			if (secondTimer.currentCount % 5 == 0)
			{
				recordPlayDacLog();
			}
		}
		
		private function recordPlayDacLog():void
		{
			if (startDelayTime != 0)
			{
				// record dac log after movie start.
				var storeObject:Object = new Object;
				storeObject.bf = bufferCount;
				storeObject.bs = bufferTime;
				storeObject.dg = seekCount;
				storeObject.vt = getTimer() - startTime;
				storeObject.du = playInfo == null ? 0 : playInfo.duration;
				storeObject.v = VERSION;
				storeObject.h = playInfo == null ? "" : playInfo.host;
				storeObject.n = decodedFileName == null ? "" : decodedFileName;
				
				logger.info("recordPlayDacLog:" + mx.utils.ObjectUtil.toString(storeObject));
				userData.setData(dacStoreKey, storeObject);		
				userData.flush();
			}	
		}
		
		private function sendPlayDacLog():void
		{
			var storeObject:Object = userData.getData(dacStoreKey);
			if (storeObject != null)
			{
				storeObject.dt = "bfr";
				storeObject.rp = 0;
				storeObject.bit = 0;
				storeObject.cs = 0;
				storeObject.sr = 0;
				storeObject.np = 0;
				storeObject.s = 1;
				storeObject.v = VERSION;
				dacLogger.sendDacLog(storeObject);
				logger.info("sendPlayDacLog:" + mx.utils.ObjectUtil.toString(storeObject));
				// clear
				userData.setData(dacStoreKey, null);
				userData.flush();
			}
			else
			{
				logger.info("sendPlayDacLog:" + mx.utils.ObjectUtil.toString(storeObject));
			}
		}
		
		private function reportPlayFailed(reason:uint):void
		{
			if (!hasReportPlayFailed)
			{
				hasReportPlayFailed = true;
				var logObject:Object = new Object;
				logObject.dt = "err";
				logObject.er = reason;
				logObject.n = decodedFileName;
				logObject.s = 1;
				logObject.v = playerVersion;
				logObject.o = 0;
				logObject.h = dtLoader.host;
				logObject.v = VERSION;
				dacLogger.sendDacLog(logObject);	
			}
		}
		
		private function onPlayBufferFull(event:PlayStatusEvent):void
		{
			if (startDelayTime == 0)
			{
				startDelayTime = getTimer() - startTime;
				
				var logObject:Object = new Object;
				logObject.dt = "sd";
				logObject.tds = startDelayTime;
				logObject.n = decodedFileName;
				logObject.m = 6;
				logObject.s = 1;
				logObject.v = playerVersion;
				logObject.o = 0;
				logObject.h = dtLoader.host;
				logObject.v = VERSION;
				dacLogger.sendDacLog(logObject);
			}
			
			if (bufferStartTime != 0)
			{
				bufferTime += getTimer() - bufferStartTime;
			}
		}
		
		private function onPlayBufferEmpty(event:PlayStatusEvent):void
		{
			if (startDelayTime != 0)
			{
				// record buffer count after play started.
				++bufferCount;
				bufferStartTime = getTimer();
			}
		}
		
		private function onStopDacLog(event:DacLogEvent):void
		{
			event.logObject.v = VERSION;
			dacLogger.sendDacLog(event.logObject);
		}
		
		private function getGuid():String
		{
			var guid:Object = userData.getData("guid");
			if(!guid)
			{
				guid = Guid.create();
				logger.error("need create guid:" + guid);
				userData.setData("guid", guid);
				userData.flush();
			}
			else
			{
				logger.info("guid exist!");
			}
			
			return guid.toString();
		}
		
		private function addList(obj:Object = null):void
		{
			logger.info("addList:" + mx.utils.ObjectUtil.toString(obj));
			var playLink:String;
			var item:Object = obj.items[obj.cur];
			if (item.hasOwnProperty("ikanpl"))
			{
				playLink = item.ikanpl;
			}
			else
			{
				playLink = item.pl;
			}

			logger.info("pl(before decode):" + playLink);
			logger.info("pl(after decode):" + Utils.decodeBase64(playLink));
			playLink = Utils.decodeBase64(playLink);
			fileName = playLink.split("||")[1];
			fileName = fileName.split("@@@")[0];
			decodedFileName = StringConvert.urldecodeGB2312(fileName);
			logger.info("fileName:" + fileName + ", decodedFileName:" + decodedFileName);
			loadPlayInfo();
		}
		
		private function startPlay(index:int):void
		{
			logger.info("startPlay: " + index);
		}
		
		private function movieMode(external:Boolean = true):void
		{
			logger.info("movieMode: " + mx.utils.ObjectUtil.toString(external));
		}
		
		private function videoSize():String
		{
			logger.info("videoSize: ");
			return "";
		}
		
		private function togglevideo():void
		{
			logger.info("togglevideo: ");
		}
		
		private function videoResize(w:Number = 0,h:Number = 0):void
		{
			logger.info("videoResize: " + w + "/" + h);
		}
		
		private function mediaEnd():void
		{
			logger.info("mediaEnd: ");
		}
		
		private function hasPlugin():void
		{
			logger.info("hasPlugin: ");
		}
		
		private function getTimeLoaded():Number
		{
			logger.info("getTimeLoaded: ");
			return 0;
		}
	}
}