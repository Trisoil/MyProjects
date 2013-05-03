package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class LightUI extends MovieClip {
		
		private var _type:String = "open";
		
		public function LightUI() {
			init();
		}
		
		private function init():void {
			this["closeLightBtn"].visible = true;
			this["openLightBtn"].visible = false;
			this["closeLightBtn"].addEventListener(MouseEvent.CLICK, onClickHandler);
			this["openLightBtn"].addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			switch(e.currentTarget) {
				case this["closeLightBtn"]:
					_type = "close";
					this["closeLightBtn"].visible = false;
					this["openLightBtn"].visible = true;
					break;
				case this["openLightBtn"]:
					_type = "open";
					this["closeLightBtn"].visible = true;
					this["openLightBtn"].visible = false;
					break;
			}
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_LIGHT, { "value":_type } ));
		}
		
	}

}