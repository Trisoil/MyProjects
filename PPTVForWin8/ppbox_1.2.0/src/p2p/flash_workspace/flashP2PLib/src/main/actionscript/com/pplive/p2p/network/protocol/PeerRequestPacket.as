package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.RID;
	import com.pplive.profile.FunctionProfiler;
	
	import flash.utils.ByteArray;

	public class PeerRequestPacket extends Packet
	{
		public var rid:RID;
		public function PeerRequestPacket(action:uint, transactionId:uint, 
										  rid:RID, protocolVersion:uint=PROTOCOL_VERSION)
		{
			super(action, transactionId, protocolVersion);
			this.rid = rid;
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
			bytes.writeBytes(rid.bytes());
		}
	}
}