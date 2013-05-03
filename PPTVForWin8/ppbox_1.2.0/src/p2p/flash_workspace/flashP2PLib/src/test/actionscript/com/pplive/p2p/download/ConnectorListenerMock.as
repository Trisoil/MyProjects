package com.pplive.p2p.download
{
	import com.pplive.p2p.network.ISocket;
	import com.pplive.p2p.struct.PeerInfo;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	
	import org.mock4as.Mock;
	
	public class ConnectorListenerMock extends Mock implements IConnectorListener
	{
		private var _eventDispatcher:EventDispatcher;
		
		public function ConnectorListenerMock()
		{
			super();
			_eventDispatcher = new EventDispatcher;
		}
		
		public function get eventDispatcher():EventDispatcher
		{
			return _eventDispatcher;
		}
		
		public function onPeerConnected(peer:PeerInfo, socket:ISocket):void
		{
			record("onPeerConnected", peer);
			eventDispatcher.dispatchEvent(new Event("onPeerConnected"));
		}
	}
}