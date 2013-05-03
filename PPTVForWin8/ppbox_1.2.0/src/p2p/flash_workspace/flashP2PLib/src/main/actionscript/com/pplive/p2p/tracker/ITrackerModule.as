package com.pplive.p2p.tracker
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;

	internal interface ITrackerModule
	{
		function onGetPeers(trackerConnection:TrackerConnection, rid:RID, peers:Vector.<PeerInfo>):void;
		function onError(trackerConnection:TrackerConnection, rid:RID):void;
		function get guid():String;
	}
}