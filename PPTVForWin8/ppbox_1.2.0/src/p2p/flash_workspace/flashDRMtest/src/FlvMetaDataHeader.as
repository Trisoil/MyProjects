package
{
	import com.pplive.util.StringConvert;
	
	import flash.net.ObjectEncoding;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.describeType;
	import flash.xml.XMLDocument;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class FlvMetaDataHeader extends Object
	{
		private static var logger:ILogger = getLogger(FlvMetaDataHeader);
		
		private var metaDataHeader:MetaDataHeader;
		private var amf:AMF0 = new AMF0();
		
		public function FlvMetaDataHeader()
		{
			metaDataHeader = new MetaDataHeader;
		}
		public function getMetaHeaderArray():ByteArray
		{
			var bytes:ByteArray = new ByteArray;
			bytes.objectEncoding = ObjectEncoding.AMF0;
			
			bytes.writeByte(2);
			//bytes.writeShort();
			bytes.writeUTF("onMetaData");
			//bytes.writeByte(8);			//ECMA Arrary
			//bytes.writeUnsignedInt(1);	//Array Element is 1
			
			//bytes.writeUTF("Encryption");
			//bytes.writeObject(metaDataHeader);
			var testObj:Array = new Array();
			testObj['duration'] = 4057053;
			testObj['width'] = 640;
			testObj['height'] = 352;
			testObj['videocodecid'] = 7; //"avc1";
			testObj['audiocodecid'] = 10; //"mp4a";
			testObj['avcprofile'] = 100;
			testObj['avclevel'] = 30;
			testObj['aacaot'] = 0;
			testObj['videoframerate'] = 23.976;
			//testObj['audiosamplerate'] = 44100.0;
			testObj['audiosamplerate'] = 24000.0;
			testObj['audiochannels'] = 2;
			//public var trackinfo:Vector.<TrackInfo>;
			var trackinfo:Array = new Array();
			var trackV:Object = new Object();
			trackV.length = 1352351;
			trackV.timescale = 30000;
			trackV.language = "eng";
			trackinfo[0] = trackV;
			//var trackA:TrackInfo = new TrackInfo(1083392, 24000);
			var trackA:Object = new Object();
			trackA.length = 1083392;
			trackA.timescale = 24000;
			trackA.language = "eng";
			trackinfo[1] = trackA;
			testObj['trackinfo'] = trackinfo;
			amf.writeData(bytes, testObj);
			
			var str:String = new String(StringConvert.byteArray2HexString(bytes));
			logger.info(str);
			
			bytes.position = 0;
			
			return bytes;
		}
	}
}

class MetaDataHeader
{
	public var duration:Number = 4057053;
	public var width:Number = 640;
	public var height:Number = 352;
	public var videocodecid:String = "avc1";
	public var audiocodecid:String = "mp4a";
	public var avcprofile:Number = 66;
	public var avclevel:Number = 30;
	public var aacaot:Number = 0;
	public var videoframerate:Number = 23.976;
	//public var audiosamplerate:Number = 44100.0;
	public var audiosamplerate:Number = 24000.0;
	public var audiochannels:Number = 2;
	//public var trackinfo:Vector.<TrackInfo>;
	public var trackinfo:Array;
	public function MetaDataHeader()
	{
		trackinfo = new Array(2);
		var trackV:TrackInfo = new TrackInfo(1352351, 30000);
		trackinfo[0] = trackV;
		var trackA:TrackInfo = new TrackInfo(1083392, 24000);
		trackinfo[1] = trackA;
	}
}

class TrackInfo
{
	public var length:Number;
	public var timescale:Number;
	public var language:String = "eng";
	public function TrackInfo(plength:Number, ptimescale:Number) 
	{
		length = plength;
		timescale = ptimescale;
	}
}


