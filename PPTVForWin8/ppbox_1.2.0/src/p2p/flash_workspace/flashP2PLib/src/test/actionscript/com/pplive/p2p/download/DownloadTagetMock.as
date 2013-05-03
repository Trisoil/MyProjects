package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.SubPiece;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.utils.ByteArray;
	
	import org.mock4as.Mock;
	
	public class DownloadTagetMock extends Mock implements IDownloadTarget
	{
		private var _eventDispatcher:EventDispatcher;
		private var _endSubPiece:SubPiece;
		
		public function DownloadTagetMock()
		{
			super();
			_eventDispatcher = new EventDispatcher;
		}
		
		public function get eventDispatcher():EventDispatcher
		{
			return _eventDispatcher;
		}
		
		public function onRecvSubPiece(subPiece:SubPiece, data:ByteArray):void
		{
			record("onRecvSubPiece");
			trace("DownloadTagetMock::onRecvSubPiece	" + subPiece);
			if (subPiece.compare(_endSubPiece) == 0)
			{
				_eventDispatcher.dispatchEvent(new Event(Event.COMPLETE));
			}
		}
		public function onHttpError(error:uint,interval:uint):void
		{			
		}
		
		public function hasPiece(piece:Piece):Boolean
		{
			record("hasPiece");
			return expectedReturnFor("hasPiece") as Boolean;
		}
		
		public function hasSubPiece(subPiece:SubPiece):Boolean
		{
			return false;
		}
		
		public function get bwType():uint
		{
			return 0;
		}
		
		public function sendStopDacLog(logObject:Object):void
		{
			
		}
		
		public function set endSubPiece(subPiece:SubPiece):void
		{
			_endSubPiece = subPiece;
		}
	}
}