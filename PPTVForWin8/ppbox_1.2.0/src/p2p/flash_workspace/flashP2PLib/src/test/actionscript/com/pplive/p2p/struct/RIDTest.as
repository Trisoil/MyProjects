package com.pplive.p2p.struct
{
	import com.pplive.test.object.valueEqualTo;
	import com.pplive.util.StringConvert;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import org.flexunit.Assert;
	import org.flexunit.assertThat;
	
	public class RIDTest
	{
		private var ridString:String;
		private var ridBytes:ByteArray;
		
		public function RIDTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			ridString = new String("275781FED82F1AD9B45637CAD3DB809F");
			
			ridBytes = new ByteArray;
			ridBytes.endian = Endian.LITTLE_ENDIAN;
			ridBytes.writeByte(0xFE);
			ridBytes.writeByte(0x81);
			ridBytes.writeByte(0x57);
			ridBytes.writeByte(0x27);
			ridBytes.writeByte(0x2F);
			ridBytes.writeByte(0xD8);
			ridBytes.writeByte(0xD9);
			ridBytes.writeByte(0x1A);
			ridBytes.writeByte(0xB4);
			ridBytes.writeByte(0x56);
			ridBytes.writeByte(0x37);
			ridBytes.writeByte(0xCA);
			ridBytes.writeByte(0xD3);
			ridBytes.writeByte(0xDB);
			ridBytes.writeByte(0x80);
			ridBytes.writeByte(0x9F);
			ridBytes.position = 0;
		}
		
		[Test]
		public function testConstructFromString():void
		{
			var rid:RID = new RID(ridString);
			Assert.assertTrue(rid.isValid());
			Assert.assertEquals(ridString, rid.toString());
			assertThat(ridBytes, valueEqualTo(rid.bytes()));
		}
		
		[Test]
		public function testConstructFromBytes():void
		{
			var rid:RID = new RID;
			rid.setBytes(ridBytes);
			Assert.assertEquals(0, ridBytes.bytesAvailable);
			Assert.assertTrue(rid.isValid());
			Assert.assertEquals(ridString, rid.toString());
			assertThat(ridBytes, valueEqualTo(rid.bytes()));
		}
	}
}