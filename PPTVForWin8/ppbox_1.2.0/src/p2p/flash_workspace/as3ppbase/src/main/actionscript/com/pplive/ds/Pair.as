package com.pplive.ds
{
	import de.polygonal.ds.Comparable;
	import com.pplive.mx.ObjectUtil;
	
	public class Pair implements Comparable
	{
		private var _key:Object;
		public var value:Object;
		
		public function Pair(key:Object, value:Object = null)
		{
			this._key = key;
			this.value = value;
		}
		
		public function get key():Object
		{
			return this._key;
		}
		
		public function compare(other:Object):int
		{
			if (this.key is Comparable)
			{
				return this.key.compare(other.key);			
			}
			else
			{
				return ObjectUtil.compare(this.key, other.key);
			}
		}
		
		public function toString():String
		{
			return "Pair:" + this.key + "/" + this.value;
		}
	}
}