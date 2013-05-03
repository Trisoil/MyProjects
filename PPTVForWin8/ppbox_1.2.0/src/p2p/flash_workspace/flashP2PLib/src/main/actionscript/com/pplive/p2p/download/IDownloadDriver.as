package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.utils.ByteArray;
	
	public interface IDownloadDriver
	{
		function getFileLength():uint;
		function requestNextPiece(downloader:IDownloader):Piece;
		function reportDownloaderPaused(downloader:IDownloader):void;
		function stopPieceDownload(piece:Piece):void;
		function onRecvSubPiece(subPiece:SubPiece, data:ByteArray, downloader:IDownloader):void;
		function onHttpError(error:uint,interval:uint):void;
		function hasSubPiece(subpiece:SubPiece):Boolean;
		function get restPlayTime():uint;
		function get dragHeadLength():uint;
	}
}