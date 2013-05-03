package com.pplive.p2p.download
{
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.network.SocketAdaptor;
	import com.pplive.p2p.network.TcpStream;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.Socket;
	import flash.utils.Endian;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class PeerConnectionTest
	{
		private static var logger:ILogger = getLogger(PeerConnectionTest);
		private var socket:SocketAdaptor;
		private var connection:PeerConnection;
		private var p2pDownloaderMock:P2PDownloaderMock;
		private var ip:String = "192.168.26.37";
		public function PeerConnectionTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			logger.info("setup");
			p2pDownloaderMock = new P2PDownloaderMock;
		}
		
		[Test(async)]
		public function testSingleConnection():void
		{
			logger.info("testSingleConnection");
			socket = new SocketAdaptor();
			socket.endian = Endian.LITTLE_ENDIAN;
			var handler:Function = Async.asyncHandler(this, onSocketConnected, 500, null);
			socket.addEventListener(Event.CONNECT, handler, false, 0, true);
			socket.addEventListener(Event.CLOSE, onClose, false, 0, true);
			socket.addEventListener(IOErrorEvent.IO_ERROR, onError, false, 0, true);
			socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError, false, 0, true);
			socket.connect(ip, 16000);
		}
		
		private function onSocketConnected(event:Event, passThroughData:Object):void
		{
			logger.info("onSocketConnected event:" + event);
			var socket:SocketAdaptor = event.target as SocketAdaptor;
			var stream:TcpStream = new TcpStream(socket, new Endpoint(ip, 16000));
			
			p2pDownloaderMock.expects("onSubPiece").times(1024).noArgs();
			
			var handler:Function = Async.asyncHandler(this, onDownloadComplete, 20000, null);
			p2pDownloaderMock.addEventListener("onSubPiece", handler, false, 0, true);
			
			connection = new PeerConnection(stream, p2pDownloaderMock);
			connection.start();
		}
		
		private function onDownloadComplete(event:Event, passThroughData:Object):void
		{
			logger.info("onDownloadComplete event:" + event);
			Assert.assertNull(p2pDownloaderMock.errorMessage());
			Assert.assertTrue(p2pDownloaderMock.success());
			
			Assert.assertTrue(socket.connected);
			connection.stop();
			Assert.assertFalse(socket.connected);
		}
		
		private function onClose(event:Event):void
		{
			logger.error("onClose event:" + event);
		}
		
		private function onError(event:Event):void
		{
			logger.error("onError event:" + event);
		}
	}
}