package com.pplive.p2p.struct
{
	import com.pplive.util.StringConvert;
	
	import flash.utils.ByteArray;

	public class BlockMap
	{
		public var blockCount:uint;
		public var bitset:ByteArray = new ByteArray;
		
		public function BlockMap(bytes:ByteArray)
		{
			blockCount = bytes.readUnsignedInt();
			bytes.readBytes(bitset, 0, uint((blockCount + 7) / 8));
		}
		
		public function isFull():Boolean
		{
			for (var i:uint = 0; i < blockCount; ++i)
			{
				if (!hasBlock(i))
					return false;
			}
			
			return true;
		}
		
		public function hasBlock(blockIndex:uint):Boolean
		{
			if (blockIndex >= blockCount)
				return false;
			else
			{
				return (bitset[uint(blockIndex / 8)] & bitMask(blockIndex % 8)) != 0;
			}
		}
		
		private function bitMask(position:uint):uint
		{
			return 1 << position;
		}
		
		public function toString():String
		{
			return "(" + blockCount + ")" + StringConvert.byteArray2HexString(bitset);
		}
	}
}