package com.pplive.p2p.struct
{
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import org.flexunit.Assert;
	
	public class ConstantsTest
	{
		[Test]
		public function testRegularFile():void
		{
			var fileLength:uint = 21800798;
			Assert.assertEquals(167, Constants.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(11, Constants.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, Constants.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(7, Constants.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(128, Constants.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(42, Constants.getSubPieceCountInPiece(fileLength, new Piece(10,6)));
			Assert.assertEquals(0, new Piece(10, 6).compare(Constants.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(10, 809).compare(Constants.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(862, Constants.getLastSubPieceSizeByFileLength(fileLength));
			Assert.assertEquals(0, new Piece(0, 0).compare(Constants.getPieceFromSubPiece(new SubPiece(0,100))));
			Assert.assertEquals(0, new Piece(1, 15).compare(Constants.getPieceFromSubPiece(new SubPiece(1,2047))));
			Assert.assertEquals(100, Constants.getSubPieceIndexInPiece(new SubPiece(0,100)));
			Assert.assertEquals(127, Constants.getSubPieceIndexInPiece(new SubPiece(0,2047)));
		}
		
		[Test]
		public function testSubPieceSizeFile():void
		{
			var fileLength:uint = 21799936;	// 21289*1024
			Assert.assertEquals(167, Constants.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(11, Constants.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, Constants.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(7, Constants.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(128, Constants.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(41, Constants.getSubPieceCountInPiece(fileLength, new Piece(10,6)));
			Assert.assertEquals(0, new Piece(10, 6).compare(Constants.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(10, 808).compare(Constants.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(1024, Constants.getLastSubPieceSizeByFileLength(fileLength));
			Assert.assertEquals(0, new Piece(0, 0).compare(Constants.getPieceFromSubPiece(new SubPiece(0,100))));
			Assert.assertEquals(0, new Piece(1, 15).compare(Constants.getPieceFromSubPiece(new SubPiece(1,2047))));
			Assert.assertEquals(100, Constants.getSubPieceIndexInPiece(new SubPiece(0,100)));
			Assert.assertEquals(127, Constants.getSubPieceIndexInPiece(new SubPiece(1,2047)));
		}
		
		[Test]
		public function testPieceSizeFile():void
		{
			var fileLength:uint = 21757952;	// 166* 128*1024
			Assert.assertEquals(166, Constants.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(11, Constants.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, Constants.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(6, Constants.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(0, Constants.getPieceCountInBlock(fileLength, 11));
			Assert.assertEquals(128, Constants.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(0, Constants.getSubPieceCountInPiece(fileLength, new Piece(10,6)));
			Assert.assertEquals(128, Constants.getSubPieceCountInPiece(fileLength, new Piece(10,5)));
			Assert.assertEquals(0, new Piece(10, 5).compare(Constants.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(10, 767).compare(Constants.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(1024, Constants.getLastSubPieceSizeByFileLength(fileLength));
		}
		
		[Test]
		public function testBlockSizeFile():void
		{
			var fileLength:uint = 20971520;	// 10* 2048*1024
			Assert.assertEquals(160, Constants.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(10, Constants.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, Constants.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(16, Constants.getPieceCountInBlock(fileLength, 9));
			Assert.assertEquals(0, Constants.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(128, Constants.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(0, Constants.getSubPieceCountInPiece(fileLength, new Piece(10,0)));
			Assert.assertEquals(128, Constants.getSubPieceCountInPiece(fileLength, new Piece(9,15)));
			Assert.assertEquals(0, new Piece(9, 15).compare(Constants.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(9, 2047).compare(Constants.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(1024, Constants.getLastSubPieceSizeByFileLength(fileLength));
		}
	}
}