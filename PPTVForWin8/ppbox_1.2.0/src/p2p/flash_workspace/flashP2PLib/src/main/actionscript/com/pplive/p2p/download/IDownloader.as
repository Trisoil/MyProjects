package com.pplive.p2p.download
{
	public interface IDownloader
	{
		function attachDownloadDriver(downloadDriver: IDownloadDriver):void;
		function deAttachDownloaderDriver(downloadDriver: IDownloadDriver):void;
		function pause():void;
		function resume():void;
		function destory():void;
		function isPausing():Boolean;
		function getCurrentSpeedInKbps():uint;
		function getAverageSpeedInKBPS():uint;
		function get restPlayTime():uint;
	}
}