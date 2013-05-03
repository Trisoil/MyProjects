package
{
	import de.polygonal.ds.Comparable;
	import de.polygonal.ds.Hashable;
	
	import flash.net.registerClassAlias;
	
	public class Piece implements de.polygonal.ds.Comparable
	{
		// After registerClassAlias, we can cast return object of  mx.utils.ObjectUtil.copy 
		// to object of this Class. 
		// Reference:http://www.darronschall.com/weblog/2007/08/on-transient-objectutilcopy-and-casting.cfm
		private static var hasRegisterClassAlias:Boolean = false;
		private function registerClassAlias():void
		{
			if (!hasRegisterClassAlias)
			{
				flash.net.registerClassAlias("com.pplive.p2p.struct.Piece", Piece);
			}
		}
		
		public var _segmentOffset:uint;
		public var _pieceInOffset:uint;
		public var _pieceIndex:uint;
		public var _offset:uint;
		public var _size:uint;
		
		public function Piece(segmentOffset:uint = 0, pieceIndex:uint = 0, pieceInOffset:uint = 0, size:uint = Utils.PIECE_SIZE)
		{
			registerClassAlias();
			_segmentOffset = segmentOffset;
			_pieceIndex = pieceIndex;
			_pieceInOffset = pieceInOffset;
			_size = size;
			updateOffset();
		}
		
		public function get segmentOffset():uint {return _segmentOffset;}
		public function get pieceIndex():uint {return _pieceIndex;}
		public function get offset():uint {return _offset;}
		public function get pieceInOffset():uint {return _pieceInOffset;}
		
		public function set segmentOffset(index:uint):void
		{
			_segmentOffset = index;
			updateOffset();
		}
		
		public function set pieceIndex(index:uint):void
		{
			_pieceIndex = index;
			updateOffset();
		}
		
		static public function createPieceFromOffset(segmentOffset:uint, offset:uint, fileLength:uint):Piece
		{
			//return new Piece(segmentOffset, offset / Utils.PIECE_SIZE, pieceInOffset);
			var pieceIndex:uint = Math.floor(offset / Utils.PIECE_SIZE);
			var pieceOffset:uint = offset % Utils.PIECE_SIZE;
			var currentPiece:Piece = new Piece(segmentOffset, pieceIndex, pieceOffset);
			var lastOffset:uint = fileLength - fileLength % Utils.PIECE_SIZE;
			if (offset < lastOffset) {
				currentPiece.size = Utils.PIECE_SIZE - pieceOffset;
			}else{
				currentPiece.size = fileLength - offset;
			}
			currentPiece._offset = offset;
			return currentPiece;
		}
		
		private function updateOffset():void
		{
			_offset = _segmentOffset + _pieceInOffset + pieceIndex * Utils.PIECE_SIZE;
		}
		
		public function get size():uint
		{
			return _size;
		}
		
		public function set size(psize:uint):void 
		{
			_size = psize;
		}
		
		public function getPiece():Piece
		{
			return new Piece(segmentOffset, pieceIndex);
		}
		
		public function compare(other:Object):int
		{
			var otherPiece:Piece = other as Piece;
			
			if (segmentOffset < otherPiece.segmentOffset)
				return -1;
			else if (segmentOffset > otherPiece.segmentOffset)
				return 1;
			else
			{
				if (_pieceIndex < otherPiece.pieceIndex)
					return -1;
				else if (pieceIndex > otherPiece.pieceIndex)
					return 1;
				else
					return 0;
			}
		}
		
		public function toString():String
		{
			return "Piece("+segmentOffset+"|"+pieceIndex+":"+offset+")";
		}
	}
}
