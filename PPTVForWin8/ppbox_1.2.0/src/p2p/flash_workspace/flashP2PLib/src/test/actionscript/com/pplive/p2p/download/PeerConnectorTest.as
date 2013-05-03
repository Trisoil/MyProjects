package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.PeerInfo;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;
	
	import flash.events.Event;

	public class PeerConnectorTest
	{
		private var connector:PeerConnector;
		private var listenerMock:ConnectorListenerMock = new ConnectorListenerMock;
		
		public function PeerConnectorTest()
		{
		}
		
		[Test(async)]
		public function test():void
		{
			connector = new PeerConnector(listenerMock);
			var peer:PeerInfo = new PeerInfo(263, "192.168.0.99", 5041, "192.168.30.163", 6001, 14, 95);
			listenerMock.expects("onPeerConnected").withArg(peer).times(1).deepCompare(true);
			
			var handler:Function = Async.asyncHandler(this, onTestComplete, 20000, null);
			listenerMock.eventDispatcher.addEventListener("onPeerConnected", handler);
			connector.connect(peer);
		}
		
		private function onTestComplete(event:Event, passThroughData:Object):void
		{
			Assert.assertNull(listenerMock.errorMessage());
			Assert.assertTrue(listenerMock.success());
		}
	}
}