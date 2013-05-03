package com.pplive.test.object
{
	import com.pplive.test.object.valueEqualTo;
	
	import org.hamcrest.assertThat;
	import org.hamcrest.core.not;
	
	public class ValueEqualToTest
	{
		[Test]
		public function testValueEqual():void
		{
			// null
			assertThat(null, valueEqualTo(null));
			
			// NaN
			assertThat(NaN, valueEqualTo(NaN));
			
			// num value
			assertThat(1, valueEqualTo(1));
			assertThat(12, valueEqualTo(12));
			assertThat(1, not(valueEqualTo(3)));
			
			// string value
			assertThat("pplive", valueEqualTo("pplive"));
			assertThat("pps", not(valueEqualTo("pplive")));
			
			// Object
			var o1:Object = new Object;
			var o2:Object = new Object;
			o1.num = 1;
			o1.str = "pplive";
			o2.num = 1;
			o2.str = "pplive";
			assertThat(o1, valueEqualTo(o2));
			
			//Array
			var a1:Array = new Array;
			var a2:Array = new Array;
			var a3:Array = new Array;
			// a1 = "plive"
			a1.push("p");
			a1.push("l");
			a1.push("i");
			a1.push("v");
			a1.push("e");
			
			// a2 = "plive"
			a2.push("p");
			a2.push("l");
			a2.push("i");
			a2.push("v");
			a2.push("e");
			
			// a3="plivf"
			a3.push("p");
			a3.push("l");
			a3.push("i");
			a3.push("v");
			a3.push("f");
			assertThat(a1, valueEqualTo(a2));
			assertThat(a1, not(valueEqualTo(a3)));
		}
	}
}