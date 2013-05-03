package com.pplive.ds
{
	import de.polygonal.ds.Cloneable;
	
	public class IntValueClonable implements Cloneable
	{
		public var val:int;
		public function IntValueClonable(val:int)
		{
			this.val = val;
		}
		
		public function clone():Object
		{
			return new IntValueClonable(val);
		}
	}
}