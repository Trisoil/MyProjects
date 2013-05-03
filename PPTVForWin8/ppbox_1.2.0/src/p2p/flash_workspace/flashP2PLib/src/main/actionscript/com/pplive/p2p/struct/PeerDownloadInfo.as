package com.pplive.p2p.struct
{
	import flash.utils.ByteArray;

	public class PeerDownloadInfo
	{
		public var isDownloading:Boolean;
		public var onLineTime:uint;
		public var avgDownload:uint;
		public var nowDownload:uint;
		public var avgUpload:uint;
		public var nowUpload:uint;
		
		public function PeerDownloadInfo(bytes:ByteArray)
		{
			isDownloading = bytes.readUnsignedByte() == 1 ? true : false;
			onLineTime = bytes.readUnsignedInt();
			avgDownload = bytes.readUnsignedShort();
			nowDownload = bytes.readUnsignedShort();
			avgUpload = bytes.readUnsignedShort();
			nowUpload = bytes.readUnsignedShort();
			bytes.position += 3;	// ignore 3 byte reserved bytes
		}
	}
}