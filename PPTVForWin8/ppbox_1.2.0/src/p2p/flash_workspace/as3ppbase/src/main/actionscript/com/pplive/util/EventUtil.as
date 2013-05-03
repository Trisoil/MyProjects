package com.pplive.util
{
	import flash.events.Event;
	import flash.events.IEventDispatcher;
	import flash.events.TimerEvent;
	import flash.utils.Timer;

	public class EventUtil
	{
		public function EventUtil()
		{
		}
		
		public static function deferDispatch(dispatcher:IEventDispatcher, event:Event, delay:uint):void
		{
			var timer:Timer = new Timer(delay, 1);
			timer.addEventListener(TimerEvent.TIMER, function():void {
				dispatcher.dispatchEvent(event);
			});
			timer.start();
		}
	}
}