package pplive.Utils
{
	public class Guid
	{
		public static function create():String {
	        var uid:String = "";
	        var ALPHA_CHARS:String = "0123456789abcdef";
	        var i:Number;
	        var j:Number;
	        for (i=0; i<8; i++) {//先成成前8位
	            uid += ALPHA_CHARS.charAt(Math.round(Math.random()*15));
	        }
	        for (i=0; i<3; i++) {//中间的三个4位16进制数
	            uid += "-";
	            for (j=0; j<4; j++) {
	                uid += ALPHA_CHARS.charAt(Math.round(Math.random()*15));
	            }
	        }
	        uid += "-";
	        var time:Number = new Date().getTime();
	        uid += ("0000000"+time.toString(16)).substr(-8);//取后边8位
	        for (i=0; i<4; i++) {
	            uid += ALPHA_CHARS.charAt(Math.round(Math.random()*15));//再循环4次随机拿出4位
	        }
	        return uid;
	    }
	}
}