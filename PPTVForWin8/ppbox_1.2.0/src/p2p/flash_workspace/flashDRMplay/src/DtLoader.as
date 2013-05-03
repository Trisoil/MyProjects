package
{
	import com.pplive.net.LoadFailedEvent;
	import com.pplive.net.UrlLoaderWithRetry;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLRequest;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class DtLoader extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(DtLoader);
		private var fileName:String;
		
		private var loader:UrlLoaderWithRetry;
		private var dtRequestStartTime:int;
		private var _dtRequestDuration:uint;
		private var _dtEnabled:Boolean = false;
		
		private var _host:String = "";
		private var _key:String;
		private var _bwType:uint;
		
		public function DtLoader(fileName:String)
		{
			this.fileName = fileName;
		}
		
		public function load():void
		{
			if (_dtEnabled) {
				loader = new UrlLoaderWithRetry(2);
				loader.addEventListener(Event.COMPLETE, onDtComplete, false, 0, true);
				loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onDtSecurityError, false, 0, true);
				loader.addEventListener(LoadFailedEvent.LOAD_FAILED, onDtLoadFailed, false, 0, true);
				loader.load(Utils.JUMP_HOST + fileName + "dt");
				logger.info("PlayInfoLoader load " + Utils.JUMP_HOST + fileName + "dt");
				dtRequestStartTime = getTimer();
			}else{ 
				//currently DT is disabled for DEMO purpose.
				_host = Utils.CDN_HOST;
				_bwType = 0;
				getCDNKey();
				dispatchEvent(new Event(Event.COMPLETE));
			}
		}
		
		public function get requestDuration():uint {return _dtRequestDuration;}
		public function get host():String {return _host;}
		public function get key():String {return _key;}
		public function get bwType():uint {return _bwType;}
		
		public function getCDNKey():String
		{
			//get local time.
			var localUTCtime:Date = new Date;
			var localTime:Number = localUTCtime.getTime() - 1800*1000;
			_key = Utils.constructKey(localTime / 1000);
			return _key;
		}
		
		private function onDtComplete(event:Event):void
		{
			_dtRequestDuration = getTimer() - dtRequestStartTime;
			
			try
			{
				var dtXml:XML = new XML(loader.data);
				logger.debug(dtXml);
				
				_host = dtXml.server_host.children()[0].toString();
				_bwType = uint(dtXml.BWType.children()[0].toString());
				var serverTime:Number = Date.parse(dtXml.server_time.children()[0].toString());
				logger.info("serverTime:" + serverTime + ", " + dtXml.server_time.children()[0].toString());
				_key = Utils.constructKey(serverTime / 1000);
				
				logger.info("host:" + _host + ", bwType:" + _bwType + ", key:" + _key);
				dispatchEvent(new Event(Event.COMPLETE));
			}
			catch(e:TypeError)
			{
				logger.error("onDtComplete parse error:" + e);
				dispatchEvent(new PlayInfoLoadFailedEvent(PlayFailedReason.DATE_TIME_ERROR));
			}
		}
		
		private function onDtSecurityError(event:SecurityErrorEvent):void
		{
			dispatchEvent(new PlayInfoLoadFailedEvent(PlayFailedReason.DATE_TIME_ERROR));
		}
		
		private function onDtLoadFailed(event:LoadFailedEvent):void
		{
			dispatchEvent(new PlayInfoLoadFailedEvent(PlayFailedReason.DATE_TIME_ERROR));
		}		
	}
}