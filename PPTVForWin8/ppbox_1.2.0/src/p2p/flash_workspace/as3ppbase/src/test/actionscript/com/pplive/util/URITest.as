package com.pplive.util
{
	import com.pplive.util.URI;
	
	import org.flexunit.Assert;
	
	public class URITest
	{
		public function URITest()
		{
		}
		
		[Test]
		public function testLegalUrl():void
		{
			var url:String = "http://192.168.21.23:8080/0/test.mp4?key=120";
			var uri:URI = new URI(url);
			Assert.assertEquals("http", uri.protocol);
			Assert.assertEquals("192.168.21.23", uri.host);
			Assert.assertEquals(8080, uri.port);
			Assert.assertEquals("/0/test.mp4", uri.path);
			Assert.assertEquals(120, uri.variables.key);
			Assert.assertEquals(url, uri.toString());
		}
		
		[Test]
		public function testMissProtocolAndPort():void
		{
			var url:String = "192.168.21.23/0/test.mp4?key=120";
			var uri:URI = new URI(url);
			Assert.assertNull(uri.protocol);
			Assert.assertEquals("192.168.21.23", uri.host);
			Assert.assertEquals(0, uri.port);
			Assert.assertEquals("/0/test.mp4", uri.path);
			Assert.assertEquals(120, uri.variables.key);
			Assert.assertEquals(url, uri.toString());
		}
		
		[Test]
		public function testMissPathAndQueyString():void
		{
			var url:String = "192.168.21.23";
			var uri:URI = new URI(url);
			Assert.assertNull(uri.protocol);
			Assert.assertEquals("192.168.21.23", uri.host);
			Assert.assertEquals(0, uri.port);
			Assert.assertNull(uri.path);
			Assert.assertNull(uri.variables);
			Assert.assertEquals(url, uri.toString());
		}
		
		[Test]
		public function testEmptyUrl():void
		{
			var uri:URI = new URI("");
			Assert.assertNull(uri.protocol);
			Assert.assertNull(uri.host);
			Assert.assertEquals(0, uri.port);
			Assert.assertNull(uri.path);
			Assert.assertNull(uri.variables);
		}
	}
}