package com.pplive.p2p.network.protocol
{
	import flash.utils.ByteArray;

	public class ReportSpeedPacket extends Packet
	{
		public static const ACTION:uint = 0xB4;
		public var speedInBytesPerSecond:uint;
		public function ReportSpeedPacket(transactionId:uint, speedInBytesPerSecond:uint, protocolVersion:uint=PROTOCOL_VERSION)
		{
			super(ACTION, transactionId, protocolVersion);
			this.speedInBytesPerSecond = speedInBytesPerSecond;
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
			bytes.writeUnsignedInt(speedInBytesPerSecond);
		}
	}
}