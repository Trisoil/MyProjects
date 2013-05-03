package com.pplive.p2p.struct
{
	import de.polygonal.ds.Comparable;
	import de.polygonal.ds.Hashable;
	
	import flash.net.registerClassAlias;

	public class SubPiece implements de.polygonal.ds.Comparable
	{
		// After registerClassAlias, we can cast return object of  mx.utils.ObjectUtil.copy 
		// to object of this Class. 
		// Reference:http://www.darronschall.com/weblog/2007/08/on-transient-objectutilcopy-and-casting.cfm
		private static var hasRegisterClassAlias:Boolean = false;
		private function registerClassAlias():void
		{
			if (!hasRegisterClassAlias)
			{
				flash.net.registerClassAlias("com.pplive.p2p.struct.SubPiece", SubPiece);
			}
		}
		
		private var _blockIndex:uint;
		private var _subPieceIndex:uint;
		private var _offset:uint;
		
		public function SubPiece(blockIndex:uint = 0, subPieceIndex:uint = 0)
		{
			registerClassAlias();
			_blockIndex = blockIndex;
			_subPieceIndex = subPieceIndex;
			updateOffset();
		}
		
		public function get blockIndex():uint {return _blockIndex;}
		public function get subPieceIndex():uint {return _subPieceIndex;}
		public function get offset():uint {return _offset;}
		
		public function set blockIndex(index:uint):void
		{
			_blockIndex = index;
			updateOffset();
		}
		
		public function set subPieceIndex(index:uint):void
		{
			_subPieceIndex = index;
			updateOffset();
		}
		
		static public function createSubPieceFromOffset(offset:uint):SubPiece
		{
			return new SubPiece(offset / Constants.BLOCK_SIZE, (offset % Constants.BLOCK_SIZE) / Constants.SUBPIECE_SIZE);
		}
		
		[Transient]
		private function updateOffset():void
		{
			_offset = blockIndex * Constants.BLOCK_SIZE + subPieceIndex * Constants.SUBPIECE_SIZE;
		}
		
		[Transient]
		public function getSize(fileLength:uint):uint
		{
			if (compare(StructUtil.getLastSubPieceByFileLength(fileLength)) == 0)
			{
				return StructUtil.getLastSubPieceSizeByFileLength(fileLength);
			}
			else
			{
				return Constants.SUBPIECE_SIZE;
			}
		}
		
		[Transient]
		public function getPiece():Piece
		{
			return new Piece(blockIndex, subPieceIndex / Constants.SUBPIECE_NUM_PER_PIECE);
		}
		
		[Transient]
		public function moveToNextSubPiece():void
		{
			++_subPieceIndex;
			
			if (_subPieceIndex == Constants.SUBPIECE_NUM_PER_BLOCK)
			{
				++_blockIndex;
				_subPieceIndex = 0;
			}
			
			updateOffset();
		}
		
		[Transient]
		public function compare(other:Object):int
		{
			var otherSubPiece:SubPiece = other as SubPiece;
			
			if (blockIndex < otherSubPiece.blockIndex)
				return -1;
			else if (blockIndex > otherSubPiece.blockIndex)
				return 1;
			else
			{
				if (subPieceIndex < otherSubPiece.subPieceIndex)
					return -1;
				else if (subPieceIndex > otherSubPiece.subPieceIndex)
					return 1;
				else
					return 0;
			}
		}
		
		[Transient]
		public function toString():String
		{
			return "SubPiece("+blockIndex+"|"+subPieceIndex+":"+offset+")";
		}
	}
}
