package com.pplive.p2p.tracker
{
	import com.pplive.p2p.IP2PModule;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class TrackerModule implements ITrackerModule
	{
		private static var logger:ILogger = getLogger(TrackerModule);
		private var p2pModule:IP2PModule;		
		private var trackers:Vector.<Vector.<TrackerInfo>>;
		private var pendingRequests:Vector.<RID> = new Vector.<RID>;
		private var connectionDictionary:Dictionary = new Dictionary;
		
		public function TrackerModule(p2pModule:IP2PModule)
		{
			this.p2pModule = p2pModule;
		}
		
		public function setTrackerList(trackers:Vector.<Vector.<TrackerInfo>>):void
		{
			logger.info("setTrackerList group:" + trackers.length);
			this.trackers = trackers;
			while (pendingRequests.length > 0)
			{
				doList(pendingRequests[0]);
				pendingRequests.shift();
			}
		}
		
		public function listPeers(rid:RID):void
		{
			if (trackers == null)
			{
				pendingRequests.push(rid);
			}
			else
			{
				doList(rid);
			}
		}
		
		private function doList(rid:RID):void
		{
			var trackersLength:uint = trackers.length;
			for (var i:uint = 0; i < trackersLength; ++i)
			{
				var tracker:TrackerInfo = trackers[i][uint(Math.random() * 1000) % trackers[i].length];
				connectionDictionary[new TrackerConnection(tracker, rid, this)] = 0;
			}
		}
		
		public function onGetPeers(trackerConnection:TrackerConnection, rid:RID, peers:Vector.<PeerInfo>):void
		{
			p2pModule.onGetPeers(rid, peers);
			destoryConnection(trackerConnection);
		}
		
		public function onError(trackerConnection:TrackerConnection, rid:RID):void
		{
			// herain:just do nothing, p2pdownloader will call listpeers again.
			destoryConnection(trackerConnection);
		}
		
		private function destoryConnection(trackerConnection:TrackerConnection):void
		{
			trackerConnection.destory();
			delete connectionDictionary[trackerConnection];
		}
		
		public function get guid():String
		{
			return p2pModule.guid;
		}
	}
}