package com.pplive.p2p.struct
{
	public class TrackerInfo
	{
		private var _ip:String;
		private var _port:uint;
		
		public function TrackerInfo(ip:String, port:uint)
		{
			_ip = ip;
			_port = port;
		}
		
		public function get ip():String
		{
			return _ip;
		}
		
		public function get port():uint
		{
			return _port;
		}
	}
}