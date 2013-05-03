package com.pplive.p2p
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	
	public interface IP2PModule
	{
		function onGetTrackerList(trackers:Vector.<Vector.<TrackerInfo>>):void;
		function onGetPeers(rid:RID, peers:Vector.<PeerInfo>):void;
		function get guid():String;
	}
}