package com.pplive.ds
{	
	public class IntValue
	{
		public var val:int;
		public function IntValue(val:int)
		{
			this.val = val;
		}
		
		public function toString():String
		{
			return new String(val);
		}
	}
}