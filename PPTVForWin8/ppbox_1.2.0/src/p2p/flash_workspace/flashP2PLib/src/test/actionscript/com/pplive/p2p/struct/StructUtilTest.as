package com.pplive.p2p.struct
{
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import org.flexunit.Assert;
	
	public class StructUtilTest
	{
		[Test]
		public function testRegularFile():void
		{
			var fileLength:uint = 21800798;
			Assert.assertEquals(167, StructUtil.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(11, StructUtil.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, StructUtil.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(7, StructUtil.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(128, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(42, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(10,6)));
			Assert.assertEquals(0, new Piece(10, 6).compare(StructUtil.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(10, 809).compare(StructUtil.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(862, StructUtil.getLastSubPieceSizeByFileLength(fileLength));
			Assert.assertEquals(0, new Piece(0, 0).compare(StructUtil.getPieceFromSubPiece(new SubPiece(0,100))));
			Assert.assertEquals(0, new Piece(1, 15).compare(StructUtil.getPieceFromSubPiece(new SubPiece(1,2047))));
			Assert.assertEquals(100, StructUtil.getSubPieceIndexInPiece(new SubPiece(0,100)));
			Assert.assertEquals(127, StructUtil.getSubPieceIndexInPiece(new SubPiece(0,2047)));
		}
		
		[Test]
		public function testSubPieceSizeFile():void
		{
			var fileLength:uint = 21799936;	// 21289*1024
			Assert.assertEquals(167, StructUtil.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(11, StructUtil.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, StructUtil.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(7, StructUtil.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(128, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(41, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(10,6)));
			Assert.assertEquals(0, new Piece(10, 6).compare(StructUtil.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(10, 808).compare(StructUtil.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(1024, StructUtil.getLastSubPieceSizeByFileLength(fileLength));
			Assert.assertEquals(0, new Piece(0, 0).compare(StructUtil.getPieceFromSubPiece(new SubPiece(0,100))));
			Assert.assertEquals(0, new Piece(1, 15).compare(StructUtil.getPieceFromSubPiece(new SubPiece(1,2047))));
			Assert.assertEquals(100, StructUtil.getSubPieceIndexInPiece(new SubPiece(0,100)));
			Assert.assertEquals(127, StructUtil.getSubPieceIndexInPiece(new SubPiece(1,2047)));
		}
		
		[Test]
		public function testPieceSizeFile():void
		{
			var fileLength:uint = 21757952;	// 166* 128*1024
			Assert.assertEquals(166, StructUtil.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(11, StructUtil.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, StructUtil.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(6, StructUtil.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(0, StructUtil.getPieceCountInBlock(fileLength, 11));
			Assert.assertEquals(128, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(0, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(10,6)));
			Assert.assertEquals(128, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(10,5)));
			Assert.assertEquals(0, new Piece(10, 5).compare(StructUtil.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(10, 767).compare(StructUtil.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(1024, StructUtil.getLastSubPieceSizeByFileLength(fileLength));
		}
		
		[Test]
		public function testBlockSizeFile():void
		{
			var fileLength:uint = 20971520;	// 10* 2048*1024
			Assert.assertEquals(160, StructUtil.getPieceCountByFileLength(fileLength)); 
			Assert.assertEquals(10, StructUtil.getBlockCountByFileLength(fileLength));
			Assert.assertEquals(16, StructUtil.getPieceCountInBlock(fileLength, 0));
			Assert.assertEquals(16, StructUtil.getPieceCountInBlock(fileLength, 9));
			Assert.assertEquals(0, StructUtil.getPieceCountInBlock(fileLength, 10));
			Assert.assertEquals(128, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(0,0)));
			Assert.assertEquals(0, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(10,0)));
			Assert.assertEquals(128, StructUtil.getSubPieceCountInPiece(fileLength, new Piece(9,15)));
			Assert.assertEquals(0, new Piece(9, 15).compare(StructUtil.getLastPieceByFileLength(fileLength)));
			Assert.assertEquals(0, new SubPiece(9, 2047).compare(StructUtil.getLastSubPieceByFileLength(fileLength)));
			Assert.assertEquals(1024, StructUtil.getLastSubPieceSizeByFileLength(fileLength));
		}
	}
}
