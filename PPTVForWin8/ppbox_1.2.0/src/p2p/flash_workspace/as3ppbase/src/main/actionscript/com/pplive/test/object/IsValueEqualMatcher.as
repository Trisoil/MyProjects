package com.pplive.test.object
{
	import org.hamcrest.BaseMatcher;
	import org.hamcrest.Description;
	import com.pplive.mx.ObjectUtil;
	
	/**
	 * Checks the item being matched is equal (mx.utils.ObjectUtil.compare).
	 * 
	 * IsValueEqualMatcher is almost the same with IsEqualMatcher in hamecrest except it use 
	 * mx.utils.ObjectUtil.compare rather than == to compare two objects.
	 *
	 * <ul>
	 * <li><code>Number</code>s match if they are equal (==) </li>
	 * <li><code>Number</code>s match if they are both <code>NaN</code>. </li>
	 * <li><code>null</code>s match.</li>
	 * <li><code>Array</code>s match if they are the same length and each item is equal.
	 *  Checked recursively for child arrays. </li>
	 * </ul>
	 * 
	 * @author Herain Wang
	 */
	internal class IsValueEqualMatcher extends BaseMatcher 
	{		
		private var _value:Object;
		
		/**
		 * Constructor
		 *
		 * @param value Object the item being matched must be equal to
		 */
		public function IsValueEqualMatcher(value:Object)
		{
			super();
			
			_value = value;
		}
		
		/**
		 * @inheritDoc
		 */
		override public function matches(item:Object):Boolean
		{
			return areEqual(item, _value);
		}
		
		/**
		 * @inheritDoc
		 */
		override public function describeTo(description:Description):void
		{
			description.appendValue(_value);
		}
		
		/**
		 * Checks if the given items are equal(mx.utils.ObjectUtil.compare)
		 * 
		 * This method can be override by sub classes with other equal criterion.
		 *
		 * @protected
		 */
		protected function areEqual(o1:Object, o2:Object):Boolean
		{
			// remember your NaN is super special, give her a call, she'll appreciate it.
			if (o1 is Array)
			{
				return o2 is Array && areArraysEqual(o1 as Array, o2 as Array);
			}
			else
			{
				return ObjectUtil.compare(o1, o2) == 0;
			}
		}
		
		/**
		 * Checks if the given arrays are of equal length, and contain the same elements.
		 *
		 * @private
		 */
		private function areArraysEqual(o1:Array, o2:Array):Boolean
		{
			return areArraysLengthsEqual(o1, o2) && areArrayElementsEqual(o1, o2);
		}
		
		/**
		 * Checks if the given arrays are of equal length
		 *
		 * @private
		 */
		private function areArraysLengthsEqual(o1:Array, o2:Array):Boolean
		{
			return o1.length == o2.length;
		}
		
		/**
		 * Checks the elements of both arrays are the equal
		 *
		 * @private
		 */
		private function areArrayElementsEqual(o1:Array, o2:Array):Boolean
		{
			for (var i:int = 0, n:int = o1.length; i < n; i++)
			{
				if (!areEqual(o1[i], o2[i]))
				{
					return false;
				}
			}
			return true;
		}
	}
}