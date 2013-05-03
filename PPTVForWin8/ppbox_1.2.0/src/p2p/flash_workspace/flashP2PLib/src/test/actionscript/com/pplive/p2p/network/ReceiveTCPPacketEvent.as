package com.pplive.p2p.network
{
	import com.pplive.p2p.network.protocol.Packet;
	
	import flash.events.Event;
	
	public class ReceiveTCPPacketEvent extends Event
	{
		public static const ANNOUNCE_PACKET:String = "_receive_announce_packet_";
		public static const SUBPIECE_PACKET:String = "_receive_subpiece_packet_";
		
		private var _packet:Packet;
		public function ReceiveTCPPacketEvent(type:String, packet:Packet, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			_packet = packet;
		}
		
		public function get packet():Packet
		{
			return _packet;
		}
	}
}