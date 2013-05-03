package com.pplive.p2p
{
	import com.pplive.p2p.struct.TrackerInfo;
	
	import flash.events.Event;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;
	import org.osmf.events.TimeEvent;
	
	public class BootStrapModuleTest
	{
		private var p2pModuleMock:P2PModuleMock = new P2PModuleMock;
		private var bsModule:BootStrapModule;
		
		public function BootStrapModuleTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			var trackerListVector:Vector.<Vector.<TrackerInfo>> = new Vector.<Vector.<TrackerInfo>>;
			var groupTrackerList:Vector.<TrackerInfo> = new Vector.<TrackerInfo>;
			groupTrackerList.push(new TrackerInfo("192.168.30.1", 1982));
			groupTrackerList.push(new TrackerInfo("192.168.60.1", 1982));
			trackerListVector.push(groupTrackerList);
			groupTrackerList = new Vector.<TrackerInfo>;
			groupTrackerList.push(new TrackerInfo("222.73.171.180", 1982));
			groupTrackerList.push(new TrackerInfo("222.73.171.181", 80));
			trackerListVector.push(groupTrackerList);
			// herain:deepCompare must be set to true.
			p2pModuleMock.expects("onGetTrackerList").times(1).withArg(trackerListVector).deepCompare(true);
		}
		
		private function onTestComplete(event:Event, passThroughData:Object):void
		{
			Assert.assertNull(p2pModuleMock.errorMessage());
			Assert.assertTrue(p2pModuleMock.success());
		}
		
		//[Test(async)]
		public function testDefaultDomain():void
		{
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			p2pModuleMock.eventDispatcher.addEventListener(P2PModuleMock.GET_TRACKER_LIST, handler, false, 0, true);			
			bsModule = new BootStrapModule(p2pModuleMock);
		}
		
		[Test(async)]
		public function testLegalDomain():void
		{
			var handler:Function = Async.asyncHandler(this, onTestComplete, 2000, null);
			p2pModuleMock.eventDispatcher.addEventListener(P2PModuleMock.GET_TRACKER_LIST, handler, false, 0, true);						
			bsModule = new BootStrapModule(p2pModuleMock, "172.16.205.20:8080");
		}
		
		private function onTestFailed(event:Event, passThroughData:Object):void
		{
			// onTestFailed should not be called.
			Assert.assertFalse(true);
		}
		
		private function onTimeOut(event:Event):void
		{
			p2pModuleMock.verify();
			Assert.assertNotNull(p2pModuleMock.errorMessage());
			Assert.assertFalse(p2pModuleMock.success());
		}
	}
}