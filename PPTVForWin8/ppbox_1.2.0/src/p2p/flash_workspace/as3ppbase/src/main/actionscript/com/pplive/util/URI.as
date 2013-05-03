package com.pplive.util
{
	import flash.net.URLVariables;
	import flash.utils.Dictionary;

	public class URI
	{
		private static const regexp:RegExp = new RegExp(
			"(?:(http|https)://)?([-\\w\\d.]+)(?::(\\d+))?(/[-\\w\\d+&@#%=~_|!:,.;/]*)?(?:\\?(.*))?"); 
		
		public var protocol:String;
		public var host:String;
		public var port:uint;
		public var path:String;
		private var _variables:URLVariables;
		
		public function URI(url:String)
		{
			var result:Object = regexp.exec(url);
			if (result != null)
			{
				protocol = result[1];
				host = result[2];
				port = uint(result[3]);
				path = result[4];
				if (result[5])
				{
					_variables = new URLVariables;
					_variables.decode(result[5]);	
				}
			}
		}
		
		public function toString():String
		{
			var url:String = (protocol == null) ? "" : protocol + "://";
			url += host;
			url += (port == 0) ? "" : ":" + port;
			url += (path == null) ? "" : path;
			url += (variables == null) ? "" : "?" + variables.toString();
			return url;
		}
		
		public function get variables():URLVariables
		{
			return _variables;
		}
		
		public static function transferToPPFixUrl(srcUrl:String):String
		{
			var destUrl:String = new String;
			var percentage:RegExp = /%/g;
			destUrl = srcUrl.replace(percentage, '__');
			return destUrl;
		}
	}
}