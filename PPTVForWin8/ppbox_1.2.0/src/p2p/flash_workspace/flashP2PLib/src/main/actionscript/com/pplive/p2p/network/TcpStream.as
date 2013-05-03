package com.pplive.p2p.network
{
	import com.pplive.p2p.network.protocol.AnnounceResponsePacket;
	import com.pplive.p2p.network.protocol.Packet;
	import com.pplive.p2p.network.protocol.SubPiecePacket;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.profile.FunctionProfiler;
	import com.pplive.util.StringConvert;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.Socket;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class TcpStream
	{
		private static var logger:ILogger = getLogger(TcpStream);
		private var _socket:ISocket;
		private var _endpoint:Endpoint;
		private var _listener:ISocketListener;
		
		private var responsePacketAction:uint;
		private var responsePacketBytes:ByteArray = new ByteArray;
		private var packetDelimiter:ByteArray = new ByteArray;
		
		private var subpiecePacketHeaderBytes:ByteArray = new ByteArray;
		private var subPiecePacketBytes:ByteArray = new ByteArray;
		private var subpiecePacketLength:uint;
		
		public function TcpStream(socket:ISocket, endpoint:Endpoint, listener:ISocketListener = null)
		{
			_socket = socket;
			_socket.addEventListener(Event.CLOSE, onClose, false, 0, true);
			_socket.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			_socket.addEventListener(ProgressEvent.SOCKET_DATA, onSockData, false, 0, true);
			
			_endpoint = endpoint;
			_listener = listener;
			
			responsePacketBytes.endian = Endian.LITTLE_ENDIAN;
			subpiecePacketHeaderBytes.endian = Endian.LITTLE_ENDIAN;
			resetReceive();
		}
		
		public function destory():void
		{
			_socket.removeEventListener(Event.CLOSE, onClose);
			_socket.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			_socket.removeEventListener(ProgressEvent.SOCKET_DATA, onSockData);			
			_socket.close();
			_socket = null;
			
			_endpoint = null;
			_listener = null;
			responsePacketBytes = null;
			packetDelimiter = null;
			subpiecePacketHeaderBytes = null;
			subPiecePacketBytes = null;
		}
		
		public function get endpoint():Endpoint {return _endpoint;}
		
		public function sendPacket(packet:Packet):void
		{
			// serialize packet and send
			var bytes:ByteArray = new ByteArray;
			bytes.endian = Endian.LITTLE_ENDIAN;
			packet.ToByteArray(bytes);
			bytes.writeByte(13);
			bytes.writeByte(10);
			bytes.writeByte(13);
			bytes.writeByte(10);
			logger.debug("sendPacket action:" + packet.action + ", transactionID:" + packet.transactionId
				+ ", size=" + bytes.length);
			_socket.writeBytes(bytes);
			_socket.flush();
		}
		
		private function onIOError(event:IOErrorEvent):void
		{
			logger.error("onIOError " + endpoint);
			_socket.close();
		}
		
		private function onClose(event:Event):void
		{
			if (_listener)
			{
				_listener.onSocketLost();
			}
		}
		
		private function onSockData(event:ProgressEvent):void
		{
			logger.debug("onSockData bytesAvailable:" + _socket.bytesAvailable);
			var profiler:FunctionProfiler = new FunctionProfiler(logger);
			var byte:int;
			// herain:socket.connected must be true to prevent socket closed by event handler.
			while(_socket.connected && _socket.bytesAvailable > 0)
			{
				if (responsePacketAction == 0)
				{
					byte = _socket.readUnsignedByte();
					responsePacketAction = byte;
					responsePacketBytes.writeByte(byte);
					logger.debug("onSockData got action:" + responsePacketAction);
					continue;
				}
				
				if (responsePacketAction != SubPiecePacket.ACTION)
				{
					byte = _socket.readUnsignedByte();
					if ((packetDelimiter.length == 0 && byte == 13) ||
						(packetDelimiter.length == 1 && byte == 10) ||
						(packetDelimiter.length == 2 && byte == 13))
					{
						packetDelimiter.writeByte(byte);
					}
					else if (packetDelimiter.length == 3 && byte == 10)
					{
						// find delimiter \r\n\r\n
						logger.debug("onSockData find delimiter \\r\\n\\r\\n");
						onResponsePacket();
					}
					else
					{
						if (packetDelimiter.length != 0)
						{
							responsePacketBytes.writeBytes(packetDelimiter);
							packetDelimiter.clear();
						}
						
						responsePacketBytes.writeByte(byte);
					}
				}
				else
				{
					// herain:SubPiecePacket is very big, we need to deal with it particularly.
					if (subpiecePacketLength == 0)
					{
						if (_socket.bytesAvailable < 28)
							break;
						else
						{
							subpiecePacketHeaderBytes.writeByte(SubPiecePacket.ACTION);
							_socket.readBytes(subpiecePacketHeaderBytes, 1, 26);
							
							subpiecePacketLength = _socket.readUnsignedShort();
							logger.debug("onSockData got subpiecePacket length:" + subpiecePacketLength);
						}
					}
					else
					{
						if (_socket.bytesAvailable < subpiecePacketLength + 4)
						{
							break;
						}
						else
						{
							_socket.readBytes(subPiecePacketBytes, 0, subpiecePacketLength);
							_socket.readInt();	// ignore delimeter \r\n\r\n
							profiler.makeSection();
							onResponsePacket();
						}
					}
				}
			}
			
			profiler.end();
		}
		
		private function onResponsePacket():void
		{
			var profiler:FunctionProfiler = new FunctionProfiler(logger, "onResponsePacket");
			var packet:Packet;
			responsePacketBytes.position = 0;
			switch(responsePacketAction)
			{
				case AnnounceResponsePacket.ACTION:
					logger.debug("onSockData received AnnounceResponsePacket:" + StringConvert.byteArray2HexString(responsePacketBytes));
					packet = new AnnounceResponsePacket(responsePacketBytes);
					break;
				case SubPiecePacket.ACTION:
					logger.debug("onSockData received SubPiecePacket");
					subpiecePacketHeaderBytes.position = 0;
					packet = new SubPiecePacket(subpiecePacketHeaderBytes, subpiecePacketLength, subPiecePacketBytes);
					break;
				default:
					break;
			}
			
			profiler.makeSection();
			resetReceive();
			
			profiler.makeSection();
			if (packet && _listener)
			{
				_listener.onPacket(packet);
			}
			
			profiler.end();
		}
		
		private function resetReceive():void
		{
			logger.debug("resetReceive");
			responsePacketAction = 0;
			responsePacketBytes.clear();
			packetDelimiter.clear();
			
			subpiecePacketHeaderBytes.clear();
			subPiecePacketBytes = new ByteArray;
			subpiecePacketLength = 0;
		}
	}
}