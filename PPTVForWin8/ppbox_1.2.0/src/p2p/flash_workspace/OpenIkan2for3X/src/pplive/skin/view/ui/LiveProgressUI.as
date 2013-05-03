package pplive.skin.view.ui {
	
	import flash.display.*;
	import flash.events.MouseEvent;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class LiveProgressUI extends MovieClip {
		
		public var _timeBg:MovieClip;
		public var _playedDot:MovieClip;
		public var _dragDot:MovieClip;
		private var _playedSlider:MovieClip;
		private var _playedBuffer:MovieClip;
		private var _startTime_txt:TextField;
		public var _endTime_txt:TextField;
		private var _standardWidth:Number;
		private var _bool:Boolean = false;
		private var _start_time:Number;
		private var _end_time:Number;
		private var _curr_time:Number;
		private var _posi_time:Number;
		private var _dragTime:Number = 0;
		private var _currWidth:Number;
		private var _offset:Number;
		private var _drag_key:Boolean = false;
		private var _updateEnd:Number = 0;
		
		public function LiveProgressUI() {
			init();
		}
		
		private function init():void {
			_timeBg = this.getChildByName("bg_mc") as MovieClip;
			_playedSlider = this.getChildByName("playedSliderMc") as MovieClip;
			_playedBuffer = this.getChildByName("bufferTimeMc") as MovieClip;
			_playedDot = this.getChildByName("playedDb") as MovieClip;
			_dragDot = this.getChildByName("DragMc") as MovieClip;
			_startTime_txt = this.getChildByName("startTime") as TextField;
			_endTime_txt = this.getChildByName("endTime") as TextField;
			_startTime_txt.x = 0;
			_playedSlider.x = _playedBuffer.x = _startTime_txt.x + _startTime_txt.width;
			_dragDot.buttonMode = true;
			_dragDot.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
			_dragDot.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			_dragDot.addEventListener(MouseEvent.CLICK, onClickHandler);
			_playedBuffer.buttonMode = true;
			_playedBuffer.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
			_playedBuffer.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			_playedBuffer.addEventListener(MouseEvent.CLICK, onClickHandler);
			_playedSlider.buttonMode = true;
			_playedSlider.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
			_playedSlider.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			_playedSlider.addEventListener(MouseEvent.CLICK, onClickHandler);
			_playedDot.buttonMode = true;
			_playedDot.mouseChildren = false;
			_playedDot.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
			_playedDot.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			_playedDot.addEventListener(MouseEvent.MOUSE_DOWN, onDownHandler);
			_standardWidth = _timeBg.width - _startTime_txt.width - _endTime_txt.width;
		}
		
		private function onOverHandler(e:MouseEvent):void {
			var timeStr:String;
			switch (e.target) {
				case _playedDot:
					timeStr = setTimeFormat(Math.floor((_end_time - _start_time) * _playedSlider.width / _standardWidth) + _start_time);	
					break;
				case _dragDot:
					timeStr = setTimeFormat(_curr_time);
					break;
				case _playedBuffer:
				case _playedSlider:
					e.target.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
					break;
			}
			function onMoveHandler(e:MouseEvent):void {
				//timeStr = setTimeFormat(Math.round((_end_time - _start_time) * (CommonMethod.tip["parent"].mouseX - _playedSlider.x) / _standardWidth) + _start_time);
				//CommonMethod.tip.text = timeStr;
				//var currX:Number;
				//if (CommonMethod.tip["parent"].mouseX - 25 < 0) {
					//currX = CommonMethod.tip["parent"].mouseX;
				//} else if (CommonMethod.tip["parent"].mouseX - 25 + CommonMethod.tip.width > _timeBg.width) {
					//currX =  CommonMethod.tip["parent"].mouseX - CommonMethod.tip.width;
				//} else {
					//currX = CommonMethod.tip["parent"].mouseX - 25;
				//}
				//CommonMethod.tip.resize(currX, _timeBg.y - 20);
				//CommonMethod.tip.show();
			}			
		}
		
		private function onOutHandler(e:MouseEvent):void {
			if (e.target == _playedBuffer || e.target == _playedSlider) {
				e.target.removeEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			}
		}
		
		private function onClickHandler(e:MouseEvent):void {
			_playedDot.x = this.mouseX - _playedDot.width / 2;
			switch (e.currentTarget) {
				case _playedSlider:
				case _playedBuffer:
					_playedSlider.width = this.mouseX - _playedSlider.x;
					break;
				case _dragDot:
					_playedSlider.width = _playedBuffer.width;
					break;
			}
			_dragTime = (_end_time - _start_time) * _playedSlider.width / _standardWidth + _start_time;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_LIVE_POSITION, { "value":setUTCTime(_dragTime) } ));			
			_bool =  false;
		}
		
		private function onDownHandler(e:MouseEvent):void {
			_bool = true;
			_drag_key = true;
			_playedDot.startDrag(false, new Rectangle(_playedSlider.x - _playedDot.width / 2, _playedDot.y, _standardWidth, 0));
			_playedDot.addEventListener(MouseEvent.MOUSE_UP, onUpHandler);
			_playedDot.stage.addEventListener(MouseEvent.MOUSE_UP, onUpHandler);
			_playedDot.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			_playedDot.stage.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
		}
		
		private function onMoveHandler(e:MouseEvent):void {
			if (!_drag_key) return;
			if (_playedDot.x > _playedBuffer.x + _playedBuffer.width) {
				_playedSlider.width = _playedBuffer.width;
			} else {
				if (this.mouseX - _playedBuffer.x > _standardWidth) {
					_playedSlider.width = _standardWidth;
				} else {
					_playedSlider.width = this.mouseX - _playedBuffer.x;
				}		
			}
		}
		
		private function onUpHandler(e:MouseEvent = null):void {
			_drag_key = false;
			if (_playedDot.x >= _playedBuffer.x + _playedBuffer.width) {
				_playedDot.x = _playedBuffer.x + _playedBuffer.width - _playedDot.width / 2;
			}
			_playedSlider.width = _playedDot.x - _playedSlider.x + _playedDot.width / 2;
			_dragTime = (_end_time - _start_time) * _playedSlider.width / _standardWidth + _start_time;
			_bool =  false;
			_playedDot.stopDrag();
			_playedDot.removeEventListener(MouseEvent.MOUSE_UP, onUpHandler);
			_playedDot.stage.removeEventListener(MouseEvent.MOUSE_UP, onUpHandler);
			_playedDot.removeEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			_playedDot.stage.removeEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_LIVE_POSITION, { "value":setUTCTime(_dragTime) } ));			
		}
		
		public function setLocation(obj:Object):void {			
			if (obj["curr"] && obj["posi"]) {
				_curr_time = setCurrTime(Number(obj["curr"]));
				_posi_time = setCurrTime(Number(obj["posi"]));	
				resize();
			}
		}
		
		public function resize():void {
			_standardWidth = _timeBg.width - _startTime_txt.width - _endTime_txt.width;			
			if (_curr_time >= _start_time && _curr_time < _end_time) {
				_playedBuffer.width = _standardWidth * (_curr_time - _start_time) / (_end_time - _start_time);
			} else {
				if (_curr_time >= _end_time) {
					_playedBuffer.width = _standardWidth;
				} else {
					_playedBuffer.width = _playedSlider.width;
				}
			}
			_dragDot.x = _playedSlider.x + _playedBuffer.width - _dragDot.width / 2;
			if (_posi_time <= _end_time) {
				if (!_bool) {
					_playedSlider.width = _standardWidth * (_posi_time - _start_time) / (_end_time - _start_time);	
					_playedDot.x = _playedSlider.x + _playedSlider.width - _playedDot.width / 2;
				}
			}
		}
		
		public function liveStopDrag():void {
			if (_bool) {
				onUpHandler();
			}
		}
		
		public function reset():void {
			_startTime_txt.text = "00:00";
			_endTime_txt.text = "00:00";
			_playedSlider.x = _playedBuffer.x = _startTime_txt.x + _startTime_txt.width;
			_dragDot.visible = false;
			_playedDot.x = _playedSlider.x - _playedDot.width / 2;
			_dragDot.x = _playedSlider.x - _dragDot.width / 2;
			_playedSlider.width = _playedBuffer.width = 0;
		}
		
		public function setTimeArea(obj:Object):void {
			if (_drag_key) return;
			_dragDot.visible = true;
			_standardWidth = _timeBg.width - _startTime_txt.width - _endTime_txt.width;
			if (obj["start"] &&  obj["end"]) {
				_start_time = setCurrTime(Number(obj["start"]));
				_end_time = setCurrTime(Number(obj["end"]));
				_startTime_txt.text = setTimeFormat(_start_time);
				_endTime_txt.text = setTimeFormat(_end_time);
			}
		}
		
		private function setTimeFormat(num:Number):String {
			var _hour_str:String;
			var _min_str:String;
			_hour_str = (new Date(num).getHours() < 10) ? "0" + new Date(num).getHours() : "" + new Date(num).getHours();
			_min_str = (new Date(num).getMinutes() < 10) ? "0" + new Date(num).getMinutes() : "" + new Date(num).getMinutes();
			return _hour_str + ":" + _min_str;
		}
		
		private function setCurrTime(num:Number):Number {
			/*var _date:Date = new Date(num);
			_offset = _date.getTimezoneOffset() * 60;
			var newNum:Number = (_offset < 0) ? num - _offset : num + _offset;
			return newNum * 1000;*/
			return num * 1000;
		}
		
		private function setUTCTime(num:Number):Number {
			/*var newNum:Number = (_offset < 0) ? num / 1000 + _offset : num / 1000 - _offset;			
			return Math.floor(newNum);*/
			return Math.floor(num / 1000);
		}
		
	}

}