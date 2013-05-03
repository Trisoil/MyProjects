package com.pplive.ds
{
	import de.polygonal.ds.Comparable;
	
	public class IntValueComparable implements Comparable
	{
		public var val:int;
		public function IntValueComparable(val:int)
		{
			this.val = val;
		}
		
		public function compare(other:Object):int
		{
			if (this.val == other.val)
				return 0;
			else if (this.val < other.val)
				return -1;
			else
				return 1;
		}
	}
}