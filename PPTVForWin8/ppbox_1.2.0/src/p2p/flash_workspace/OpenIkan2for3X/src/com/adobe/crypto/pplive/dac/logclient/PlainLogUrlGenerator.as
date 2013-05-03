package com.pplive.dac.logclient
{
	internal class PlainLogUrlGenerator extends LogUrlGenerator
	{
		public function PlainLogUrlGenerator(logSource:DataLogSource)
		{
			super(logSource);
		}
		
		override public function getLogUrl(metaItems:Vector.<DataLogItem>, items:Vector.<DataLogItem>) : String
		{
			var urlParams:String = "";
			if (metaItems != null)
			{
				urlParams = super.appendUrlParam(urlParams, metaItems, true);
			}

			if (items != null)
			{
				urlParams = super.appendUrlParam(urlParams, items, true);
			}

			if (urlParams.length > 0 && urlParams.charAt(urlParams.length - 1) == '&')
			{
				urlParams = urlParams.substring(0, urlParams.length - 1);
			}

			return this.getLogSource().getBaseUrl() + urlParams;
		}
	}

}
