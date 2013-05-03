package com.pplive.dac.logclient
{
	import flash.utils.ByteArray;

	public final class DataLogSource
	{
		public static const IKanLiveApp:DataLogSource = new DataLogSource("http://ik.synacast.com/1.html?", "pplive", DataLogSourceKind.Normal);
		public static const IKanVodApp:DataLogSource = new DataLogSource("http://ik.synacast.com/1.html?", "pplive", DataLogSourceKind.Normal);
		public static const IKanOnlineApp:DataLogSource = new DataLogSource("http://ol.synacast.com/2.html?", "&#$EOQWIU31!DA421", DataLogSourceKind.RealtimeOnline);
			
		private var _baseUrl : String;
		public function getBaseUrl() : String { return this._baseUrl; }

		private var _key : String;
		public function getKey() : String { return this._key; }

		private var _keyBytes : ByteArray;
		public function getKeyBytes() : ByteArray { return this._keyBytes; }

		private var _kind : DataLogSourceKind;
		public function getKind() : DataLogSourceKind { return this._kind; }
		 
		public function isSourceKindNormal() : Boolean { return this._kind == DataLogSourceKind.Normal; }
		public function isSourceKindRealtimeOnline() : Boolean { return this._kind == DataLogSourceKind.RealtimeOnline; }
		
		public function DataLogSource(baseUrl:String, key:String, kind:DataLogSourceKind)
		{
			this._baseUrl = baseUrl;
			this._kind = kind;
			this._key = key;
			var keyBytes:ByteArray = new ByteArray();
			keyBytes.writeUTFBytes(this._key);
			keyBytes.position = 0;		
			this._keyBytes = keyBytes;
		}
	}
}