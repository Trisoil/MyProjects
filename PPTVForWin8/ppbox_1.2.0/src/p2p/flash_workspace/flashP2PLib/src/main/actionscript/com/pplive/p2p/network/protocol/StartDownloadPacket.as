package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.RID;
	import flash.utils.ByteArray;	
	import flash.utils.Endian;
	
	public class StartDownloadPacket extends Packet
	{
		public static const ACTION:uint = 0xD6;

		//这些变量里，目前只有downloadurl有用，表示要下载的url		
		private var _downloadUrl:String;
		private var _referUrl:String;
		private var _userAgent:String;
		private var _fileName:String;
		
		public function StartDownloadPacket(downloadUrl:String)
		{
			super(ACTION, Packet.NewTransactionID(), Packet.PROTOCOL_VERSION);
			_downloadUrl = downloadUrl;
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
			bytes.endian = Endian.LITTLE_ENDIAN;
			bytes.writeUTF(_downloadUrl);
			
			//后面3个没有赋值，先写上3个长度0
			bytes.writeShort(0);
			bytes.writeShort(0);
			bytes.writeShort(0);			
		}
	}
}