package com.pplive.dac.logclient
{
	import flash.utils.ByteArray;

	internal class LogUrlGenerator
	{
		private var _logSource : DataLogSource;
		public function getLogSource() : DataLogSource { return this._logSource; }

		public function LogUrlGenerator(logSource:DataLogSource)
		{
			this._logSource = logSource;
		}

		public function getLogUrl(metaItems:Vector.<DataLogItem>, items:Vector.<DataLogItem>) : String
		{
			// do nothing;
			return this._logSource.getBaseUrl();
		}

		protected function appendUrlParam(urlParams:String, items:Vector.<DataLogItem>) : String
		{
			for each (var item:DataLogItem in items)
			{
				urlParams = appendUrlParamImpl(urlParams, item.Name, item.Value);
			}
			return urlParams;
		}

		protected function appendUrlParamImpl(urlParams:String, name:String, value:String) : String
		{
			if (value.indexOf("%") >= 0 || value.indexOf("&") >= 0 || value.indexOf("=") >= 0)
			{
				value = encodeURIComponent(value);
			}

			return urlParams + name + "=" + value + "&";
		}
	}

}