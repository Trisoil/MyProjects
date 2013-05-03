package pplive.skin.view.components {
	
	import com.greensock.TweenLite;
	
	import flash.display.BitmapData;
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.display.StageDisplayState;
	import flash.events.Event;
	import flash.events.FullScreenEvent;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.geom.Matrix;
	import flash.geom.Rectangle;
	import flash.ui.Mouse;
	import flash.utils.Timer;
	import flash.utils.getDefinitionByName;
	
	import pplive.Utils.Common;
	import pplive.skin.view.events.SkinEvent;
	import pplive.skin.view.ui.ArrowUI;
	import pplive.skin.view.ui.PlayerTipUI;
	import pplive.skin.view.ui.SharePanelUI;
	import pplive.skin.view.utils.EffectMove;
	import pplive.skin.view.utils.JEventDelegate;
	import pplive.skin.view.utils.UtilsMethod;
	import pplive.ui.InfoTip;
		
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class BaseSkin extends MovieClip {
		protected var _controlMc:MovieClip;
		protected var _contrlBgMc:ControlBgMc;
		protected var _playpauseMc:MovieClip;
		protected var _stopMc:SimpleButton;
		protected var _prevMc:SimpleButton;
		protected var _nextMc:SimpleButton;
		protected var _brightnessMc:SimpleButton;
		protected var _muteMc:MovieClip;
		protected var _unmuteMc:MovieClip;
		protected var _soundMc:MovieClip;
		protected var _movieMc:SimpleButton;//影院模式
		protected var _fullscreenMc:SimpleButton;
		protected var _topMc:MovieClip;
		protected var _bigPlayMc:SimpleButton;
		protected var _startFreshMc:MovieClip;
		protected var _loadingMc:MovieClip;
		protected var _fullCloseMc:MovieClip;
		protected var _setStreamMc:MovieClip;
		protected var _logoMc:MovieClip;
		protected var _errorMc:MovieClip;
		protected var _portalMc:MovieClip;
		protected var _timeMc:MovieClip;
		protected var _recommendMc:MovieClip;
		//新UI
		protected var _handyShareMc:SharePanelUI;
		protected var _leftMc:Sprite;
		protected var _rightMc:Sprite;
		protected var _leftArr:Array = [];
		protected var _rightArr:Array = [];
		protected var _conArr:Array = [];
		protected var _uiArr:Array = [];
		protected var _objArr:Array = [];
		protected var _elementNum:Number = 0;
		protected var _mode:String = "normal";
		protected var _containWidth:Number;
		protected var _containHeight:Number;
		protected var _showWidth:Number = 0;
		protected var _showHeight:Number = 0;
		protected var _islayout:Boolean = false;
		protected var _isEPG:Boolean = true;
		protected var _effectArr:Array;
		protected var delaytime:Timer;
		protected var _handyArr:Array = [];
		protected var _playerTip:MovieClip;
		
		//
		protected var _lightMc:MovieClip;
		protected var _shareMc:SimpleButton;
		protected var _broadcastMc:SimpleButton;
		protected var _adjustMc:MovieClip;
		
		protected var _arrowMc:ArrowUI;
		protected var _handyBarMc:MovieClip;
		protected var _handyMc:MovieClip;
		protected var _playerXscale:Number;
		protected var _playerYscale:Number;
		protected var _width_:Number;
		protected var _height_:Number;

		
		public function BaseSkin() {
			super();
		}
		public function setData(list:XML):void{
			
		}
		
		protected function onEnterFrameHandler(e:Event):void {
			if (_objArr.length == _elementNum) {
				this.removeEventListener(Event.ENTER_FRAME, onEnterFrameHandler);
				for (var i:int = 0; i < _elementNum; i++) {
					if (_objArr[i]["name"] == "playpauseUI") {
						_playpauseMc = _objArr[i]["mc"] as MovieClip;
						_playpauseMc.addEventListener(SkinEvent.MEDIA_PLAY, onClickHandler);
						_playpauseMc.addEventListener(SkinEvent.MEDIA_PAUSE, onClickHandler);
					}
					if (_objArr[i]["name"] == "stopUI") {
						_stopMc = _objArr[i]["mc"] as SimpleButton;
						_stopMc.addEventListener(SkinEvent.MEDIA_STOP, onClickHandler);
					}
					if (_objArr[i]["name"] == "prevUI") {
						_prevMc = _objArr[i]["mc"] as SimpleButton;
						_prevMc.addEventListener(SkinEvent.MEDIA_PREV, onClickHandler);
					}
					if (_objArr[i]["name"] == "nextUI") {
						_nextMc = _objArr[i]["mc"] as SimpleButton;
						_nextMc.addEventListener(SkinEvent.MEDIA_NEXT, onClickHandler);
					}
					if (_objArr[i]["name"] == "soundUI") {
						_soundMc = _objArr[i]["mc"] as MovieClip;
						_soundMc.addEventListener(SkinEvent.MEDIA_SOUND, onClickHandler);
					}
					if (_objArr[i]["name"] == "brightnessUI") {
						_brightnessMc = _objArr[i]["mc"];
						_brightnessMc.addEventListener(SkinEvent.MEDIA_BRIGHTNESS, onClickHandler);
					}
					if (_objArr[i]["name"] == "hdUI") {
						_setStreamMc = _objArr[i]["mc"] as MovieClip;
						_setStreamMc.addEventListener(SkinEvent.MEDIA_CHANGE, onClickHandler);
					}
					if (_objArr[i]["name"] == "movieUI") {
						_movieMc = _objArr[i]["mc"] as SimpleButton; 
						_movieMc.addEventListener(SkinEvent.MEDIA_CINEMA, onClickHandler);
					}
					if (_objArr[i]["name"] == "fullScreenUI") {
						_fullscreenMc = _objArr[i]["mc"] as SimpleButton;
					}
					if (_objArr[i]["name"] == "liveprogressUI") {
						progressMc = _objArr[i]["mc"] as MovieClip;
						progressMc.addEventListener(SkinEvent.MEDIA_LIVE_POSITION, onClickHandler);
					}
					if (_objArr[i]["name"] == "progressUI") {
						progressMc = _objArr[i]["mc"] as MovieClip;
						progressMc.addEventListener(SkinEvent.MEDIA_VOD_POSITION, onClickHandler);
					}
					if (_objArr[i]["name"] == "topUI") {
						_topMc = _objArr[i]["mc"] as MovieClip;
					}
					if (_objArr[i]["name"] == "bigPlayUI") {
						_bigPlayMc = _objArr[i]["mc"] as SimpleButton;
						_bigPlayMc.addEventListener(SkinEvent.MEDIA_PLAY, onClickHandler);
					}
					if (_objArr[i]["name"] == "startFreshUI") {
						_startFreshMc = _objArr[i]["mc"] as MovieClip;
					}
					if (_objArr[i]["name"] == "loadingUI") {
						_loadingMc = _objArr[i]["mc"] as MovieClip;
					}
					if (_objArr[i]["name"] == "fullCloseUI") {
						_fullCloseMc = _objArr[i]["mc"] as MovieClip;
					}
					if (_objArr[i]["name"] == "logoUI") {
						_logoMc = _objArr[i]["mc"] as MovieClip;
						_logoMc.addEventListener(SkinEvent.MEDIA_LOGO, onClickHandler);
					}
					if (_objArr[i]["name"] == "errorUI") {
						_errorMc = _objArr[i]["mc"] as MovieClip;
						_errorMc.addEventListener(SkinEvent.MEDIA_RECOMMEND, onClickHandler);
						_errorMc.addEventListener(SkinEvent.PLUGIN_VERSION, onClickHandler);
					}
//					if (_objArr[i]["name"] == "portalUI") {
//						_portalMc = _objArr[i]["mc"] as MovieClip;
//						_portalMc.addEventListener(SkinEvent.MEDIA_SHARE, onClickHandler);
//					}
					if (_objArr[i]["name"] == "lightUI") {
						_lightMc = _objArr[i]["mc"] as MovieClip;
						_lightMc.addEventListener(SkinEvent.MEDIA_LIGHT, onClickHandler);
					}
					if (_objArr[i]["name"] == "brightnessUI") {
						_adjustMc.showTab("brightness");
						_brightnessMc = _objArr[i]["mc"] as SimpleButton;
						_brightnessMc.addEventListener(SkinEvent.MEDIA_SHOW_BRIGHTNESS, onClickHandler);
					}
					if (_objArr[i]["name"] == "shareUI") {
						_shareMc = _objArr[i]["mc"] as SimpleButton;
						_shareMc.addEventListener(SkinEvent.MEDIA_SHARE, onClickHandler);
					}
					if (_objArr[i]["name"] == "broadcastUI") {
						_adjustMc.showTab("broadcast");
						_broadcastMc = _objArr[i]["mc"] as SimpleButton;
						_broadcastMc.addEventListener(SkinEvent.MEDIA_SHOW_BROADCAST, onClickHandler);
					}
					////////
					if (_objArr[i]["name"] == "timeUI") {
						_timeMc = _objArr[i]["mc"] as MovieClip;
					}
					if (_objArr[i]["name"] == "shareUI") {
						//_shareMc = _objArr[i]["mc"] as MovieClip;
						//_shareMc.addEventListener(SkinEvent.PLUGIN_VERSION, onClickHandler);
					}
					if (_objArr[i]["name"] == "recommendUI") {
						_recommendMc = _objArr[i]["mc"] as MovieClip;
						_recommendMc.addEventListener(SkinEvent.MEDIA_PLAY, onClickHandler);
					}
					/*if (_objArr[i]["name"] == "activeXUI") {
						_activeMc = _objArr[i]["mc"] as MovieClip;
						_activeMc.addEventListener(SkinEvent.PLUGIN_VERSION, onClickHandler);
					}*/
					if (_objArr[i]["name"] == "playerTipUI") {
						_playerTip = _objArr[i]["mc"] as MovieClip;
						_playerTip.addEventListener(SkinEvent.MEDIA_SHOW_BROADCAST, onClickHandler);
					}
				}
				if (_startFreshMc) this.setChildIndex(_startFreshMc, 0);
				_islayout = true;
				resize();
				this.stage.addEventListener(Event.RESIZE, onResizeHandler);
				if (_topMc) _topMc.isEffect = true;
				if (_fullCloseMc) _fullCloseMc.isEffect = false;
				if (_controlMc) _controlMc.isEffect = false;
				if (_handyMc) _handyMc.isEffect = true;
				_effectArr = [_topMc, progressMc, _fullCloseMc, _controlMc,_handyMc];
				if (_handyMc) {
					_handyMc.buttonMode = true;
					_handyMc.addEventListener(MouseEvent.ROLL_OVER, onHandyHandler);
					_handyMc.addEventListener(MouseEvent.ROLL_OUT, onHandyHandler);
					_adjustMc.addEventListener(SkinEvent.MEDIA_BRIGHTNESS, onClickHandler);
					_adjustMc.addEventListener(SkinEvent.MEDIA_SKIP_PLAY, onClickHandler);
				}
				//_effectArr = [_topMc, _fullCloseMc, _controlMc];
//				EffectMove.mouseShowAlpha( [_topMc, progressMc], this);
				delaytime = new Timer(3000);
				this.stage.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
				this.stage.addEventListener(FullScreenEvent.FULL_SCREEN, onFullscreenHandler);
				//KEY
				this.stage.addEventListener(KeyboardEvent.KEY_UP, onSpaceHandler);
			}
		}
		private function onHandyHandler(e:MouseEvent):void {
			//TweenLite.killTweensOf(_handyBarMc);
			switch (e.type) {
				case MouseEvent.ROLL_OVER:
					_handyBarMc.visible = true;
					TweenLite.from(_handyBarMc, .3, { x:_handyBarMc.width, onComplete:function():void {
						_arrowMc.visible = false;
					}} );
					break;
				case MouseEvent.ROLL_OUT:
					TweenLite.to(_handyBarMc, .3, { x:_handyBarMc.width, onComplete:function():void {
						_handyBarMc.visible = false;
						_handyBarMc.x = 0;
						_arrowMc.visible = true;
					}} );
					break;
				default:
					break;
			}
		}
		private function onSpaceHandler(e:KeyboardEvent):void {
			if (e.charCode == 32) {
				if (_playpauseMc.status == 'pause') {
					setPlayState(true);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
				} else {
					setPlayState(false);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PAUSE));
				}				
			}
		}
		protected function onFullscreenHandler(e:FullScreenEvent):void {
			delaytime.stop();
			delaytime.removeEventListener(TimerEvent.TIMER, onTimerHandler);
			delaytime = null;
			//this.stage.removeEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			for (var i:int = 0, len:int = _effectArr.length; i < len; i++) {
				if (_effectArr[i]) {
					_effectArr[i].isEffect = false;
					_effectArr[i].visible = true;
					_effectArr[i].alpha = 1;
					_effectArr[i].removeEventListener(Event.ENTER_FRAME, onHideHandler);
					_effectArr[i].removeEventListener(Event.ENTER_FRAME, onShowHandler);
				}
			}
			_effectArr = new Array();
			if (e.fullScreen) {
				if (_fullCloseMc) _fullCloseMc.isEffect = true;
				if (progressMc) progressMc.isEffect = false;
				if (_controlMc) _controlMc.isEffect = true;
			} else {
				if (progressMc) progressMc.isEffect = true;
				if (_fullCloseMc) {
					_fullCloseMc.isEffect = false;
					_fullCloseMc.visible = false;
				}
				if (_controlMc) _controlMc.isEffect = false;
			}
			if (_topMc) _topMc.isEffect = true;
			if (_handyMc) _handyMc.isEffect = true;
