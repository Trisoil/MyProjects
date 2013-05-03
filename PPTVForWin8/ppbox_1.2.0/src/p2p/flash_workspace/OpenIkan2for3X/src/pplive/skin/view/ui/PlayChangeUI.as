package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class PlayChangeUI extends MovieClip {
		
		private var _check_mc:MovieClip;
		private var _isSelect:Boolean = false;
		private var _confirm_btn:SimpleButton;
		private var _cancel_btn:SimpleButton;
		
		public function PlayChangeUI() {
			_check_mc = this.getChildByName("check_mc") as MovieClip;
			_check_mc.buttonMode = true;
			_check_mc.addEventListener(MouseEvent.CLICK, onCheckHandler);
			_confirm_btn = this.getChildByName("confirm_btn") as SimpleButton;
			_cancel_btn = this.getChildByName("cancel_btn") as SimpleButton;
			_confirm_btn.addEventListener(MouseEvent.CLICK, onClickHandler);
			_cancel_btn.addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			switch(e.target) {
				case _confirm_btn:
					_isSelect = (_check_mc.currentFrame == 1) ? false : true;
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SKIP_PLAY, { "bool":_isSelect } ));
					break;
				case _cancel_btn:
					isSelect = _isSelect;
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SKIP_PLAY));
					break;
			}
		}
		
		private function onCheckHandler(e:MouseEvent):void {
			if (_check_mc.currentFrame == 1) {
				_check_mc.gotoAndStop(2);
			} else {
				_check_mc.gotoAndStop(1);
			}
		}
		
		public function set isSelect(value:Boolean):void {
			_isSelect = value;
			if (_isSelect) {
				_check_mc.gotoAndStop(2);
			} else {
				_check_mc.gotoAndStop(1);
			}
		}
		
		public function get isSelect():Boolean { return _isSelect; }
		
	}

}