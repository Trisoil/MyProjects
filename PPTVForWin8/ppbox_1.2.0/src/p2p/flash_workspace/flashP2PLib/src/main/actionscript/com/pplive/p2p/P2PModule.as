package com.pplive.p2p
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	import com.pplive.p2p.tracker.TrackerModule;
	
	import com.pplive.util.GUID;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.utils.ByteArray;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	import com.pplive.p2p.events.GetPeersEvent;
	
	public class P2PModule extends EventDispatcher implements IP2PModule
	{
		private static var logger:ILogger = getLogger(P2PModule);
		static private var _instance:P2PModule;
		private var bsModuleInstance:BootStrapModule;
		private var trackerModuleInstance:TrackerModule;
		private var _guid:String;
		
		public function P2PModule(enforcer:SingleEnforcer)
		{
			super(null);
			bsModuleInstance = new BootStrapModule(this);
			trackerModuleInstance = new TrackerModule(this);
			_guid = GUID.create();
		}
		
		static public function instance():P2PModule
		{
			if (_instance == null)
			{
				_instance = new P2PModule(new SingleEnforcer);
			}
			
			return _instance;
		}
		
		public function listPeers(rid:RID):void
		{
			logger.debug("listPeers rid:" + rid);
			trackerModuleInstance.listPeers(rid);
		}
		
		public function onGetTrackerList(trackers:Vector.<Vector.<TrackerInfo>>):void
		{
			logger.debug("onGetTrackerList group_count:" + trackers.length);
			trackerModuleInstance.setTrackerList(trackers);
		}
		
		public function onGetPeers(rid:RID, peers:Vector.<PeerInfo>):void
		{
			logger.debug("onGetPeers peer_count:" + peers.length);
			dispatchEvent(new GetPeersEvent(GetPeersEvent.GET_PEERS, rid, peers));
		}
		
		public function get guid():String
		{
			return _guid;
		}
	}
}

// TODO(herain):A trick to implement singleton pattern in actionscript.
class SingleEnforcer{}