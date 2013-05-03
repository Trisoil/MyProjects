package com.pplive.p2p.network
{
	import de.polygonal.ds.Comparable;

	public class Endpoint implements Comparable
	{
		private var _ip:String;
		private var _port:uint;
		
		public function Endpoint(ip:String, port:uint)
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
		
		public function compare(other:Object):int
		{
			var anotherEndpoint:Endpoint = other as Endpoint;
			
			if (ip < anotherEndpoint.ip)
				return -1;
			else if (ip > anotherEndpoint.ip)
				return 1;
			else
			{
				if (port < anotherEndpoint.port)
					return -1;
				else if (port > anotherEndpoint.port)
					return 1;
				else
					return 0;
			}
		}
		
		public function toString():String
		{
			return ip + ":" + port;
		}
	}
}