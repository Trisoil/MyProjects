package com.pplive.p2p
{
	import com.pplive.p2p.CacheManager;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.utils.ByteArray;
	
	import haxe.io.Bytes;
	
	import org.flexunit.Assert;
	
	public class CacheManagerTest
	{
		private var cacheManager:CacheManager;
		
		public function CacheManagerTest()
		{
		}
		
		[Test]
		public function testRegularFile():void
		{			
			cacheManager = new CacheManager(21800712);
			
			for(var i:uint = 0; i < 130; ++i)
			{
				var array:ByteArray = new ByteArray;
				array.writeInt(i);
				array.position = 0;
				Assert.assertTrue(cacheManager.addSubPiece(new SubPiece(0, i), array));
			}
			
			for(i = 768; i < 810; ++i)
			{
				array = new ByteArray;
				array.writeInt(i);
				array.position = 0;
				Assert.assertTrue(cacheManager.addSubPiece(new SubPiece(10, i), array));
			}
			
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(0,0), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(0,50), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(0,129), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(10,768), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(10,800), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(10,809), new ByteArray));
			
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(0,0)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(0,50)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(0,129)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(10,768)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(10,800)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(10,809)));
			
			Assert.assertTrue(cacheManager.hasPiece(new Piece(0,0)));
			Assert.assertTrue(cacheManager.hasPiece(new Piece(10,6)));
			Assert.assertFalse(cacheManager.hasPiece(new Piece(0,1)));
			Assert.assertFalse(cacheManager.hasPiece(new Piece(10,5)));
			Assert.assertFalse(cacheManager.hasPiece(new Piece(10,7)));
			
			// TODO(herain):2011-8-31:验证Piece带有subpieceIndex时的hasPiece
			
			var byteArray:ByteArray = cacheManager.getSubPiece(new SubPiece(0, 50));
			Assert.assertEquals(50, byteArray.readInt());
			
			byteArray = cacheManager.getSubPiece(new SubPiece(0, 129));
			Assert.assertEquals(129, byteArray.readInt());
			
			Assert.assertEquals(null, cacheManager.getSubPiece(new SubPiece(0, 768)));
			byteArray = cacheManager.getSubPiece(new SubPiece(10, 768));
			Assert.assertEquals(768, byteArray.readInt());
			byteArray = cacheManager.getSubPiece(new SubPiece(10, 800));
			Assert.assertEquals(800, byteArray.readInt());
			byteArray = cacheManager.getSubPiece(new SubPiece(10, 809));
			Assert.assertEquals(809, byteArray.readInt());
			
			cacheManager = null;
		}
		
		[Test]
		public function testSubPieceSizeFile():void
		{
			cacheManager = new CacheManager(21799936);
			
			for(var i:uint = 0; i < 130; ++i)
			{
				var array:ByteArray = new ByteArray;
				array.writeInt(i);
				array.position = 0;
				Assert.assertTrue(cacheManager.addSubPiece(new SubPiece(0, i), array));
			}
			
			for(i = 640; i < 768; ++i)
			{
				array = new ByteArray;
				array.writeInt(i);
				array.position = 0;
				Assert.assertTrue(cacheManager.addSubPiece(new SubPiece(10, i), array));
			}
			
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(0,0), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(0,50), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(0,129), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(10,640), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(10,700), new ByteArray));
			Assert.assertFalse(cacheManager.addSubPiece(new SubPiece(10,767), new ByteArray));
			
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(0,0)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(0,50)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(0,129)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(10,640)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(10,700)));
			Assert.assertTrue(cacheManager.hasSubPiece(new SubPiece(10,767)));
			
			Assert.assertTrue(cacheManager.hasPiece(new Piece(0,0)));
			Assert.assertTrue(cacheManager.hasPiece(new Piece(10,5)));
			Assert.assertFalse(cacheManager.hasPiece(new Piece(0,1)));
			Assert.assertFalse(cacheManager.hasPiece(new Piece(10,4)));
			Assert.assertFalse(cacheManager.hasPiece(new Piece(10,6)));
			
			var byteArray:ByteArray = cacheManager.getSubPiece(new SubPiece(0, 50));
			Assert.assertEquals(50, byteArray.readInt());
			
			byteArray = cacheManager.getSubPiece(new SubPiece(0, 129));
			Assert.assertEquals(129, byteArray.readInt());
			
			Assert.assertEquals(null, cacheManager.getSubPiece(new SubPiece(0, 768)));
			byteArray = cacheManager.getSubPiece(new SubPiece(10, 640));
			Assert.assertEquals(640, byteArray.readInt());
			byteArray = cacheManager.getSubPiece(new SubPiece(10, 700));
			Assert.assertEquals(700, byteArray.readInt());
			byteArray = cacheManager.getSubPiece(new SubPiece(10, 767));
			Assert.assertEquals(767, byteArray.readInt());
			
			cacheManager = null;
		}
		
		[Test]
		public function testNextNonExistSubPiece():void
		{
			cacheManager = new CacheManager(21800712);
			Assert.assertEquals(0, new SubPiece(0,0).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,100).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			cacheManager.addSubPiece(new SubPiece(0, 0), new ByteArray);
			Assert.assertEquals(0, new SubPiece(0,1).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,100).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			cacheManager.addSubPiece(new SubPiece(0, 3), new ByteArray);
			Assert.assertEquals(0, new SubPiece(0,1).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,100).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			cacheManager.addSubPiece(new SubPiece(0, 2), new ByteArray);
			Assert.assertEquals(0, new SubPiece(0,1).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,100).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			cacheManager.addSubPiece(new SubPiece(0, 1), new ByteArray);
			Assert.assertEquals(0, new SubPiece(0,4).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,100).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			cacheManager.addSubPiece(new SubPiece(0, 101), new ByteArray);
			Assert.assertEquals(0, new SubPiece(0,4).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,100).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			cacheManager.addSubPiece(new SubPiece(0, 100), new ByteArray);
			Assert.assertEquals(0, new SubPiece(0,4).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 0))));
			Assert.assertEquals(0, new SubPiece(0,102).compare(cacheManager.getNextNonExistSubPiece(new SubPiece(0, 100))));
			
			// test end
			cacheManager.addSubPiece(new SubPiece(10, 809), new ByteArray);
			Assert.assertNull((cacheManager.getNextNonExistSubPiece(new SubPiece(10, 809))));
		}
	}
}