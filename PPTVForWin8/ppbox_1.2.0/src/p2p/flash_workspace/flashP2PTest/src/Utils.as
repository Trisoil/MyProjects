package
{
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.external.ExternalInterface;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLVariables;
	import flash.net.navigateToURL;
	import flash.system.Capabilities;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	
	public class Utils
	{
		
		public static const ENCRYPT_ROUNDS:uint = 32; // at least 32
		public static const DELTA:uint = 0x9E3779B9;
		public static const FINAL_SUM:uint = 0xC6EF3720;
		public static const BLOCK_SIZE:uint = (4 << 1);
		public static const BLOCK_SIZE_TWICE:uint = ((4 << 1) << 1);
		public static const BLOCK_SIZE_HALF:uint = ((4 << 1) >> 1);		
		
		public static const SERVER_KEY:String = "qqqqqww";
		public static const SPLITER:String = "||";
		
		
		public static var REDRECT_URL:String = "http://player.pptv.com/fixpp/";		 //跳转到地址
		public static var BASE64_KEY:String = "kioe257ds";		
		private static const BASE64_CHARS:String = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
		public static const PLAYXML:String = 'webplay3';
		public static const PLAYXMLLIMIT:String = 'webplay4';
		public static const HOST:String = 'jump.synacast.com';
		public static const ERRORTIP:String = '非常抱歉,未能找到您所访问的';
		private static const IKANSPLIT:String = "@@@"; 
		public static const tf:TextField = new TextField();
		public static var showDebug:Boolean = true;
		
		public static var session:String;
		public static var refer:String = "";
		
		public static var sendCount:Number = 0;
		public static var recvCount:Number = 0;
		public static var seeked:Boolean = false;
		
		public static var ChannelId:String;
		public static var CatalogId:String;
		public static const TypeId:int = 3;
		private static var _lines:Number = 0;
		
		public static var player_x_scale:Number = 1;
		public static var player_y_scale:Number = 1;
		/**
		 * 
		 * 设置是什么版本的播放器 ！！！！！！重要！！！！！！
		 * */
		
		
		public function Utils()
		{}

		public static function decodeBase64(url:String,key:String = null):String
		{
			if(!key)key = BASE64_KEY;
			var bin:ByteArray = new ByteArray();			
			bin = decodeToByteArray(url);			
			bin.position = 0;
			
			var bout:ByteArray = new ByteArray();
			bout.length = bin.length;
			bout.position = 0;
			
			var bkey:ByteArray = new ByteArray();
			bkey.writeUTFBytes(key);//多两位
			bkey.position = 0;		
			
			for (var i:Number = 0; i < bin.length; i++)
					bout[i] = (bin[i] - bkey[i % bkey.length]);
			
			bout.position = 0;
			//ExternalInterface.call("console.log",bout.bytesAvailable);
			return bout.readUTFBytes(bout.bytesAvailable);					
		}
		
		private static function decodeToByteArray(data:String):ByteArray {
			//var charset:String = ExternalInterface.call("eval","document.charset");
			//if(!charset)charset =  ExternalInterface.call("eval","document.characterSet") || "";
			
			var output:ByteArray = new ByteArray();
			var dataBuffer:Array = new Array(4);
			var outputBuffer:Array = new Array(3);
			for (var i:uint = 0; i < data.length; i += 4) {
				for (var j:uint = 0; j < 4 && i + j < data.length; j++) {
					dataBuffer[j] = BASE64_CHARS.indexOf(data.charAt(i + j));
				}
				outputBuffer[0] = (dataBuffer[0] << 2) + ((dataBuffer[1] & 0x30) >> 4);
				outputBuffer[1] = ((dataBuffer[1] & 0x0f) << 4) + ((dataBuffer[2] & 0x3c) >> 2);		
				outputBuffer[2] = ((dataBuffer[2] & 0x03) << 6) + dataBuffer[3];
				for (var k:uint = 0; k < outputBuffer.length; k++) {
					if (dataBuffer[k+1] == 64) break;
					output.writeByte(outputBuffer[k]);					
				}
			}
			output.position = 0;
			return output;
		}
		
		public static function decodePlayLink(pl:String = ''):void{
			var uv:URLVariables = new URLVariables(decodeBase64(pl,'pplive'));
			var tchid:String = '-1';
			var tcid:String = '-1';
			for(var obj:Object in uv){
				switch(obj.toString()){
					case 'a':
						tchid = uv[obj].toString();
						break;
					case 'b':
						CatalogId = uv[obj].toString();
						break;
					case 'd':
						tcid = uv[obj].toString();
						break;
				}
			}
			if(tcid == '-1'){
				ChannelId = tchid;
			} else {
				ChannelId = tcid;
			}
		}
		
		/**
		 * 根据servertime和url用utils中的加密算法得到key参数
		 * 		 
		 * ***/
		public static function constructKey(sevTime:Number):String
		{
			var s:String = time2String(sevTime);
			var arr:Array = s.split("");
			if(arr.length<16)
			{
				s = add(s,16-arr.length);
			}
			var cipher:String = "";
			var key:String = Utils.SERVER_KEY;			
			if(key.length<16)
			{
				key = add(key,16 - key.length);
			}
			//trace("s:"+"length:"+s.length+","+s+",key"+key+",length:"+key.length);
			cipher = Utils.encrypt(s,key);
			cipher = Utils.Str2Hex(cipher);			
			return cipher;			
		}
		
		/**
		 * 秒数
		 * 返回16字节的字符串
		 * **/
		private static function time2String(time:Number):String
		{
			var arr:Array = new Array(8);
			var bytes:ByteArray = new ByteArray();
			var str:String = "0123456789abcdef";
			var xxx:Array = str.split("") ;
			for(var i:uint =0; i<8 ; i++)
			{
				var num:Number = (time >>> (28 - i % 8 * 4)) & 15;
				arr[i] = xxx[num];        
			}
			var ret:String = arr.join("");	        
			return ret;
		}
		
		private static function add(str:String,n:Number):String
		{
			for(var i:Number = 0;i< n;i++)
			{
				str+=getCharFromAscii(0);
			}			
			return str;
		}	
		
		private static function getkey(str:String):uint
		{			
			var arr:Array = str.split("");
			var key_num:uint = 0;
			for(var i:Number=0;i<arr.length;i++){
				var _str:String = arr[i];
				var num:uint = _str.charCodeAt(0);
				var add:uint = (num <<(i%4*8));				
				key_num ^= add;
			}
			return key_num;
		}
		
		private static function encrypt(strbuf:String, strkey:String):String
		{
			var SIZEA:int = 16;
			var k0:uint= getkey(strkey), k1:uint, k2:uint, k3:uint;
			
			var arrBuf:Array = strbuf.split("");
			var arrKey:Array = strkey.split("");
			
			var mid:Number = k0;
			k1 = (mid<<8)|(mid>>>24);
			k2 = (mid<<16)|(mid>>>16);
			k3 = (mid<<24)|(mid>>>8);
			
			var res:String = '';
			for(var i:uint=0;i+SIZEA <= arrBuf.length; i+=SIZEA)
			{
				var ascii0:uint = (arrBuf[i].charCodeAt(0)<<0);
				var ascii1:uint = (arrBuf[i+1].charCodeAt(0)<<8);
				var ascii2:uint = (arrBuf[i+2].charCodeAt(0)<<16);
				var ascii3:uint = (arrBuf[i+3].charCodeAt(0)<<24);
				var ascii4:uint = (arrBuf[i+4].charCodeAt(0)<<0);
				var ascii5:uint = (arrBuf[i+5].charCodeAt(0)<<8);
				var ascii6:uint = (arrBuf[i+6].charCodeAt(0)<<16);
				var ascii7:uint = (arrBuf[i+7].charCodeAt(0)<<24);
				
				var v0 :uint= 0 | ascii0 | ascii1 | ascii2 | ascii3;
				var v1 :uint =0 | ascii4 | ascii5 | ascii6 | ascii7;
				var sum:uint = 0;
				
				for(var j:int=0; j< 32; j++)
				{
					sum += DELTA;
					var t01:uint = (v1 << 4) + k0;
					var t02:uint = v1 + sum;
					var t03:uint = (v1 >>> 5) + k1;
					var addV0:uint = (t01 ^ t02 ^ t03);
					v0 += addV0;
					
					var t11:uint = (v0 << 4) + k2;
					var t12:uint = v0 + sum;
					var ttt:uint = 	(v0 >>> 5);				
					var t13:uint = (ttt + k3);
					
					var addV1:uint = (t11 ^ t12 ^ t13);			
					v1 += addV1; 
				}
				
				var xxx0:uint = (v0>>>0) & (0xFF);
				var xxx1:uint  = (v0>>>8) & (0xFF);
				var xxx2:uint  = (v0>>>16) & (0xFF);
				var xxx3:uint  = (v0>>>24) & (0xFF);
				
				var xxx4:uint  = (v1>>>0)&(0xFF);
				var xxx5:uint  = (v1>>>8) & (0xFF);
				var xxx6:uint  = (v1>>>16) &(0xFF);
				var xxx7:uint  = (v1>>>24) &(0xFF);
				
				res += getCharFromAscii((v0>>>0) & (0xFF));
				res += getCharFromAscii((v0>>>8) & (0xFF));
				res += getCharFromAscii((v0>>>16) & (0xFF));
				res += getCharFromAscii((v0>>>24) & (0xFF));
				
				res += getCharFromAscii((v1>>>0)&(0xFF));
				res += getCharFromAscii((v1>>>8) & (0xFF));
				res += getCharFromAscii((v1>>>16) &(0xFF) );
				res += getCharFromAscii((v1>>>24) &(0xFF));
			}
			res += strbuf.substr(8,8);
			//var l:Number = res.length;
			return res;
		}
		
		private static function getCharFromAscii(num:uint):String
		{
			return String.fromCharCode(num);
		}
		
		private static function Str2Hex(buffer:String):String
		{
			//trace("Utils.Str2Hex:"+buffer);
			var str:String =  "0123456789abcdef";
			var char_index:Array = str.split("");	
			var arr:Array = buffer.split("");		
			var hexstr:Array = new Array(2*arr.length+1);
			var buf_size:int = arr.length;
			
			var v0:Number,v1:Number;
			for (var i :int= 0; i < buf_size; i++)
			{
				if(i<8)
				{
					v0 = arr[i].charCodeAt(0)&0xF;
					v1 = (arr[i].charCodeAt(0)>>>0x4)&0xF;
					
					hexstr[2*i] = char_index[arr[i].charCodeAt(0)&0xF];
					hexstr[2*i+1] = char_index[(arr[i].charCodeAt(0)>>>0x4)&0xF];
				}
				else
				{
					hexstr[2*i] = char_index[Math.floor(Math.random()*15)];
					hexstr[2*i+1] = char_index[Math.floor(Math.random()*15)];
				}
			}
			//hexstr[2*buf_size] = getCharFromAscii(0);
			return hexstr.join("");
		}
	}
}