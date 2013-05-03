package com.pplive.ds
{
	import de.polygonal.ds.Itr;
	
	public class StandardMapKeyIterator implements Itr
	{
		private var map:StandardMap;
		private var keys:Array;
		private var cursor:uint;
		
		public function StandardMapKeyIterator(map:StandardMap)
		{
			this.map = map;
			reset();
		}
		
		public function hasNext():Boolean
		{
			return cursor < keys.length;
		}
		
		public function next():Object
		{
			return keys[cursor++];
		}
		
		public function reset():Itr
		{
			keys = new Array();
			cursor = 0;
			
			var iter:Itr = map.iterator();
			while(iter.hasNext())
			{
				var pair:Pair = iter.next() as Pair;
				keys.push(pair.key);
			}		
			
			return this;
		}
	}
}