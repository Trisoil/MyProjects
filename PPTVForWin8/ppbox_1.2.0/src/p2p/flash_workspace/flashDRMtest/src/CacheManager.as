package
{
	//import com.pplive.p2p.struct.StructUtil;
	
	import de.polygonal.ds.HashMap;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	import mx.utils.ObjectUtil;
	
	public class CacheManager
	{		
		private var fileLength:uint;
		private var pieceMap:HashMap;
		private var startPiece:Piece;
		private var nonExistPiece:Piece;
		
		public function CacheManager(fileLength:uint)
		{
			this.fileLength = fileLength;
			pieceMap = new HashMap;
			nonExistPiece = new Piece(0, 0);
			
			var pieceCount:uint = Math.ceil(fileLength / Utils.PIECE_SIZE);
			for (var pieceIndex:uint = 0; pieceIndex < pieceCount; ++pieceIndex)
			{
				pieceMap.set(pieceIndex * Utils.PIECE_SIZE, null);
			}
		}
		
		public function destory():void
		{
			pieceMap.clear(true);
			pieceMap = null;
			startPiece = null;
			nonExistPiece = null;
		}
		
		public function addPiece(piece:Piece, data:ByteArray):Boolean
		{
			if (!hasPiece(piece))
			{
				pieceMap.set(piece.offset, data);
				if (piece.compare(nonExistPiece) == 0)
				{
					nonExistPiece = getNextNonExistPiece(piece);
				}
				
				return true;
			}
			else
			{
				return false;	
			}
		}
		
		public function hasPiece(piece:Piece):Boolean
		{
			if (piece == null) {
				return false;
			}else{
				return pieceMap.hasKey(piece.offset);
			}
		}
		
		public function getPiece(piece:Piece):ByteArray
		{
			var bytes:ByteArray = pieceMap.get(piece.offset) as ByteArray; 
			if (bytes != null)
			{
				bytes.position = 0;
			}
			
			return bytes;
		}
		
		public function getNextNonExistPiece(start:Piece):Piece
		{
			if (startPiece == null || startPiece.compare(start) != 0)
			{
				startPiece = start;
				nonExistPiece = start;
			}
			
			var lastPiece:Piece = new Piece(0, Math.floor(fileLength / Utils.PIECE_SIZE));
			if (start.compare(lastPiece) < 0) {
				var newpiece:Piece = new Piece(0, start.pieceIndex + 1);
				if (newpiece.compare(lastPiece) == 0) {
					newpiece.size = fileLength % Utils.PIECE_SIZE;
				} else {
					newpiece.size = Utils.PIECE_SIZE;
				}
				return newpiece;
			}else{
				return null;
			}
		}
	}
}