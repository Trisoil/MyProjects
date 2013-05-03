package com.pplive.p2p.struct
{
	import de.polygonal.ds.Comparable;
	
	import flash.net.registerClassAlias;
	
	public class Piece implements de.polygonal.ds.Comparable
	{
		// After registerClassAlias, we can cast return object of  mx.utils.ObjectUtil.copy 
		// to object of this Class. 
		// Reference:http://www.darronschall.com/weblog/2007/08/on-transient-objectutilcopy-and-casting.cfm
		private static var hasRegisterClassAlias:Boolean = false;
		static private function registerClassAlias():void
		{
			if (!hasRegisterClassAlias)
			{
				flash.net.registerClassAlias("com.pplive.p2p.struct.Piece", Piece);
				hasRegisterClassAlias = true;
			}
		}
		
		public var blockIndex:uint;
		public var pieceIndex:uint;
		public var subPieceIndex:uint;
		
		public function Piece(blockIndex:uint = 0, pieceIndex:uint = 0, subPieceIndex:uint = 0)
		{
			registerClassAlias();
			this.blockIndex = blockIndex;
			this.pieceIndex = pieceIndex;
			this.subPieceIndex = subPieceIndex;
		}
		
		static public function createPieceFromOffset(offset:uint):Piece
		{
			return new Piece(offset / Constants.BLOCK_SIZE, (offset % Constants.BLOCK_SIZE) / Constants.PIECE_SIZE
							, (offset % Constants.PIECE_SIZE) / Constants.SUBPIECE_SIZE);
		}
		
		[Transient]
		public function getOffset():uint
		{
			return blockIndex * Constants.BLOCK_SIZE + pieceIndex * Constants.PIECE_SIZE
				+ subPieceIndex * Constants.SUBPIECE_SIZE;
		}
		
		[Transient]
		public function getFirstSubPiece():SubPiece
		{
			return new SubPiece(blockIndex, pieceIndex * Constants.SUBPIECE_NUM_PER_PIECE + subPieceIndex);
		}
		
		[Transient]
		public function getNthSubPiece(n:uint):SubPiece
		{
			return new SubPiece(blockIndex, pieceIndex * Constants.SUBPIECE_NUM_PER_PIECE + n);
		}
		
		[Transient]
		public function compare(other:Object):int
		{
			var otherPiece:Piece = other as Piece;
			
			if (blockIndex < otherPiece.blockIndex)
				return -1;
			else if (blockIndex > otherPiece.blockIndex)
				return 1;
			else
			{
				if (pieceIndex < otherPiece.pieceIndex)
					return -1;
				else if (pieceIndex > otherPiece.pieceIndex)
					return 1;
				else
				{
					if (subPieceIndex < otherPiece.subPieceIndex)
						return -1;
					else if (subPieceIndex > otherPiece.subPieceIndex)
						return 1;
					else
						return 0;
				}
			}
		}
		
		[Transient]
		public function moveToNextPiece():void
		{
			++pieceIndex;
			subPieceIndex = 0;
			
			if (pieceIndex == Constants.PIECE_NUM_PER_BLOCK)
			{
				++blockIndex;
				pieceIndex = 0;
			}
		}
		
		[Transient]
		public function toString():String
		{
			return "Piece("+blockIndex+"|"+pieceIndex+"|"+subPieceIndex+")";
		}
	}
}