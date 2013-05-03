package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.utils.ByteArray;

	internal interface IP2PDownloader extends IDownloader
	{
		function get rid():RID;
		function get priority():uint;
		function reset():void;
		function stop():void;
		function getNextSubPiece():SubPiece;
		function onSubPiece(subpiece:SubPiece, data:ByteArray):void;
		function onPeerDisconnected(peer:PeerConnection):void;
	}
}