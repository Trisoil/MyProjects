package com.pplive.p2p.download
{
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class SpeedMeterTest
	{
		private var speedMeter:SpeedMeter;
		
		public function SpeedMeterTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			speedMeter = new SpeedMeter;
		}
		
		[After]
		public function teardown():void
		{
			speedMeter = null;
		}
		
		[Test(async)]
		public function testDefaultRecordTime():void
		{
			var timer:Timer = new Timer(1000, 15);
			
			var asyncHandler:Function = Async.asyncHandler(this, onDefaultTestComplete, 70000, null);
			timer.addEventListener(TimerEvent.TIMER, onTimer, false, 0, true);
			timer.addEventListener(TimerEvent.TIMER_COMPLETE, asyncHandler, false, 0, true);
			timer.start();
			speedMeter.resume();
		}
		
		public function onTimer(event:TimerEvent):void
		{
			var timer:Timer = event.target as Timer;
			trace("onTimer:" + timer.currentCount);
			
			if (timer.currentCount == 4)
			{
				speedMeter.pause();
			}
			
			if (timer.currentCount == 8)
			{
				speedMeter.resume();
			}
			
			// [1, 3] && [8, 15] running
			if (timer.currentCount <= 3 || timer.currentCount >= 8)
				speedMeter.submitBytes(timer.currentCount * 1024);
		}
		
		public function onDefaultTestComplete(event:TimerEvent, passThroughData:Object):void
		{
			Assert.assertEquals(14, speedMeter.getSecondSpeedInKBPS());
			Assert.assertEquals(12, speedMeter.getRecentSpeedInKBPS());
			Assert.assertEquals(11, speedMeter.getMinuteSpeedInKBPS());
			// 因为Timer不是很精准，这个assert有可能有1左右的偏差
			Assert.assertTrue(8 <= speedMeter.getTotalAvarageSpeedInKBPS() && speedMeter.getTotalAvarageSpeedInKBPS() >= 9);
		}
		
		[Test(async)]
		public function testCustomerRecordTime():void
		{
			speedMeter = new SpeedMeter(30);
			var timer:Timer = new Timer(1000, 65);
			
			var asyncHandler:Function = Async.asyncHandler(this, onCustomerTestComplete, 70000, null);
			timer.addEventListener(TimerEvent.TIMER, onTimer, false, 0, true);
			timer.addEventListener(TimerEvent.TIMER_COMPLETE, asyncHandler, false, 0, true);
			timer.start();
			
			speedMeter.resume();
		}
		
		public function onCustomerTestComplete(event:TimerEvent, passThroughData:Object):void
		{
			Assert.assertEquals(64, speedMeter.getSecondSpeedInKBPS());
			Assert.assertEquals(62, speedMeter.getRecentSpeedInKBPS());
			Assert.assertEquals(49, speedMeter.getMinuteSpeedInKBPS());
			
			// 因为Timer不是很精准，这个assert有可能有1左右的偏差
			Assert.assertTrue(33 <= speedMeter.getTotalAvarageSpeedInKBPS() && speedMeter.getTotalAvarageSpeedInKBPS() >= 34);
		}
	}
}