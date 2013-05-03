package com.pplive.p2p.download
{
	import com.pplive.ds.Pair;
	import com.pplive.ds.StandardMap;
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.profile.FunctionProfiler;
	
	import de.polygonal.ds.ArrayUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	internal class IPPool
	{
		private static var logger:ILogger = getLogger(IPPool);
		
		private var allPeers:StandardMap = new StandardMap;
		private var unUsedPeers:StandardMap = new StandardMap;	//key:upload priority, value:PeerInfo
		private var usedPeers:Vector.<PeerInfo> = new Vector.<PeerInfo>;
		
		public function IPPool()
		{
		}
		
		public function destory():void
		{
			allPeers.destory();
			allPeers = null;
			
			unUsedPeers.destory();
			unUsedPeers = null;
			
			usedPeers.length = 0;
			usedPeers = null;
		}
		
		public function addCandidatePeers(peerVector:Vector.<PeerInfo>):void
		{
			var peerVectorLength:uint = peerVector.length; 
			for (var i:uint = 0; i < peerVectorLength; ++i)
			{
				var peer:PeerInfo = peerVector[i];
				var endpoint:Endpoint = new Endpoint(peer.detectIp, peer.detectPort);
				if (allPeers.hasKey(endpoint))
				{
					allPeers.remap(endpoint, peer);
				}
				else
				{
					allPeers.set(endpoint, peer);
					var priority:PeerPriority = new PeerPriority(peer.uploadPriority, endpoint);				
					unUsedPeers.set(priority, peer);
				}
			}
		}
		
		public function getNextPeer():PeerInfo
		{
			var profiler:FunctionProfiler = new FunctionProfiler(null, "getNextPeer");
			if (!unUsedPeers.isEmpty())
			{
				var pair:Pair = unUsedPeers.begin();
				unUsedPeers.clr(pair.key);
				usedPeers.push(pair.value as PeerInfo);				
				logger.debug("getNextPeer from unUsedPeers.");
				profiler.end();
				return pair.value as PeerInfo;
			}
			else if (usedPeers.length > 0)
			{
				// herain：没有unused的peer了，从usedpeers列表中随机选择一个返回
				var nextPeer:PeerInfo = usedPeers[Math.random()*10000 % usedPeers.length];
				logger.debug("getNextPeer from usedPeers.");
				profiler.end();
				return nextPeer;
			}
			
			return null;
		}
		
		private function endpointCompare(ep1:Endpoint, ep2:Endpoint):int
		{
			return ep1.compare(ep2);
		}
		
		public function getPeerCount():uint
		{
			return unUsedPeers.size() + usedPeers.length;
		}
		
		public function getUnTriedPeerCount():uint
		{
			return unUsedPeers.size();	
		}
		
		public function onConnectFailed(endpoint:Endpoint):void
		{
		}
		
		public function onPeerDisconnect(endpoint:Endpoint):void
		{
		}
	}
}

import com.pplive.p2p.network.Endpoint;
import com.pplive.p2p.struct.PeerInfo;

import de.polygonal.ds.Comparable;

class PeerPriority implements Comparable
{
	public var uploadPriority:uint;
	public var endpoint:Endpoint;
	
	public function PeerPriority(uploadPriority:uint, endpoint:Endpoint)
	{
		this.uploadPriority = uploadPriority;
		this.endpoint = endpoint;
	}
	
	public function compare(other:Object):int
	{
		var otherPriority:PeerPriority = other as PeerPriority;
		
		// herain:优先级大的反而比较中认为小，从而在从小到大排序的map中排在前面 
		if(uploadPriority > otherPriority.uploadPriority)
		{
			return -1;
		}
		else if (uploadPriority < otherPriority.uploadPriority)
		{
			return 1;
		}
		else
		{
			return endpoint.compare(otherPriority.endpoint);
		}
	}
}