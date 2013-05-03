package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	import com.pplive.mx.ObjectUtil;
	
	import org.mock4as.Mock;
	
	public class DownloadDriverMock extends Mock implements IDownloadDriver
	{
		static public const URL_ERROR:String = "HTTP_URL_ERROR";
		static public const IO_ERROR:String = "HTTP_IO_ERROR";
		static public const SECURITY_ERROR:String = "HTTP_SECURITY_ERROR";
		
		private var _eventDispatcher:EventDispatcher;
		private var _endSubPiece:SubPiece;
		private var _subpieceDict:Dictionary;
		private var _nextPiece:Piece;
		
		public function DownloadDriverMock()
		{
			super();
			_eventDispatcher = new EventDispatcher;
			_subpieceDict = new Dictionary;
		}
		
		public function get eventDispatcher():EventDispatcher
		{
			return _eventDispatcher;
		}
		
		public function getFileLength():uint
		{
			record("getFileLength");
			return expectedReturnFor("getFileLength") as uint;
		}
		
		public function requestNextPiece(downloader:IDownloader):Piece
		{
			record("requestNextPiece");
			if (_nextPiece == null)
			{
				_nextPiece =  expectedReturnFor("requestNextPiece") as Piece;	
			}
			else
			{
				_nextPiece.moveToNextPiece();
			}
			
			return com.pplive.mx.ObjectUtil.copy(_nextPiece) as Piece;
		}
		
		public function onRecvSubPiece(subPiece:SubPiece, data:ByteArray):void
		{
			record("onRecvSubPiece");
			_subpieceDict[subPiece.getOffset()] = true;
			trace(subPiece + ":" + data.length);
			if (subPiece.compare(_endSubPiece) == 0)
			{
				_eventDispatcher.dispatchEvent(new Event(Event.COMPLETE));
			}
		}
		
		public function hasSubPiece(subPiece:SubPiece):Boolean
		{
			if (_subpieceDict[subPiece.getOffset()])
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		public function onHttpError(error:uint):void
		{
			record("onHttpError", error);
			trace("onHttpError:" + error);
			
			var e:Event;
			switch (error)
			{
				case HttpDownloader.HTTP_IO_ERROR:
					e = new Event(IO_ERROR);
					break;
				case HttpDownloader.HTTP_URL_ERROR:
					e = new Event(URL_ERROR);
					break;
				case HttpDownloader.HTTP_SECURITY_ERROR:
					e = new Event(SECURITY_ERROR);
					break;
			}
			
			if (e != null)
			{
				_eventDispatcher.dispatchEvent(e);
			}
		}
		
		public function set endSubPiece(subPiece:SubPiece):void
		{
			_endSubPiece = subPiece;
		}
	}
}