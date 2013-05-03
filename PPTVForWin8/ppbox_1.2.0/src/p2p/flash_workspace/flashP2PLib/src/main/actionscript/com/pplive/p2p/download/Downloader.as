package com.pplive.p2p.download
{	
	public class Downloader implements IDownloader 
	{
		protected var isRunning:Boolean = false;
		protected var speedMeter:SpeedMeter = new SpeedMeter;
		protected var downloadDriver:IDownloadDriver;
		
		public function Downloader()
		{
		}
		
		public function attachDownloadDriver(downloadDriver:IDownloadDriver):void
		{
			this.downloadDriver = downloadDriver;
		}
		
		public function deAttachDownloaderDriver(downloadDriver:IDownloadDriver):void
		{
			if (this.downloadDriver == downloadDriver)
			{
				this.downloadDriver = null;
			}
		}
		
		public function pause():void
		{
			// do nothing untrivial. Subclasses must override this method.
			isRunning = false;
			speedMeter.pause();
			downloadDriver.reportDownloaderPaused(this);
		}
		
		public function resume():void
		{
			isRunning = true;
			speedMeter.resume();
		}
		
		public function isPausing():Boolean
		{
			return !isRunning;
		}
		
		public function destory():void
		{
			// Subclasses must override this method.
			pause();
			speedMeter.destory();
			speedMeter = null;
			downloadDriver = null;
		}
		
		public function getCurrentSpeedInKbps():uint
		{
			return speedMeter.getRecentSpeedInKBPS();
		}
		
		public function getAverageSpeedInKBPS():uint
		{
			return speedMeter.getTotalAvarageSpeedInKBPS();
		}
		
		public function get restPlayTime():uint
		{
			return downloadDriver.restPlayTime;
		}
		
		public function get dragHeadLength():uint
		{
			return downloadDriver.dragHeadLength;
		}
	}
}