package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.Piece;
	
	import flash.events.Event;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class DownloaderDriverTest
	{
		private var downloadDriver:DownloadDriver;
		private var targetMock:DownloadTagetMock;
		
		public function DownloaderDriverTest()
		{
		}
		
		//[Test(async)]
		public function testHttpDownloadFile():void
		{
			var stream:URLStream = new URLStream;
			var asyncHandler:Function = Async.asyncHandler(this, onKeyReceived, 5000, "testDownloadFile");
			stream.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			stream.load(new URLRequest("http://122.225.69.241/gKey.txt"));
		}
		
		public function onKeyReceived(event:Event, passThroughData:Object):void
		{
			var urlstream:URLStream = event.target as URLStream;
			var key:String = urlstream.readUTFBytes(urlstream.bytesAvailable);
			trace("onKeyReceived:" + key);
			
			var fileLength:uint = 21239727;
			targetMock = new DownloadTagetMock;
			targetMock.endSubPiece = StructUtil.getLastSubPieceByFileLength(fileLength);
			targetMock.expects("hasPiece").times(1).noArgs().willReturn(false);
			targetMock.expects("onRecvSubPiece").times(262).noArgs();
			
			var asyncHandler:Function = Async.asyncHandler(this, onTestComplete, 20000, "onKeyReceived");
			targetMock.eventDispatcher.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			
			var url:String = "http://192.168.13.21:8080/0/%CE%E4%B5%B1%28%B5%DA13%BC%AF%29.mp4?key=" + key;
			downloadDriver = new DownloadDriver(targetMock, false, url, new Vector.<String>, fileLength, new Piece(10, 0).getOffset(),0,0);
			downloadDriver.start();
		}
		
		private function onTestComplete(event:Event, passThroughData:Object):void
		{
			targetMock.verify();
			Assert.assertNull(targetMock.errorMessage());
			Assert.assertTrue(targetMock.success());
		}
		
		//[Test]
		public function testP2SPDownloadFile():void
		{
			// TODO(herain):2011-9-2: need to be implemented
		}
		
		[Test]
		public function testRequestNextSubPiece():void
		{
			var fileLength:uint = 21239727;
			targetMock = new DownloadTagetMock;
			targetMock.expects("hasPiece").times(2).noArgs().willReturn(false);

			var url:String = "http://192.168.13.21:8080/0/%CE%E4%B5%B1%28%B5%DA13%BC%AF%29.mp4?key=";
			var startPiece:Piece = new Piece(10, 0);
			downloadDriver = new DownloadDriver(targetMock, false, url, new Vector.<String>, fileLength, startPiece.getOffset(),0,0);
			//downloadDriver.start();
			
			var d1:DownloaderMock = new DownloaderMock;
			d1.attachDownloadDriver(downloadDriver);
			var d2:DownloaderMock = new DownloaderMock;
			d2.attachDownloadDriver(downloadDriver);
			
			Assert.assertEquals(0, startPiece.compare(d1.requestNextPiece()));
			d1.pause();
			Assert.assertEquals(0, startPiece.compare(d2.requestNextPiece()));
			
			targetMock.verify();
			Assert.assertNull(targetMock.errorMessage());
			Assert.assertTrue(targetMock.success());
		}
	}
}