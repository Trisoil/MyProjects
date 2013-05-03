package com.pplive.ds
{
	import de.polygonal.ds.BST;
	import de.polygonal.ds.BinaryTreeNode;
	import de.polygonal.ds.Collection;
	import de.polygonal.ds.DA;
	import de.polygonal.ds.Itr;
	import de.polygonal.ds.ListSet;
	import de.polygonal.ds.Map;
	import de.polygonal.ds.Set;
	
	public class StandardMap implements Map
	{
		protected var pairs:BST;
		
		public function StandardMap()
		{
			pairs = new BST();
		}
		
		public function size():int
		{
			return pairs.size();
		}
		
		public function get(key:Object):Object
		{
			var node:BinaryTreeNode = pairs.find(new Pair(key));
			if (node == null)
				return null;
			else
				return node.val.value;
		}
		
		public function hasKey(key:Object):Boolean
		{
			return pairs.contains(new Pair(key));
		}
		
		public function set(key:Object, x:Object):Boolean
		{
			var node:BinaryTreeNode = pairs.find(new Pair(key));
			if (node == null)
			{
				pairs.insert(new Pair(key, x));
				return true;
			}
			else
			{
				node.val.value = x;
				return false;
			}
		}
		
		public function remap(key:Object, x:Object):Boolean
		{
			var node:BinaryTreeNode = pairs.find(new Pair(key));
			if (node == null)
			{
				return false;
			}
			else
			{
				node.val.value = x;
				return true;
			}
		}
		
		/**
		 *  Returns true if this map contains a mapping for the element x. 
		 */
		public function has(x:Object):Boolean
		{
			var iter:Itr = pairs.iterator();
			while(iter.hasNext())
			{
				var pair:Pair= iter.next() as Pair;
				if (pair.value == x)
					return true;
			}
			
			return false;
		}
		
		/**
		 * Returns true if this collection contains the element x. 
		 */
		public function contains(x:Object):Boolean
		{
			// TODO(herain):contains meaning is not clear
			return has(x);
		}
		
		public function keys():Itr
		{
			return new StandardMapKeyIterator(this);
		}
		
		public function clr(key:Object):Boolean
		{
			return pairs.remove(new Pair(key));
		}
		
		/**
		 *	Returns an Set storing all keys in this map. 
		 */
		public function toKeySet():Set
		{
			var set:ListSet = new ListSet;
			var iter:Itr = new StandardMapKeyIterator(this);
			while(iter.hasNext())
			{
				set.set(iter.next());
			}
			
			return set;
		}
		
		/**
		 *	Returns an Set storing all values in this map. 
		 */
		public function toValSet():Set
		{
			var set:ListSet = new ListSet;
			var iter:Itr = new StandardMapValueIterator(this);
			while(iter.hasNext())
			{
				set.set(iter.next());
			}
			
			return set;
		}
		
		/**
		 *	Returns an DA storing all values in this map. 
		 */
		public function toDA():DA
		{
			var da:DA = new DA(size());
			var iter:Itr = new StandardMapValueIterator(this);
			while (iter.hasNext())
			{
				da.pushBack(iter.next());	
			}
			
			return da;
		}
		
		/**
		 *	Returns an array storing all values in this map. 
		 */
		public function toArray():Array
		{
			var array:Array = new Array;
			var iter:Itr = new StandardMapValueIterator(this);
			while (iter.hasNext())
			{
				array.push(iter.next());	
			}
			
			return array;
		}
		
		public function isEmpty():Boolean
		{
			return pairs.isEmpty();
		}
		
		public function remove(x:Object):Boolean
		{
			return pairs.remove(new Pair(x));
		}
		
		public function clear(purge:Boolean=false):void
		{
			// TODO:? purge ?
			pairs.clear(purge);
		}
		
		public function destory():void
		{
			clear(true);
			pairs = null;
		}
		
		public function iterator():Itr
		{
			return new StandardMapIterator(this);
		}
		
		public function free():void
		{
			// Destroys this object by explicitly nullifying all nodes, pointers and elements for GC'ing used resources
			pairs.free();
			pairs = null;
		}
		
		/**
		 * Duplicates this map.
		 * 
		 * @param assign If true, the copier parameter is ignored and primitive elements are copied 
		 * by value whereas objects are copied by reference.
		 * If false, the clone() method is called on each element. 
		 * In this case all values have to implement Cloneable or have the clone instance method. 
		 * 
		 * @param copier A custom function for copying elements. Replaces value.clone() if assign is false.
		 * 
		 */
		public function clone(assign:Boolean, copier:Object=null):Collection
		{
			var copy:StandardMap = new StandardMap;
			var iter:Itr = iterator();
			while(iter.hasNext())
			{
				var pair:Pair = iter.next() as Pair;
				
				if (assign)
				{
					copy.set(pair.key, pair.value);
				}
				else if (copier == null)
				{
					copy.set(pair.key, pair.value.clone());
				}
				else
				{
					copy.set(pair.key, copier(pair.value));
				}
			}
			
			return copy;
		}
		
		public function toString():String
		{
			return pairs.toString();
		}
		
		internal function root():BinaryTreeNode
		{
			return pairs.root();
		}
		
		public function begin():Pair
		{
			var node:BinaryTreeNode = pairs.root();
			while(node.hasL())
			{
				node = node.l;
			}
			
			return node.val as Pair;
		}
	}
}