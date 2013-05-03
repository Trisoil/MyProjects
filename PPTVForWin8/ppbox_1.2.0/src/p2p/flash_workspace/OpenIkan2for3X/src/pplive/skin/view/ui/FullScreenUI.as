package pplive.skin.view.ui {
	
	import flash.display.SimpleButton;
	import flash.display.StageDisplayState;
	import flash.events.MouseEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class FullScreenUI extends SimpleButton {
		
		public function FullScreenUI() {
			init();
		}
		
		private function init():void {
			this.visible = true;
			this.addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			if (this.stage.displayState == StageDisplayState.FULL_SCREEN) {
				this.stage.displayState = StageDisplayState.NORMAL;
			} else {
				this.stage.displayState = StageDisplayState.FULL_SCREEN;
			}
		}
		
	}

}