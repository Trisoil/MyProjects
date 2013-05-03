package com.pplive.util
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.utils.getTimer;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class EventUtilTest
	{
		public function EventUtilTest()
		{
		}
		
		[Test(async)]
		public function testDeferDispatch():void
		{
			var dispatcher:EventDispatcher = new EventDispatcher;
			var event:Event = new Event(Event.COMPLETE);
			var now:int = getTimer();
			var handler:Function = Async.asyncHandler(this, onComplete, 500, now, null);
			dispatcher.addEventListener(Event.COMPLETE, handler);
			EventUtil.deferDispatch(dispatcher, event, 200);
		}
		
		private function onComplete(event:Event, passThourghData:Object):void
		{
			Assert.assertEquals(Event.COMPLETE, event.type);
			var delay:int = getTimer() - (int)(passThourghData);
			trace("delay:" + delay);
			Assert.assertTrue(delay > 150 && delay < 250);
		}
	}
}