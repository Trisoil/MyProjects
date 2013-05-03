package com.pplive.dac.logclient 
{
	import com.adobe.crypto.MD5;
	
	internal class RealtimeLogUrlGenerator extends LogUrlGenerator 
	{	
		public function RealtimeLogUrlGenerator(logSource:DataLogSource) 
		{
			super(logSource);
		}
		
		override public function getLogUrl(metaItems:Vector.<DataLogItem>, items:Vector.<DataLogItem>) : String
		{
			var urlParams:String = "";
			if (metaItems != null)
			{
				urlParams = super.appendUrlParam(urlParams, metaItems);
			}

			if (items != null)
			{
				urlParams = super.appendUrlParam(urlParams, items);
			}

			if (urlParams.length > 0 && urlParams.charAt(urlParams.length - 1) == '&')
			{
				urlParams = urlParams.substring(0, urlParams.length - 1);
			}

			var encodedUrlParams:String = encodeUrlParams(urlParams);
			return this.getLogSource().getBaseUrl() + encodedUrlParams;			
		}
		
		public function encodeUrlParams(urlParams:String) : String
		{
			var data:String = Base64.EncodeString(urlParams);
			var newUrlParams:String = "data=" + data + "&md5=" + MD5.hash(data + this.getLogSource().getKey);
			return newUrlParams;
		}
	}

}