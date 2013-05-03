package com.pplive.p2p.struct
{
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import org.flexunit.Assert;

	public class PeerDownloadInfoTest
	{
		public function PeerDownloadInfoTest()
		{
		}
		
		[Test]
		public function test():void
		{
			var bytes:ByteArray = new ByteArray;
			bytes.endian = Endian.LITTLE_ENDIAN;
			bytes.writeByte(0x01);
			bytes.writeUnsignedInt(100);
			bytes.writeShort(10*1024);
			bytes.writeShort(20*1024);
			bytes.writeShort(30*1024);
			bytes.writeShort(40*1024);
			bytes.writeUnsignedInt(0);
			bytes.position = 0;
			
			var peerDownloadInfo:PeerDownloadInfo = new PeerDownloadInfo(bytes);
			Assert.assertEquals(1, bytes.bytesAvailable);
			Assert.assertEquals(true, peerDownloadInfo.isDownloading);
			Assert.assertEquals(100, peerDownloadInfo.onLineTime);
			Assert.assertEquals(10*1024, peerDownloadInfo.avgDownload);
			Assert.assertEquals(20*1024, peerDownloadInfo.nowDownload);
			Assert.assertEquals(30*1024, peerDownloadInfo.avgUpload);
			Assert.assertEquals(40*1024, peerDownloadInfo.nowUpload);
		}
	}
}