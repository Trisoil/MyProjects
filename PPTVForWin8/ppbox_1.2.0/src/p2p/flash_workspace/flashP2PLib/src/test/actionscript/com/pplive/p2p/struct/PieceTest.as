package com.pplive.p2p.struct
{
	import com.pplive.p2p.struct.Piece;
	
	import flash.net.getClassByAlias;
	import flash.utils.ByteArray;
	
	import com.pplive.mx.ObjectUtil;
	
	import org.flexunit.Assert;
	import org.flexunit.assertThat;
	import org.hamcrest.core.allOf;
	import org.hamcrest.core.throws;
	import org.hamcrest.object.instanceOf;
	
	public class PieceTest
	{
		[Test]
		public function testCompare():void
		{
			var s1:Piece = new Piece(0,0);
			var s2:Piece = new Piece(0,1);
			var s3:Piece = new Piece(0,15);
			var s4:Piece = new Piece(1,2);
			var s5:Piece = new Piece(0,1);
			
			Assert.assertEquals(s1.compare(s2), -1);
			Assert.assertEquals(s1.compare(s3), -1);
			Assert.assertEquals(s1.compare(s4), -1);
			Assert.assertEquals(s2.compare(s3), -1);
			Assert.assertEquals(s2.compare(s4), -1);
			
			Assert.assertEquals(s2.compare(s1), 1);
			Assert.assertEquals(s3.compare(s1), 1);
			Assert.assertEquals(s4.compare(s1), 1);
			Assert.assertEquals(s4.compare(s3), 1);
			Assert.assertEquals(s3.compare(s2), 1);
			
			Assert.assertEquals(s5.compare(s1), 1);
			Assert.assertEquals(s5.compare(s2), 0);
			Assert.assertEquals(s5.compare(s3), -1);
			Assert.assertEquals(s5.compare(s4), -1);
		}
		
		[Test]
		public function testCopy():void
		{
			var piece:Piece = new Piece(1,2);
			var pieceCopy:Piece = Piece(ObjectUtil.copy(piece));
			Assert.assertTrue(pieceCopy is Piece);
			Assert.assertNotNull(pieceCopy);
			Assert.assertEquals(piece.blockIndex, pieceCopy.blockIndex);
			Assert.assertEquals(piece.pieceIndex, pieceCopy.pieceIndex);	
		}
		
		[Test]
		public function testSubPieceIndex():void
		{
			var s1:Piece = new Piece(0, 0, 12);
			var s2:Piece = new Piece(0, 1);
			var s3:Piece = new Piece(0, 1, 18);
			var s4:Piece = new Piece(0, 1, 0);
			
			Assert.assertEquals(s1.compare(s2), -1);
			Assert.assertEquals(s2.compare(s3), -1);
			Assert.assertEquals(s1.compare(s3), -1);
			Assert.assertEquals(s2.compare(s4), 0);
			Assert.assertEquals(s2.compare(s1), 1);
			Assert.assertEquals(s3.compare(s2), 1);
			Assert.assertEquals(s3.compare(s1), 1);
			
			var piece:Piece = new Piece(1,2,3);
			var pieceCopy:Piece = Piece(ObjectUtil.copy(piece));
			Assert.assertTrue(pieceCopy is Piece);
			Assert.assertNotNull(pieceCopy);
			Assert.assertEquals(piece.blockIndex, pieceCopy.blockIndex);
			Assert.assertEquals(piece.pieceIndex, pieceCopy.pieceIndex);
			Assert.assertEquals(piece.subPieceIndex, pieceCopy.subPieceIndex);
		}
		
		[Test]
		public function testGetOffset():void
		{
			var p1:Piece = new Piece(0, 0, 12);
			var p2:Piece = new Piece(0, 12, 10);
			var p3:Piece = new Piece(12, 10, 9);
			
			Assert.assertEquals(12288, p1.getOffset());
			Assert.assertEquals(1583104, p2.getOffset());
			Assert.assertEquals(26485760, p3.getOffset());
		}
		
		[Test]
		public function testGetFirstSubPiece():void
		{
			var p1:Piece = new Piece(1, 1);
			var p2:Piece = new Piece(1, 1, 10);
			
			Assert.assertEquals(0, new SubPiece(1, 128).compare(p1.getFirstSubPiece()));
			Assert.assertEquals(0, new SubPiece(1, 138).compare(p2.getFirstSubPiece()));
		}
	}
}