//			_effectArr = [_topMc, _fullCloseMc, _controlMc];
			_effectArr = [_topMc, progressMc, _fullCloseMc, _controlMc,_handyMc];
			delaytime = new Timer(3000);
			this.stage.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
		}
		
		protected function onMoveHandler(e:MouseEvent):void {				
			for (var i:int = 0, len:int = _effectArr.length; i < len; i++) {
				if (_effectArr[i] && _effectArr[i].isEffect) {					
					_effectArr[i].visible = true;
					_effectArr[i].removeEventListener(Event.ENTER_FRAME, onHideHandler);
					_effectArr[i].addEventListener(Event.ENTER_FRAME, onShowHandler);
				}
			}
		} 
		protected function onShowHandler(e:Event):void {
			delaytime.removeEventListener(TimerEvent.TIMER, onTimerHandler);
			if (e.target.alpha >= 1) {
				e.target.alpha = 1;
				if (! e.target.hitTestPoint(this.stage.mouseX, this.stage.mouseY, false)) {
					e.target.removeEventListener(Event.ENTER_FRAME, onShowHandler);
					delaytime.addEventListener(TimerEvent.TIMER, JEventDelegate.create(onTimerHandler, e.target));
					delaytime.start();
				} else {
					delaytime.stop();
				}
			} else {
				e.target.alpha += 0.1;
				if (e.target.alpha > 0.9) {
					e.target.visible = true;
				}
			}
		}		
		protected function onHideHandler(e:Event):void {
			if (e.target.alpha <= 0) {
				e.target.alpha = 0;
				e.target.removeEventListener(Event.ENTER_FRAME, onHideHandler);
				delaytime.addEventListener(TimerEvent.TIMER, JEventDelegate.create(onTimerHandler, e.target));
			} else {
				e.target.alpha -= 0.1;
				if (e.target.alpha < 0.1) {
					e.target.visible = false;
				}
			}
		}		
		protected function onTimerHandler(t:TimerEvent, target:*):void {
			delaytime.stop();
			target.addEventListener(Event.ENTER_FRAME, onHideHandler);
		}
		
		protected function showUI(obj:Object, contain:Sprite, xmllist:XMLList):void {
//			if (xmllist.length() != 0) {
				var ClassMc:Class = getDefinitionByName(xmllist) as Class;
				var target:*;
				target = new ClassMc();
				contain.addChild(target);
				obj = { };
				obj.mc = target;
				obj.name = xmllist.localName();
				_objArr.push( { "mc":obj.mc, "name":obj.name } );
				obj.mc.x = Number(xmllist.@x);
				obj.mc.y = Number(xmllist.@y);
				/*if (xmllist.hasOwnProperty("@sep") && xmllist.@sep == "1") {
					var _separate_mc:MovieClip = new SeparateMc();
					_separate_mc.x = target.x - 2;
					_separate_mc.y = target.y;
					contain.addChild(_separate_mc);
				}*/
//			}
		}
		
		protected function onClickHandler(e:SkinEvent):void {
			switch(e.type) {
				case SkinEvent.MEDIA_PLAY:
					setPlayState();
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
					break;
				case SkinEvent.MEDIA_PAUSE:
					setPlayState(false);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PAUSE));
					break;
				case SkinEvent.MEDIA_STOP:
					setPlayState(false);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_STOP));
					break;
				case SkinEvent.MEDIA_SOUND:
					if (_soundMc) _soundMc.goto(e.currObj["value"]);
					if (e.currObj["value"] != 0) {
						if (_soundMc) _soundMc.setSoundState();
					} else {
						if (_soundMc) _soundMc.setSoundState(false);
					}
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SOUND, { "value":e.currObj["value"] } ));
					break;
				case SkinEvent.MEDIA_BRIGHTNESS:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_BRIGHTNESS, e.currObj ));
					break; 
				case SkinEvent.MEDIA_CINEMA:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_CINEMA));
					break; 
				case SkinEvent.MEDIA_LIVE_POSITION:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_LIVE_POSITION, { "value":e.currObj["value"] } ));
					break;
				case SkinEvent.MEDIA_VOD_POSITION:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_VOD_POSITION, { "value":e.currObj["value"] } ));
					break;
				case SkinEvent.MEDIA_PREV:
					//showRecommend();
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PREV));
					break;
				case SkinEvent.MEDIA_NEXT:
					//showRecommend();
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_NEXT));
					break;
				case SkinEvent.MEDIA_CHANGE:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_CHANGE, { "value":true } ));				
					break;
				case SkinEvent.MEDIA_LOGO:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_LOGO));
					break;
				case SkinEvent.MEDIA_RECOMMEND:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_RECOMMEND, { "value":e.currObj["value"] } ));
					break;
				case SkinEvent.PLUGIN_VERSION:
					this.dispatchEvent(new SkinEvent(SkinEvent.PLUGIN_VERSION));
					break;
				case SkinEvent.MEDIA_SHARE:
					if(_adjustMc)_adjustMc.visible = false;
					setSnapshotPic();
					_handyShareMc.visible = true;
					break;
				case SkinEvent.MEDIA_LIGHT:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_LIGHT, { "value":e.currObj["value"] } ));
					break;
				case SkinEvent.MEDIA_SHOW_BRIGHTNESS:
					if(_handyShareMc)_handyShareMc.visible = false;
					_adjustMc.visible = true;
					_adjustMc.reset(0);
					break;
