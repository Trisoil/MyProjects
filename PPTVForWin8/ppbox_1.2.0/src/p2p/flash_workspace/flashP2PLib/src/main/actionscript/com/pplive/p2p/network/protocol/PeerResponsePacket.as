package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.RID;
	
	import flash.utils.ByteArray;

	public class PeerResponsePacket extends Packet
	{
		public var rid:RID = new RID;
		public function PeerResponsePacket(bytes:ByteArray)
		{
			var packet:Packet = Packet.CreateFromByteArray(bytes);
			super(packet.action, packet.transactionId, packet.protocolVersion);
			// TODO(herain):watch out! Does rid position change?
			rid.setBytes(bytes);
		}
		
		// herain:We don't need override toByteArray function because it is no sense for flash 
		// client to send AnnounceResponsePacket.
	}
}