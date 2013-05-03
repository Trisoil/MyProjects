package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class LoadingUI extends MovieClip {
		
		private var _buffer_txt:TextField;
		
		public function LoadingUI() {
			init();
		}
		
		private function init():void {
			_buffer_txt = this["bufferPrecent"] as TextField;
			this.visible = false;
			this.mouseChildren = false;
			this.mouseEnabled = false;
		}
		
		public function showLoading(num:Number):void {
			if (num >= 100) {
				this.visible = false;
			} else {
				this.visible = true;
				_buffer_txt.text = num + "%";
				_buffer_txt.setTextFormat(txtFormat());
			}
		}
		
		private function txtFormat():TextFormat{
			var _txtFormat:TextFormat = new TextFormat();
			_txtFormat.font = "Tahoma";
			_txtFormat.bold = true;
			_txtFormat.size = 16;
			return _txtFormat;
		}
		
	}

}