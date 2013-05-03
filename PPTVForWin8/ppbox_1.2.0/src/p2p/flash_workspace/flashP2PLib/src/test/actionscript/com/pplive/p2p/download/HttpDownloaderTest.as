package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.Piece;
	
	import flash.events.Event;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;
	
	public class HttpDownloaderTest
	{
		private var ddMock:DownloadDriverMock;
		private var http:HttpDownloader;
		
		public function HttpDownloaderTest()
		{
		}
		
		[Before]
		public function setUp():void
		{
			ddMock = new DownloadDriverMock;
		}
		
		[After]
		public function tearDown():void
		{
			ddMock = null;
			http = null;
		}
		
		public function onTestComplete(event:Event, passThroughData:Object):void
		{
			ddMock.verify();
			Assert.assertNull(ddMock.errorMessage());
			Assert.assertTrue(ddMock.success());
		}
		
		//[Test(async)]
		public function testInvalidUrl():void
		{
			ddMock.expects("getFileLength").times(1).noArgs().willReturn(21800798);
			ddMock.expects("requestNextPiece").times(1).withAnyArgs().willReturn(new Piece(0, 0));
			ddMock.expects("onRecvSubPiece").times(0);
			ddMock.expects("onHttpError").times(1).withArg(HttpDownloader.HTTP_URL_ERROR).noReturn();
			
			var asyncHandler:Function = Async.asyncHandler(this, onTestComplete, 500, "testInvalidUrl", onTimeOut);
			ddMock.eventDispatcher.addEventListener(DownloadDriverMock.URL_ERROR, asyncHandler, false, 0, true);
			
			http = new HttpDownloader("");
			http.attachDownloadDriver(ddMock);
			http.resume();
		}
		
		//[ignore]
		//[Test(async)]
		public function testInvalidServer():void
		{
			ddMock.expects("getFileLength").times(1).noArgs().willReturn(21800798);
			ddMock.expects("requestNextPiece").times(1).withAnyArgs().willReturn(new Piece(0, 0));
			ddMock.expects("onRecvSubPiece").times(0);
			ddMock.expects("onHttpError").times(1).withArg(HttpDownloader.HTTP_IO_ERROR).noReturn();
			
			var asyncHandler:Function = Async.asyncHandler(this, onTestComplete, 500, "testInvalidServer", onTimeOut);
			ddMock.eventDispatcher.addEventListener(DownloadDriverMock.IO_ERROR, asyncHandler, false, 0, true);
			
			// invalid server 192.168.192.168
			http = new HttpDownloader("http://192.168.192.168:8080/0/test.mp4?key=0");
			http.attachDownloadDriver(ddMock);
			http.resume();
		}
		
		public function onTimeOut(passThroughDdata:Object):void
		{
			Assert.fail("HttpDownloaderTest::onTimeOut test:" +　passThroughDdata);
		}
		
		//[Test(async)]
		public function testInSecurityServer():void
		{
			ddMock.expects("getFileLength").times(1).noArgs().willReturn(21800798);
			ddMock.expects("requestNextPiece").times(1).withAnyArgs().willReturn(new Piece(0, 0));
			ddMock.expects("onRecvSubPiece").times(0);
			ddMock.expects("onHttpError").times(1).withArg(HttpDownloader.HTTP_SECURITY_ERROR).noReturn();
			
			var asyncHandler:Function = Async.asyncHandler(this, onTestComplete, 500, "testInSecurityServer", onTimeOut);
			ddMock.eventDispatcher.addEventListener(DownloadDriverMock.SECURITY_ERROR, asyncHandler, false, 0, true);
			
			// insecurity server 192.168.192.168
			http = new HttpDownloader("http://www.baidu.com/0/test.mp4?key=0");
			http.attachDownloadDriver(ddMock);
			http.resume();
		}
		
		[Test(async)]
		public function testLegalUrl():void
		{
			var stream:URLStream = new URLStream;
			var asyncHandler:Function = Async.asyncHandler(this, onKeyReceived, 5000, "testLegalUrl", onTimeOut);
			stream.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			stream.load(new URLRequest("http://122.225.69.241/gKey.txt"));
		}
		
		public function onKeyReceived(event:Event, passThroughData:Object):void
		{
			var urlstream:URLStream = event.target as URLStream;
			var key:String = urlstream.readUTFBytes(urlstream.bytesAvailable);
			trace("onKeyReceived:" + key);
			
			var fileLength:uint = 21239727;
			ddMock.expects("getFileLength").times(262).noArgs().willReturn(fileLength);
			ddMock.expects("requestNextPiece").times(1).withAnyArgs().willReturn(new Piece(5, 0));
			ddMock.expects("onRecvSubPiece").times(262).noArgs();
			ddMock.expects("onHttpError").times(0);
			ddMock.endSubPiece = Constants.getLastSubPieceByFileLength(fileLength);
			
			var asyncHandler:Function = Async.asyncHandler(this, onTestComplete, 20000, "onKeyReceived", onTimeOut);
			ddMock.eventDispatcher.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			
			// download range [1000000, 1100000]
			var url:String = "http://192.168.13.21:8080/0/%CE%D2%B0%AE%BC%C7%B8%E8%B4%CA-20110121-%CD%F5%CC%CF%BA%A3%B7%E5%CE%AA%C1%CB%B4%F7%BE%FD%D6%F1%D5%F9%B7%E7%B3%D4%B4%D7.mp4?key=" + key;
			http = new HttpDownloader(url);
			http.attachDownloadDriver(ddMock);
			http.resume();
		}
	}
}