package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.utils.ByteArray;

	public class SubPieceRequestPacket extends PeerRequestPacket
	{
		public static const ACTION:uint = 0xB2;
		public var subpieces:Vector.<SubPiece>;
		public var priority:uint;
		
		public function SubPieceRequestPacket(transactionId:uint, 
											  rid:RID, subpieces:Vector.<SubPiece>, 
											  priority:uint, protocolVersion:uint=PROTOCOL_VERSION)
		{
			super(ACTION, transactionId, rid, protocolVersion);
			this.subpieces = subpieces;
			this.priority = priority;
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
			bytes.writeShort(subpieces.length);
			for(var i:int = 0; i < subpieces.length; ++i)
			{
				bytes.writeShort(subpieces[i].blockIndex);
				bytes.writeShort(subpieces[i].subPieceIndex);
			}
			
			bytes.writeShort(priority);
		}
	}
}