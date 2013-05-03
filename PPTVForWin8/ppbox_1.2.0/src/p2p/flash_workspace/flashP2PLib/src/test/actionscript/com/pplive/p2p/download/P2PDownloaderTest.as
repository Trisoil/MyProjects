package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.test.object.valueEqualTo;
	
	import org.flexunit.Assert;
	import org.hamcrest.assertThat;

	public class P2PDownloaderTest
	{
		public function P2PDownloaderTest()
		{
		}
		
		[Test]
		public function testGetNextSubPiece():void
		{
			var rid:RID = new RID("2E9AC1EB0406BFBCD023F572B24A2B18");
			var p2pDownloader:P2PDownloader = new P2PDownloader(rid);
			var ddMock:DownloadDriverMock = new DownloadDriverMock;
			var fileLength:uint = 21239727;
			ddMock.expects("getFileLength").times(262).noArgs().willReturn(fileLength);
			ddMock.expects("requestNextPiece").times(1).withAnyArgs().willReturn(new Piece(0, 0));
			p2pDownloader.attachDownloadDriver(ddMock);
			p2pDownloader.resume();
			var offset:uint = 0;
			for(var i:uint = 0; i < 21239727/1024; ++i)
			{
				assertThat(p2pDownloader.getNextSubPiece(), valueEqualTo(SubPiece.createSubPieceFromOffset(offset)));
				offset += 1024;
			}
			
			Assert.assertNull(p2pDownloader.getNextSubPiece());
		}
	}
}