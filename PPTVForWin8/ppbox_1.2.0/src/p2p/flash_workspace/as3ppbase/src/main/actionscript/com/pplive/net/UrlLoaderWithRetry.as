package com.pplive.net
{
	import flash.errors.IOError;
	import flash.errors.IllegalOperationError;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class UrlLoaderWithRetry extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(UrlLoaderWithRetry);
		private var loader:URLLoader;
		private var url:String;
		private var maxRetryCount:uint;
		private var requestCount:uint = 0;
		
		public function UrlLoaderWithRetry(retryCount:uint)
		{
			this.maxRetryCount = retryCount;
			
			loader = new URLLoader;
			loader.addEventListener(Event.COMPLETE, onComplete, false, 0, true);
			loader.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError, false, 0, true);
			loader.addEventListener(HTTPStatusEvent.HTTP_STATUS, onHttpStatus, false, 0, true);
		}
		
		public function destory():void
		{
			closeLoader();
			loader.removeEventListener(Event.COMPLETE, onComplete);
			loader.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			loader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
			loader.removeEventListener(HTTPStatusEvent.HTTP_STATUS, onHttpStatus);
			loader = null;
		}
		
		public function load(url:String):void
		{
			this.url = url;
			doLoad();
		}
		
		public function set urlLoaderDataFormat(format:String):void
		{
			loader.dataFormat = format;
		}
		
		public function get urlLoader():URLLoader
		{
			return loader;
		}
		
		public function get data():*
		{
			return loader.data;
		}
		
		private function doLoad():void
		{
			logger.info("load " + url + ", times:" + requestCount);
			if (requestCount != 0)
			{
				closeLoader();
			}
			
			++requestCount;
			try
			{
				loader.load(new URLRequest(url));	
			}
			catch(e:IOError)
			{
				closeLoader();
				dispatchEvent(new LoadFailedEvent);
			}
			catch(e:SecurityError)
			{
				closeLoader();
				dispatchEvent(new LoadFailedEvent);
			}
		}
		
		private function closeLoader():void
		{
			try
			{
				loader.close();	
			}
			catch (error:IllegalOperationError)
			{	
				logger.error("IllegalOperationError on close");
			}
		}
		
		private function onComplete(event:Event):void
		{
			logger.info("onComplete");
			dispatchEvent(new Event(Event.COMPLETE));
		}
		
		private function onIOError(event:IOErrorEvent):void
		{
			onError(event);
		}
		
		private function onSecurityError(event:SecurityErrorEvent):void
		{
			logger.info("onSecurityError:" + event);
			dispatchEvent(new SecurityErrorEvent(SecurityErrorEvent.SECURITY_ERROR));
		}
		
		private function onHttpStatus(event:HTTPStatusEvent):void
		{
			logger.info("onHttpStatus " + event.status);
			if (uint(event.status / 100) == 4 || uint(event.status / 100) == 5)
			{
				onError(event);
			}
		}
		
		private function onError(event:Event):void
		{
			logger.info("onError " + event);
			if (requestCount < maxRetryCount)
			{
				doLoad();
			}
			else
			{
				dispatchEvent(new LoadFailedEvent);
			}
		}
	}
}