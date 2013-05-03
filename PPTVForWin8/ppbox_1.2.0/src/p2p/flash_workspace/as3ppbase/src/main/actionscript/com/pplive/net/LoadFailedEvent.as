package com.pplive.net
{
	import flash.events.Event;
	
	public class LoadFailedEvent extends Event
	{
		public static const LOAD_FAILED:String = "LOAD_FAILED";
		public function LoadFailedEvent(type:String = LOAD_FAILED, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
		}
	}
}