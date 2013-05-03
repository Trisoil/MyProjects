package com.pplive.p2p.network.protocol
{
	import flash.net.Socket;
	import flash.utils.ByteArray;
	
	public class Packet
	{
		public static const PROTOCOL_VERSION_V0:uint = 0x0000;
		public static const PROTOCOL_VERSION_V1:uint = 0x0001;
		public static const PROTOCOL_VERSION:uint = PROTOCOL_VERSION_V1;
		private static var globalTransactionID:uint = 0;
		
		public var action:uint;
		public var transactionId:uint;
		public var protocolVersion:uint;
		
		public function Packet(action:uint, transactionId:uint, 
							   protocolVersion:uint = PROTOCOL_VERSION)
		{
			this.action = action;
			this.transactionId = transactionId;
			this.protocolVersion = protocolVersion;
		}
		
		public static function CreateFromByteArray(bytes:ByteArray):Packet
		{
			var action:uint = bytes.readUnsignedByte();
			var transactionId:uint = bytes.readUnsignedInt();
			var protocolVersion:uint = bytes.readUnsignedShort();
			var p:Packet = new Packet(action, transactionId, protocolVersion);
			return p;
		}
		
		public static function NewTransactionID():uint
		{
			return globalTransactionID++;
		}
		
		public function ToByteArray(bytes:ByteArray):void
		{
			//NetUtil.PacketToByteArray(bytes, this);
			bytes.writeByte(action);
			bytes.writeUnsignedInt(transactionId);
			bytes.writeShort(protocolVersion);
		}
	}
}