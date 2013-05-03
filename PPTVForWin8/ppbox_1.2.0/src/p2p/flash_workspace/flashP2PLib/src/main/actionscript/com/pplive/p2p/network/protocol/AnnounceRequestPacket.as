package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.RID;
	
	import flash.utils.ByteArray;

	public class AnnounceRequestPacket extends PeerRequestPacket
	{
		public static const ACTION:uint = 0xB0;
		public function AnnounceRequestPacket(transactionId:uint, 
									   rid:RID, protocolVersion:uint=PROTOCOL_VERSION)
		{
			super(ACTION, transactionId, rid, protocolVersion);
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
		}
	}
}