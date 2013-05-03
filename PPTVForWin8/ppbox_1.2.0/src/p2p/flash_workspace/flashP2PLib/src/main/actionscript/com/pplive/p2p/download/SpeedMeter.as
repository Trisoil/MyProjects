package com.pplive.p2p.download
{
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	import flash.utils.getTimer;

	public class SpeedMeter
	{
		static private const RECENT_TIME_SCALE_IN_SECONDS:uint = 5;
		static private const MINUTE_TIME_SCALE_IN_SECONDS:uint = 60;
		
		private var maxRecordSeconds:uint;
		
		// TODO(herain):2011-12-17:使用循环数组重构historySpeed
		private var historySpeed:Vector.<uint>;
		private var currentSecondBytes:uint;
		private var archiveTimer:Timer;
		private var isRunning:Boolean = false;
		private var totalDownloadBytes:uint;
		private var totalDownloadTime:uint;
		private var startDownloadTime:int;
		
		public function SpeedMeter(maxRecordSeconds:uint = MINUTE_TIME_SCALE_IN_SECONDS)
		{
			this.maxRecordSeconds = maxRecordSeconds;
			historySpeed = new Vector.<uint>;
			currentSecondBytes = 0;
			archiveTimer = new Timer(1000);
			archiveTimer.addEventListener(TimerEvent.TIMER, onArchiveTimer, false, 0, true);
			archiveTimer.start();
		}
		
		public function resume():void
		{
			if (!isRunning)
			{
				isRunning = true;
				startDownloadTime = getTimer();
				historySpeed = new Vector.<uint>;
			}
		}
		
		public function pause():void
		{
			if (isRunning)
			{
				isRunning = false;
				totalDownloadTime += getTimer() - startDownloadTime;
			}
		}
		
		public function destory():void
		{
			historySpeed.length = 0;
			historySpeed = null;
			archiveTimer.stop();
			archiveTimer.removeEventListener(TimerEvent.TIMER, onArchiveTimer);
			archiveTimer = null;
		}
		
		public function submitBytes(bytesCount:uint):void
		{
			currentSecondBytes += bytesCount;
			totalDownloadBytes += bytesCount;
		}
		
		public function getSecondSpeedInKBPS():uint
		{
			if (historySpeed.length > 0)
				return historySpeed[historySpeed.length - 1] / 1024;
			else
				return 0;
		}
		
		public function getRecentSpeedInKBPS():uint
		{
			return getMultiSecondsSpeedInKBPS(RECENT_TIME_SCALE_IN_SECONDS);
		}
		
		public function getMinuteSpeedInKBPS():uint
		{
			return getMultiSecondsSpeedInKBPS(MINUTE_TIME_SCALE_IN_SECONDS);
		}
		
		public function getTotalAvarageSpeedInKBPS():uint
		{
			var nowTotalDownloadTime:uint = totalDownloadTime;
			if (isRunning)
			{
				nowTotalDownloadTime += getTimer() - startDownloadTime;
			}

			return totalDownloadBytes / nowTotalDownloadTime;
		}
		
		private function getMultiSecondsSpeedInKBPS(seconds:uint):uint
		{
			var totalBytes:uint = 0;
			var historySpeedLength:uint = historySpeed.length;
			if (historySpeedLength >= seconds)
			{
				for(var i:uint = historySpeedLength - seconds; i < historySpeedLength; ++i)
				{
					totalBytes += historySpeed[i];	
				}
				
				return totalBytes / seconds / 1024;
			}
			else if (historySpeedLength > 0)
			{
				for(i = 0; i < historySpeedLength; ++i)
				{
					totalBytes += historySpeed[i];
				}
				
				return totalBytes / historySpeedLength / 1024;
			}
			else
			{
				return 0;
			}
		}
		
		private function onArchiveTimer(event:Event):void
		{
			if (historySpeed.length == maxRecordSeconds)
			{
				historySpeed.shift();
			}
			
			historySpeed.push(currentSecondBytes);
			currentSecondBytes = 0;
		}
	}
}