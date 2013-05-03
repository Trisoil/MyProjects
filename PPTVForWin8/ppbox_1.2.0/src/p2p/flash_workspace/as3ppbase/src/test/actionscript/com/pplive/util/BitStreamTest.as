package com.pplive.util
{
	import com.pplive.util.StringConvert;
	
	import flash.utils.ByteArray;
	
	import org.flexunit.Assert;

	public class BitStreamTest
	{
		private var stream:BitStream;
		
		public function BitStreamTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			stream = new BitStream(StringConvert.hexString2ByteArray("139056E5A54800"));
		}
		
		[After]
		public function teardown():void
		{
			stream = null;
		}
		
		[Test]
		public function testRead():void
		{			
			Assert.assertEquals(2, stream.read(5));
			Assert.assertEquals(7, stream.read(4));
			Assert.assertEquals(2, stream.read(4));
			Assert.assertEquals(0, stream.read(1));
			Assert.assertEquals(0, stream.read(1));
			Assert.assertEquals(0, stream.read(1));
			Assert.assertEquals(0x2B7, stream.read(11));
			Assert.assertEquals(5, stream.read(5));
			Assert.assertEquals(1, stream.read(1));
			Assert.assertEquals(4, stream.read(4));
			Assert.assertEquals(0x15200, stream.read(17));
			
			// test read overflow
			Assert.assertEquals(0, stream.read(33));
			Assert.assertEquals(0, stream.read(4));
		}
		
		[Test]
		public function testSkip():void
		{
			stream.skip(5);
			Assert.assertEquals(7, stream.read(4));
			stream.skip(4);
			Assert.assertEquals(0, stream.read(1));
			stream.skip(1);
			Assert.assertEquals(0, stream.read(1));
			stream.skip(11);
			Assert.assertEquals(5, stream.read(5));
			stream.skip(1);
			Assert.assertEquals(4, stream.read(4));
			stream.skip(17);
			Assert.assertEquals(0, stream.read(2));
		}
	}
}