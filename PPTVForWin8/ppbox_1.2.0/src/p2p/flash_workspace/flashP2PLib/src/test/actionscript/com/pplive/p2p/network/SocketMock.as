package com.pplive.p2p.network
{
	import com.pplive.test.EventMock;
	
	import flash.events.Event;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class SocketMock extends EventMock implements ISocket
	{
		private var _inputBuffer:ByteArray;
		private var _outputBuffer:ByteArray = new ByteArray;
		public function SocketMock()
		{
			super();
			_outputBuffer.endian = Endian.LITTLE_ENDIAN;
		}
		
		public function set inputBufer(buffer:ByteArray):void
		{
			_inputBuffer = buffer;
		}
		
		public function get inputBufer():ByteArray
		{
			return _inputBuffer;
		}
		
		public function readBytes(bytes:ByteArray, offset:uint=0, length:uint=0):void
		{
			_inputBuffer.readBytes(bytes, offset, length);
		}
		
		public function readBoolean():Boolean
		{
			return _inputBuffer.readBoolean();
		}
		
		public function readByte():int
		{
			return _inputBuffer.readByte();
		}
		
		public function readUnsignedByte():uint
		{
			return _inputBuffer.readUnsignedByte();
		}
		
		public function readShort():int
		{
			return _inputBuffer.readShort();
		}
		
		public function readUnsignedShort():uint
		{
			return _inputBuffer.readUnsignedShort();
		}
		
		public function readInt():int
		{
			return _inputBuffer.readInt();
		}
		
		public function readUnsignedInt():uint
		{
			return _inputBuffer.readUnsignedInt();
		}
		
		public function readFloat():Number
		{
			return _inputBuffer.readFloat();
		}
		
		public function readDouble():Number
		{
			return _inputBuffer.readDouble();
		}
		
		public function readMultiByte(length:uint, charSet:String):String
		{
			return _inputBuffer.readMultiByte(length, charSet);
		}
		
		public function readUTF():String
		{
			return _inputBuffer.readUTF();
		}
		
		public function readUTFBytes(length:uint):String
		{
			return _inputBuffer.readUTFBytes(length);
		}
		
		public function get bytesAvailable():uint
		{
			return _inputBuffer.bytesAvailable;
		}
		
		public function readObject():*
		{
			return _inputBuffer.readObject();
		}
		
		public function get objectEncoding():uint
		{
			return _inputBuffer.objectEncoding;
		}
		
		public function set objectEncoding(version:uint):void
		{
			_inputBuffer.objectEncoding = version;
		}
		
		public function get endian():String
		{
			return _inputBuffer.endian;
		}
		
		public function set endian(type:String):void
		{
			_inputBuffer.endian = type;
		}
		
		public function writeBytes(bytes:ByteArray, offset:uint=0, length:uint=0):void
		{
			_outputBuffer.writeBytes(bytes, offset, length);
		}
		
		public function writeBoolean(value:Boolean):void
		{
			_outputBuffer.writeBoolean(value);
		}
		
		public function writeByte(value:int):void
		{
			_outputBuffer.writeByte(value);
		}
		
		public function writeShort(value:int):void
		{
			_outputBuffer.writeShort(value);
		}
		
		public function writeInt(value:int):void
		{
			_outputBuffer.writeInt(value);
		}
		
		public function writeUnsignedInt(value:uint):void
		{
			_outputBuffer.writeUnsignedInt(value);
		}
		
		public function writeFloat(value:Number):void
		{
			_outputBuffer.writeFloat(value);
		}
		
		public function writeDouble(value:Number):void
		{
			_outputBuffer.writeDouble(value);
		}
		
		public function writeMultiByte(value:String, charSet:String):void
		{
			_outputBuffer.writeMultiByte(value, charSet);
		}
		
		public function writeUTF(value:String):void
		{
			_outputBuffer.writeUTF(value);
		}
		
		public function writeUTFBytes(value:String):void
		{
			_outputBuffer.writeUTFBytes(value);
		}
		
		public function writeObject(object:*):void
		{
			_outputBuffer.writeObject(object);
		}
		
		public function get outputBufer():ByteArray
		{
			return _outputBuffer;
		}
		
		public function close():void
		{
			record("close");
			dispatchEvent(new Event(Event.CLOSE));
		}
		
		public function flush():void
		{
			
		}
		
		public function get connected():Boolean
		{
			return true;
		}
	}
}