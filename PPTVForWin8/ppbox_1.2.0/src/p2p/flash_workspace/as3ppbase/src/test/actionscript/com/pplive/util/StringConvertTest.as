package com.pplive.util
{
	import flash.utils.ByteArray;
	import org.flexunit.Assert;

	public class StringConvertTest
	{
		public function StringConvertTest()
		{
		}
		
		[Test]
		public function test():void 
		{
			var str:String = "275781FED82F1AD9B45637CAD3DB809F";
			trace("str:"+str);
			var bytes:ByteArray = StringConvert.hexString2ByteArray(str);
			var str1:String = StringConvert.byteArray2HexString(bytes);
			trace("str1:"+str1);
			Assert.assertTrue(str == str1);
		}
	}
}