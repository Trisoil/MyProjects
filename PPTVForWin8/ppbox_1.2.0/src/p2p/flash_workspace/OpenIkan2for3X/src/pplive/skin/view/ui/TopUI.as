package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class TopUI extends MovieClip {
		
		private var _title_txt:TextField;
		
		public function TopUI() {
			init();
		}
		
		private function init():void {
			_title_txt = new TextField();
			this.addChild(_title_txt);
			_title_txt.text = "";
			_title_txt.setTextFormat(txtFormat());
			_title_txt.x = 7;
			_title_txt.y = 7;
			_title_txt.autoSize = TextFieldAutoSize.LEFT;
			_title_txt.textColor = 0xFFFFFF;
			this.mouseEnabled = false;
			this.mouseChildren = false;
		}
		
		public function setTitle(str:String):void {
			_title_txt.text = str;
			_title_txt.setTextFormat(txtFormat());
		}
		
		private function txtFormat():TextFormat{
			var _txtFormat:TextFormat = new TextFormat();
			_txtFormat.font = "Arial";
			_txtFormat.bold = false;
			_txtFormat.size = 14;
			return _txtFormat;
		}
		
	}

}