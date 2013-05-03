package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.display.StageDisplayState;
	import flash.events.MouseEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class FullCloseUI extends MovieClip {
		
		private var _closebtn:SimpleButton;
		
		public function FullCloseUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			_closebtn = this.getChildByName("CloseBtn") as SimpleButton;
			_closebtn.addEventListener(MouseEvent.CLICK, onCloseFullScreenHandler);
		}
		
		private function onCloseFullScreenHandler(e:MouseEvent):void {
			if (this.stage.displayState == StageDisplayState.FULL_SCREEN) {
				this.stage.displayState = StageDisplayState.NORMAL;
			} else {
				this.stage.displayState = StageDisplayState.FULL_SCREEN;
			}
		}
	}

}