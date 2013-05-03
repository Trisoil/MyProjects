package com.pplive.p2p.network.protocol
{
	import flash.utils.ByteArray;	
	import flash.utils.Endian;
	
	public class StopDownloadPacket extends Packet
	{
		public static const ACTION:uint = 0xD7;
		
		//表示要停止下载的url
		private var _downloadUrl:String;		
		
		public function StopDownloadPacket(downloadUrl:String)
		{
			super(ACTION, Packet.NewTransactionID(), Packet.PROTOCOL_VERSION);
			_downloadUrl = downloadUrl;
		}
		
		override public function ToByteArray(bytes:ByteArray):void
		{
			super.ToByteArray(bytes);
			bytes.endian = Endian.LITTLE_ENDIAN;
			bytes.writeUTF(_downloadUrl);				
		}
	}
}