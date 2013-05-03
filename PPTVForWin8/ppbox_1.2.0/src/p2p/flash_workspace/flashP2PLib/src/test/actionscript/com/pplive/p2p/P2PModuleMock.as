package com.pplive.p2p
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	import com.pplive.util.GUID;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	
	import org.mock4as.Mock;
	
	public class P2PModuleMock extends Mock implements IP2PModule
	{
		public static const GET_TRACKER_LIST:String = "onGetTrackerList";
		public static const GET_PEERS:String = "onGetPeers";
		
		private var _eventDispatcher:EventDispatcher;
		private var _guid:String;
		
		public function P2PModuleMock()
		{
			super();
			_eventDispatcher = new EventDispatcher;
			_guid = GUID.create();
		}
		
		public function get eventDispatcher():EventDispatcher
		{
			return _eventDispatcher;
		}
		
		public function onGetTrackerList(trackers:Vector.<Vector.<TrackerInfo>>):void
		{
			record("onGetTrackerList", trackers);
			_eventDispatcher.dispatchEvent(new Event(GET_TRACKER_LIST));
		}
		
		public function onGetPeers(rid:RID, peers:Vector.<PeerInfo>):void
		{
			record("onGetPeers", rid, peers);
			_eventDispatcher.dispatchEvent(new Event(GET_PEERS));
		}
		
		public function get guid():String
		{
			return _guid;
		}
	}
}