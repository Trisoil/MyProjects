package com.pplive.p2p
{
	import org.flexunit.Assert;
	import org.flexunit.async.Async;
	
	import com.pplive.p2p.events.GetPeersEvent;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.test.object.valueEqualTo;
	
	import org.hamcrest.assertThat;
	import com.pplive.p2p.events.GetPeersEvent;
	
	public class P2PModuleTest
	{		
		private var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
		public function P2PModuleTest()
		{
		}
		
		[Test(async)]
		public function testListPeers():void
		{
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			P2PModule.instance().addEventListener(GetPeersEvent.GET_PEERS, handler, false, 0, true);
			P2PModule.instance().listPeers(rid);
		}
		
		private function onTestComplete(event:GetPeersEvent, passThroughData:Object):void
		{
			Assert.assertEquals(rid, event.rid);
			
			var peerListVector:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			peerListVector.push(new PeerInfo(263, "192.168.1.105", 5041, "125.116.201.91", 23294, 37, 55));
			peerListVector.push(new PeerInfo(263, "192.168.1.100", 5041, "218.88.141.103", 1064, 58, 65));
			peerListVector.push(new PeerInfo(263, "192.168.0.100", 5041, "219.128.61.44", 60022, 12, 85));

			assertThat(peerListVector, valueEqualTo(event.peers));
		}
	}
}