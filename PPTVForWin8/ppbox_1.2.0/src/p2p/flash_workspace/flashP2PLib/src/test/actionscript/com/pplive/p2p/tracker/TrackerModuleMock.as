package com.pplive.p2p.tracker
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.util.GUID;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	
	import org.mock4as.Mock;
	
	public class TrackerModuleMock extends Mock implements ITrackerModule
	{
		private var _eventDispatcher:EventDispatcher;
		private var _guid:String;
		public function TrackerModuleMock()
		{
			super();
			_eventDispatcher = new EventDispatcher;
			_guid = GUID.create();
		}
		
		public function get eventDispatcher():EventDispatcher
		{
			return _eventDispatcher;
		}
		
		public function onGetPeers(trackerConnection:TrackerConnection, rid:RID, peers:Vector.<PeerInfo>):void
		{
			record("onGetPeers", rid, peers);
			_eventDispatcher.dispatchEvent(new Event("onGetPeers"));
		}
		
		public function onError(trackerConnection:TrackerConnection, rid:RID):void
		{
			record("onError", rid);
			_eventDispatcher.dispatchEvent(new Event("onError"));
		}
		
		public function get guid():String
		{
			return _guid;
		}
	}
}