package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.RID;
	
	import flash.utils.ByteArray;
	
	public class ReportStatusPacket extends PeerRequestPacket
	{
		public static const ACTION:uint = 0xD0;
		
		private var _restPlayTimeInSeconds:uint;
		public function ReportStatusPacket(rid:RID, restPlayTimeInSeconds:uint)
		{
			super(ACTION, Packet.NewTransactionID(), rid, Packet.PROTOCOL_VERSION);
			_restPlayTimeInSeconds = restPlayTimeInSeconds;
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
			bytes.writeShort(_restPlayTimeInSeconds);
		}
	}
}