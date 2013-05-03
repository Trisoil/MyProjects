package com.pplive.p2p.events
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	
	import flash.events.Event;
	import flash.utils.ByteArray;
	
	public class GetPeersEvent extends Event
	{
		static public const GET_PEERS:String = "_GET_PPERS_";
		private var _rid:RID;
		private var _peers:Vector.<PeerInfo>;
		
		public function GetPeersEvent(type:String, rid:RID, peers:Vector.<PeerInfo>, 
									  bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			_rid = rid;
			_peers = peers;
		}
		
		public function get peers():Vector.<PeerInfo>
		{
			return _peers;
		}
		
		public function get rid():RID
		{
			return _rid;
		}
	}
}