package com.pplive.dac.logclient
{
	import flash.utils.ByteArray;
	
	internal class SimpleEncrypt
	{
		private var _keyBytes:ByteArray;
		
		public function SimpleEncrypt(keyBytes:ByteArray)
		{
			this._keyBytes = new ByteArray();
			this._keyBytes.writeBytes(keyBytes);
			this._keyBytes.position = 0;
		}
		
		public function Encrypt(input:String) : ByteArray
		{
			return EncryptImpl(input, this._keyBytes);
		}
		
		private function EncryptImpl(input:String, keyBytes:ByteArray) : ByteArray
		{
			var inputBytes:ByteArray = new ByteArray();
			inputBytes.writeUTFBytes(input);
			inputBytes.position = 0;
			
			var result:ByteArray = new ByteArray();
			result.length = inputBytes.length;
			result.position = 0;
			keyBytes.position = 0;
			for (var i:int = 0; i < inputBytes.length; i+=1)
			{
				result[i] = (inputBytes[i] + keyBytes[i % keyBytes.length]);
			}
			result.position = 0;
			return result;
		}
	}

}