package com.pplive.p2p.network
{
	import com.pplive.p2p.network.ReceiveTCPPacketEvent;
	import com.pplive.p2p.network.protocol.AnnounceRequestPacket;
	import com.pplive.p2p.network.protocol.AnnounceResponsePacket;
	import com.pplive.p2p.network.protocol.Packet;
	import com.pplive.p2p.network.protocol.ReportSpeedPacket;
	import com.pplive.p2p.network.protocol.SubPiecePacket;
	import com.pplive.p2p.network.protocol.SubPieceRequestPacket;
	import com.pplive.p2p.struct.BlockMap;
	import com.pplive.p2p.struct.PeerDownloadInfo;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.test.object.valueEqualTo;
	import com.pplive.util.StringConvert;
	
	import flash.events.Event;
	import flash.events.ProgressEvent;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	import org.flexunit.Assert;
	import org.flexunit.assertThat;
	import org.flexunit.async.Async;
	
	public class TcpStreamTest
	{
		private static var logger:ILogger = getLogger(TcpStreamTest);
		private var listener:SocketListenerMock;
		private var stream:TcpStream
		private var socketMock:SocketMock;
		
		private var rid:RID;
		private var transactionId:uint;
		private var protocolVersion:uint;
		private var expectedBytes:ByteArray;
		
		public function TcpStreamTest()
		{
		}
		
		[Before]
		public function setup():void
		{
			logger.info("setup");
			socketMock = new SocketMock;
			listener = new SocketListenerMock;
			stream = new TcpStream(socketMock, new Endpoint("127.0.0.1", 9000), listener);
			rid = new RID("275781FED82F1AD9B45637CAD3DB809F");
			transactionId = Packet.NewTransactionID();
			protocolVersion = 0;
			expectedBytes = new ByteArray;
			expectedBytes.endian = Endian.LITTLE_ENDIAN;
		}
		
		[Test]
		public function testSendAnnounceRequestPacket():void
		{
			logger.info("testSendAnnounceRequestPacket");
			var packet:AnnounceRequestPacket = new AnnounceRequestPacket(transactionId, rid);
			stream.sendPacket(packet);
			
			expectedBytes.writeByte(AnnounceRequestPacket.ACTION);
			expectedBytes.writeUnsignedInt(transactionId);
			expectedBytes.writeShort(Packet.PROTOCOL_VERSION);
			expectedBytes.writeBytes(rid.bytes());
			expectedBytes.writeByte(13);
			expectedBytes.writeByte(10);
			expectedBytes.writeByte(13);
			expectedBytes.writeByte(10);
			assertThat(socketMock.outputBufer, valueEqualTo(expectedBytes));
		}
		
		[Test]
		public function testSendSubPieceRequestPacket():void
		{
			logger.info("testSendSubPieceRequestPacket");
			var subpieces:Vector.<SubPiece> = new Vector.<SubPiece>;
			subpieces.push(new SubPiece(1, 3));
			subpieces.push(new SubPiece(1, 5));
			subpieces.push(new SubPiece(1, 20));
			var priority:uint = 10;
			var packet:SubPieceRequestPacket = new SubPieceRequestPacket(transactionId, rid, subpieces, priority);
			stream.sendPacket(packet);
			
			expectedBytes.writeByte(SubPieceRequestPacket.ACTION);
			expectedBytes.writeUnsignedInt(transactionId);
			expectedBytes.writeShort(Packet.PROTOCOL_VERSION);
			expectedBytes.writeBytes(rid.bytes());
			expectedBytes.writeShort(subpieces.length);
			for (var i:uint = 0; i < subpieces.length; ++i)
			{
				expectedBytes.writeShort(subpieces[i].blockIndex);
				expectedBytes.writeShort(subpieces[i].subPieceIndex);	
			}
			expectedBytes.writeShort(priority);
			expectedBytes.writeByte(13);
			expectedBytes.writeByte(10);
			expectedBytes.writeByte(13);
			expectedBytes.writeByte(10);
			
			logger.debug("actualBytes:(" + socketMock.outputBufer.length + ")" + StringConvert.byteArray2HexString(socketMock.outputBufer));
			logger.debug("expectedBytes:(" + expectedBytes.length + ")" + StringConvert.byteArray2HexString(expectedBytes));
			assertThat(socketMock.outputBufer, valueEqualTo(expectedBytes));
		}

		[Test]
		public function testSendReportSpeedPacket():void
		{
			logger.info("testSendReportSpeedPacket");
			var speedInBytesPerSecond:uint = 20 * 1024;
			var packet:ReportSpeedPacket = new ReportSpeedPacket(transactionId, speedInBytesPerSecond);
			
			expectedBytes.writeByte(ReportSpeedPacket.ACTION);
			expectedBytes.writeUnsignedInt(transactionId);
			expectedBytes.writeShort(Packet.PROTOCOL_VERSION);
			expectedBytes.writeUnsignedInt(speedInBytesPerSecond);
			expectedBytes.writeByte(13);
			expectedBytes.writeByte(10);
			expectedBytes.writeByte(13);
			expectedBytes.writeByte(10);
			
			stream.sendPacket(packet);
			logger.debug("actualBytes:(" + socketMock.outputBufer.length + ")" + StringConvert.byteArray2HexString(socketMock.outputBufer));
			logger.debug("expectedBytes:(" + expectedBytes.length + ")" + StringConvert.byteArray2HexString(expectedBytes));
			assertThat(socketMock.outputBufer, valueEqualTo(expectedBytes));
		}
		
		[Test(async)]
		public function testReceiveAnnounceResponsePacket():void
		{
			logger.info("testReceiveAnnounceResponsePacket");
			var handler:Function = Async.asyncHandler(this, testOnReceiveAnnounceResponsePacket, 500, null);
			listener.addEventListener(ReceiveTCPPacketEvent.ANNOUNCE_PACKET, handler, false, 0, true);
			
			var receivedBytes:ByteArray = new ByteArray;
			receivedBytes.endian = Endian.LITTLE_ENDIAN;
			socketMock.inputBufer = receivedBytes;
			
			receivedBytes.writeByte(AnnounceResponsePacket.ACTION);
			receivedBytes.writeUnsignedInt(transactionId);
			receivedBytes.writeShort(protocolVersion);
			receivedBytes.writeBytes(rid.bytes());
			
			receivedBytes.position = 0;	
			socketMock.dispatchEvent(new ProgressEvent(ProgressEvent.SOCKET_DATA, false, false, 0, 0));
			
			var position:uint = receivedBytes.position;
			receivedBytes.position = receivedBytes.length;
			
			// write peerDownloadInfo
			receivedBytes.writeByte(0x01);
			receivedBytes.writeUnsignedInt(100);
			receivedBytes.writeShort(10*1024);
			receivedBytes.writeShort(20*1024);
			receivedBytes.writeShort(30*1024);
			receivedBytes.writeShort(40*1024);
			receivedBytes.writeByte(0);
			receivedBytes.writeByte(0);
			receivedBytes.writeByte(0);
			
			receivedBytes.position = position;
			socketMock.dispatchEvent(new ProgressEvent(ProgressEvent.SOCKET_DATA, false, false, 0, 0));
			
			position = receivedBytes.length;
			receivedBytes.position = receivedBytes.length;
			
			receivedBytes.writeUnsignedInt(10);
			receivedBytes.writeByte(0x7F);
			receivedBytes.writeByte(0x02);
			receivedBytes.writeByte(13);	// delimiter: \r\n\r\n
			receivedBytes.writeByte(10);
			receivedBytes.writeByte(13);
			receivedBytes.writeByte(10);
			
			receivedBytes.writeByte(SubPiecePacket.ACTION);
			receivedBytes.writeUnsignedInt(0xFFFFFFFF);
			receivedBytes.writeShort(protocolVersion);
			receivedBytes.writeBytes(rid.bytes());
			receivedBytes.writeShort(1);
			receivedBytes.writeShort(0);
			// write low byte of packet length
			receivedBytes.writeByte(0);
			
			receivedBytes.position = position;
			socketMock.dispatchEvent(new ProgressEvent(ProgressEvent.SOCKET_DATA, false, false, 0, 0));
		}
		
		public function testOnReceiveAnnounceResponsePacket(event:ReceiveTCPPacketEvent, passThroughData:Object):void
		{
			var packet:AnnounceResponsePacket = event.packet as AnnounceResponsePacket;			
			// verify action, transactionId, protocolVersion, rid
			Assert.assertEquals(AnnounceResponsePacket.ACTION, packet.action);
			Assert.assertEquals(transactionId, packet.transactionId);
			Assert.assertEquals(protocolVersion, packet.protocolVersion);
			assertThat(rid, valueEqualTo(packet.rid));
			
			// verify peerDownloadInfo
			Assert.assertEquals(true, packet.peerDownloadInfo.isDownloading);
			Assert.assertEquals(100, packet.peerDownloadInfo.onLineTime);
			Assert.assertEquals(10*1024, packet.peerDownloadInfo.avgDownload);
			Assert.assertEquals(20*1024, packet.peerDownloadInfo.nowDownload);
			Assert.assertEquals(30*1024, packet.peerDownloadInfo.avgUpload);
			Assert.assertEquals(40*1024, packet.peerDownloadInfo.nowUpload);
			
			// verify BlockMap
			var blockMap:BlockMap = packet.blockMap;
			Assert.assertEquals(10, blockMap.blockCount);
			Assert.assertFalse(blockMap.isFull());
			Assert.assertTrue(blockMap.hasBlock(0));
			Assert.assertTrue(blockMap.hasBlock(1));
			Assert.assertTrue(blockMap.hasBlock(2));
			Assert.assertTrue(blockMap.hasBlock(3));
			Assert.assertTrue(blockMap.hasBlock(4));
			Assert.assertTrue(blockMap.hasBlock(5));
			Assert.assertTrue(blockMap.hasBlock(6));
			Assert.assertFalse(blockMap.hasBlock(7));
			Assert.assertFalse(blockMap.hasBlock(8));
			Assert.assertTrue(blockMap.hasBlock(9));
			Assert.assertFalse(blockMap.hasBlock(10));
			
			var handler:Function = Async.asyncHandler(this, onReceiveSubPiecePacket, 500, null);
			listener.addEventListener(ReceiveTCPPacketEvent.SUBPIECE_PACKET, handler, false, 0, true);
			
			// write subpiece data and delimiter
			var receivedBytes:ByteArray = socketMock.inputBufer;
			var position:uint = receivedBytes.position;
			receivedBytes.position = receivedBytes.length;
			
			// write high byte of packet length
			receivedBytes.writeByte(4);
			for (var i:uint = 0; i < 1024; ++i)
			{
				receivedBytes.writeByte(uint(Math.random()));
			}			
			receivedBytes.writeByte(13);	// delimiter: \r\n\r\n
			receivedBytes.writeByte(10);
			receivedBytes.writeByte(13);
			receivedBytes.writeByte(10);
			
			receivedBytes.position = position;
			socketMock.dispatchEvent(new ProgressEvent(ProgressEvent.SOCKET_DATA, false, false, 0, 0));
		}
		
		public function onReceiveSubPiecePacket(event:ReceiveTCPPacketEvent, passThroughData:Object):void
		{
			var packet:SubPiecePacket = event.packet as SubPiecePacket;
			// verify action, transactionId, protocolVersion, rid
			Assert.assertEquals(SubPiecePacket.ACTION, packet.action);
			Assert.assertEquals(0xFFFFFFFF, packet.transactionId);
			Assert.assertEquals(protocolVersion, packet.protocolVersion);
			assertThat(rid, valueEqualTo(packet.rid));
			
			assertThat(packet.subpiece, valueEqualTo(new SubPiece(1, 0)));
			Assert.assertEquals(1024, packet.subpieceLength);
			Assert.assertEquals(1024, packet.data.length);
		}
		
		[Test(async)]
		public function testReceiveSubPiecePacket():void
		{
			var handler:Function = Async.asyncHandler(this, onReceiveSubPiecePacket, 500, null);
			listener.addEventListener(ReceiveTCPPacketEvent.SUBPIECE_PACKET, handler, false, 0, true);
			
			var receivedBytes:ByteArray = new ByteArray;
			receivedBytes.endian = Endian.LITTLE_ENDIAN;
			socketMock.inputBufer = receivedBytes;
			
			receivedBytes.writeByte(SubPiecePacket.ACTION);
			receivedBytes.writeUnsignedInt(0xFFFFFFFF);
			receivedBytes.writeShort(protocolVersion);
			receivedBytes.writeBytes(rid.bytes());
			receivedBytes.writeShort(1);
			receivedBytes.writeShort(0);
			// write low byte of packet length
			receivedBytes.writeByte(0);
			
			receivedBytes.position = 0;	
			socketMock.dispatchEvent(new ProgressEvent(ProgressEvent.SOCKET_DATA, false, false, 0, 0));
			
			var position:uint = receivedBytes.position;
			receivedBytes.position = receivedBytes.length;
			// write high byte of packet length
			receivedBytes.writeByte(4);
			for (var i:uint = 0; i < 1024; ++i)
			{
				receivedBytes.writeByte(uint(Math.random()));
			}			
			receivedBytes.writeByte(13);	// delimiter: \r\n\r\n
			receivedBytes.writeByte(10);
			receivedBytes.writeByte(13);
			receivedBytes.writeByte(10);
			
			receivedBytes.position = position;
			socketMock.dispatchEvent(new ProgressEvent(ProgressEvent.SOCKET_DATA, false, false, 0, 0));
		}
	}
}