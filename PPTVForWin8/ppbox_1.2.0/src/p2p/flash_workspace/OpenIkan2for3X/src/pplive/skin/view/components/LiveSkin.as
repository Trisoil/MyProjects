package pptv.skin.view.components {
	
	import com.greensock.TweenLite;
	import flash.display.DisplayObject;
	import flash.display.DisplayObjectContainer;
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.display.StageDisplayState;
	import flash.events.Event;
	import flash.events.FullScreenEvent;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.external.ExternalInterface;
	import flash.text.TextField;
	import flash.ui.Mouse;
	import flash.utils.clearTimeout;
	import flash.utils.getDefinitionByName;
	import flash.utils.setTimeout;
	import flash.utils.Timer;
	import pplive.p2p.utils.PrintDebug;
	import pptv.skin.view.events.SkinEvent;
	import pptv.skin.view.ui.ArrowUI;
	import pptv.skin.view.utils.EffectMove;
	import pptv.skin.view.utils.JEventDelegate;
	
	/**
	 * ...
	 * @author minliang1112@foxmail.com
	 */
	public class LiveSkin extends BaseSkin {
		
		private var _liveprogressMc:MovieClip;
		private var _playerTipMc:MovieClip;
		private var _isMouseMove:Boolean = false;
		
		public function LiveSkin() {
			super();
		}
		
		public function setData(list:XML):void {		
			for (var k:int = 0, index:int = list.children().length(); k < index; k++) {
				if (list.child(k).localName() == "controlUI") {
					_controlMc = new MovieClip();
					this.addChild(_controlMc);
					_controlMc.x = Number(list.child("controlUI").@x);
					_controlMc.y = Number(list.child("controlUI").@y);
					if (list.child("controlUI").hasOwnProperty("leftUI") || list.child("controlUI").hasOwnProperty("rightUI")) {
						_contrlBgMc = new ControlBgMc();
						_controlMc.addChild(_contrlBgMc);
					}
					if (list.child("controlUI").hasOwnProperty("leftUI")) {
						_leftMc = new Sprite();
						_controlMc.addChild(_leftMc);
						for (var i:int = 0, len:int = list.child("controlUI").child("leftUI").children().length(); i < len; i++) {
							if (list.child("controlUI").child("leftUI").child(i).children().length() != 0) {
								var LClassMc:Class = getDefinitionByName(list.child("controlUI").child("leftUI").child(i)) as Class;
								_leftArr[i] = { };
								_leftArr[i].mc = new LClassMc();
								_leftArr[i].name = list.child("controlUI").child("leftUI").child(i).localName();
								_leftMc.addChild(_leftArr[i].mc);
								_objArr.push( { "mc":_leftArr[i].mc, "name":_leftArr[i].name } );
								if (i == 0) {
									_leftArr[i].mc.x = 0;
								} else {
									_leftArr[i].mc.x = _leftArr[i - 1].mc.x + _leftArr[i - 1].mc.width;
								}
								_leftArr[i].mc.y = 2;
								_elementNum++;
							}							
						}
					}
					if (list.child("controlUI").hasOwnProperty("rightUI")) {
						_rightMc = new Sprite();
						_controlMc.addChild(_rightMc);
						for (var j:int = 0, num:int = list.child("controlUI").child("rightUI").children().length(); j < num; j++) {
							if (list.child("controlUI").child("rightUI").child(j).children().length() != 0) {
								var RClassMc:Class = getDefinitionByName(list.child("controlUI").child("rightUI").child(j)) as Class;
								_rightArr[j] = { };
								_rightArr[j].mc = new RClassMc();
								_rightArr[j].name = list.child("controlUI").child("rightUI").child(j).localName();
								_rightMc.addChild(_rightArr[j].mc); 
								_objArr.push( { "mc":_rightArr[j].mc, "name":_rightArr[j].name } );
								if (j == 0) {
									_rightArr[j].mc.x = 0;
								} else {
									_rightArr[j].mc.x = _rightArr[j - 1].mc.x + _rightArr[j - 1].mc.width;
								}
								_rightArr[j].mc.y = 2;
								_elementNum++;
							}							
						}
					}					
					for (var s:int = 0, total:int = list.child("controlUI").children().length(); s < total; s++) {
						if (list.child("controlUI").child(s).localName() != "leftUI" && list.child("controlUI").child(s).localName() != "rightUI") {
							if (list.child("controlUI").child(s).children().length() != 0) {
								showUI(_conArr[s], _controlMc, list.child("controlUI").child(s));
								_elementNum++;
							}							
						}
					}
				} else if (list.child(k).localName() == "handyUI" && list.child("handyUI").children().length() != 0) {
					_handyMc = new MovieClip();
					this.addChild(_handyMc);
					_handyBarMc = new MovieClip();
					_handyMc.addChild(_handyBarMc);
					for (var t:int = 0, dis:int = list.child("handyUI").children().length(); t < dis; t++) {
						if (list.child("handyUI").child(t).children().length() != 0) {
							var hClassMc:Class = getDefinitionByName(list.child("handyUI").child(t)) as Class;
							_handyArr[t] = { };
							_handyArr[t].mc = new hClassMc();
							_handyArr[t].name = list.child("handyUI").child(t).localName();
							_handyBarMc.addChild(_handyArr[t].mc); 
							_objArr.push( { "mc":_handyArr[t].mc, "name":_handyArr[t].name } );
							_handyArr[t].mc.x = 5;
							if (t == 0) {
								_handyArr[t].mc.y = 15;
							} else {
								_handyArr[t].mc.y = _handyArr[t - 1].mc.y + _handyArr[t - 1].mc.height + 15;
							}
							/*if (list.child("handyUI").child(t)["@ena"] != "1") {
								_handyArr[t].mc.mouseEnabled = false;
								_handyArr[t].mc.mouseChildren = false;
								_handyArr[t].mc.alpha = .3;
							}*/
							_elementNum++;
						}						
					}
					var _w_:Number = _handyArr[0].mc.width + 10;
					var _h_:Number = _handyArr[0].mc.height * dis + (dis + 1) * 15;
					_handyBarMc.graphics.clear();
					_handyBarMc.graphics.beginFill(0x000000, .8);
					_handyBarMc.graphics.drawRoundRectComplex(0, 0, _w_, _h_, 5, 0, 5, 0);
					_handyBarMc.graphics.endFill();
					var _handyBarMask:MovieClip = new MovieClip();
					_handyBarMask.graphics.clear();
					_handyBarMask.graphics.beginFill(0xFF0000);
					_handyBarMask.graphics.drawRect(0, 0, _w_, _h_);
					_handyBarMask.graphics.endFill();
					_handyMc.addChild(_handyBarMask);
					_handyBarMc.mask = _handyBarMask;
					_arrowMc = new ArrowUI(_w_, _h_);
					_handyMc.addChild(_arrowMc);
					_handyMc.setChildIndex(_arrowMc, 0);
					_handyBarMc.visible = false;
					_adjustMc = new AdjustMc();
					this.addChild(_adjustMc);
					_adjustMc.visible = false;
				} else {
					if (list.child(k).children().length() != 0) {
						showUI(_uiArr[k], this, list.child(k));
						_elementNum++; 
					}
				}
			}
			this.addEventListener(Event.ENTER_FRAME, onEnterFrameHandler);
			ExternalInterface.call("console.log", "_elementNum  " + _elementNum);
		}
		
		private function showUI(obj:Object, contain:Sprite, xmllist:XMLList):void {
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
			//ExternalInterface.call("console.log", 'xmllist   ' + xmllist.localName(),xmllist.children().length());
			/*if (xmllist.hasOwnProperty("@sep") && xmllist.@sep == "1") {
				var _separate_mc:MovieClip = new SeparateMc();
				_separate_mc.x = target.x - 2;
				_separate_mc.y = target.y;
				contain.addChild(_separate_mc);
			}*/
		}
		
		private function onEnterFrameHandler(e:Event):void {
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
					if (_objArr[i]["name"] == "hdUI") {
						_smoothMc = _objArr[i]["mc"] as MovieClip;
						_smoothMc.addEventListener(SkinEvent.MEDIA_CHANGE, onClickHandler);
					}
					/*if (_objArr[i]["name"] == "movieUI") {
						_movieMc = _objArr[i]["mc"] as SimpleButton; 
						_movieMc.addEventListener(SkinEvent.MEDIA_CINEMA, onClickHandler);
					}*/
					if (_objArr[i]["name"] == "fullScreenUI") {
						_fullscreenMc = _objArr[i]["mc"] as SimpleButton;
					}
					if (_objArr[i]["name"] == "liveprogressUI") {
						_liveprogressMc = _objArr[i]["mc"] as MovieClip;
						_liveprogressMc.addEventListener(SkinEvent.MEDIA_LIVE_POSITION, onClickHandler);
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
					if (_objArr[i]["name"] == "playerTipUI") {
						_playerTipMc = _objArr[i]["mc"] as MovieClip;
						_playerTipMc.addEventListener(SkinEvent.PLUGIN_VERSION, onClickHandler);
					}
					if (_objArr[i]["name"] == "lightUI") {
						_lightMc = _objArr[i]["mc"] as MovieClip;
						_lightMc.addEventListener(SkinEvent.MEDIA_LIGHT, onClickHandler);
					}
					if (_objArr[i]["name"] == "brightnessUI") {
						_brightnessMc = _objArr[i]["mc"] as SimpleButton;
						_brightnessMc.addEventListener(SkinEvent.MEDIA_SHOW_BRIGHTNESS, onClickHandler);
					}
					if (_objArr[i]["name"] == "shareUI") {
						_shareMc = _objArr[i]["mc"] as SimpleButton;
						_shareMc.addEventListener(SkinEvent.MEDIA_SHARE, onClickHandler);
					}
					if (_objArr[i]["name"] == "broadcastUI") {
						_broadcastMc = _objArr[i]["mc"] as SimpleButton;
						_broadcastMc.addEventListener(SkinEvent.MEDIA_SHOW_BROADCAST, onClickHandler);
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
				_effectArr = [_topMc, _liveprogressMc, _fullCloseMc, _controlMc, _handyMc];
				//EffectMove.mouseShowAlpha( [_topMc, _liveprogressMc], this);
				if (_handyMc) {
					_handyMc.buttonMode = true;
					_handyMc.addEventListener(MouseEvent.ROLL_OVER, onHandyHandler);
					_handyMc.addEventListener(MouseEvent.ROLL_OUT, onHandyHandler);
					_adjustMc.addEventListener(SkinEvent.MEDIA_BRIGHTNESS, onClickHandler);
					_adjustMc.addEventListener(SkinEvent.MEDIA_SKIP_PLAY, onClickHandler);
				}
				delaytime = new Timer(3000);
				this.stage.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
				this.stage.addEventListener(FullScreenEvent.FULL_SCREEN, onFullscreenHandler);
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
		
		private function onFullscreenHandler(e:FullScreenEvent):void {
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
				if (_liveprogressMc) _liveprogressMc.isEffect = false;
				if (_controlMc) _controlMc.isEffect = true;
			} else {
				if (_liveprogressMc) _liveprogressMc.isEffect = true;
				if (_fullCloseMc) {
					_fullCloseMc.isEffect = false;
					_fullCloseMc.visible = false;
				}
				if (_controlMc) _controlMc.isEffect = false;
			}
			if (_topMc) _topMc.isEffect = true;
			if (_handyMc) _handyMc.isEffect = true;
			_effectArr = [_topMc, _liveprogressMc, _fullCloseMc, _controlMc, _handyMc];
			delaytime = new Timer(3000);
			//this.stage.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
		}
		
		private function onMoveHandler(e:MouseEvent):void {
			if (!_isMouseMove) return;
			for (var i:int = 0, len:int = _effectArr.length; i < len; i++) {
				if (_effectArr[i] && _effectArr[i].isEffect) {					
					_effectArr[i].visible = true;
					_effectArr[i].removeEventListener(Event.ENTER_FRAME, onHideHandler);
					_effectArr[i].addEventListener(Event.ENTER_FRAME, onShowHandler);
				}
			}
		} 
		private	function onShowHandler(e:Event):void {
			delaytime.removeEventListener(TimerEvent.TIMER, onTimerHandler);
			if (e.target.alpha >= 1) {
				if (e.target == _handyMc) _arrowMc.setFilter(false);
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
		private	function onHideHandler(e:Event):void {
			if (e.target.alpha <= 0) {
				if (e.target == _handyMc) _arrowMc.setFilter();
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
		private	function onTimerHandler(t:TimerEvent, target:*):void {
			delaytime.stop();
			target.addEventListener(Event.ENTER_FRAME, onHideHandler);
		}
		
		private function sendEvent(type:String, obj = null):void {
			this.dispatchEvent(new SkinEvent(type, obj));
		}
		
		private function onClickHandler(e:SkinEvent):void {
			switch(e.type) {
				case SkinEvent.MEDIA_PLAY:
					setPlayState();
					sendEvent(SkinEvent.MEDIA_PLAY);
					break;
				case SkinEvent.MEDIA_PAUSE:
					setPlayState(false);
					sendEvent(SkinEvent.MEDIA_PAUSE);
					break;
				case SkinEvent.MEDIA_STOP:
					setPlayState(false);
					sendEvent(SkinEvent.MEDIA_STOP);
					break;
				case SkinEvent.MEDIA_SOUND:
					if (_soundMc) _soundMc.goto(e.currObj["value"]);
					if (e.currObj["value"] != 0) {
						if (_soundMc) _soundMc.setSoundState();
					} else {
						if (_soundMc) _soundMc.setSoundState(false);
					}
					sendEvent(SkinEvent.MEDIA_SOUND, e.currObj);
					break;
				case SkinEvent.MEDIA_BRIGHTNESS:
					sendEvent(SkinEvent.MEDIA_BRIGHTNESS, e.currObj);
					break; 
				/*case SkinEvent.MEDIA_CINEMA:
					sendEvent(SkinEvent.MEDIA_CINEMA);
					break;*/ 
				case SkinEvent.MEDIA_LIVE_POSITION:
					sendEvent(SkinEvent.MEDIA_LIVE_POSITION, e.currObj);
					break;
				case SkinEvent.MEDIA_PREV:
					//hideRecommend();
					sendEvent(SkinEvent.MEDIA_PREV);
					break;
				case SkinEvent.MEDIA_NEXT:
					//hideRecommend();
					sendEvent(SkinEvent.MEDIA_NEXT);
					break;
				case SkinEvent.MEDIA_CHANGE:
					sendEvent(SkinEvent.MEDIA_CHANGE, { "value":true } );				
					break;
				case SkinEvent.MEDIA_LOGO:
					sendEvent(SkinEvent.MEDIA_LOGO);
					break;
				case SkinEvent.MEDIA_RECOMMEND:
					sendEvent(SkinEvent.MEDIA_RECOMMEND, { "value":e.currObj["value"] } );
					break;
				case SkinEvent.PLUGIN_VERSION:
					sendEvent(SkinEvent.PLUGIN_VERSION, e.currObj);
					break;
				case SkinEvent.MEDIA_LIGHT:
					sendEvent(SkinEvent.MEDIA_LIGHT, e.currObj);
					break;
				case SkinEvent.MEDIA_SHOW_BRIGHTNESS:
					if ((_containWidth < _adjustMc.width || _containHeight < _adjustMc.height) && this.stage.displayState ==StageDisplayState.NORMAL) return;
					_adjustMc.visible = true;
					_adjustMc.reset(0);
					break;
				case SkinEvent.MEDIA_SHARE:
					sendEvent(SkinEvent.MEDIA_SHARE);
					break;
				case SkinEvent.MEDIA_SHOW_BROADCAST:
					if ((_containWidth < _adjustMc.width || _containHeight < _adjustMc.height) && this.stage.displayState ==StageDisplayState.NORMAL) return;
					_adjustMc.visible = true;
					_adjustMc.reset(1);
					break;
				case SkinEvent.MEDIA_SKIP_PLAY:
					sendEvent(SkinEvent.MEDIA_SKIP_PLAY, e.currObj);
					break;
				default:
					break;
			}
		}
		
		private function onResizeHandler(e:Event):void {
			resize();
			if (this.stage.displayState == StageDisplayState.FULL_SCREEN) {
				setMovie(false);
				if (_fullCloseMc) _fullCloseMc.visible = true;
			} else {
				Mouse.show();
				setMovie(true);
				if (_controlMc) _controlMc.visible = true;
				if (_liveprogressMc) {		
					_liveprogressMc._timeBg.visible = true;
					_liveprogressMc._playedDot.visible = true;
					_liveprogressMc._dragDot.visible = true;
				}
				if (_fullCloseMc) _fullCloseMc.visible = false;
				if (_containWidth < _adjustMc.width || _containHeight < _adjustMc.height) _adjustMc.visible = false;
			}
		}
		
		private function setMovie(bool:Boolean):void {
			if (_movieMc) setButtonState(_movieMc, bool);
		}
		
		private function setButtonState(obj:*, bool:Boolean):void {
			obj.mouseEnabled = bool;
			if (bool) {
				obj.alpha = 1;
			} else {
				obj.alpha = .5;
			}
		}
		
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
			if (this.stage.displayState == StageDisplayState.FULL_SCREEN) {
				this["parent"].graphics.clear();
				this["parent"].graphics.beginFill(0x000000, 1);
				this["parent"].graphics.drawRect(0, 0, this.stage.stageWidth, this.stage.stageHeight);
				this["parent"].graphics.endFill();
				this["parent"].x = 0;
				this["parent"].y = 0;
				_showWidth = this.stage.stageWidth;
				_showHeight = this.stage.stageHeight;
			} else {
				this["parent"].graphics.clear();
				this["parent"].graphics.beginFill(0x000000, 1);
				this["parent"].graphics.drawRect(0, 0, _containWidth, _containHeight);
				this["parent"].graphics.endFill();
				this["parent"].x = Math.round((this.stage.stageWidth - _containWidth) / 2);
				this["parent"].y = Math.round((this.stage.stageHeight - _containHeight) / 2);
				_showWidth = _containWidth;
				_showHeight = _containHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0);
				/*if (_mode == "normal") {
					this["parent"].graphics.clear();
					this["parent"].graphics.beginFill(0x000000, 0);
					this["parent"].graphics.drawRect(0, 0, _containWidth, _containHeight);
					this["parent"].graphics.endFill();
					this["parent"].x = Math.round((this.stage.stageWidth - _containWidth) / 2);
					this["parent"].y = Math.round((this.stage.stageHeight - _containHeight) / 2);
					_showWidth = _containWidth;
					_showHeight = _containHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0);
				} else if (_mode == "cinema") {
					this["parent"].graphics.clear();
					this["parent"].graphics.beginFill(0x000000, 0);
					this["parent"].graphics.drawRect(0, 0, this.stage.stageWidth, this.stage.stageHeight);
					this["parent"].graphics.endFill();
					this["parent"].x = 0;
					this["parent"].y = 0;
					_showWidth = this.stage.stageWidth;
					_showHeight = this.stage.stageHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0);
				}*/
			}
			sendEvent(SkinEvent.MEDIA_RESIZE, { "w": _showWidth, "h":_showHeight } );
			if (_topMc) {
				_topMc.x = 0;
				_topMc.y = 0;
				_topMc.resize(_showWidth);
			}
			if (_adjustMc) {
				_adjustMc.x = (_showWidth - _adjustMc.width) / 2;
				_adjustMc.y = (_showHeight - _adjustMc.height) / 2;
			}
			if (_handyMc) {
				_handyMc.x = _showWidth - _arrowMc.ArrowWidth - 5;
				_handyMc.y = (_showHeight - _arrowMc.height) / 2;
			}
			if (_controlMc) {
				_controlMc.x = 0;
				_controlMc.y = (this.stage.displayState == StageDisplayState.FULL_SCREEN) ? _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) : _showHeight;
			}
			if (_bigPlayMc) {
				_bigPlayMc.x = 10;
				_bigPlayMc.y = (this.stage.displayState == StageDisplayState.FULL_SCREEN) ? _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) - _bigPlayMc.height - 15 : _showHeight - _bigPlayMc.height - 15;
			}
			if (_leftMc) _leftMc.x = 0;
			if (_rightMc) _rightMc.x = _showWidth - _rightMc.width;
			if (_contrlBgMc) _contrlBgMc.width = _showWidth;
			if (_liveprogressMc) {
				_liveprogressMc.y = -12;
				_liveprogressMc.resize(_showWidth);
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
			if (_playerTipMc) {
				_playerTipMc.resize(_showWidth, (this.stage.displayState == StageDisplayState.FULL_SCREEN) ?  _showHeight - ((_contrlBgMc) ? _contrlBgMc.height : 0) - 13 : _showHeight - 13);
			}
			if (_islayout) {
				sendEvent(SkinEvent.LAYOUT_SUCCESS);
				_islayout = false;
			}
		}
		/**
		 * UI是否满足请求缓冲广告的条件
		 */
		public function get isRequestPatch():Boolean {
			var _standardWidth:Number = 0;
			if (_adjustMc && !_errorMc) {
				_standardWidth = _adjustMc.width;
			} else if (!_adjustMc && _errorMc) {
				_standardWidth = _errorMc.width;
			} else if (_adjustMc && _errorMc) {
				_standardWidth = Math.max(_adjustMc.width, _errorMc.width);				
			}
			if (_containWidth >= _standardWidth) {
				return true;
			} else {
				return false;
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
		 * 进度条是否可用
		 * @param	bool
		 */
		public function isProgressEnable(bool:Boolean = true):void {
			if (_liveprogressMc) {
				if (bool) {
					_liveprogressMc.mouseEnabled = true;
					_liveprogressMc.mouseChildren = true;
					_liveprogressMc.visible = true;
					_liveprogressMc.isEffect = true;
				} else {
					_liveprogressMc.mouseEnabled = false;
					_liveprogressMc.mouseChildren = false;
					_liveprogressMc.visible = false;
					_liveprogressMc.isEffect = false;
				}
			}			
		}
		/**
		 * 显示EPG
		 * @param	obj
		 */
		public function setTimeArea(obj:Object = null):void {			
			if (_liveprogressMc) {
				if (obj == null) {
					_liveprogressMc.isEffect = false;					
					_liveprogressMc.visible = false;
				} else {
					_liveprogressMc.isEffect = true;
					_liveprogressMc.visible = true;
					_liveprogressMc.setTimeArea(obj);
				}				
			}
		}
		/**
		 * 拖动中右键释放
		 */
		public function liveStopDrag():void {
			if (_liveprogressMc) _liveprogressMc.liveStopDrag();
		}
		/**
		 * 播放进度
		 * @param	obj
		 */
		public function setLocation(obj:Object):void {
			if (_liveprogressMc && _liveprogressMc.isEffect) _liveprogressMc.setLocation(obj);
		}
		/**
		 * 是否显示中间缓冲进度  num为0---100整数
		 */
		public function showLoading(bool:Boolean = true, num:Number = -1):void {
			if (bool) {
				if (_loadingMc) {
					_loadingMc.visible = true;
					if (num != -1) _loadingMc.showLoading(num);
				}
				if (_startFreshMc) _startFreshMc.visible = false;
			} else {
				if (_loadingMc) _loadingMc.visible = false;
			}
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
		 * 设置是否跳过片头片尾
		 * @param	bool
		 */
		public function setSelect(bool:Boolean):void {
			if (_adjustMc) _adjustMc.setSelect(bool);
		}
		/**
		 * 设置码流切换
		 * @param	value
		 */
		public function setHd(value:Number):void {
			if (_smoothMc) _smoothMc.setHd(value);
		}
		/**
		 * UI重置
		 */
		public function reset():void {
			if (_liveprogressMc) _liveprogressMc.reset();
			if (_playerTipMc) _playerTipMc.visible = false;
			if (_adjustMc) _adjustMc.visible = false;
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
					_isMouseMove = true;
				} else {
					_controlMc.mouseEnabled = false;
					_controlMc.mouseChildren = false;
					_isMouseMove = false;
				}
			}
		}
		/**
		 * 是否显示切换一代直播提示
		 * @param	bool
		 */
		public function setPlayerTip(obj:Object = null):void {
			if (_playerTipMc) {
				if (obj != null) {
					_playerTipMc.setObj(obj);
					_playerTipMc.visible = true;
				} else {
					_playerTipMc.visible = false;
				}
			}
		}
		
		public function get startFreshMc():MovieClip { return _startFreshMc; }
		
		public function get showWidth():Number { return _showWidth; }
		public function get showHeight():Number { return _showHeight; }
		
		public function get containWidth():Number { return _containWidth; }
		public function set containWidth(value:Number):void {
			_containWidth = value;
		}
		
		public function get containHeight():Number { return _containHeight; }
		public function set containHeight(value:Number):void {
			_containHeight = value;
		}
		
	}

}