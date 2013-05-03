package com.pplive.p2p
{
	import com.pplive.net.LoadFailedEvent;
	import com.pplive.net.UrlLoaderWithRetry;
	import com.pplive.p2p.BootStrapConfig;
	import com.pplive.p2p.struct.TrackerInfo;
	
	import flash.events.Event;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	internal class BootStrapModule
	{
		private static var logger:ILogger = getLogger(BootStrapModule);
		private static const BootStrapDomain:String = "player.aplus.pptv.com";
		private static const MAX_TRY_COUNT:uint = 3;		
		
		private var p2pModule:IP2PModule;
		private var domain:String;
		private var loader:UrlLoaderWithRetry;
		private var triedCount:uint;	
		
		
		public function BootStrapModule(p2pModule:IP2PModule, domain:String = BootStrapDomain)
		{
			this.p2pModule = p2pModule;
			this.domain = domain;
			requestBsConfig();		

		}	

		
		private function requestBsConfig():void
		{
			
			loader = new UrlLoaderWithRetry(MAX_TRY_COUNT);
			loader.addEventListener(Event.COMPLETE, onComplete, false, 0, true);
			loader.addEventListener(LoadFailedEvent.LOAD_FAILED, onLoadFailed, false, 0, true);
			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError, false, 0, true);
			
			var url:String = "http://" + domain + "/config/vodp2pconfig";
			logger.info("load vodp2pconfig from :" + url);
			loader.load(url);
		}
		
		private function onComplete(event:Event):void
		{
			try
			{
				logger.debug("bs data:"+loader.data);
				var bsconfigXML:XML = new XML(loader.data);				
				var trarckerListArray:Vector.<Vector.<TrackerInfo>> = parseTrackerList(bsconfigXML);
				p2pModule.onGetTrackerList(trarckerListArray);
				
				var value:int = getTrackerControlValue(bsconfigXML,"lupcfl");
				if( -1 != value)
				{
					BootStrapConfig.leastUntriedPeerCountForList = int(value);
				}
																					 
				value = getTrackerControlValue(bsconfigXML,"fplt");
				if( -1 != value)
				{
					BootStrapConfig.frequentPeerListTimes = int(value);
				}
					
				value = getTrackerControlValue(bsconfigXML,"ftipl");
				if( -1 != value)
				{
					BootStrapConfig.frequentTimeIntervalPeerList = int(value);
				}
					
				value = getTrackerControlValue(bsconfigXML,"nftipl");
				if( -1 != value)
				{
					BootStrapConfig.nofrequentTimeIntervalPeerList = int(value);
				}
				
				value = getSwitchControlValue(bsconfigXML,"sprpt");
				if( -1 != value)
				{
					BootStrapConfig.startP2pRestPlayTime = int(value);
				}
				
				value = getSwitchControlValue(bsconfigXML,"rhrpt");
				if( -1 != value)
				{
					BootStrapConfig.resumeHttpRestPlayTime = int(value);
				}
				
				value = getSwitchControlValue(bsconfigXML,"p2pp");
				if( -1 != value)
				{
					//获取到的是无插的情况下，使用p2p的比例,把这个比例转换成是否使用p2p
					var rand:Number =  Math.random()*100;
					BootStrapConfig.useP2p = (int(value)>(rand));
					logger.info("value is:"+value+" useP2p:"+BootStrapConfig.useP2p+" rand:"+rand);
				}
					
			}
			catch(e:TypeError)
			{
				logger.error("onTrackerList error:" + e);
			}
		}
		
		private function onLoadFailed(event:LoadFailedEvent):void
		{
			logger.error("onLoadFailed:" + event);
		}
		
		private function onSecurityError(event:SecurityErrorEvent):void
		{
			// TODO(herain):need report error
			logger.error("onSecurityError:" + event);
		}
		
		private function parseTrackerList(bsconfigXML:XML):Vector.<Vector.<TrackerInfo>>
		{
			var trackerListVector:Vector.<Vector.<TrackerInfo>> = new Vector.<Vector.<TrackerInfo>>;
			var groups:XMLList = bsconfigXML.group;
			var groupsLength:uint = groups.length();
			for(var groupIndex:uint = 0; groupIndex < groupsLength; ++groupIndex)
			{
				var group:XML = groups[groupIndex];
				var trackers:XMLList = group.t;
				var groupTrackers:Vector.<TrackerInfo> = new Vector.<TrackerInfo>;
				var trackersLength:uint = trackers.length();
				for (var trackerIndex:uint = 0; trackerIndex < trackersLength; ++trackerIndex)
				{
					groupTrackers.push(new TrackerInfo(trackers[trackerIndex].attribute("i"), trackers[trackerIndex].attribute("p")));
				}
				trackerListVector.push(groupTrackers);
			}		
			return trackerListVector;
		}
		
		private function getTrackerControlValue(bsconfigXML:XML,attr:String):int
		{	
			var trackerFrequentControl:* = bsconfigXML.tfc;			
			if((undefined != trackerFrequentControl)
				&& 
				(undefined != trackerFrequentControl.attribute(attr)))
			{
				logger.info("attr:"+attr+" value:"+trackerFrequentControl.attribute(attr));				
				return trackerFrequentControl.attribute(attr);				
			}
			
			logger.info("control empty,attr:"+attr);
			return -1;			
		}
		
		private function getSwitchControlValue(bsconfigXML:XML,attr:String):int
		{	
			var switchControl:* = bsconfigXML.sc;			
			if((undefined != switchControl)
				&& 
				(undefined != switchControl.attribute(attr)))
			{
				logger.info("attr:"+attr+" value:"+switchControl.attribute(attr));				
				return switchControl.attribute(attr);				
			}
			
			logger.info("control empty,attr:"+attr);
			return -1;			
		}
		
		
	}
}