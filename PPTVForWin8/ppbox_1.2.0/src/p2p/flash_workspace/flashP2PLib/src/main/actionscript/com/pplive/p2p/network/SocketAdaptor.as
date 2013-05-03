package com.pplive.p2p.network
{
	import flash.net.Socket;
	
	public class SocketAdaptor extends Socket implements ISocket
	{
		public function SocketAdaptor(host:String=null, port:int=0)
		{
			super(host, port);
		}
	}
}