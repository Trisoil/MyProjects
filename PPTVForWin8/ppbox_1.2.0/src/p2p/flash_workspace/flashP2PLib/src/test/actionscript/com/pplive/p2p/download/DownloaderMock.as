package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Piece;
	
	import org.mock4as.Mock;
	
	public class DownloaderMock extends Mock implements IDownloader
	{
		private var _paused:Boolean = false;
		private var _downloadDriver:IDownloadDriver;
		
		public function DownloaderMock()
		{
			super();
		}
		
		public function attachDownloadDriver(downloadDriver:IDownloadDriver):void
		{
			_downloadDriver = downloadDriver;
		}
		
		public function deAttachDownloaderDriver(downloadDriver:IDownloadDriver):void
		{
			_downloadDriver = null;
		}
		
		public function pause():void
		{
			_paused = true;
		}
		
		public function resume():void
		{
			_paused = false;
		}
		
		public function destory():void
		{
		}
		
		public function isPausing():Boolean
		{
			return _paused;
		}
		
		public function getCurrentSpeedInKbps():uint
		{
			return 0;
		}
		
		public function getAverageSpeedInKBPS():uint
		{
			return 0;
		}
		
		public function requestNextPiece():Piece
		{
			return _downloadDriver.requestNextPiece(this);
		}
		
		public function get restPlayTime():uint
		{
			return _downloadDriver.restPlayTime;
		}
	}
}