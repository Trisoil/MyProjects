package com.pplive.p2p.network
{
	import com.pplive.p2p.network.protocol.AnnounceResponsePacket;
	import com.pplive.p2p.network.protocol.Packet;
	import com.pplive.p2p.network.protocol.SubPiecePacket;
	import com.pplive.test.EventMock;
	
	import flash.events.Event;
	
	public class SocketListenerMock extends EventMock implements ISocketListener
	{
		public static const ON_PACKET:String = "onPacket";
		public static const ON_SOCKET_LOST:String = "onSocketLost";
		
		public function SocketListenerMock()
		{
			super();
		}
		
		public function onPacket(packet:Packet):void
		{
			switch(packet.action)
			{
				case SubPiecePacket.ACTION:
					dispatchEvent(new ReceiveTCPPacketEvent(ReceiveTCPPacketEvent.SUBPIECE_PACKET, packet));
					break;
				case AnnounceResponsePacket.ACTION:
					dispatchEvent(new ReceiveTCPPacketEvent(ReceiveTCPPacketEvent.ANNOUNCE_PACKET, packet));
					break;
			}
		}
		
		public function onSocketLost():void
		{
			dispatchEvent(new Event("onSocketLost"));
		}
	}
}