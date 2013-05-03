package com.pplive.events
{
	import flash.events.Event;
	
	public class PlayStatusEvent extends Event
	{
		public static const PLAY_START:String = "PLAY_STATUS_START";	// info为int型，0表示无插，1表示有插
		public static const PLAY_FAILED:String = "PLAY_STATUS_FAILED";	// info为null
		public static const PLAY_COMPLETE:String = "PLAY_STATUS_COMPLETE";	// info为null
		public static const PLAY_SEEK_NOTIFY:String = "PLAY_STATUS_SEEK_NOTIFY"; // info为null
		public static const PLAY_PAUSED:String = "PLAY_STATUS_PAUSED";	// info为null
		public static const BUFFER_EMPTY:String = "PLAY_STATUS_BUFFER_EMPTY";	// info为null
		public static const BUFFER_FULL:String = "PLAY_STATUS_BUFFER_FULL";		// info为null
		public static const KERNEL_DETECTED:String = "PLAY_STATUS_KERNEL_DECTECTED"; // info为探测时间
		
		private var _info:Object;
		public function PlayStatusEvent(type:String, info:Object = null)
		{
			super(type);
			_info = info;
		}
		
		public function get info():Object {return _info;}
	}
}