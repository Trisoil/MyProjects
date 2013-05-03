package com.pplive.p2p.struct
{
	public class PeerInfo
	{
		private var _version:uint;
		private var _ip:String;
		private var _port:uint;
		private var _detectIp:String;
		private var _detectPort:uint;		
		private var _uploadPriority:uint;
		private var _trackerPriority:uint;
		
		public function PeerInfo(version:uint, ip:String, port:uint, detectIp:String, detectPort:uint,
								uploadPriority:uint, trackerPriority:uint)
		{
			_version = version;
			_ip = ip;
			_port = port;
			_detectIp = detectIp;
			_detectPort = detectPort;
			_uploadPriority = uploadPriority;
			_trackerPriority = trackerPriority;
		}
		
		public function get version():uint {return _version;}
		public function get ip():String {return _ip;}
		public function get port():uint {return _port;}
		public function get detectIp():String {return _detectIp;}
		public function get detectPort():uint {return _detectPort;}
		public function get uploadPriority():uint {return _uploadPriority;}
		public function get trackerPriority():uint {return _trackerPriority;}
		
		public function toString():String
		{
			return "version:" + version + ", ip:" + ip + ", port:" + port + 
				", detectIp:" + detectIp + ", detectPort:" + detectPort + ", priority:" + uploadPriority;
		}
	}
}