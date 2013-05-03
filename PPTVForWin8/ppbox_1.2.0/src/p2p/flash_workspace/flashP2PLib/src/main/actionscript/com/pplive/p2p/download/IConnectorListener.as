package com.pplive.p2p.download
{
	import com.pplive.p2p.network.ISocket;
	import com.pplive.p2p.network.Endpoint;
	import com.pplive.p2p.struct.PeerInfo;	
	
	internal interface IConnectorListener
	{
		function onPeerConnected(endpoint:Endpoint, socket:ISocket):void;
		function onPeerConnectFailed(endpoint:Endpoint):void;
	}
}