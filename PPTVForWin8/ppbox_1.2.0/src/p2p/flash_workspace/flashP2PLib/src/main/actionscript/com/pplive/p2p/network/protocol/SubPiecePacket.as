package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.utils.ByteArray;
	
	public class SubPiecePacket extends PeerResponsePacket
	{
		public static const ACTION:uint = 0xB3;
		
		public var subpiece:SubPiece = new SubPiece;
		public var subpieceLength:uint;
		public var data:ByteArray;
		
		public function SubPiecePacket(headerBytes:ByteArray, subpieceLength:uint, data:ByteArray)
		{
			super(headerBytes);
			subpiece.blockIndex = headerBytes.readUnsignedShort();
			subpiece.subPieceIndex = headerBytes.readUnsignedShort();
			this.subpieceLength = subpieceLength;
			this.data = data;
		}
	}
}