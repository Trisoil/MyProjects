// ActionScript file

package com.pplive.test.object
{
	import org.hamcrest.Matcher;
	
	public function valueEqualTo(value:Object):Matcher
	{
		return new IsValueEqualMatcher(value);
	}
}