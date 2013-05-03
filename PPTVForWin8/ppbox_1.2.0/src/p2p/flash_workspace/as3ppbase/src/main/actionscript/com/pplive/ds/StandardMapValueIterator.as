package com.pplive.ds
{
	import de.polygonal.ds.Itr;
	
	public class StandardMapValueIterator implements Itr
	{
		private var map:StandardMap;
		private var values:Array;
		private var index:uint;
		
		public function StandardMapValueIterator(map:StandardMap)
		{
			this.map = map;
			reset();
		}
		
		public function hasNext():Boolean
		{
			return index < values.length;
		}
		
		public function next():Object
		{
			return values[index++];
		}
		
		public function reset():Itr
		{
			values = new Array();
			index = 0;
			
			var iter:Itr = map.iterator();
			while(iter.hasNext())
			{
				var pair:Pair = iter.next() as Pair;
				values.push(pair.value);
			}		
			
			return this;
		}
	}
}