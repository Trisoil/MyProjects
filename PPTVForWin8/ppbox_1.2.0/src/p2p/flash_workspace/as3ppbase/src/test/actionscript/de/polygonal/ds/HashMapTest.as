package de.polygonal.ds
{
	import de.polygonal.ds.HashMap;
	import org.flexunit.Assert;
	
	public class HashMapTest
	{
		public function HashMapTest()
		{
		}
		
		[Test]
		public function testHasKey():void
		{
			var map:HashMap = new HashMap;
			map.set(1, 2);
			Assert.assertTrue(map.hasKey(1));
			Assert.assertFalse(map.hasKey(2));
			Assert.assertEquals(2, map.get(1));
		}
	}
}