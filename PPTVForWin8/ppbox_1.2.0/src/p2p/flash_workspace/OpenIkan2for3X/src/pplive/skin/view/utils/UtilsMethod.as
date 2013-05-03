package pplive.skin.view.utils {	
	import flash.external.ExternalInterface;
	import flash.net.URLRequest;
	import flash.net.navigateToURL;
	
	import pplive.ui.InfoTip;

	/**
	 * ...
	 * @author wuwl
	 */
	public class UtilsMethod {		
		
		
		public static var curr_link:String = null;
		public static var curr_sharelink:String = null;
		public static var curr_title:String = null;
		public static var curr_img:String = null;
		
		
		public static var duration:Number = 0;
		public static var tip:InfoTip = new InfoTip();
		
		public static var effectArr:Array = null;
		/**
		 * 格式化时间
		 * @param	num
		 * @return
		 */
		public static function showTip(str:String, target:*):void {
			if (tip) {
				tip.text = str;
				tip.resize(0,0);
				tip.resize((target.x + tip.width) > target["parent"].width ? target.x + target.width - tip.width : target.x + 10, target.y - 20);
				tip.show();
			}
		}
		/**
		 * 打开新窗口
		 * @param	url
		 * @param	window
		 */
		public static function getURL(url:String, window:String = "_blank"):void {
			if (url == null || url == "") return;
			try {
				if (ExternalInterface.available) {
					ExternalInterface.call("window.open", url, window);
				} else {
					navigateToURL(new URLRequest(url), window);
				}
			} catch (e:Error) {
				//skin.showError( { "title":"当前网页不允许新开窗口，请复制以下地址：" + url } );
			};
		}
		public static function hideTip():void {
			if (tip) {
				tip.text = "";
				tip.hide();
			}
		}
		public static function showTimeFormat(num:Number = 0):String {
        	var hou:Number;
            var min:Number;
			var sec:Number;
			var houStr:String;
			var minStr:String;
			var secStr:String;
			var time:String;
			hou = Math.floor(num / 3600);	
			min = Math.floor(num % 3600 / 60);		
			sec = Math.floor(num % 3600 % 60);
			if (hou<10) {
				houStr = String("0" + hou);
			} else {
				houStr = String(hou);
			}
			if (min<10) {
				minStr = String("0" + min);
			} else {
				minStr = String(min);
			}
			if (sec<10) {
				secStr = String("0" + sec);
			} else {
				secStr = String(sec);
			}
			if (houStr == "00") {
				time = minStr + ":" + secStr;
			} else {
				time = houStr + ":" + minStr + ":" + secStr
			}
			return time;
        }
		
	}

}