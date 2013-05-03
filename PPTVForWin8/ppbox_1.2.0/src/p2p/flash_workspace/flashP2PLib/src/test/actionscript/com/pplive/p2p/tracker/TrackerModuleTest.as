package com.pplive.p2p.tracker
{
	import com.pplive.p2p.P2PModuleMock;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	
	import flash.events.Event;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class TrackerModuleTest
	{
		private static var logger:ILogger = getLogger(TrackerConnectionTest);
		private var trackerModule:TrackerModule;
		private var p2pModuleMock:P2PModuleMock = new P2PModuleMock;
		
		public function TrackerModuleTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			logger.debug("setup");
			trackerModule = new TrackerModule(p2pModuleMock);
		}
		
		[Test(async)]
		public function testListBeforeGetTrackerList():void
		{			
			logger.debug("testListBeforeGetTrackerList");
			var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
			var peerListVector:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			peerListVector.push(new PeerInfo(263, "192.168.1.105", 5041, "125.116.201.91", 23294, 37, 55));
			peerListVector.push(new PeerInfo(263, "192.168.1.100", 5041, "218.88.141.103", 1064, 58, 65));
			peerListVector.push(new PeerInfo(263, "192.168.0.100", 5041, "219.128.61.44", 60022, 12, 85));
			p2pModuleMock.expects("onGetPeers").times(1).withArgs(rid, peerListVector).deepCompare(true);					
			
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			p2pModuleMock.eventDispatcher.addEventListener("onGetPeers", handler, false, 0, true);
			
			trackerModule.listPeers(rid);
			var trackers:Vector.<Vector.<TrackerInfo>> = new Vector.<Vector.<TrackerInfo>>;
			var group0:Vector.<TrackerInfo> = new Vector.<TrackerInfo>;
			group0.push(new TrackerInfo("172.16.205.20", 8080));
			trackers.push(group0);
			trackerModule.setTrackerList(trackers);
		}
		
		[Test(async)]
		public function testListAfterGetTrackerList():void
		{			
			logger.debug("testListAfterGetTrackerList");
			var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
			var peerListVector:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			peerListVector.push(new PeerInfo(263, "192.168.1.105", 5041, "125.116.201.91", 23294, 37, 55));
			peerListVector.push(new PeerInfo(263, "192.168.1.100", 5041, "218.88.141.103", 1064, 58, 65));
			peerListVector.push(new PeerInfo(263, "192.168.0.100", 5041, "219.128.61.44", 60022, 12, 85));
			p2pModuleMock.expects("onGetPeers").times(1).withArgs(rid, peerListVector).deepCompare(true);					
			
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			p2pModuleMock.eventDispatcher.addEventListener("onGetPeers", handler, false, 0, true);
			
			var trackers:Vector.<Vector.<TrackerInfo>> = new Vector.<Vector.<TrackerInfo>>;
			var group0:Vector.<TrackerInfo> = new Vector.<TrackerInfo>;
			group0.push(new TrackerInfo("172.16.205.20", 8080));
			trackers.push(group0);
			trackerModule.setTrackerList(trackers);
			
			trackerModule.listPeers(rid);
		}
		
		// herain:测试同时多个group中的tracker.
		[Test(async)]
		public function testListMultiGroup():void
		{			
			logger.debug("testListMultiGroup");
			var rid:RID = new RID("275781FED82F1AD9B45637CAD3DB809F");
			p2pModuleMock.expects("onGetPeers").withAnyArgs().times(2);			
			
			var handler:Function = Async.asyncHandler(this, onTestListMultiGroupFirstReturn, 2000, null);
			p2pModuleMock.eventDispatcher.addEventListener("onGetPeers", handler, false, 0, true);
			
			var trackers:Vector.<Vector.<TrackerInfo>> = new Vector.<Vector.<TrackerInfo>>;
			var group:Vector.<TrackerInfo> = new Vector.<TrackerInfo>;
			group.push(new TrackerInfo("172.16.205.20", 8080));
			trackers.push(group);
			group = new Vector.<TrackerInfo>;
			group.push(new TrackerInfo("113.108.219.37", 1982));
			trackers.push(group);
			trackerModule.setTrackerList(trackers);
			
			trackerModule.listPeers(rid);
		}
		
		public function onTestListMultiGroupFirstReturn(event:Event, passThroughData:Object):void
		{
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			p2pModuleMock.eventDispatcher.addEventListener("onGetPeers", handler, false, 0, true);
		}
		
		private function onTestComplete(event:Event, passThroughData:Object):void
		{
			logger.debug("onTestComplete event:" + event);
			Assert.assertNull(p2pModuleMock.errorMessage());
			Assert.assertTrue(p2pModuleMock.success());
		}
	}
}