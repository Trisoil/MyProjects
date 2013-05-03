package com.pplive.p2p.struct
{
	import com.pplive.util.StringConvert;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class RID
	{
		public static const RID_LENGTH:uint = 16;
		private static const RID_HEX_STRING_LENGTH:uint = 32;
		
		private var data1:uint;
		private var data2:uint;
		private var data3:uint;
		private var data4:ByteArray = new ByteArray;
		private var _isVaild:Boolean = false;
		public function RID(ridString:String = null)
		{
			if(ridString != null && ridString.length == RID_HEX_STRING_LENGTH)
			{
				var bytes:ByteArray = new ByteArray;
				bytes.endian = Endian.BIG_ENDIAN;
				bytes.writeBytes(StringConvert.hexString2ByteArray(ridString));
				bytes.position = 0;
				setBytes(bytes);
			}
		}
		
		public function setBytes(inputBytes:ByteArray):void
		{
			if (inputBytes.bytesAvailable >= RID_LENGTH)
			{
				data1 = inputBytes.readUnsignedInt();
				data2 = inputBytes.readUnsignedShort();
				data3 = inputBytes.readUnsignedShort();
				inputBytes.readBytes(data4, 0, 8);
				_isVaild = true;
			}
		}
		
		public function isValid():Boolean 
		{ 
			return _isVaild; 
		}
		
		public function bytes():ByteArray 
		{
			var bytes:ByteArray = new ByteArray;
			bytes.endian = Endian.LITTLE_ENDIAN;
			getBytes(bytes);
			return bytes;
		}
		
		private function getBytes(bytes:ByteArray):void
		{
			bytes.writeUnsignedInt(data1);
			bytes.writeShort(data2);
			bytes.writeShort(data3);
			bytes.writeBytes(data4);
		}
		
		public function toString():String
		{
			var bytes:ByteArray = new ByteArray;
			bytes.endian = Endian.BIG_ENDIAN;
			getBytes(bytes);
			return StringConvert.byteArray2HexString(bytes);
		}
	}
}