package com.pplive.p2p.download
{
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.network.SocketAdaptor;
	import com.pplive.p2p.network.TcpStream;
	import com.pplive.p2p.struct.PeerInfo;
	import com.pplive.profile.FunctionProfiler;
	
	import flash.errors.IOError;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.Socket;
	import flash.utils.Dictionary;
	import flash.utils.Endian;
	import flash.system.Security;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	internal class PeerConnector
	{
		private static var logger:ILogger = getLogger(PeerConnector);
		private var listener:IConnectorListener;
		private var socketDict:Dictionary = new Dictionary;
		
		public function PeerConnector(listener:IConnectorListener)
		{
			this.listener = listener;
		}
		
		public function connect(endpoint:Endpoint):void
		{
			logger.debug("connect " + endpoint);
			try
			{
				// TODO(herain):这里还是有时候会报未处理的IOErrorEvent.Release版本应该没有影响。
				var socket:SocketAdaptor = new SocketAdaptor;
				socket.timeout = 10000;	// socket连接的超时时间设置为10s，默认为20s。
				socket.endian = Endian.LITTLE_ENDIAN;
				socketDict[socket] = endpoint;
				socket.addEventListener(Event.CONNECT, onConnectSucceed, false, 0, true);
				socket.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
				socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError, false, 0, true);
				
				Security.loadPolicyFile("xmlsocket://" + endpoint.ip + ":" + endpoint.port);
				socket.connect(endpoint.ip, endpoint.port);
			}
			catch(e:IOError)
			{
				logger.error("catch IOError:" + e);
				onConnectFailed(socket);
			}
			catch(e:SecurityError)
			{
				logger.error("catch SecurityError:" + e);
				onConnectFailed(socket);
			}
		}

		public function destory():void
		{
			logger.info("destory");
			for (var key:Object in socketDict)
			{
				try
				{
					var socket:SocketAdaptor = key as SocketAdaptor;
					logger.info("close " + socketDict[key]);
					socket.close();
				}
				catch(e:IOError)
				{
				}
				
				delete socketDict[key];
			}
			
			socketDict = null;
			listener = null;
		}
		
		private function onConnectSucceed(event:Event):void
		{
			var socket:SocketAdaptor = event.target as SocketAdaptor;
			var endpoint:Endpoint = socketDict[socket] as Endpoint;
			logger.debug("connected to " + endpoint);
			listener.onPeerConnected(endpoint, socket);
			removeListeners(socket);
			delete socketDict[socket];
		}
		
		private function removeListeners(socket:SocketAdaptor):void
		{
			socket.removeEventListener(Event.CONNECT, onConnectSucceed);
			socket.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			socket.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);			
		}
		
		private function onIOError(event:IOErrorEvent):void
		{
			logger.error(event);
			onConnectFailed(event.target as SocketAdaptor);
		}
		
		private function onSecurityError(event:SecurityErrorEvent):void
		{
			logger.error(event);
			onConnectFailed(event.target as SocketAdaptor);
		}
		
		private function onConnectFailed(socket:SocketAdaptor):void
		{
			var profiler:FunctionProfiler = new FunctionProfiler;
			var endpoint:Endpoint = socketDict[socket] as Endpoint;
			if (endpoint != null)
			{
				socket.close();
				profiler.makeSection();
				removeListeners(socket);
				logger.debug("onConnectFailed " + endpoint);
				listener.onPeerConnectFailed(endpoint);
				profiler.makeSection();
				delete socketDict[socket];
			}
			profiler.end();
		}
	}
}