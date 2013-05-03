package com.pplive.util
{
	import flash.utils.ByteArray;

	public class StringConvert
	{
		public static function byteArray2HexString(bytes:ByteArray):String 
		{
			var hexString:String = new String;
			for (var i:uint = 0; i < bytes.length; ++i)
			{
				hexString += byte2hex(bytes[i]);
			}
			
			return hexString;
		}
		
		private static function byte2hex(byte:uint):String {
			var hex:String = '';
			var arr:String = 'FEDCBA';
			
			for(var i:uint = 0; i < 2; i++) {
				if(((byte & (0xF0 >> (i * 4))) >> (4 - (i * 4))) > 9){
					hex += arr.charAt(15 - ((byte & (0xF0 >> (i * 4))) >> (4 - (i * 4))));
				}
				else{
					hex += String((byte & (0xF0 >> (i * 4))) >> (4 - (i * 4)));
				}
			}
			
			return hex;
		}
		
		public static function hexString2ByteArray(hexString:String):ByteArray 
		{
			if (hexString.length % 2 != 0)
				return null;
			
			var bytes:ByteArray = new ByteArray;
			for (var i:uint = 0; i < hexString.length / 2; ++i)
			{
				bytes.writeByte(hex2byte(hexString.substr(2 * i, 2)));
			}
			
			return bytes;
		}
		
		private static function hex2byte(hex:String):uint {
			if (hex.length != 2)
				return 0;
			
			var byte:uint = 0;
			for(var i:uint = 0; i < hex.length; ++i)
			{
				var char:String = hex.charAt(i);
				if (char >= '0' && char <= '9')
				{
					byte = 16 * byte + (uint(char) - uint('0'));
				}
				else if (char >= 'a' && char <= 'f')
				{
					byte = 16 * byte + (hexChar2Uint(char) - hexChar2Uint('a') + 10);
				}
				else if (char >= 'A' && char <= 'F')
				{
					byte = 16 * byte + (hexChar2Uint(char) - hexChar2Uint('A') + 10);
				}
				else
				{
					return 0;
				}
			}
			
			return byte;
		}
		
		private static function hexChar2Uint(hex:String):uint
		{
			if(hex == 'a' || hex == 'A')
				return 10;
			else if(hex == 'b' || hex == 'B')
				return 11;
			else if(hex == 'c' || hex == 'C')
				return 12;
			else if(hex == 'd' || hex == 'D')
				return 13;
			else if(hex == 'e' || hex == 'E')
				return 14;
			else if(hex == 'f' || hex == 'F')
				return 15;
			else
				return 0;
		}
		
		public static function urldecodeGB2312(str:String):String {
			var result:String = "";
			var byte:ByteArray = new ByteArray();
			str = unescape(str);
			for (var i:int; i < str.length; i++) {
				byte[i] = str.charCodeAt(i);
			}
			result = byte.readMultiByte(byte.length, "gb2312");
			return result;
		}
	}
}