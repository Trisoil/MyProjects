package com.pplive.p2p.tracker
{
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.TrackerInfo;
	import com.pplive.profile.FunctionProfiler;
	
	import flash.events.Event;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class TrackerConnection
	{
		private static var logger:ILogger = getLogger(TrackerConnection);
		private static const REQUEST_TIME_OUT_IN_SECONDS:uint = 5;
		
		private var host:String;
		private var port:uint;
		private var rid:RID;
		private var trackerModule:ITrackerModule;
		private var urlLoader:URLLoader;
		private var timer:Timer;
		
		public function TrackerConnection(trackerInfo:TrackerInfo, rid:RID, trackerModule:ITrackerModule)
		{
			this.host = trackerInfo.ip;
			this.port = trackerInfo.port;
			this.rid = rid;
			this.trackerModule = trackerModule;
			doRequest();
		}
		
		public function destory():void
		{
			rid = null;
			trackerModule = null;
			
			urlLoader.removeEventListener(Event.COMPLETE, onComplete);
			urlLoader.removeEventListener(IOErrorEvent.IO_ERROR, onError);
			urlLoader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onError);
			urlLoader.removeEventListener(HTTPStatusEvent.HTTP_STATUS, onHttpStatus);
			urlLoader = null;
		}
		
		private function doRequest():void
		{
			urlLoader = new URLLoader;
			urlLoader.addEventListener(Event.COMPLETE, onComplete, false, 0, true);
			urlLoader.addEventListener(IOErrorEvent.IO_ERROR, onError, false, 0, true);
			urlLoader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError, false, 0, true);
			urlLoader.addEventListener(HTTPStatusEvent.HTTP_STATUS, onHttpStatus, false, 0, true);
			
			timer = new Timer(REQUEST_TIME_OUT_IN_SECONDS * 1000, 1);
			timer.addEventListener(TimerEvent.TIMER, onTimeOut, false, 0, true);
			timer.start();
			
			var url:String = constructUrl();
			logger.info(url);
			urlLoader.load(new URLRequest(url));
		}
		
		private function constructUrl():String
		{
			return "http://" + host + ":" + port + "/trackercgi?rid=" + rid.toString() + "&pid=" + trackerModule.guid
				+ "&count=200";
		}
		
		private function stopTimer():void
		{
			timer.stop();
			timer.removeEventListener(TimerEvent.TIMER, onTimeOut);
			timer = null;
		}
		
		private function onComplete(event:Event):void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "onComplete");
			stopTimer();
			try
			{
				var peerListXml:XML = new XML(urlLoader.data);
				urlLoader.close();
				logger.debug(peerListXml);
				var peerListVector:Vector.<PeerInfo> = parsePeerList(peerListXml);
				trackerModule.onGetPeers(this, rid, peerListVector);
			}
			catch(e:TypeError)
			{
				logger.error("peerListXml parse error:" + e);
				trackerModule.onError(this, rid);
			}
			profiler.end();
		}
		
		private function onTimeOut(event:Event):void
		{
			//这里如果不做close，那么就有可能在remove event之后，urlload的事件得不到处理。
			urlLoader.close();
			onError(event);
		}
		
		private function onError(event:Event):void
		{
			logger.error("request tracker error:" + event);
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			stopTimer();
			trackerModule.onError(this, rid);
			profiler.end();
		}
		
		private function onHttpStatus(event:HTTPStatusEvent):void
		{
			logger.info("request tracker http status:" + event.status + ", rid=" + rid.toString());
			if (event.status / 100 == 4 || event.status / 100 == 5)
			{
				trackerModule.onError(this, rid);
			}
		}
		
		private function parsePeerList(peerListXml:XML):Vector.<PeerInfo>
		{
			var peerList:Vector.<PeerInfo> = new Vector.<PeerInfo>;
			var result:XMLList = peerListXml.result;
			if (result.attribute("type") == 0)
			{
				var peers:XMLList = result.data.p;
				var peersLength:uint = peers.length();
				for(var i:uint = 0; i < peersLength; ++i)
				{
					var p:XML = peers[i];
					peerList.push(new PeerInfo(p.attribute("v"), p.attribute("i1"), p.attribute("p1"),
								p.attribute("i2"), p.attribute("p2"), p.attribute("up"), p.attribute("tp")));
				}
			}
			
			return peerList;
		}
	}
}