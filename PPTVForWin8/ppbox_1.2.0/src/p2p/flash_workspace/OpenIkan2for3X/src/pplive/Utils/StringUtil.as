package pplive.Utils
{
	/**
	 * 字符串工具类
	 * 
	 * @author xushengs@gmail.com
	 * 
	 */	
	public class StringUtil
	{
		public function StringUtil()
		{
		}
		
		/**
		 * 格式化一个字符串，类似于C#的format 
		 * 
		 * @param format
		 * @param args
		 * @return String
		 * 
		 */		
		public static function Format(format:String, ...args):String{
			for(var i:int=0, l:int=args.length; i<l; i++){
				format = format.replace(new RegExp('\\{' + i + '\\}', 'g'), args[i]);
			}
			
			return format;
		}
		public static function LTrim(s : String):String
		{
			return ((s.charCodeAt(0) == 32 || s.charCodeAt(0) == 13 || s.charCodeAt(0) == 10 || s.charCodeAt(0) == 9)?LTrim(s.substring(1,s.length)):s);
		}
		  
		public static function RTrim(s : String):String
		{
			return ((s.charCodeAt(s.length-1) == 32 || s.charCodeAt(s.length-1) == 13 || s.charCodeAt(s.length-1) == 10 || s.charCodeAt(s.length-1) == 9)?RTrim(s.substring(0,s.length-1)):s);
		}
		  
		public static function trim(s : String):String
		{
			return LTrim(RTrim(s));
		}
		public static function timeFormat(seconds:Number):String{
			var times:String = '';
			var hour:Number = 0;
			var minute:Number = 0;
			var second:Number = 0;
			if(seconds < 3600){
				minute = Math.floor(seconds / 60);
				second = Math.floor(seconds % 60);
			} else {
				var tempTime:Number = Math.floor(seconds / 60);
				hour = Math.floor(tempTime / 60);
				minute = Math.floor(tempTime % 60);
				second = Math.floor(seconds % 60);
			}
			if(hour > 0){
				if(hour < 10){
					times += '0' + hour;
				} else {
					times += hour;
				}
				times += ':';
			}
			if(minute > 9){
				times += minute;
			} else {
				times += "0"+minute;
			}
			times += ':';
			if(second > 9){
				times += second;
			} else {
				times += "0"+second;
			}
			return times;
		}

	}
}