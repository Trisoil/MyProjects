package com.pplive.p2p.mp4
{
	import com.pplive.p2p.struct.Constants;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.utils.ByteArray;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class Mp4StreamTest
	{
		private var stream:Mp4Stream;
		private var urlStream:URLStream;
		
		public function Mp4StreamTest()
		{
		}
		
		[Before(async)]
		public function setup():void
		{
			trace("setup");
			stream = new Mp4Stream;
			urlStream = new URLStream;
			urlStream.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			var asyncHandler:Function = Async.asyncHandler(this, onHeaderComplete, 500, null, onTimeOut);
			urlStream.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			urlStream.load(new URLRequest("../../../TestResource/header.bin"));
		}
		
		private function onHeaderComplete(event:Event, passThroughData:Object):void
		{
			trace("onHeaderComplete bytesAvailable:" + urlStream.bytesAvailable);
			while(urlStream.bytesAvailable > 0)
			{
				var bytes:ByteArray = new ByteArray;
				if (urlStream.bytesAvailable > 1024)
					urlStream.readBytes(bytes, 0, 1024);
				else
					urlStream.readBytes(bytes);
				stream.appendBytes(bytes);
			}
		}
		
		[After]
		public function teardown():void
		{
			urlStream = null;
			stream = null;
		}
		
		[Test(async)]
		public function testRegularStream():void
		{
			Assert.assertTrue(stream.hasMp4Header());
			
			urlStream = new URLStream;
			var asyncHandler:Function = Async.asyncHandler(this, onRegularFileComplete, 500, null, onTimeOut);
			urlStream.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			urlStream.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			urlStream.load(new URLRequest("../../../TestResource/first7sample.bin"));
		}
		
		private function onRegularFileComplete(event:Event, passThroughData:Object):void
		{
			trace("onRegularFileComplete bytesAvailable:" + urlStream.bytesAvailable);
			while(urlStream.bytesAvailable > 0)
			{
				var bytes:ByteArray = new ByteArray;
				if (urlStream.bytesAvailable > 1024)
					urlStream.readBytes(bytes, 0, 1024);
				else
					urlStream.readBytes(bytes);
				stream.appendBytes(bytes);
			}
			
			var flvBytes:ByteArray = stream.readBytes();
			Assert.assertEquals(33468, flvBytes.length);
			flvBytes.position = 0;
			// test flv header
			Assert.assertEquals(70, flvBytes.readUnsignedByte());
			Assert.assertEquals(76, flvBytes.readUnsignedByte());
			Assert.assertEquals(86, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			Assert.assertEquals(5, flvBytes.readUnsignedByte());
			Assert.assertEquals(9, flvBytes.readInt());
			Assert.assertEquals(0, flvBytes.readInt());
			
			// test audio header
			// 		type = 8
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			//		body length = 4
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(4, flvBytes.readUnsignedByte());
			//		timestamp = 0
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			flvBytes.position += 3;
			//		body
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x12, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x10, flvBytes.readUnsignedByte());
			//		tagsize
			Assert.assertEquals(15, flvBytes.readUnsignedInt());
			
			// test audio sample 0
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			// 		type
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			//		audio sample 0 first 4 bytes
			Assert.assertEquals(0x21, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x08, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x4f, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xf7, flvBytes.readUnsignedByte());
			flvBytes.position += 170;
			flvBytes.position += 4;
			
			// test video header
			Assert.assertEquals(9, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			//		type
			Assert.assertEquals(0x17, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			//		composition time
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			//		AVCDecoderConfiguration
			Assert.assertEquals(0x01, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x64, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x15, flvBytes.readUnsignedByte());
			flvBytes.position += 36;
			flvBytes.position += 4;
			
			// test video sample 0
			Assert.assertEquals(9, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			//		type
			Assert.assertEquals(0x17, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			//		composition time
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x29, flvBytes.readUnsignedByte());
			//		video sample 0 first 4 bytes
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x70, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x40, flvBytes.readUnsignedByte());
			flvBytes.position += 28736;
			flvBytes.position += 4;
			// test audio sample 1
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			// 		type
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			flvBytes.position += 172;
			flvBytes.position += 4;
			// test audio sample 2
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			// 		type
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			flvBytes.position += 172;
			flvBytes.position += 4;
			
			// test video sample 1
			Assert.assertEquals(9, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			// 		type
			Assert.assertEquals(0x27, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			//		composition time
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x53, flvBytes.readUnsignedByte());
			//		video sample 1 first 4 bytes
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x0e, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x5d, flvBytes.readUnsignedByte());
		}
		
		[Test(async)]
		public function testSeekStream():void
		{
			Assert.assertTrue(stream.hasMp4Header());
			
			urlStream = new URLStream;
			var asyncHandler:Function = Async.asyncHandler(this, onSeekFileComplete, 500, null, onTimeOut);
			urlStream.addEventListener(Event.COMPLETE, asyncHandler, false, 0, true);
			urlStream.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			urlStream.load(new URLRequest("../../../TestResource/firstSeekPoint.bin"));
		}
		
		public function onSeekFileComplete(event:Event, passThroughData:Object):void
		{
			trace("onSeekFileComplete bytesAvailable:" + urlStream.bytesAvailable);
			var offset:uint = stream.seek(4);
			Assert.assertEquals(0xA5d8B, offset);
			
			var seekSubpieceIndex:uint = offset / 1024;
			var seekSubpieceOffset:uint = offset % 1024;
			
			var subpieceIndex:uint = 0;
			while(urlStream.bytesAvailable > 0)
			{
				var bytes:ByteArray = new ByteArray;
				if (subpieceIndex < seekSubpieceIndex)
				{
					urlStream.readBytes(bytes, 0, 1024);
				}
				else if (subpieceIndex == seekSubpieceIndex)
				{
					Assert.assertTrue(urlStream.bytesAvailable > 1024);
					urlStream.readBytes(bytes, 0, 1024);
					bytes.position = seekSubpieceOffset;
					stream.appendBytes(bytes);
				}
				else if (subpieceIndex > seekSubpieceIndex)
				{
					if (urlStream.bytesAvailable > 1024)
						urlStream.readBytes(bytes, 0, 1024);
					else
						urlStream.readBytes(bytes);
					stream.appendBytes(bytes);
				}
				
				++subpieceIndex;
			}
			
			var flvBytes:ByteArray = stream.readBytes();
			Assert.assertEquals(26753, flvBytes.length);
			Assert.assertEquals(0, flvBytes.position);
			// test flv header
			Assert.assertEquals(70, flvBytes.readUnsignedByte());
			Assert.assertEquals(76, flvBytes.readUnsignedByte());
			Assert.assertEquals(86, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			Assert.assertEquals(5, flvBytes.readUnsignedByte());
			Assert.assertEquals(9, flvBytes.readInt());
			Assert.assertEquals(0, flvBytes.readInt());
			
			// test audio header
			// 		type = 8
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			//		body length = 4
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(4, flvBytes.readUnsignedByte());
			//		timestamp = 4829
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x12, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xDD, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			flvBytes.position += 3;
			//		body
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x12, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x10, flvBytes.readUnsignedByte());
			//		tagsize
			Assert.assertEquals(15, flvBytes.readUnsignedInt());
			
			// test audio sample 208
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			flvBytes.position +=3;		// bodylength
			//		timestamp = 4829
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x12, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xDD, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			flvBytes.position += 3;
			// 		type
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			//		audio sample 208 first 8 bytes
			Assert.assertEquals(0x21, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x08, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x4f, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xff, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xff, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xff, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xfe, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x9c, flvBytes.readUnsignedByte());
			flvBytes.position += 172;
			flvBytes.position += 4;
			
			// test audio sample 209
			Assert.assertEquals(8, flvBytes.readUnsignedByte());
			flvBytes.position +=3;		// bodylength
			//		timestamp = 4852
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x12, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xF4, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			flvBytes.position += 3;
			// 		type
			Assert.assertEquals(0xAF, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			//		audio sample 208 first 8 bytes
			Assert.assertEquals(0x21, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x08, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x4f, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xff, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xff, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xff, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xfe, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x90, flvBytes.readUnsignedByte());
			flvBytes.position += 189;
			flvBytes.position += 4;
			
			// test video header
			Assert.assertEquals(9, flvBytes.readUnsignedByte());
			flvBytes.position +=3;		// bodylength
			//		timestamp = 4833
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x12, flvBytes.readUnsignedByte());
			Assert.assertEquals(0xE1, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			flvBytes.position += 3;
			//		type
			Assert.assertEquals(0x17, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			//		composition time
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			Assert.assertEquals(0, flvBytes.readUnsignedByte());
			//		AVCDecoderConfiguration
			Assert.assertEquals(0x01, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x64, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x15, flvBytes.readUnsignedByte());
			flvBytes.position += 36;
			flvBytes.position += 4;
			
			// test video sample 116
			Assert.assertEquals(9, flvBytes.readUnsignedByte());
			flvBytes.position += 10;
			//		type
			Assert.assertEquals(0x17, flvBytes.readUnsignedByte());
			Assert.assertEquals(1, flvBytes.readUnsignedByte());
			//		composition time
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x29, flvBytes.readUnsignedByte());
			//		video sample 0 first 4 bytes
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x00, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x66, flvBytes.readUnsignedByte());
			Assert.assertEquals(0x72, flvBytes.readUnsignedByte());
			flvBytes.position += 26224;
			flvBytes.position += 4;
		}
		
		public function onTimeOut(passThroughData:Object):void
		{
			Assert.fail("onTimeOut");
		}
		
		public function onIOError(event:IOErrorEvent):void
		{
			Assert.fail("onIOError" + event);
		}
	}
}