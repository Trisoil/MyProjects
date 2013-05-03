package pplive.skin.view.ui {
	
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class BrightnessUI extends SimpleButton {
		
		public function BrightnessUI() {
			init();
		}
		
		private function init():void {
			this.visible = true;
			this.addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SHOW_BRIGHTNESS));
		}
		
	}

}