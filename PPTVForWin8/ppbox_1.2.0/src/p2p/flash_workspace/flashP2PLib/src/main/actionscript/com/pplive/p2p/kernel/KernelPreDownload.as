package com.pplive.p2p.kernel
{
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.network.ISocketListener;
	import com.pplive.p2p.network.SocketAdaptor;
	import com.pplive.p2p.network.TcpStream;
	import com.pplive.p2p.network.protocol.Packet;
	import com.pplive.p2p.network.protocol.StartDownloadPacket;
	import com.pplive.p2p.network.protocol.StopDownloadPacket;
	import com.pplive.p2p.struct.RID;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class KernelPreDownload implements ISocketListener
	{		
		private static var logger:ILogger = getLogger(KernelPreDownload);
		private static var localHost:String = "127.0.0.1";
		
		private var _port:uint;
		private var _socket:SocketAdaptor;
		private var _stream:TcpStream;
		private var _url:String;
		private var _restPlayTime:uint;
		private var _isStart:Boolean;
		
		public function KernelPreDownload(port:uint)
		{
			_port = port;			
		}
		
		public function start(url:String,restPlayTime:uint):void
		{
			_url = url;
			_restPlayTime = restPlayTime;
			_isStart = true;
			execute();
		}		
		
		public function stop(url:String):void
		{
			_url = url;
			_isStart = false;
			execute();			
		}	
		
		private function execute():void
		{
			if(null == _socket)
			{
				_socket = new SocketAdaptor;
				_socket.addEventListener(Event.CONNECT, onConnect, false, 0, true);
				_socket.addEventListener(IOErrorEvent.IO_ERROR, onError, false, 0, true);
				_socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError, false, 0, true);
				_socket.connect(localHost, _port);
				//测试用的，袁的机器
				//_socket.connect("192.168.30.228",16000);
			}
		}
		
		public function destory():void
		{
			logger.info("destory");
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
		}		
		
		public function onPacket(packet:Packet):void
		{
			// ISocketListener.onPacket do nothing.
			logger.info("onPacket,"+packet);
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
			logger.info("onConnect,to start predownload?"+_isStart);
			removeSocketListen();
			_stream = new TcpStream(_socket, new Endpoint(localHost, _port), this);
			
			//这里是同步做的，不知道会不会有效率问题？
			SendPacket();
			destory();
		}
		
		private function onError(event:Event):void
		{
			logger.error("onError " + event);
		}
		
		private function SendPacket():void
		{
			if (_stream)
			{
				//通过url和restplaytime生成新的url，对于stop命令，这个字段无意义。
				var downloadUrl:String = _url + "&resttime="+_restPlayTime;
				logger.info("url to kernel:"+downloadUrl);
				if(_isStart)
				{
					//开启
					var startPacket:StartDownloadPacket = new StartDownloadPacket(downloadUrl);
					_stream.sendPacket(startPacket);		
				}	
				else
				{
					//停止
					var stopPacket:StopDownloadPacket = new StopDownloadPacket(downloadUrl);
					_stream.sendPacket(stopPacket);		
				}
			}			
		}
	}
	
}