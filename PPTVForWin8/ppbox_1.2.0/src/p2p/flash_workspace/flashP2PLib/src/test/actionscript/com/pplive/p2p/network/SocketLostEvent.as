package com.pplive.p2p.network
{
	import flash.events.Event;
	
	public class SocketLostEvent extends Event
	{
		public static const SOCKET_LOST:String = "SocketLost";
		public function SocketLostEvent(type:String, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
		}
	}
}