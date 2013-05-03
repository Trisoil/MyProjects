package com.pplive.p2p.download
{
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.test.object.valueEqualTo;
	
	import org.flexunit.Assert;
	import org.flexunit.internals.namespaces.classInternal;
	import org.hamcrest.assertThat;

	public class IPPoolTest
	{
		public function IPPoolTest()
		{
		}
		
		[Test]
		public function testAddPeer():void
		{
			var ippool:IPPool = new IPPool;
			Assert.assertNull(ippool.getNextPeer());
			
			var peers:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			var e1:Endpoint = new Endpoint("125.116.201.91", 23294);
			var p1:PeerInfo = new PeerInfo(263, "192.168.1.105", 5041, e1.ip, e1.port, 37, 55);
			var e2:Endpoint = new Endpoint("218.88.141.103", 1064);
			var p2:PeerInfo = new PeerInfo(263, "192.168.1.100", 5041, e2.ip, e2.port, 58, 65);
			var e3:Endpoint = new Endpoint("219.128.61.44", 60022);
			var p3:PeerInfo = new PeerInfo(263, "192.168.0.100", 5041, e3.ip, e3.port, 12, 85);
			var e4:Endpoint = new Endpoint("219.128.61.43", 60011);
			var p4:PeerInfo = new PeerInfo(263, "192.168.0.99", 5041, e4.ip, e4.port, 14, 95);
			
			peers.push(p1);
			peers.push(p2);
			peers.push(p3);
			peers.push(p4);
			
			ippool.addCandidatePeers(peers);
			Assert.assertEquals(4, ippool.getPeerCount());
			Assert.assertEquals(4, ippool.getUnTriedPeerCount());
			
			// herain:测试增加重复的peer
			ippool.addCandidatePeers(peers);
			Assert.assertEquals(4, ippool.getPeerCount());
			Assert.assertEquals(4, ippool.getUnTriedPeerCount());
			
			assertThat(ippool.getNextPeer(), valueEqualTo(p2));
			Assert.assertEquals(4, ippool.getPeerCount());
			Assert.assertEquals(3, ippool.getUnTriedPeerCount());
			
			assertThat(ippool.getNextPeer(), valueEqualTo(p1));
			Assert.assertEquals(4, ippool.getPeerCount());
			Assert.assertEquals(2, ippool.getUnTriedPeerCount());
			
			// herain:测试中途增加一个peer
			var e5:Endpoint = new Endpoint("219.128.61.42", 60010);
			var p5:PeerInfo = new PeerInfo(263, "192.168.0.99", 5041, e5.ip, e5.port, 25, 85);
			peers = new Vector.<PeerInfo>;
			peers.push(p5);
			ippool.addCandidatePeers(peers);
			
			assertThat(ippool.getNextPeer(), valueEqualTo(p5));
			Assert.assertEquals(5, ippool.getPeerCount());
			Assert.assertEquals(2, ippool.getUnTriedPeerCount());
			
			// herain:测试中途增加一个已存在的peer
			var e6:Endpoint = new Endpoint("219.128.61.42", 60010);
			var p6:PeerInfo = new PeerInfo(263, "192.168.0.99", 5041, e6.ip, e6.port, 35, 85);
			assertThat(ippool.getNextPeer(), valueEqualTo(p4));
			Assert.assertEquals(5, ippool.getPeerCount());
			Assert.assertEquals(1, ippool.getUnTriedPeerCount());
			
			assertThat(ippool.getNextPeer(), valueEqualTo(p3));
			Assert.assertEquals(5, ippool.getPeerCount());
			Assert.assertEquals(0, ippool.getUnTriedPeerCount());
		}
	}
}