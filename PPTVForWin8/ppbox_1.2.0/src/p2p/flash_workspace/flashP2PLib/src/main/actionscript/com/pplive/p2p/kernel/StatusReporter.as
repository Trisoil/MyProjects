package com.pplive.p2p.kernel
{
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.network.ISocketListener;
	import com.pplive.p2p.network.SocketAdaptor;
	import com.pplive.p2p.network.TcpStream;
	import com.pplive.p2p.network.protocol.Packet;
	import com.pplive.p2p.network.protocol.ReportStatusPacket;
	import com.pplive.p2p.struct.RID;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class StatusReporter implements ISocketListener
	{
		private static var logger:ILogger = getLogger(StatusReporter);
		private static var localHost:String = "127.0.0.1";
		
		private var _port:uint;
		private var _rid:RID;
		private var _socket:SocketAdaptor;
		private var _stream:TcpStream;
		private var _rest_play_time:uint;
		private var _secondTimer:Timer;
		
		public function StatusReporter(port:uint, rid:RID)
		{
			try
			{
				_port = port;
				_rid = rid;
				_socket = new SocketAdaptor;
				_socket.addEventListener(Event.CONNECT, onConnect, false, 0, true);
				_socket.addEventListener(IOErrorEvent.IO_ERROR, onError, false, 0, true);
				_socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError, false, 0, true);
				
				_socket.connect(localHost, _port);
			}				
			catch(e:Object)
			{
				logger.error("StatusReporter error:" + e);
			}					
		}
		
		public function destory():void
		{
			logger.info("destory");
			_rid = null;
			if(_socket)
			{				
				removeSocketListen();
				_socket = null;
			}
			
			
			if (_stream)
			{
				_stream.destory();
				_stream = null;	
			}			
			
			if (_secondTimer)
			{
				_secondTimer.stop();
				_secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
				_secondTimer = null;
			}
		}
		
		public function setRestPlayTime(time:uint):void
		{
			_rest_play_time = time;
		}
		
		public function onPacket(packet:Packet):void
		{
			// ISocketListener.onPacket do nothing.
		}
		
		public function onSocketLost():void
		{
			logger.info("onSocketLost");
			_stream.destory();
			_stream = null;
		}
		private function removeSocketListen():void
		{
			_socket.removeEventListener(Event.CONNECT, onConnect);
			_socket.removeEventListener(IOErrorEvent.IO_ERROR, onError);
			_socket.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onError);				
		}
			
		
		private function onConnect(event:Event):void
		{
			logger.info("onConnect");
			removeSocketListen();
			_stream = new TcpStream(_socket, new Endpoint(localHost, _port), this);
			_secondTimer = new Timer(1000);
			_secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
			_secondTimer.start();
		}
		
		private function onError(event:Event):void
		{
			logger.error("onError " + event);
		}
		
		private function onSecondTimer(event:TimerEvent):void
		{
			if (_stream)
			{
				var packet:ReportStatusPacket = new ReportStatusPacket(_rid, _rest_play_time);
				_stream.sendPacket(packet);
			}			
		}
	}
}