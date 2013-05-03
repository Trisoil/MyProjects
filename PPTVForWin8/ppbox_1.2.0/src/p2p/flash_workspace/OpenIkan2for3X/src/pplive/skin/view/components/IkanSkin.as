package pplive.skin.view.components {
	
	import flash.display.DisplayObject;
	import flash.display.DisplayObjectContainer;
	import flash.display.Loader;
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.StageDisplayState;
	import flash.events.Event;
	import flash.external.ExternalInterface;
	import flash.net.URLRequest;
	import flash.utils.clearTimeout;
	import flash.utils.getDefinitionByName;
	import flash.utils.setTimeout;
	
	import pplive.skin.view.events.SkinEvent;
	import pplive.skin.view.ui.ArrowUI;
	import pplive.skin.view.ui.PlayerTipUI;
	import pplive.skin.view.utils.UtilsMethod;
	import pplive.skin.view.ui.SharePanelUI;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class IkanSkin extends BaseSkin{

		public var _progressMc:MovieClip;
		
		
		
		private var _totalDuration:Number = 0;
		private var _currNum:Number;
		private var _bufferNum:Number;
		private var _isDoubleClick:Boolean = false;
		private var _doubleInter:uint;
		
		//和直播的
		private var _cutImage:Sprite;
		private var _cutImageLoader:Loader;
		public var movieCon:MovieClip;
		
		public function IkanSkin() {
			super();
			_cutImage = new Sprite();
			this.addChild(_cutImage);
			movieCon = new MovieClip();
			this.addChild(movieCon);
		}
		public override function setData(list:XML):void{
			for (var k:int = 0, index:int = list.children().length(); k < index; k++) {
				if (list.child(k).localName() == "controlUI") {
					_controlMc = new MovieClip();
					this.addChild(_controlMc);
					_controlMc.x = Number(list.child("controlUI").@x);
					_controlMc.y = Number(list.child("controlUI").@y);
					
					for (var s:int = 0, total:int = list.child("controlUI").children().length(); s < total; s++) {
						if (list.child("controlUI").child(s).localName() != "leftUI" && list.child("controlUI").child(s).localName() != "rightUI") {
							if (list.child("controlUI").child(s).children().length() != 0) {
								showUI(_conArr[s], _controlMc, list.child("controlUI").child(s));
								_elementNum++;
							}							
						}
					}
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
				} else if (list.child(k).localName() == "handyUI" && list.child("handyUI").children().length() != 0) {
					_handyMc = new MovieClip();
					this.addChild(_handyMc);
					_handyMc.x = Number(list.child("handyUI").@x);
					_handyMc.y = Number(list.child("handyUI").@y);
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
					_handyShareMc = new SharePanelUI();
					_handyShareMc.openPanel(SharePanelUI.SHARE_PIC);
					this.addChild(_handyShareMc);
					_handyShareMc.visible = false;
				} else {
					if (list.child(k).children().length() != 0) {
						showUI(_uiArr[k], this, list.child(k));
						_elementNum++; 
					}
				}
			}
			this.addEventListener(Event.ENTER_FRAME, onEnterFrameHandler);
			ExternalInterface.call("console.log", "_elementNum  " + _elementNum);
			//测试
//			_handyShareMc = new SharePanelUI();
//			_handyShareMc.graphics.beginFill(0xFF0000);
//			_handyShareMc.graphics.drawRect(0,0,480,380);
//			_handyShareMc.graphics.endFill();
//			_handyShareMc.openPanel(SharePanelUI.SHARE_PIC);
//			var ptu:PlayerTipUI = new PlayerTipUI();
//			var dd:Object = {hasPlugin:1,txt:'<div>提示文字提示文字提示文字提示文字<a href="#">提示文字提示文字</a></div>',interval:3000,times:1};
//			ptu.setObj(dd);
//			this.addChild(_handyShareMc);
		}
		/**
		 * 停止重新设置属性
		 * @param bool
		 * 
		 */		
		public override function reset(bool:Boolean = true):void{
			super.reset(bool);
			if (_timeMc) _timeMc.setTime(0);
			if (progressMc) {
				progressMc.bufferPosition(_bufferNum);
				progressMc.sliderPosition(_currNum);
			}
			_bufferNum = 0;
			_currNum = 0;
		}
		/**
		 * 显示/隐藏后推荐
		 * @param obj 为空隐藏
		 * 
		 */	
		public override function showRecommend(obj:Object = null):void{
			if (_recommendMc){ 
				if(obj){
					_recommendMc.showRecommend(obj, _showWidth, _showHeight + 2);
				} else {
					_recommendMc.hideRecommend();
				}
			}
		}
		/**
		 * 显示/隐藏截图
		 * @param fixurl 为空隐藏
		 * 
		 */
		public function loadScreenshot(url:String = ''):void{
			if (url == null || url == ""){
				_cutImage.visible = false;
				return;
			}
			_cutImage.visible = true;
			while (_cutImage.numChildren > 0) {
				_cutImage.removeChildAt(0);
			}
			var _loader:Loader = new Loader();
			_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onImageEndHandler);
			_loader.load(new URLRequest(url));
			function onImageEndHandler(e:Event):void {
				_cutImage.addChild(_loader);
				if (_startFreshMc) _startFreshMc.visible = false;
				_cutImageLoader = _loader;
				cutImageResize(_loader);
			}
		}
		private function cutImageResize(ld:Loader):void {
			var per:Number = ld.width / ld.height;
			if (_cutImage.stage.stageWidth / _cutImage.stage.stageHeight > per) {
				ld.height = _cutImage.stage.stageHeight;
				ld.width = Math.round(ld.height * per);
			} else {
				ld.width = _cutImage.stage.stageWidth;
				ld.height = Math.round(ld.width / per);
			}
			ld.x = Math.round((_cutImage.stage.stageWidth - ld.width) / 2);
			ld.y = Math.round((_cutImage.stage.stageHeight - ld.height) / 2);
		}
		override public function resize():void{
			super.resize();
			movieCon.graphics.clear();
			movieCon.graphics.beginFill(0xff00ff,0);
			movieCon.graphics.drawRect(0,0,_width_,_height_);
			movieCon.graphics.endFill();
		}
		/**
		 * 设置进度、时间的显示
		 * @param	bool
		 */
		public function setPlayerProgress(bool:Boolean=true):void {
//			if (_progressMc) {
//				if (bool) {
//					_progressMc.isVisible();
//				} else {
//					_progressMc.isVisible(false);
//				}
//			}
//			if (_timeMc) {
//				if (bool) {
//					_timeMc.isVisible();
//				} else {
//					_timeMc.isVisible(false);
//				}
//			}
		}
		/**
		 * 设置片头片尾显示初始化
		 */
		public function setCuePoint(arr:Array = null):void {
			if(arr){
				if(progressMc)progressMc.setCuePoint(arr);
			}
		}
		/**
		 * 设置是否跳过片头片尾
		 * @param	bool
		 */
		public override function setSelect(bool:Boolean):void {
			if (_adjustMc) _adjustMc.setSelect(bool);
			if(progressMc)progressMc.showCuePoint(bool);
		}
		/**
		 * 显示标题栏
		 */
		public function showTitle(str:String):void {
//			if (_topMc) _topMc.isVisible(str);
		}
		/**
		 * 设置当前播放总时间
		 */		
		public function set totalDuration(value:Number):void {
			_totalDuration = value;
			UtilsMethod.duration = _totalDuration;
		}
		/**
		 * 显示加载进度  num:Number 为当前加载百分比
		 */
		public function showBuffer(num:Number):void {
			if (num > 1) num = 1;
			_bufferNum = num;
			if (_progressMc) _progressMc.bufferPosition(_bufferNum);
		}
		/**
		 * 显示时间text  num:Number 均为自然整数
		 */
		public function showTime(num:Number):void {
			if (num > _totalDuration) num = _totalDuration;
			_currNum = num;
			if (_progressMc) _progressMc.sliderPosition(_currNum);
			if (_timeMc) _timeMc.setTime(_currNum);
		}
		/**
		 * 是否暂停或者播放
		 */
		public function playState(bool:Boolean = true):void {
//			if (bool) {
//				if (_bigPlayMc) _bigPlayMc.isVisible(false);
//				if (_playMc) _playMc.isVisible(false);
//				if (_pauseMc) _pauseMc.isVisible();
//			} else {
//				if (_bigPlayMc) _bigPlayMc.isVisible();
//				if (_playMc) _playMc.isVisible();
//				if (_pauseMc) _pauseMc.isVisible(false);
//			}
		}
		/**
		 * stageVideo UI
		 */
//		public function showStageVideo(bool:Boolean = true):void {
//			if(bool){
//				_brightnessMc.alpha = .3;
//				_brightnessMc.mouseEnabled = false;
//				_brightnessMc.useHandCursor = false;
//				_adjustMc.hideTab(0);
//			} else {
//				_brightnessMc.alpha = 1;
//				_brightnessMc.mouseEnabled = true;
//				_brightnessMc.useHandCursor = true;
//			}
//		}
		/**
		 * 显示分享
		 * @param	obj
		 */
		public function loadShare(obj:Object, bool:Boolean = false):void {
//			if (obj == null) return;
//			CommonMethod.curr_link = obj["data"]["pptv"]["link"];
//			CommonMethod.curr_sharelink = obj["data"]["pptv"]["sharelink"];
//			CommonMethod.curr_img = obj["data"]["pptv"]["fiximg"];
//			CommonMethod.curr_title = obj["data"]["pptv"]["title"];
//			if (_shareMc) {
//				if (bool == true) {
//					_shareMc.isVisible();
//				} else {
//					_shareMc.isVisible(false);
//				}
//				_shareMc.showAddress(CommonMethod.curr_link, CommonMethod.curr_sharelink);
//				_shareMc.showImage(CommonMethod.curr_img);
//				_shareMc.showTitle(CommonMethod.curr_title);
//				_shareMc.showIcon(obj["data"]["share"] as Array);
//			}
//			if (_shareTopMc) {
//				_shareTopMc.showTitle(CommonMethod.curr_title);
//			}
		}
		/**
		 * 检测单双击事件
		 * @param	obj
		 */
		private function checkDoubleClick(bool:Boolean):void {
			_isDoubleClick = false;
			if (_doubleInter) clearTimeout(_doubleInter);
			if (bool == true){
				if (_playpauseMc.status == "play") {
					setPlayState(false);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PAUSE));
				} else {
					setPlayState();
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
				}
			}
		}
		/**
		 * 显示分享
		 * @param	桌面单击
		 */
		public function onStageClick():void {
			if (_isDoubleClick) {
				checkDoubleClick(false);
				if (stage.displayState == StageDisplayState.FULL_SCREEN) {
					stage.displayState = StageDisplayState.NORMAL;
				} else {
					stage.displayState = StageDisplayState.FULL_SCREEN;
				}
			} else {
				_isDoubleClick = true;
				_doubleInter = setTimeout(checkDoubleClick, 500, true);
			}
		}
		/**
		 * 是否存在PPTV插件
		 * @param	obj
		 */
		public override function get containWidth():Number { return _containWidth; }
		public override function set containWidth(value:Number):void {
			_containWidth = value;
		}
		public override function get progressMc():MovieClip{return _progressMc};
		public override function set progressMc(v:MovieClip):void{_progressMc = v};
		public function hasPPTV(bool:Boolean = false):void {
			//CommonMethod.isPPTV = bool;
		}
		
		public override function get startFreshMc():MovieClip { return _startFreshMc; }
		
		public override function get showWidth():Number { return _showWidth; }
		public override function get showHeight():Number { return _showHeight; }
		
		
		public override function get containHeight():Number { return _containHeight; }
		public override function set containHeight(value:Number):void {
			_containHeight = value;
		}
	}

}