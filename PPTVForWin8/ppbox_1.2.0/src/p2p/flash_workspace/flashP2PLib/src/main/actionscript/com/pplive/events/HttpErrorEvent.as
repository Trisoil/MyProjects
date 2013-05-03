// ActionScript file
package com.pplive.events
{
	import flash.events.Event;
	
	public class HttpErrorEvent extends Event
	{
		public static const VOD_HTTP_ERROR:String = "VOD_HTTP_ERROR";
		public static const KERNEL_HTTP_ERROR:String = "KERNEL_HTTP_ERROR";
		
		//失败距离连接请求发起的时间间隔
		private var _interval:uint;
		
		//大等于10000表示过程中失败，小于10000表示没有受到包体就失败了
		private var _error:uint;
		
		public function HttpErrorEvent(type:String,interval:uint,error:uint)
		{
			super(type);
			_interval = interval;
			_error = error;
		}	
		
		public function get interval():uint {return _interval;}
		public function get error():uint {return _error;};
	}
}