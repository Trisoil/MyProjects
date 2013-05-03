package de.polygonal.ds
{
	import org.flexunit.Assert;
	import de.polygonal.ds.ArrayUtil;
	public class ArrayUtilTest
	{
		public function ArrayUtilTest()
		{
		}
		
		[Test]
		public function test_bsearchInt():void
		{
			var intArray:Array = new Array;
			intArray.push(1);
			intArray.push(117);
			intArray.push(173);
			intArray.push(209);
			intArray.push(309);
			intArray.push(384);
			intArray.push(489);
			intArray.push(550);
			intArray.push(609);
			intArray.push(654);
			
			Assert.assertEquals(0, ArrayUtil.bsearchInt(intArray, 1, 0, intArray.length - 1));
			Assert.assertEquals(1, ArrayUtil.bsearchInt(intArray, 117, 0, intArray.length - 1));
			Assert.assertEquals(2, ArrayUtil.bsearchInt(intArray, 173, 0, intArray.length - 1));
			Assert.assertEquals(3, ArrayUtil.bsearchInt(intArray, 209, 0, intArray.length - 1));
			Assert.assertEquals(4, ArrayUtil.bsearchInt(intArray, 309, 0, intArray.length - 1));
			Assert.assertEquals(5, ArrayUtil.bsearchInt(intArray, 384, 0, intArray.length - 1));
			Assert.assertEquals(6, ArrayUtil.bsearchInt(intArray, 489, 0, intArray.length - 1));
			Assert.assertEquals(7, ArrayUtil.bsearchInt(intArray, 550, 0, intArray.length - 1));
			Assert.assertEquals(8, ArrayUtil.bsearchInt(intArray, 609, 0, intArray.length - 1));
			Assert.assertEquals(9, ArrayUtil.bsearchInt(intArray, 654, 0, intArray.length - 1));
			
			Assert.assertEquals(~0, ArrayUtil.bsearchInt(intArray, 0, 0, intArray.length - 1));
			Assert.assertEquals(~1, ArrayUtil.bsearchInt(intArray, 100, 0, intArray.length - 1));
			Assert.assertEquals(~2, ArrayUtil.bsearchInt(intArray, 150, 0, intArray.length - 1));
			Assert.assertEquals(~3, ArrayUtil.bsearchInt(intArray, 200, 0, intArray.length - 1));
			Assert.assertEquals(~4, ArrayUtil.bsearchInt(intArray, 250, 0, intArray.length - 1));
			Assert.assertEquals(~5, ArrayUtil.bsearchInt(intArray, 350, 0, intArray.length - 1));
			Assert.assertEquals(~6, ArrayUtil.bsearchInt(intArray, 480, 0, intArray.length - 1));
			Assert.assertEquals(~7, ArrayUtil.bsearchInt(intArray, 500, 0, intArray.length - 1));
			Assert.assertEquals(~8, ArrayUtil.bsearchInt(intArray, 600, 0, intArray.length - 1));
			Assert.assertEquals(~9, ArrayUtil.bsearchInt(intArray, 650, 0, intArray.length - 1));
			Assert.assertEquals(~10, ArrayUtil.bsearchInt(intArray, 700, 0, intArray.length - 1));
		}
	}
}