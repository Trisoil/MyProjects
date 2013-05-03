package com.pplive.p2p.struct
{
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.test.object.valueEqualTo;
	
	import de.polygonal.ds.HashMap;
	
	import com.pplive.mx.ObjectUtil;
	
	import org.flexunit.Assert;
	import org.hamcrest.assertThat;
	
	public class SubPieceTest
	{
		[Test]
		public function testCompare():void
		{
			var s1:SubPiece = new SubPiece(0,0);
			var s2:SubPiece = new SubPiece(0,1);
			var s3:SubPiece = new SubPiece(0,100);
			var s4:SubPiece = new SubPiece(1,2);
			var s5:SubPiece = new SubPiece(0,1);
			
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
		public function testMoveNext():void
		{
			var s1:SubPiece = new SubPiece(0,0);
			var s2:SubPiece = new SubPiece(0,1);
			
			s1.moveToNextSubPiece();
			Assert.assertEquals(0, s2.compare(s1));		
			
			s1 = new SubPiece(0, 2047);
			s2 = new SubPiece(1, 0);
			s1.moveToNextSubPiece();
			Assert.assertEquals(0, s2.compare(s1));
		}
		
		[Test]
		public function testCopy():void
		{
			var subPiece:SubPiece = new SubPiece(1,2);
			var subPieceCopy:SubPiece = SubPiece(com.pplive.mx.ObjectUtil.copy(subPiece));
			Assert.assertNotNull(subPieceCopy);
			Assert.assertEquals(subPiece.blockIndex, subPieceCopy.blockIndex);
			Assert.assertEquals(subPiece.subPieceIndex, subPieceCopy.subPieceIndex);
		}
		
		[Test]
		public function testGetOffset():void
		{
			var s1:SubPiece = new SubPiece(0, 100);
			var s2:SubPiece = new SubPiece(1, 20);
			
			Assert.assertEquals(102400, s1.offset);
			Assert.assertEquals(2117632, s2.offset);
			
			s1.moveToNextSubPiece();
			Assert.assertEquals(103424, s1.offset);
			s2.moveToNextSubPiece();
			Assert.assertEquals(2118656, s2.offset);
			
			var s3:SubPiece = new SubPiece(0, 2047);
			s3.moveToNextSubPiece();
			Assert.assertEquals(2097152, s3.offset);
		}
		
		[Test]
		public function testGetSize():void
		{
			var fileLength:uint = 21800798;
			
			Assert.assertEquals(1024, new SubPiece(10, 808).getSize(fileLength));
			Assert.assertEquals(862, new SubPiece(10, 809).getSize(fileLength));
		}
		
		[Test]
		public function testHashable():void
		{
			var map:HashMap = new HashMap;
			
			var s1:SubPiece = new SubPiece(0,0);
			Assert.assertTrue(map.set(s1, 1));
			Assert.assertFalse(map.set(s1, 2));
			
			Assert.assertTrue(map.hasKey(s1));
			Assert.assertFalse(map.hasKey(new SubPiece(0,0)));
			Assert.assertFalse(map.hasKey(new SubPiece(0,1)));
			Assert.assertEquals(1, map.get(s1));
		}
		
		[Test]
		public function testGetPiece():void
		{
			var s1:SubPiece = new SubPiece(0,0);
			var s2:SubPiece = new SubPiece(0,127);
			var s3:SubPiece = new SubPiece(0,128);
			var p1:Piece = new Piece(0,0);
			var p2:Piece = new Piece(0,1);
			
			assertThat(s1.getPiece(), valueEqualTo(p1));
			assertThat(s2.getPiece(), valueEqualTo(p1));
			assertThat(s3.getPiece(), valueEqualTo(p2));
		}
	}
}