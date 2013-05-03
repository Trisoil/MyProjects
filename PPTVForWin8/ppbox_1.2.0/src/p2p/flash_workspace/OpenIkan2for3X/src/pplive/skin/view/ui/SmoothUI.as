package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class SmoothUI extends MovieClip {
		
		private var _status:Number;
		private var _arr:Array;
		
		public function SmoothUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			this.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
			this.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			_arr = [this["smooth_0_mc"], this["smooth_1_mc"], this["smooth_2_mc"]];
			for (var i:int, len:int = _arr.length; i < len; i++) {
				_arr[i].index = i;
				_arr[i].buttonMode = true;
				_arr[i].addEventListener(MouseEvent.CLICK, onClickHandler);
			}
		}
		
		private function onOverHandler(e:MouseEvent):void {
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SHOW_CHANGE));
		}
		
		private function onOutHandler(e:MouseEvent):void {
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_HIDE_CHANGE));
		}
		
		private function onClickHandler(e:MouseEvent):void {
			_status = e.currentTarget.index;
			setState(_status);
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_CHANGE));
		}	
		
		public function setState(curr:int):void {
			for (var i:int, len:int = _arr.length; i < len; i++) {
				if (i == curr) {
					_arr[i].gotoAndStop(2);
				} else {
					_arr[i].gotoAndStop(1);
				}
			}
		}
		
		public function get status():Number { return _status; }		
		public function set status(value:Number):void {
			_status = value;
		}
		
	}

}