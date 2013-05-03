package com.pplive.ds
{
	import de.polygonal.ds.BinaryTreeNode;
	import de.polygonal.ds.Itr;
	
	internal class StandardMapIterator implements Itr
	{
		private var map:StandardMap;
		private var nodes:Array;
		private var index:uint;
		
		public function StandardMapIterator(map:StandardMap)
		{
			this.map = map;
			reset();
		}
		
		public function hasNext():Boolean
		{
			return index < nodes.length;
		}
		
		public function next():Object
		{
			return nodes[index++];
		}
		
		public function reset():Itr
		{
			nodes = new Array();
			if (map.root() != null)
			{
				inOrderRecusive(map.root(), nodes);	
			}
			
			index = 0;
			return this;
		}
		
		private function inOrderRecusive(node:BinaryTreeNode, array:Array):void
		{
			if (node.hasL())
				inOrderRecusive(node.l, array);
			array.push(node.val);
			if(node.hasR())
				inOrderRecusive(node.r, array);
		}
	}
}