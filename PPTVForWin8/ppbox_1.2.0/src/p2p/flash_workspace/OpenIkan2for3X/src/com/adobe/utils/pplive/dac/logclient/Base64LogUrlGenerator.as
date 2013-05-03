package com.pplive.dac.logclient
{
	import flash.utils.ByteArray;

	internal class Base64LogUrlGenerator extends LogUrlGenerator
	{
		public function Base64LogUrlGenerator(logSource:DataLogSource)
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
			var encrypt:SimpleEncrypt = new SimpleEncrypt(this.getLogSource().getKeyBytes());
			var urlParamsBytes:ByteArray = encrypt.Encrypt(urlParams);
			
			var result:String = Base64.Encode(urlParamsBytes);
			return result;
		}
	}

}