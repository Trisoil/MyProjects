package com.pplive.p2p.kernel
{	
	import com.pplive.p2p.IP2PStream;
	import com.pplive.p2p.struct.Constants;
	
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.system.Security;
	import flash.utils.getTimer;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class KernelDectecter
	{
		private static var logger:ILogger = getLogger(KernelDectecter);
		private var _p2pStream:IP2PStream;
		private var _loader0:URLLoader;
		private var _loader1:URLLoader;
		private var _startTime:int;
		private var _timeoutTimer:Timer;
		
		public function KernelDectecter(p2pStream:IP2PStream)
		{
			_p2pStream = p2pStream;
			_loader0 = new URLLoader;
			_loader1 = new URLLoader;
			
			DetectPort(_loader0,Constants.KERNEL_MAGIC_PORT0);
			DetectPort(_loader1,Constants.KERNEL_MAGIC_PORT1);
			
			_startTime = getTimer();
			logger.info("start time:" + _startTime);			
			
			_timeoutTimer = new Timer(1000);
			_timeoutTimer.addEventListener(TimerEvent.TIMER, onTimeOut, false, 0, true);
			_timeoutTimer.start();
			
		}
		
		private function DetectPort(loader:URLLoader,port:uint):void
		{
			loader.addEventListener(IOErrorEvent.IO_ERROR, function(e:IOErrorEvent):void{ onIOError(e,port); });
			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, function(e:SecurityErrorEvent):void{ onSecurityError(e,port); });
			loader.addEventListener(Event.COMPLETE,function(e:Event):void{ onComplete(e,port,loader); });			
			var url:String = "http://127.0.0.1:"+port +"/synacast.xml?rnd=" + Math.random().toString();
			loader.load(new URLRequest(url));			
		}
		
		private function RemoveLoaderEvent(loader:URLLoader):void
		{
			loader.removeEventListener(IOErrorEvent.IO_ERROR,onIOError);
			loader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR,onSecurityError);
			loader.removeEventListener(Event.COMPLETE,onComplete);
		}
		
		public function destory():void
		{
			RemoveLoaderEvent(_loader0);			
			_loader0.close();
			_loader0 = null;	
			RemoveLoaderEvent(_loader1);
			_loader1.close();
			_loader1 = null;	
		}
		
		private function stopTimer():void
		{
			if(_timeoutTimer != null)
			{
				_timeoutTimer.stop();
				_timeoutTimer.removeEventListener(TimerEvent.TIMER, onTimeOut);
				_timeoutTimer = null;
			}
		}
		
		private function onTimeOut(event:TimerEvent):void
		{
			logger.error("timeout error");
			onError();
		}
		
		private function onComplete(event:Event,port:uint,loader:URLLoader):void
		{
			logger.debug("complete enent:"+event+" port:"+port);
			try
			{
				var kernelVersionXml:XML = new XML(loader.data);
				logger.info(kernelVersionXml);
				if (kernelVersionXml.PPVA != null)
				{	
					Constants.KERNEL_MAGIC_PORT = port;
					var kernelVersion:String = kernelVersionXml.PPVA.attribute("v");
					var versionVector:Array = kernelVersion.split(", ");
					var majorVersion:uint = uint(versionVector[0]);
					var minorVersion:uint = uint(versionVector[1]);
					var macroVersion:uint = uint(versionVector[2]);
					var extraVersion:uint = uint(versionVector[3]);
					var tcpPort:uint = uint(kernelVersionXml.PPVA.attribute("p"));
					var kernelDescription:KernelDescription = new KernelDescription(majorVersion,
						minorVersion, macroVersion, extraVersion, tcpPort);
					_p2pStream.reportKernelStatus(true, getTimer() - _startTime, kernelDescription);					
					stopTimer();
				}
				else
				{
					logger.info("too old kernel version");
					onError();
				}
			}
			catch(e:TypeError)
			{
				logger.error("kernelVersionXml parse error:" + e);
				onError();
			}
		}
		
		private function onIOError(event:IOErrorEvent,port:uint):void
		{
			//onError();
			//这里不会处理error了，因为2个端口，可能一个错了，另一个对的，处理在定时器里做
			logger.error("onIOError " + event);
		}
		
		private function onSecurityError(event:SecurityErrorEvent,port:uint):void
		{
			//onError();
			//这里不会处理error了，因为2个端口，可能一个错了，另一个对的，处理在定时器里做
			logger.error("onSecurityError");
		}
		
		private function onError():void
		{
			stopTimer();
			_p2pStream.reportKernelStatus(false, getTimer() - _startTime);
		}
	}
}
