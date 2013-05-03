package pplive.ui
{
	import flash.display.DisplayObjectContainer;
	import flash.display.MovieClip;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	
	
	public class ToolBar
	{
		private var _movieCon:MovieClip = null;//电影的显示容器
		public var playBtn:MovieClip = null;//播放按纽
		public var stopBtn:MovieClip = null;//停止按纽
		public var pauseBtn:MovieClip = null;//暂停按纽
		public var togglePlayBtn:MovieClip = null;//大暂停
		public var prevBtn:MovieClip = null;//上一集按纽
		public var nextBtn:MovieClip = null;//下一集按纽
		public var muteEnbBtn:MovieClip = null;//静音按纽
		public var muteDisBtn:MovieClip = null;//静音按纽
		public var soundSlider:MovieClip = null;//声音条
		public var soundBg:MovieClip = null;//声音背景
		public var soundDot:MovieClip = null;//声音条按纽
		
		public var hightLightCon:MovieClip = null//亮度部分
		public var hightLightBtn:MovieClip = null//亮度按纽
		public var hlBg:MovieClip = null//亮度背景
		public var hlSlider:MovieClip = null//亮度条
		public var hlDot:MovieClip = null//亮度条
		public var hlText:TextField = null//亮度提示
		
		public var infoTxtCon:MovieClip = null;
		public var infoMask:Shape = null;
		public var movieModeBtn:MovieClip = null;//还原按纽
//		public var returnBtn:MovieClip = null;//还原按纽
		public var fullScreenBtn:MovieClip = null;//全屏按纽
		public var fullClose:MovieClip = null;//全屏还原
		public var control:MovieClip = null;
		public var ccenter:MovieClip = null;
//		public var cleft:MovieClip = null;
//		public var cright:MovieClip = null;
		public var cotBg:MovieClip = null;
		public var moVa:MovieClip = null;
		
		public var topCon:MovieClip = null;
		public var topConTxt:TextField = null;
		public var topConBg:MovieClip = null;
		
		private var _loadingBtn:MovieClip = null;
		
		private var _loadingBufferTxt:TextField = null;
		
		private var _debugCon:Sprite= null;
		public var _debugTxt:TextField = null;
		public  var _shp:Shape = null;
		
		private var _nextTipTxt:String = '';
		private var _prevTipTxt:String = '';
		
		public var anchorRect:Rectangle = null;//滑动模型
		public var anchorSoundRect:Rectangle = null;//音量拖动Key滑动模型
		public var anchorHlRect:Rectangle = null;//亮度拖动Key滑动模型
		
		public var playedTime:TextField = null;
//		public var totleTime:TextField = null;
		public var planBg:MovieClip= null;
		public var playedSlider:MovieClip = null;
		public var bufferSlider:Shape = null;
		public var playedBtn:MovieClip = null;
		public var playLine:MovieClip = null;
		public var timeBorder:MovieClip = null;
		public var BufferTimeCol:MovieClip = null;//进度条
		
//		public var nopuginTxt:MovieClip = null;//无插提示
		//20091210新增加功能 portal
//		private var _portal:Portal = null;
//		private var _ppShare:PPShare = null;
//		private var _shareClose:MovieClip = null;
//		public var portalBtn:MovieClip = null;
		//20100527新增加功能缓冲提示
		public var flushT:MovieClip = null;
		public var flushClose:MovieClip = null;
		
		
		public var canvas:DisplayObjectContainer;
		
		public var playerTip:InfoTip;
		
//		public function set portal(value:Portal):void{
//			_portal = value;
//		}
//		public function get portal():Portal{
//			return _portal;
//		}
		
		public function set prevTipTxt(value:String):void{
			_prevTipTxt = value;
		}
		public function get prevTipTxt():String{
			return _prevTipTxt;
		}
		
		public function set nextTipTxt(value:String):void{
			_nextTipTxt = value;
		}
		public function get nextTipTxt():String{
			return _nextTipTxt;
		}
		
		public function set loadingBtn(value:MovieClip):void{
			this._loadingBtn = value;
		}
		public function get loadingBtn():MovieClip{
			return _loadingBtn;
		}
		public function set loadingBufferTxt(value:TextField):void{
			this._loadingBufferTxt = value;
		}
		public function get loadingBufferTxt():TextField{
			return _loadingBufferTxt;
		}
		
		public function set movieCon(value:MovieClip):void{
			this._movieCon = value;
		}
		public function get movieCon():MovieClip{
			return this._movieCon;
		}
		public function ToolBar(ctl:Object)
		{
			init(ctl);
		}
		
		private function init(ctl:Object):void
		{
			canvas = ctl as DisplayObjectContainer;
			
			//大的框架
			control = ctl.videoContainer;
			topCon = ctl.TopCon;
			topConTxt = ctl.TopCon.TopConTxt;
			topConBg = topCon.TopConBg;
			topCon.visible = false;
			movieCon = ctl.movieCon;
			movieCon.mouseEnabled = false;
			
			//页面提示
			loadingBtn = ctl.loadingBtn;
			loadingBtn.mouseEnabled = false;
			loadingBtn.visible = false;
			loadingBtn.alpha = 0;
			_loadingBufferTxt = loadingBtn.bufferPrecent;
			
			moVa = ctl.mcStartPPVA;
			moVa.buttonMode = false;
			moVa.visible = true;
			moVa.alpha = 1;
			//暂停键
			togglePlayBtn = ctl.togglePlayMc;
			togglePlayBtn.buttonMode = true;
			togglePlayBtn.visible = false;
			
			
			//控制条背景
			cotBg = control.cotBg;
			
			
			ccenter = control.ccenter;
			
//			走马灯部分
//			infoTxtCon = ccenter.infoTxtCon;
//			infoMask = new Shape();
//			infoMask.graphics.lineStyle(0, 0x000000);
//			infoMask.graphics.beginFill(0xff0000);
//			infoMask.graphics.drawRect(0, 0, infoTxtCon.width,infoTxtCon.height);
//			infoMask.graphics.endFill();
//			ccenter.addChild(infoMask);
//			infoTxtCon.mask = infoMask;

			//控制部分注册
			flushT = ccenter.flushT;
			flushT.visible = false;
			flushClose = flushT.flushCloseMc;
			flushClose.buttonMode = true;
			pauseBtn = ccenter.pauseMc;
			playBtn = ccenter.playMc;
			stopBtn = ccenter.stopMc;
			muteEnbBtn = ccenter.muteEnbMc;
			muteDisBtn = ccenter.muteDisMc;
			prevBtn = ccenter.prevMc;
			nextBtn = ccenter.nextMc;
			
			pauseBtn.visible = true;
			playBtn.visible = false;
			//声音部分
			soundSlider = ccenter.soundSliderMc;
			soundBg = ccenter.soundBgMc;
			soundDot = ccenter.soundDotMc;
			
			soundSlider.mouseEnabled = false;
			//亮度部分
			hightLightBtn = ccenter.hightLightMc;
			hightLightCon = ccenter.hightLightConMc;
			hightLightCon.visible= false;
			
			hlSlider = hightLightCon.hlSliderMc;
			hlBg = hightLightCon.hlBgMc;
			hlDot = hightLightCon.hlDotMc;
			hlText = hightLightCon.hlText;
			hlBg.buttonMode = true;
			hlDot.buttonMode = true;
			hlSlider.mouseEnabled = false;
			
			
			//全屏部分
			fullScreenBtn = ccenter.fullScreenMc;
			movieModeBtn = ccenter.moviesMc;
//			movieModeBtn.visible = false;
			//去除
			fullClose = ctl.fullCloseMc;
			fullClose.visible = false;
//			returnBtn = cright.returnMc;
			//无插提示
//			nopuginTxt = ctl.nopuginTxt;
//			nopuginTxt.visible = false;
			
			timeBorder = control.timeBorder;
			planBg = control.timeLineSliderBgMc;
			playedSlider = control.playedSliderMc;
			
			BufferTimeCol = control.BufferTimeMc;
			BufferTimeCol.mouseEnabled = false;
			playedBtn = control.playedDb;
			playedTime = ccenter.playedTime;
//			totleTime = control.totleTime;
			//debug
			debugUtil();
			//end
			playerTip =  new InfoTip();
			canvas.stage.addChild(playerTip);
			
			anchorRect = new Rectangle(planBg.x - playedBtn.width / 4,planBg.y + playedBtn.height / 3,planBg.width - playedBtn.width / 4,0);
			anchorSoundRect = new Rectangle(soundBg.x - 2,soundDot.y,soundBg.width,0);
			anchorHlRect = new Rectangle(hlBg.x - hlDot.width / 4,hlBg.y - hlDot.height / 2,0,hlBg.height);
			//20091210新增加功能 portal
//			_portal = new Portal(ctl.Portal);
//			_ppShare = new PPShare(ctl.PPShare);
//			_portal.self.visible = false;
//			_ppShare.self.visible = false;
//			_shareClose = ctl.PPShare.shareCloseMc;
//			_shareClose.visible = false;
			
		}
		public function show():void{
//			if(!control.visible){
//				control.visible = true;
//				control.y = control.y - 40;
//			}
		}
		private function debugUtil():void{
			var debug:DebugTxt = new DebugTxt(control);
			canvas.stage.addChild(debug.text);
		}
		public function print(info:String):void{
//			_debugTxt.appendText('\n'+info);
		}
	}
}