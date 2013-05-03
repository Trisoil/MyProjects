package com.pplive.dac.logclient 
{
	import flash.net.sendToURL;
	import flash.net.URLLoader;
	import flash.net.URLRequest;

	public class DataLog 
	{
		
		private var  _logSource : DataLogSource;
		public function getDataLogSource() : DataLogSource { return this._logSource; }

		public function DataLog(logSource:DataLogSource)
		{
			this._logSource = logSource;
		}

		public function sendLogRequestAsync(metaItems:Vector.<DataLogItem>, items:Vector.<DataLogItem>) : void
		{
			var logUrl : String = this.getLogUrl(metaItems, items);
			if (logUrl)
			{
				var urlRequest:URLRequest = new URLRequest(logUrl);
				//sendToURL(urlRequest);
				var urlLoader:URLLoader = new URLLoader();
				urlLoader.addEventListener(IOErrorEvent.IO_ERROR, function(e:IOErrorEvent):void{} );			
				urlLoader.addEventListener(Event.COMPLETE, function(e:Event):void{} );
				urlLoader.load(urlRequest);
			}
		}

		public function getLogUrl(metaItems:Vector.<DataLogItem>, items:Vector.<DataLogItem>) : String
		{
			var generator:LogUrlGenerator = CreateLogUrlGenerator();
			return generator.getLogUrl(metaItems, items);
		}

		private function CreateLogUrlGenerator() : LogUrlGenerator
		{
			if (this._logSource == DataLogSource.IKanLiveApp || this._logSource == DataLogSource.IKanVodApp)
			{
				return new Base64LogUrlGenerator(this._logSource);
			}
			else if (this._logSource == DataLogSource.IKanOnlineApp)
			{
				return new RealtimeLogUrlGenerator(this._logSource);
			}
			// should never go here
			return new Base64LogUrlGenerator(this._logSource);
		}
		
	}

}