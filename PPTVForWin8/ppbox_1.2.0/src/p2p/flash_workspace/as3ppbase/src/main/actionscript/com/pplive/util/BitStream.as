package com.pplive.util
{
	import flash.utils.ByteArray;

	public class BitStream
	{
		private const Mask:Array=[
			0x00000000,
			0x00000001, 0x00000003, 0x00000007, 0x0000000f,
			0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
			0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
			0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
			0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
			0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
			0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
			0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,
		];
		
		private var _bytes:ByteArray = new ByteArray;
		private var _buffer_index:uint;
		private var _buffer_size:uint;
		private var _last_byte:uint;
		private var _last_byte_size:uint;
		private var _buffer_under_run:Boolean;
		public function BitStream(bytes:ByteArray)
		{
			_bytes.writeBytes(bytes, 0);
			_buffer_size = _bytes.length * 8;
			_buffer_under_run = false;
		}
		
		public function left_bits():uint
		{
			return _buffer_size + _last_byte_size;
		}
		
		public function read(bits_count:uint):uint
		{
			var return_value:uint;
			if (bits_count == 0 || bits_count > 32 || _buffer_under_run)
				return 0;
			
			if (bits_count > _buffer_size+_last_byte_size)
			{
				_buffer_size=0;
				_last_byte_size=0;
				_buffer_under_run=true;
				return 0;
			}
			
			if (bits_count <= _last_byte_size)
			{
				_last_byte_size -= bits_count;
				return_value= _last_byte >> _last_byte_size;
			}
			else
			{
				var new_bits:uint = bits_count - _last_byte_size;
				if (new_bits == 32)
					return_value = 0;
				else
					return_value = _last_byte << new_bits;
				switch (uint((new_bits-1)/8))
				{
					case 3:
						new_bits -= 8;
						return_value |= _bytes[_buffer_index] << new_bits;
						_buffer_index++;
						_buffer_size -= 8;
					case 2:
						new_bits -=8 ;
						return_value |= _bytes[_buffer_index] << new_bits;
						_buffer_index++;
						_buffer_size-=8;
					case 1:
						new_bits-=8;
						return_value |= _bytes[_buffer_index] << new_bits;
						_buffer_index++;
						_buffer_size -= 8;
					case 0:
						_last_byte = _bytes[_buffer_index];
						_buffer_index++;
				}
				
				_last_byte_size = Math.min(8, _buffer_size) - new_bits;
				_buffer_size -= Math.min(8, _buffer_size);
				return_value |= (_last_byte >> _last_byte_size) & Mask[new_bits];
			}
				
			return return_value & Mask[bits_count];
		}
		
		public function skip(bits_count:uint):void
		{
			if (bits_count == 0 || _buffer_under_run)
				return;
			
			if (bits_count > 32) //Algorithm is only for <=32 bits
			{
				do
				{
					skip(32);
					bits_count -= 32;
				}
				while(bits_count > 32);
				
				if (bits_count)
					skip(bits_count);
				return;
			}
			
			if (bits_count > _buffer_size + _last_byte_size)
			{
				_buffer_size=0;
				_last_byte_size=0;
				_buffer_under_run=true;
				return;
			}
			
			if (bits_count <= _last_byte_size)
				_last_byte_size -= bits_count;
			else
			{
				var new_bits:uint = bits_count - _last_byte_size;
				switch (uint((new_bits-1)/8))
				{
					case 3 :
						new_bits -= 8;
						_buffer_index++;
						_buffer_size -= 8;
					case 2 :    
						new_bits -= 8;
						_buffer_index++;
						_buffer_size-=8;
					case 1 :    
						new_bits-=8;
						_buffer_index++;
						_buffer_size-=8;
					case 0 :
						_last_byte = _bytes[_buffer_index];
						_buffer_index++;
				}
				
				_last_byte_size = Math.min(8, _buffer_size) - new_bits;
				_buffer_size -= Math.min(8, _buffer_size);
			}
		}
	}
}