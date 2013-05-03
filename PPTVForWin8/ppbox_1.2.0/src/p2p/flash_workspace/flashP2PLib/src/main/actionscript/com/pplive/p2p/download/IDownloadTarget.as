package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.utils.ByteArray;
	
	public interface IDownloadTarget
	{
		function onRecvSubPiece(subPiece:SubPiece, data:ByteArray):void;
		function onHttpError(error:uint,interval:uint):void; 
		function hasPiece(piece:Piece):Boolean;
		function hasSubPiece(subPiece:SubPiece):Boolean;
		function get bwType():uint;
		function sendStopDacLog(logObject:Object):void;
	}
}