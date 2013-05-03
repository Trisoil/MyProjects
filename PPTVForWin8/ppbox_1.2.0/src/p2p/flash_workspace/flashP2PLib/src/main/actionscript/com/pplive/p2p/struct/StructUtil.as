package com.pplive.p2p.struct
{
	public class StructUtil
	{
		public function StructUtil()
		{
		}
		
		public static function getPieceCountByFileLength(fileLength:uint):uint
		{
			return Math.ceil(fileLength / Constants.PIECE_SIZE);
		}
		
		public static function getBlockCountByFileLength(fileLength:uint):uint
		{
			return Math.ceil(fileLength / Constants.BLOCK_SIZE);
		}
		
		public static function getPieceCountInBlock(fileLength:uint, blockIndex:uint):uint
		{
			var lastBlockIndex:uint = getBlockCountByFileLength(fileLength) - 1;
			if (blockIndex == lastBlockIndex)
			{
				if (fileLength % Constants.BLOCK_SIZE == 0)
					return Constants.PIECE_NUM_PER_BLOCK;
				else
					return Math.ceil((fileLength % Constants.BLOCK_SIZE) / Constants.PIECE_SIZE);
			}
			else if (blockIndex < lastBlockIndex)
			{
				return Constants.PIECE_NUM_PER_BLOCK;
			}
			else
			{
				return 0;
			}
		}
		
		public static function getSubPieceCountInPiece(fileLength:uint, piece:Piece):uint
		{
			var lastPiece:Piece = getLastPieceByFileLength(fileLength);
			if (piece.compare(lastPiece) == 0)
			{
				if (fileLength % Constants.PIECE_SIZE == 0)
					return Constants.SUBPIECE_NUM_PER_PIECE;
				else
					return Math.ceil((fileLength % Constants.PIECE_SIZE) / Constants.SUBPIECE_SIZE);
			}
			else if (piece.compare(lastPiece) < 0)
			{
				return 	Constants.SUBPIECE_NUM_PER_PIECE;
			}
			else
			{
				return 0;
			}
		}
		
		public static function getLastPieceByFileLength(fileLength:uint):Piece
		{
			var lastBlockSize:uint = fileLength % Constants.BLOCK_SIZE; 
			if (lastBlockSize == 0)
			{
				return new Piece(fileLength / Constants.BLOCK_SIZE - 1, Constants.PIECE_NUM_PER_BLOCK - 1)
			}
			else
			{
				return new Piece(fileLength / Constants.BLOCK_SIZE, Math.ceil(lastBlockSize / Constants.PIECE_SIZE) - 1);
			}
		}
		
		public static function getLastSubPieceByFileLength(fileLength:uint):SubPiece
		{
			var lastBlockSize:uint = fileLength % Constants.BLOCK_SIZE; 
			if (lastBlockSize == 0)
			{
				return new SubPiece(fileLength / Constants.BLOCK_SIZE - 1, Constants.SUBPIECE_NUM_PER_BLOCK - 1)
			}
			else
			{
				return new SubPiece(fileLength / Constants.BLOCK_SIZE, Math.ceil(lastBlockSize / Constants.SUBPIECE_SIZE) - 1);
			}
		}
		
		public static function getLastSubPieceSizeByFileLength(fileLength:uint):uint
		{
			var lastSubPieceSize:uint = fileLength % Constants.SUBPIECE_SIZE;
			return lastSubPieceSize != 0 ? lastSubPieceSize : Constants.SUBPIECE_SIZE;
		}
		
		public static function getPieceFromSubPiece(subPiece:SubPiece):Piece
		{
			return new Piece(subPiece.blockIndex, subPiece.subPieceIndex / Constants.SUBPIECE_NUM_PER_PIECE);
		}
		
		public static function getSubPieceIndexInPiece(subPiece:SubPiece):uint
		{
			return subPiece.subPieceIndex % Constants.SUBPIECE_NUM_PER_PIECE;
		}
	}
}
