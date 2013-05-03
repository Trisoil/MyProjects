package com.pplive.p2p
{
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import de.polygonal.ds.HashMap;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	import com.pplive.mx.ObjectUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class CacheManager
	{		
		private var fileLength:uint;
		private var subPieceMap:HashMap;
		private var pieceMap:HashMap;
		private var startSubPiece:SubPiece;
		private var nonExistSubPiece:SubPiece;
		
		private static var logger:ILogger = getLogger(CacheManager);
		
		public function CacheManager(fileLength:uint)
		{
			this.fileLength = fileLength;
			subPieceMap = new HashMap;
			pieceMap = new HashMap;
			nonExistSubPiece = new SubPiece(0, 0);
			
			var blockCount:uint = StructUtil.getBlockCountByFileLength(fileLength);
			for (var blockIndex:uint = 0; blockIndex < blockCount; ++blockIndex)
			{
				var pieceCount:uint = StructUtil.getPieceCountInBlock(fileLength, blockIndex);
				for (var pieceIndex:uint = 0; pieceIndex < pieceCount; ++pieceIndex)
				{
					pieceMap.set(new Piece(blockIndex, pieceIndex).getOffset(), 0 as uint);
				}
			}
		}
		
		public function destory():void
		{
			subPieceMap.clear(true);
			subPieceMap = null;
			pieceMap.clear(true);
			pieceMap = null;
			startSubPiece = null;
			nonExistSubPiece = null;
		}
		
		public function addSubPiece(subPiece:SubPiece, 
									data:ByteArray):Boolean
		{
			if (!hasSubPiece(subPiece))
			{
				subPieceMap.set(subPiece.offset, data);
				var pieceOffset:uint = StructUtil.getPieceFromSubPiece(subPiece).getOffset();
				var currentNum:uint = pieceMap.get(pieceOffset) as uint;
				pieceMap.remap(pieceOffset, currentNum+1);
				if (subPiece.compare(nonExistSubPiece) == 0)
				{
					nonExistSubPiece.moveToNextSubPiece();
				}
				
				return true;
			}
			else
			{
				logger.debug("add subpiece exist");
				return false;	
			}
		}
		
		public function hasSubPiece(subPiece:SubPiece):Boolean
		{
			return subPieceMap.hasKey(subPiece.offset);
		}
		
		public function hasPiece(piece:Piece):Boolean
		{
			var subPieceCount:uint = StructUtil.getSubPieceCountInPiece(fileLength, piece);
			if (piece.subPieceIndex == 0)
			{				
				return pieceMap.get(piece.getOffset()) == subPieceCount;	
			}
			else
			{
				//不是整个piece都是需要下载的（例如拖动），所以要从开始下载的地方判断每一个subpiece。
				for(var subPieceIndex:uint = piece.subPieceIndex; subPieceIndex < subPieceCount; ++subPieceIndex)
				{
					if (!hasSubPiece(piece.getNthSubPiece(subPieceIndex)))
						return false;
				}
				return true;
			}
		}
		
		public function getSubPiece(subPiece:SubPiece):ByteArray
		{
			var bytes:ByteArray = subPieceMap.get(subPiece.offset) as ByteArray; 
			if (bytes != null)
			{
				bytes.position = 0;
			}
			
			return bytes;
		}
		
		public function getNextNonExistSubPiece(start:SubPiece):SubPiece
		{
			if (startSubPiece == null || startSubPiece.compare(start) != 0)
			{
				startSubPiece = start;
				nonExistSubPiece = start;
			}
			
			var lastSubPiece:SubPiece = StructUtil.getLastSubPieceByFileLength(fileLength);			
			while (hasSubPiece(nonExistSubPiece) && nonExistSubPiece.compare(lastSubPiece) <= 0)
			{
				nonExistSubPiece.moveToNextSubPiece();
			}
			
			if (nonExistSubPiece.compare(lastSubPiece) <= 0)
				return SubPiece(com.pplive.mx.ObjectUtil.copy(nonExistSubPiece));
			else
				return null;
		}
	}
}