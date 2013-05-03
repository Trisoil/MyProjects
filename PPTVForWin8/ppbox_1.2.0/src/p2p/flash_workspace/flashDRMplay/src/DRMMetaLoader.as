package
{
	import com.pplive.net.LoadFailedEvent;
	import com.pplive.net.UrlLoaderWithRetry;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLLoaderDataFormat;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class DRMMetaLoader extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(DRMMetaLoader);
		private var fileName:String;
		
		private var drmMetaLoader:UrlLoaderWithRetry;
		private var drmMetaRequestStartTime:int;
		private var _requestDuration:uint;
		
		private var _videoHeight:uint = 0;
		private var _videoWidth:uint = 0;
		
		public function DRMMetaLoader(fileName:String)
		{
			this.fileName = fileName;
		}
		
		public function load():void
		{
			logger.info("DRM Meta Loader load");			
			drmMetaLoader = new UrlLoaderWithRetry(2);
			drmMetaLoader.urlLoaderDataFormat = URLLoaderDataFormat.BINARY;
			drmMetaLoader.addEventListener(Event.COMPLETE, onDRMMetaComplete, false, 0, true);
			drmMetaLoader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onDRMMetaSecurityError, false, 0, true);
			drmMetaLoader.addEventListener(LoadFailedEvent.LOAD_FAILED, onDRMMetaLoadFailed, false, 0, true);
			drmMetaLoader.load(fileName);
			drmMetaRequestStartTime = getTimer();
		}
		
		public function get requestDuration():uint {return _requestDuration;}
		public function get videoHeight():uint {return _videoHeight;}
		public function get videoWidth():uint {return _videoWidth;}	
		public function get data():* {return drmMetaLoader.data;}
		public function get urlLoader():URLLoader {return drmMetaLoader.urlLoader;}
		
		private function onDRMMetaComplete(event:Event):void
		{
			_requestDuration = getTimer() - drmMetaRequestStartTime;
			trace("meta downloaded, taking " + _requestDuration +" msec.");
			dispatchEvent(new Event(Event.COMPLETE));
		}
		
		private function onDRMMetaSecurityError(event:SecurityErrorEvent):void
		{
			trace("meta download error, security");
			dispatchEvent(new DRMLoadFailedEvent(DRMFailedReason.DRMMETA_ERROR));
		}
		
		private function onDRMMetaLoadFailed(event:LoadFailedEvent):void
		{
			trace("meta download error, load");
			dispatchEvent(new DRMLoadFailedEvent(DRMFailedReason.DRMMETA_ERROR));
		}
		
	}
}