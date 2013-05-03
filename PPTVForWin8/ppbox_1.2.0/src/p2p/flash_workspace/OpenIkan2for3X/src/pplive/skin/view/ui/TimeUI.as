package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import pplive.skin.view.utils.UtilsMethod;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class TimeUI extends MovieClip {
		
		private var _time_txt:TextField;
		
		public function TimeUI() {
			_time_txt = this["playedTime"] as TextField;
			_time_txt.autoSize = TextFieldAutoSize.LEFT;
			this.visible = true;
			setTime();
		}
		
		public function setTime(curr:Number = 0):void {
			if (curr == 0) {
				_time_txt.text = "00:00 | 00:00";
				return;
			}
			_time_txt.text = UtilsMethod.showTimeFormat(curr) + " | " + UtilsMethod.showTimeFormat(UtilsMethod.duration);
		}
		
	}

}