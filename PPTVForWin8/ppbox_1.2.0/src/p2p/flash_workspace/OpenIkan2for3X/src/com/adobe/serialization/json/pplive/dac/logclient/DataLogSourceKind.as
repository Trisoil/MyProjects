package com.pplive.dac.logclient
{

	public final class DataLogSourceKind
	{
		public static const Normal:DataLogSourceKind = new DataLogSourceKind("Normal");
		public static const RealtimeOnline:DataLogSourceKind = new DataLogSourceKind("RealtimeOnline");
		
		private var _name:String;
		public function getName():String { return this._name; }
		
		public function DataLogSourceKind(name:String)
		{
			this._name = name;
		}
	}

}