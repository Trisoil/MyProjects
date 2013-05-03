package
{
	import com.pplive.util.StringConvert;
	
	import flash.net.ObjectEncoding;
	import flash.utils.ByteArray;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class FlvEncryptionHeader extends Object
	{
		private static var logger:ILogger = getLogger(FlvEncryptionHeader);
		public var drmHeader:AdditionalHeader;
		public var flashAccessv2:FlashAccessv2;
		public var keyInformation:KeyInformation;
		public var encodingParams:StandardEncodingParameters;
		public var encryptionHeader:EncryptionHeader;
		public function FlvEncryptionHeader(metaData:String)
		{
			flashAccessv2 = new FlashAccessv2(metaData);
			keyInformation = new KeyInformation(flashAccessv2);
			encodingParams = new StandardEncodingParameters(keyInformation);
			encryptionHeader = new EncryptionHeader(encodingParams);
			drmHeader = new AdditionalHeader(encryptionHeader);
		}
		public function getEncryptedHeaderArray():ByteArray
		{
			var bytes:ByteArray = new ByteArray;
			bytes.objectEncoding = ObjectEncoding.AMF0;
			
			bytes.writeByte(2);
			//bytes.writeShort();
			bytes.writeUTF("|AdditionalHeader");
			//bytes.writeByte(8);			//ECMA Arrary
			//bytes.writeUnsignedInt(1);	//Array Element is 1

			//bytes.writeUTF("Encryption");
			bytes.writeObject(drmHeader);
			
			var str:String = new String(StringConvert.byteArray2HexString(bytes));
			logger.info(str);
			
			bytes.position = 0;
			
			return bytes;
		}
	}
}



class EncryptionHeader
{
	public var Version:Number = 2;
	public var Method:String = "Standard";
	public var Flags:Number = 0;
	public var Params:StandardEncodingParameters;
	
	public function EncryptionHeader(params:StandardEncodingParameters)
	{
		Params = params;
	}
}

class StandardEncodingParameters
{
	public var Version:Number = 1;
	public var EncryptionAlgorithm:String = "AES-CBC";
	public var EncryptionParams:AES_CBCEncryptionParameters;
	public var KeyInfo:KeyInformation;
	public function StandardEncodingParameters(keyinfo:KeyInformation)
	{
		EncryptionParams = new AES_CBCEncryptionParameters();
		KeyInfo = keyinfo;
	}
}


class AES_CBCEncryptionParameters
{
	public var KeyLength:Number = 16;
	public function AES_CBCEncryptionParameters()
	{
	}
}


class KeyInformation 
{
	//public var subType:String = "FlashAccessv2";
	//public var FMRMS_METADATA:FlashAccessv2;
	public var SubType:String = "FlashAccessv2";
	public var Data:FlashAccessv2;
	public function KeyInformation(data:FlashAccessv2)
	{
		//FMRMS_METADATA = data;
		Data = data;
	}
}

class FlashAccessv2
{
	public var Metadata:String;
	public function FlashAccessv2(metaData:String) 
	{
		Metadata = metaData;
	}
}

class AdditionalHeader
{
	public var Encryption:EncryptionHeader;
	public function AdditionalHeader(ench:EncryptionHeader) 
	{
		Encryption = ench;
	}	
}





