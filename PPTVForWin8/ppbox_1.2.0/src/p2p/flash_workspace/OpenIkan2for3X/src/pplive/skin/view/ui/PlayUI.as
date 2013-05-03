package pplive.skin.view.ui{
	
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class PlayUI extends SimpleButton {
		
		public function PlayUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			this.addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			this.visible = false;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
		}
		
	}

}