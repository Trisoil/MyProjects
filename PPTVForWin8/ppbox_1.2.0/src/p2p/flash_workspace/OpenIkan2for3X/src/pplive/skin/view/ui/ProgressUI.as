package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import flash.geom.Rectangle;
	
	import pplive.skin.view.events.SkinEvent;
	import pplive.skin.view.utils.UtilsMethod;
	/**
	 * ...
	 * @author wuwl
	 */
	public class ProgressUI extends MovieClip {
		
		public var Name:String = "progress";
		public var _timeBg:MovieClip;
		public var _playedDot:MovieClip;
		private var _playedSlider:MovieClip;
		private var _playedBuffer:MovieClip;
		private var _dis:Number = 6;
		private var _playedY:Number;
		private var _playedH:Number;
		private var _position:Number;
		private var _drag_key:Boolean = false;
		public var _buffer_percent:Number = 0;
		public var _position_percent:Number = 0;
		private var _cueList:Array = [];
		
		
		public function ProgressUI() {
			init();
		}
		
		private function init():void {
			_timeBg = this.getChildByName("timeLineSliderBgMc") as MovieClip;
			_playedSlider = this.getChildByName("playedSliderMc") as MovieClip;
			_playedBuffer = this.getChildByName("BufferTimeMc") as MovieClip;
			_timeBg.addEventListener(MouseEvent.CLICK, onTimeClickHandler);
			_timeBg.addEventListener(MouseEvent.MOUSE_OVER, onTimeOverHandler);
			_timeBg.addEventListener(MouseEvent.MOUSE_OUT, onTimeOutHandler);	
			_playedSlider.addEventListener(MouseEvent.CLICK, onTimeClickHandler);
			_playedSlider.addEventListener(MouseEvent.MOUSE_OVER, onTimeOverHandler);
			_playedSlider.addEventListener(MouseEvent.MOUSE_OUT, onTimeOutHandler);
			_playedBuffer.addEventListener(MouseEvent.CLICK, onTimeClickHandler);
			_playedBuffer.addEventListener(MouseEvent.MOUSE_OVER, onTimeOverHandler);
			_playedBuffer.addEventListener(MouseEvent.MOUSE_OUT, onTimeOutHandler);
			_playedDot = this.getChildByName("playedDb") as MovieClip;
			_playedDot.buttonMode = true;
			_playedDot.addEventListener(MouseEvent.MOUSE_DOWN, onPlayedDbDownHandler);
			_playedSlider.x = _dis;
			_playedBuffer.x = _dis;
			_playedDot.x = _dis - _playedDot.width / 2;
			_playedY = _playedSlider.y = _playedBuffer.y;
			_playedH = _playedSlider.height = _playedBuffer.height;
		}
		
		public function updatePlayed():void {
			if (_timeBg.visible) {
				_playedSlider.x = _playedBuffer.x = _dis;
				_playedSlider.y = _playedBuffer.y = _playedY;
				_playedSlider.height = _playedBuffer.height = _playedH;
				_playedBuffer.width = Math.round(_buffer_percent * (_timeBg.width - _dis * 2));
				_playedSlider.width = Math.round(_position_percent * (_timeBg.width - _dis * 2));				
			} else {
				_playedSlider.x = _playedBuffer.x = 0;
				_playedSlider.y = _playedBuffer.y = _timeBg.y - _playedH / 4 + 2;
				_playedSlider.height = _playedBuffer.height = _playedH / 4;
				_playedBuffer.width = Math.round(_buffer_percent * _timeBg.width);
				_playedSlider.width = Math.round(_position_percent * _timeBg.width);
			}
		}
		
		private function onPlayedDbDownHandler(e:MouseEvent):void {
			this.addChild(UtilsMethod.tip);
			_playedDot.startDrag(false, new Rectangle(_playedSlider.x - _playedDot.width / 2, _playedDot.y, _timeBg.width - _dis * 2, 0));
			_drag_key = true;
			_playedDot.addEventListener(MouseEvent.MOUSE_UP, onPlayedDotUpHandler);
			_playedDot.stage.addEventListener(MouseEvent.MOUSE_UP, onPlayedDotUpHandler);
			_playedDot.addEventListener(MouseEvent.MOUSE_MOVE, onPlayedDotMoveHandler);
			_playedDot.stage.addEventListener(MouseEvent.MOUSE_MOVE, onPlayedDotMoveHandler);
		}
		
		private function onPlayedDotUpHandler(e:MouseEvent):void {
			_playedDot.stopDrag();
			_drag_key = false;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_VOD_POSITION, { "value":_position } )); trace("当前位置 >>>>> " + _position);
			_playedDot.removeEventListener(MouseEvent.MOUSE_UP, onPlayedDotUpHandler);
			_playedDot.stage.removeEventListener(MouseEvent.MOUSE_UP, onPlayedDotUpHandler);
			_playedDot.removeEventListener(MouseEvent.MOUSE_MOVE, onPlayedDotMoveHandler);
			_playedDot.stage.removeEventListener(MouseEvent.MOUSE_MOVE, onPlayedDotMoveHandler);
		}
		
		private function onPlayedDotMoveHandler(e:MouseEvent):void {
			if (!_drag_key) return;
			_playedSlider.width = _playedDot.x + _playedDot.width / 2 - _playedSlider.x;
			_position = Math.round(_playedSlider.width / (_timeBg.width - _dis * 2) * 100) / 100;
		}
		
		private function onTimeClickHandler(e:MouseEvent):void {
			var _currX:Number;
			if (this.mouseX < _playedSlider.x) {
				_currX = _playedSlider.x;
			} else if (this.mouseX > _playedSlider.x + (_timeBg.width - _dis * 2)) {
				_currX = _playedSlider.x + (_timeBg.width - _dis * 2);
			} else {
				_currX = this.mouseX;
			}
			_playedDot.x = _currX - _playedDot.width / 2;
			_playedSlider.width = _currX - _playedSlider.x;
			_position = Math.round(_playedSlider.width / (_timeBg.width - _dis * 2) * 100) / 100;			
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_VOD_POSITION, { "value":_position } )); trace("当前位置 >>>>> " + _position);
		}
		
		private function onTimeOverHandler(e:MouseEvent):void {
			this.addChild(UtilsMethod.tip);
			e.target.addEventListener(MouseEvent.MOUSE_MOVE, onTimeMoveHandler);
			e.target.stage.addEventListener(MouseEvent.MOUSE_MOVE, onTimeMoveHandler);
		}
		
		private function onTimeOutHandler(e:MouseEvent):void {
			UtilsMethod.hideTip();
			e.target.removeEventListener(MouseEvent.MOUSE_MOVE, onTimeMoveHandler);
			e.target.stage.removeEventListener(MouseEvent.MOUSE_MOVE, onTimeMoveHandler);
		}
		
		private function onTimeMoveHandler(e:MouseEvent):void {
			var _currX:Number;
			if (this.mouseX < _playedSlider.x) {
				_currX = _playedSlider.x;
			} else if (this.mouseX > _playedSlider.x + (_timeBg.width - _dis * 2)) {
				_currX = _playedSlider.x + (_timeBg.width - _dis * 2);
			} else {
				_currX = this.mouseX;
			}
			var num:Number = Math.round((_currX - _playedSlider.x) / (_timeBg.width - _dis * 2) * 100) / 100;
			if (UtilsMethod.tip) {
				if (UtilsMethod.duration != 0) {
					UtilsMethod.tip.text = UtilsMethod.showTimeFormat(num * UtilsMethod.duration);
				} else {
					UtilsMethod.tip.text = UtilsMethod.showTimeFormat();
				}				
				var currX:Number;
				if (this.mouseX - 25 < 0) {
					currX = this.mouseX;
				} else if (this.mouseX - 25 + UtilsMethod.tip.width > _timeBg.width) {
					currX =  this.mouseX - UtilsMethod.tip.width;
				} else {
					currX = this.mouseX - 25;
				}
				UtilsMethod.tip.resize(currX, _timeBg.y - 20);
				UtilsMethod.tip.show();
			}
		}
		private function onTimeCueOverHandler(e:MouseEvent):void{
			if (UtilsMethod.tip) {
				for(var i:int = 0 ;i<_cueList.length; i ++){
					if(i == 0 && _cueList[i]["obj"] == e.currentTarget){
						UtilsMethod.tip.text = '片头';
					} else if(i == _cueList.length - 1 && _cueList[i]["obj"] == e.currentTarget){
						UtilsMethod.tip.text = '片尾';
					}
				}
				var currX:Number;
				if (this.mouseX - 25 < 0) {
					currX = this.mouseX;
				} else if (this.mouseX - 25 + UtilsMethod.tip.width > _timeBg.width) {
					currX =  this.mouseX - UtilsMethod.tip.width;
				} else {
					currX = this.mouseX - 25;
				}
				UtilsMethod.tip.resize(currX, _timeBg.y - 20);
				UtilsMethod.tip.show();
			}
		}
		/**
		 * 设置片头片尾
		 * @param cueList
		 * 
		 */		
		public function setCuePoint(cueList:Array = null):void{
			for each(var cu:Object in _cueList){
				cu["obj"].removeEventListener(MouseEvent.MOUSE_OVER,onTimeOverHandler);
				cu["obj"].removeEventListener(MouseEvent.MOUSE_OUT,onTimeOutHandler);
				this.removeChild(cu["obj"]);
				cu["obj"] = null;
			}
			_cueList = [];
			for(var i:int = 0; i < cueList.length; i++){
				if(Number(cueList[i]) > 0){
					var cueP:CuePointUI = new CuePointUI();
					cueP.y = 2.1;
					cueP.addEventListener(MouseEvent.MOUSE_OVER,onTimeCueOverHandler);
					cueP.addEventListener(MouseEvent.MOUSE_OUT,onTimeOutHandler);
					cueP.addEventListener(MouseEvent.CLICK, onTimeClickHandler);
					_cueList.push({'obj':cueP,'time':Number(cueList[i])});
					cueP.alpha = .9;
					this.addChild(cueP);
					this.swapChildren(cueP,_playedDot);
				}
			}
			resize();
		}
		public function showCuePoint(bool:Boolean = true):void{
			if(_cueList.length <= 0 )return;
			for(var i:int = 0; i < _cueList.length; i++){
				_cueList[i]['obj'].visible = bool ? true : false;
			}
		}
		public function resize():void{
			for(var i:int = 0; i < _cueList.length; i++){
				_cueList[i]['obj'].x = Math.round(Number(_cueList[i]['time']) / UtilsMethod.duration * (_timeBg.width - _dis * 2));
			}
		}
		public function sliderPosition(curr:Number):void {
			_position_percent = curr / UtilsMethod.duration;
			SliderLocation(_position_percent);
		}
		
		public function bufferPosition(num:Number):void {
			_buffer_percent = num;
			BufferLocation(_buffer_percent);
		}
		
		private function SliderLocation(num:Number):void {
			if (_drag_key) return;
			if (_timeBg.visible) {
				_playedSlider.x = _dis;
				_playedSlider.width = Math.round(num * (_timeBg.width - _dis * 2));
			} else {
				_playedSlider.x = 0;
				_playedSlider.width = Math.round(num * _timeBg.width);
			}
			_playedDot.x = _playedSlider.x + _playedSlider.width - _playedDot.width / 2;
		}
		
		private function BufferLocation(num:Number):void {
			if (_timeBg.visible) {
				_playedBuffer.x = _dis;
				_playedBuffer.width = Math.round(num * (_timeBg.width - _dis * 2));
			} else {
				_playedBuffer.x = 0;
				_playedBuffer.width = Math.round(num * _timeBg.width);
			}
		}
		
		public function isVisible(bool:Boolean = true):void {
			this.visible = bool;
		}
		public function reset():void {
			SliderLocation(0)
			_buffer_percent = 0;
			_position_percent = 0;
		}
		
	}

}