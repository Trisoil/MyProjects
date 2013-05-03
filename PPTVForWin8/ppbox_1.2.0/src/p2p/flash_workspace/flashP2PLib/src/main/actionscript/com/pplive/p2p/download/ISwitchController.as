package com.pplive.p2p.download
{
	internal interface ISwitchController
	{
		function start():void;
		function stop():void;
		function addHttpDownloader(httpDownloader:HttpDownloader):void;
		function addP2PDownloader(p2pDownloader:IDownloader):void;
		function setRestPlayTime(restPlayTime:uint):void;
		function setBWType(bwType:uint):void
	}
}