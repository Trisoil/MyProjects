package com.pplive.util
{
	import org.flexunit.Assert;
	
	public class GUIDTest
	{
		public function GUIDTest()
		{
		}
		
		[Test]
		public function testGuid():void
		{
			var guid1:String = GUID.create();
			Assert.assertEquals(32, guid1.length);
			
			var guid2:String = GUID.create();
			Assert.assertTrue(guid1 != guid2);
		}
	}
}