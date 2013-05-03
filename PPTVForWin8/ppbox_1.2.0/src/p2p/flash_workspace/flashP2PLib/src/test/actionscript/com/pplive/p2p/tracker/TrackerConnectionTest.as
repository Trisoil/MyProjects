package com.pplive.p2p.tracker
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	
	import flash.events.Event;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class TrackerConnectionTest
	{
		private static var logger:ILogger = getLogger(TrackerConnectionTest);
		private var trackerModuleMock:TrackerModuleMock;
		private var trackerConnection:TrackerConnection;
		
		public function TrackerConnectionTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			logger.debug("setup");
			trackerModuleMock = new TrackerModuleMock;
		}
		
		[Test(async)]
		public function testListApacheServer():void
		{
			logger.debug("testListApacheServer");
			var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
			var peerListVector:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			peerListVector.push(new PeerInfo(263, "192.168.1.105", 5041, "125.116.201.91", 23294, 37, 55));
			peerListVector.push(new PeerInfo(263, "192.168.1.100", 5041, "218.88.141.103", 1064, 58, 65));
			peerListVector.push(new PeerInfo(263, "192.168.0.100", 5041, "219.128.61.44", 60022, 12, 85));
			trackerModuleMock.expects("onGetPeers").times(1).withArgs(rid, peerListVector).deepCompare(true);
			
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			trackerModuleMock.eventDispatcher.addEventListener("onGetPeers", handler, false, 0, true);
			trackerConnection = new TrackerConnection(new TrackerInfo("172.16.205.20", 8080), rid, trackerModuleMock);
		}
		
		[Test(async)]
		public function testListInvalidServer():void
		{
			logger.debug("testListInvalidServer");
			var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
			var peerListVector:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			trackerModuleMock.expects("onError").times(1).withArg(rid).deepCompare(true);
			
			var handler:Function = Async.asyncHandler(this, onTestComplete, 10000, null);
			trackerModuleMock.eventDispatcher.addEventListener("onError", handler, false, 0, true);
			trackerConnection = new TrackerConnection(new TrackerInfo("172.16.205.21", 8080), rid, trackerModuleMock);
		}
		
		[Test(async)]
		public function testListCgiServer():void
		{
			logger.debug("testListCgiServer");
			var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
			trackerModuleMock.expects("onGetPeers").times(1).withAnyArgs();
			
			var handler:Function = Async.asyncHandler(this, onTestComplete, 5000, null);
			trackerModuleMock.eventDispatcher.addEventListener("onGetPeers", handler, false, 0, true);
			trackerConnection = new TrackerConnection(new TrackerInfo("113.108.219.37", 1982), rid, trackerModuleMock);
		}
		
		private function onTestComplete(event:Event, passThroughData:Object):void
		{
			logger.debug("onTestComplete event:" + event);
			Assert.assertNull(trackerModuleMock.errorMessage());
			Assert.assertTrue(trackerModuleMock.success());
		}
	}
}