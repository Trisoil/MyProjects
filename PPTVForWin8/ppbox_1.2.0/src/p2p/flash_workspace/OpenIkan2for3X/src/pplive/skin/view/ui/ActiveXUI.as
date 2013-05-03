package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class ActiveXUI extends MovieClip {
		
		public function ActiveXUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			this["info_mc"].buttonMode = true;
			this["info_mc"].addEventListener(MouseEvent.CLICK, onClickHandler);
			this["close_btn"].buttonMode = true;
			this["close_btn"].addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			switch(e.currentTarget) {
				case this["info_mc"]:
					this.dispatchEvent(new SkinEvent(SkinEvent.PLUGIN_VERSION));
					break;
				case this["close_btn"]:
					this.visible = false;
					break;
			}
		}
		
		public function resize(w:Number, h:Number):void {
			this["bg_mc"].width = w;
			this["info_mc"].x = Math.round((this["bg_mc"].width - this["info_mc"].width) / 2);
			this["close_btn"].x = this["info_mc"].x + this["info_mc"].width;
			this.y = h - this.height;
		}
		
	}

}