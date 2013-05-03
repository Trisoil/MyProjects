package com.pplive.p2p.kernel
{
	public class KernelDescription
	{
		private var _majorVersion:uint;
		private var _minorVersion:uint;
		private var _macroVersion:uint;
		private var _extraVersion:uint;
		private var _tcpPort:uint;
		public function KernelDescription(majorVersion:uint, minorVersion:uint, macroVersion:uint, 
										  extraVersion:uint, tcpPort:uint)
		{
			_majorVersion = majorVersion;
			_minorVersion = minorVersion;
			_macroVersion = macroVersion;
			_extraVersion = extraVersion;
			_tcpPort = tcpPort;
		}
		
		public function get majorVersion():uint {return _majorVersion;}
		public function get minorVersion():uint {return _minorVersion;}
		public function get macroVersion():uint {return _macroVersion;}
		public function get extraVersion():uint {return _extraVersion;}
		public function get tcpPort():uint {return _tcpPort;}
	}
}