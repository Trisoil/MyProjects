package com.pplive.p2p.struct
{
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import org.flexunit.Assert;

	public class BlockMapTest
	{
		public function BlockMapTest()
		{
		}
		
		[Test]
		public function testNonFullBlock():void
		{
			var bytes:ByteArray = new ByteArray;
			bytes.endian = Endian.LITTLE_ENDIAN;
			bytes.writeUnsignedInt(10);
			bytes.writeByte(0x7F);
			bytes.writeByte(0x02);
			bytes.position = 0;
			var blockMap:BlockMap = new BlockMap(bytes);
			
			Assert.assertEquals(10, blockMap.blockCount);
			Assert.assertFalse(blockMap.isFull());
			Assert.assertTrue(blockMap.hasBlock(0));
			Assert.assertTrue(blockMap.hasBlock(1));
			Assert.assertTrue(blockMap.hasBlock(2));
			Assert.assertTrue(blockMap.hasBlock(3));
			Assert.assertTrue(blockMap.hasBlock(4));
			Assert.assertTrue(blockMap.hasBlock(5));
			Assert.assertTrue(blockMap.hasBlock(6));
			Assert.assertFalse(blockMap.hasBlock(7));
			Assert.assertFalse(blockMap.hasBlock(8));
			Assert.assertTrue(blockMap.hasBlock(9));
			Assert.assertFalse(blockMap.hasBlock(10));
		}
		
		[Test]
		public function testFullBlock():void
		{
			var bytes:ByteArray = new ByteArray;
			bytes.endian = Endian.LITTLE_ENDIAN;
			bytes.writeUnsignedInt(10);
			bytes.writeByte(0xFF);
			bytes.writeByte(0x03);
			bytes.position = 0;
			var blockMap:BlockMap = new BlockMap(bytes);
			
			Assert.assertEquals(10, blockMap.blockCount);
			Assert.assertTrue(blockMap.isFull());
			for(var i:uint = 0; i < 10; ++i)
			{
				Assert.assertTrue(blockMap.hasBlock(i));
			}
		}
	}
}