//				case SkinEvent.MEDIA_SHARE:
//					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SHARE));
//					break;
				case SkinEvent.MEDIA_SHOW_BROADCAST:
					if(_handyShareMc)_handyShareMc.visible = false;
					_adjustMc.visible = true;
					_adjustMc.reset(1);
					break;
				case SkinEvent.MEDIA_SKIP_PLAY:
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SKIP_PLAY, e.currObj ));
					break;
				default:
					break;
			}
		}
		
		protected function onResizeHandler(e:Event):void {
			resize();
			if (this.stage.displayState == StageDisplayState.FULL_SCREEN) {
				setMovie(false);
				if (_fullCloseMc) _fullCloseMc.visible = true;
			} else {
				setMovie(true);
				if (_controlMc) _controlMc.visible = true;
				if (progressMc) {
					progressMc._timeBg.visible = true;
					progressMc._playedDot.visible = true;
					if(progressMc._dragDot)progressMc._dragDot.visible = true;
				}
				if (_fullCloseMc) _fullCloseMc.visible = false;
			}
		}
		
		protected function setMovie(bool:Boolean):void {
			if (_movieMc) setButtonState(_movieMc, bool);
		}	
		
		protected function setButtonState(obj:*, bool:Boolean):void {
			obj.mouseEnabled = bool;
			if (bool) {
				obj.alpha = 1;
			} else {
				obj.alpha = .5;
			}
		}
		/**
		 * 是否暂停或者播放
		 * @param bool
		 * 
		 */		
		public function setPlayState(bool:Boolean = true):void {
			if (bool) {
				if (_bigPlayMc) _bigPlayMc.visible = false;
				if (_playpauseMc) _playpauseMc.setPlayState();
			} else {
				if (_bigPlayMc) _bigPlayMc.visible = true;
				if (_playpauseMc) _playpauseMc.setPlayState(false);
			}
		}
		/**
		 * 影院模式切换时 播放窗口 “canvas” 定位
		 * @param	mode
		 */
		public function setMode(mode:String = "normal"):void {
			_mode = mode;
		}
		
		public function resize():void {
			var _back:String;
			if (this.stage.displayState == StageDisplayState.FULL_SCREEN) {
				_width_ = this.stage.stageWidth;
				_height_ = this.stage.stageHeight;
				this["parent"].graphics.clear();
				this["parent"].graphics.beginFill(0x000000, 0);
				this["parent"].graphics.drawRect(0, 0, this.stage.stageWidth, this.stage.stageHeight);
				this["parent"].graphics.endFill();
				this["parent"].x = 0;
				this["parent"].y = 0;
				_showWidth = this.stage.stageWidth;
				_showHeight = this.stage.stageHeight;
				_back = "hide";
			} else {
				_width_ = this.stage.stageWidth / _playerXscale;
				_height_ = this.stage.stageHeight / _playerYscale;
//				if (_mode == "normal") {
					this["parent"].graphics.clear();
					this["parent"].graphics.beginFill(0x000000, 0);
					this["parent"].graphics.drawRect(0, 0, _width_, _height_);
					this["parent"].graphics.endFill();
					this["parent"].x = Math.round((this.stage.stageWidth - _width_) / 2);
					this["parent"].y = Math.round((this.stage.stageHeight - _height_) / 2);
//					this["parent"].x = Math.round((this.stage.stageWidth - _containWidth) / 2);
//					this["parent"].y = Math.round((this.stage.stageHeight - _containHeight) / 2);
					_showWidth = _width_;
					_showHeight = _height_ - ((_contrlBgMc) ? _contrlBgMc.height : 0);
					_back = "show";
//				} else if (_mode == "cinema") {
//					this["parent"].graphics.clear();
//					this["parent"].graphics.beginFill(0x000000, 0);
//					this["parent"].graphics.drawRect(0, 0, this.stage.stageWidth, this.stage.stageHeight);
//					this["parent"].graphics.endFill();
//					this["parent"].x = 0;
//					this["parent"].y = 0;
//					_showWidth = this.stage.stageWidth;
//					_showHeight = this.stage.stageHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0);
//					_back = "hide";
//				}
			}
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_RESIZE, { "back":_back, "w": _showWidth, "h":_showHeight} ));
			if (_topMc) {
				_topMc.x = 0;
				_topMc.y = 0;
				_topMc["bg_mc"].width = _showWidth;
			}
			if (_adjustMc) {
				_adjustMc.x = (_showWidth - _adjustMc.width) / 2;
				_adjustMc.y = (_showHeight - _adjustMc.height) / 2;
			}
			if (_handyShareMc) {
				_handyShareMc.x = (_showWidth - _handyShareMc.width) / 2;
				_handyShareMc.y = (_showHeight - _handyShareMc.height) / 2;
			}
			if (_broadcastMc){
			
			} else if (_adjustMc){
				_adjustMc.hideTab(1);
			}
			if (_handyMc) {
				_handyMc.x = _showWidth - _arrowMc.width - 5;
				_handyMc.y = (_showHeight - _arrowMc.height) / 2;
			}
			if(_recommendMc){
				_recommendMc.resize(_showWidth,_showHeight);
			}
			if (_controlMc) {
				_controlMc.x = 0;
				_controlMc.y = (this.stage.displayState == StageDisplayState.FULL_SCREEN) ? _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) : _showHeight;
			}
			if (_bigPlayMc) {
				_bigPlayMc.x = 10;
				_bigPlayMc.y = (this.stage.displayState == StageDisplayState.FULL_SCREEN) ? _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) - _bigPlayMc.height - 10 : _showHeight - _bigPlayMc.height - 10;
			}
			if (_leftMc) _leftMc.x = 0;
			if (_rightMc) _rightMc.x = _showWidth - _rightMc.width;
			if (_contrlBgMc) _contrlBgMc.width = _showWidth;
			if (progressMc) {
				progressMc.y = -12;
				progressMc._timeBg.x = 0;
				progressMc._timeBg.width = _showWidth;
//				progressMc._endTime_txt.x = _showWidth - progressMc._endTime_txt.width;
				progressMc.resize();
			}
			if (_startFreshMc) {
				_startFreshMc.x = (_showWidth - _startFreshMc.width) / 2;
				_startFreshMc.y = (_showHeight + ((_contrlBgMc) ? _contrlBgMc.height : 0) - _startFreshMc.height) / 2;
			}
			if (_loadingMc) {
				_loadingMc.x = (_showWidth - _loadingMc.width) / 2;
				_loadingMc.y = (_showHeight + ((_contrlBgMc) ? _contrlBgMc.height : 0) - _loadingMc.height) / 2;
			}
			if (_fullCloseMc) {
				_fullCloseMc.x = _showWidth - _fullCloseMc.width - 6;
				_fullCloseMc.y = 6;
			}
			if (_errorMc) {
				_errorMc.resize(_showWidth, _showHeight + ((_contrlBgMc) ? _contrlBgMc.height : 0));
			}
			/*if (_activeMc) {
				_activeMc.resize(_showWidth, (this.stage.displayState == StageDisplayState.FULL_SCREEN) ?  _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) - 10 : _showHeight - 10);
			}*/
			if (_playerTip) {
				_playerTip.resize(_showWidth, (this.stage.displayState == StageDisplayState.FULL_SCREEN) ?  _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) - 13 : _showHeight - 13);
			}
			if (_islayout) {
				this.dispatchEvent(new SkinEvent(SkinEvent.LAYOUT_SUCCESS));
				_islayout = false;
			}
		}
		/**
		 * 显示提示
		 * @param	obj
		 */
		public function showError(obj:Object):void {
			if (_errorMc) _errorMc.showError(_showWidth, _showHeight + ((_contrlBgMc) ? _contrlBgMc.height : 0), obj);
		}
		/**
		 * 隐藏提示
		 * @param	obj
		 */
		public function hideError():void {
			if (_errorMc) _errorMc.hideError();
		}

		/**
		 * 拖动中右键释放
		 */
		public function liveStopDrag():void {
			if (progressMc) progressMc.liveStopDrag();
		}
		/**
		 * 视频后推荐
		 * @param	obj
		 */
		public function showRecommend(obj:Object = null):void {
			
		}
		/**
		 * 是否显示中间缓冲进度  num为0---100整数
		 */
		public function showLoading(bool:Boolean = true, num:Number = -1):void {
			if (bool) {
				if (_loadingMc) {
					_loadingMc.visible = true;
					if(num != -1)_loadingMc.showLoading(num);
				}
				if (_startFreshMc) _startFreshMc.visible = false;
			} else {
				if (_loadingMc) _loadingMc.visible = false;
			}
		}
		/**
		 * 显示播放提示
		 */
		public function setPlayerTip(obj:Object = null):void{
			if(obj && _playerTip)_playerTip.setObj(obj);
		}
		/**
		 * 显示标题
		 * @param	str
		 */
		public function setTitle(str:String):void {
			if (_topMc) _topMc.setTitle(str);
		}
		/**
		 * 设置音量控制条位置
		 * @param	value
		 */
		public function setSound(value:Number):void {
			if (_soundMc) _soundMc.setSlider(value);
		}
		/**
		 * 设置亮度控制条位置
		 * @param	value
		 */
		public function setBrightness(bt:Number, ct:Number):void {
			if (_adjustMc) _adjustMc.setBrightness(bt, ct);
		}
		/**
		 * 设置码流切换
		 * @param	value
		 */
		public function setStream(value:Number = 0):void {
			if (_setStreamMc) _setStreamMc.setStream(value);
		}
		/**
		 * 设置码流切换
		 * @param	value
		 */
		public function setSnapshotPic():void {
			try{
				var bd:BitmapData
				if(Common.playerVideo == null){
					bd = new BitmapData(0,0);
				} else {
					bd = new BitmapData(Common.playerVideo.parent.width,Common.playerVideo.parent.height,true,0x000000);
					var mtx:Matrix = new Matrix(1,0,0,1,-Common.playerVideo.x,-Common.playerVideo.y);
					var dw:Number = Common.playerVideo.parent.width;
					var dh:Number = Common.playerVideo.parent.height;
					var rct:Rectangle = new Rectangle(0,0,dw,dh);
					bd.draw(Common.playerVideo.parent,mtx,null,null,rct);
				}
				if(_handyShareMc)_handyShareMc.showImage(bd);
			} catch (e:Error){
				trace(e);
			}
		}
		/**
		 * 设置是否跳过片头片尾
		 * @param	bool
		 */
		public function setSelect(bool:Boolean):void {
		
		}
		/**
		 * UI重置
		 */
		public function reset(bool:Boolean = true):void {
			if (progressMc) progressMc.reset();
			setPlayState(bool);
			if (_adjustMc) _adjustMc.visible = false;
			if (_loadingMc) _loadingMc.visible = false;
		}
		/**
		 * 控制条是否可用
		 * @param	bool
		 */
		public function isEnable(bool:Boolean = true):void {
			if (_controlMc) {
				if (bool) {
					_controlMc.mouseEnabled = true;
					_controlMc.mouseChildren = true;
				} else {
					_controlMc.mouseEnabled = false;
					_controlMc.mouseChildren = false;
				}
			}
		}		
		/**
		 * 是否显示切换一代直播提示
		 * @param	bool
		 
		public function setActiveXTip(bool:Boolean = true):void {
			if (_activeMc) {
				if (bool) {
					_activeMc.visible = true;
				} else {
					_activeMc.visible = false;
				}
			}
		}*/
		public function get progressMc():MovieClip{return new MovieClip()};
		public function set progressMc(v:MovieClip):void{};
		public function get startFreshMc():MovieClip { return _startFreshMc; }
		
		public function get showWidth():Number { return _showWidth; }
		public function get showHeight():Number { return _showHeight; }
		
		public function get containWidth():Number { return _containWidth; }
		public function set containWidth(value:Number):void {
			_containWidth = value;
		}
		public function set playerXscale(value:Number):void {
			_playerXscale = value;
		}
		public function get playerXscale():Number { return _playerXscale; }
		public function set playerYscale(value:Number):void {
			_playerYscale = value;
		}
		public function get playerYscale():Number { return _playerYscale; }
		
		public function get containHeight():Number { return _containHeight; }
		public function set containHeight(value:Number):void {
			_containHeight = value;
		}
		
	}

}