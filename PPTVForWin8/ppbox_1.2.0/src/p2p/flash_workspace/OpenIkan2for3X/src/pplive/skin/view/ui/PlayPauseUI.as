package pplive.skin.view.ui{
	
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class PlayPauseUI extends MovieClip {
		
		private var _playbtn:SimpleButton;
		private var _pausebtn:SimpleButton;
		private var _status:String = "play";
		
		public function PlayPauseUI() {			
			_playbtn = this.getChildByName("PlayBtn") as SimpleButton;
			_pausebtn = this.getChildByName("PauseBtn") as SimpleButton;			
			init();
		}
		
		private function init():void {
			_playbtn.visible = false;
			_pausebtn.visible = true;
			_playbtn.addEventListener(MouseEvent.CLICK, onClickHandler);
			_pausebtn.addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			switch (e.currentTarget) {
				case _playbtn:
					setPlayState();
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
					break;
				case _pausebtn:
					setPlayState(false);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PAUSE));
					break;
			}		
		}
		
		public function setPlayState(bool:Boolean = true):void {
			if (bool) {
				_playbtn.visible = false;
				_pausebtn.visible = true;
				_status = "play";
			} else {
				_playbtn.visible = true;
				_pausebtn.visible = false;
				_status = "pause";
			}
		}
		
		public function get status():String { return _status; }
		
	}

}