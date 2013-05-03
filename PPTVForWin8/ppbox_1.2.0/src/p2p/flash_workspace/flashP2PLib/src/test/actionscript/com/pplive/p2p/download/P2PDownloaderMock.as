package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.RID;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.test.EventMock;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	import flash.events.Event;
	import flash.utils.ByteArray;
	
	import com.pplive.mx.ObjectUtil;
	
	public class P2PDownloaderMock extends EventMock implements IP2PDownloader
	{
		private static var logger:ILogger = getLogger(PeerConnectionTest);
		private var _rid:RID = new RID("2E9AC1EB0406BFBCD023F572B24A2B18");
		private var fileLength:uint = 19298337;
		private var subpiece:SubPiece = new SubPiece;
		private var lastSubPiece:SubPiece;
		public function P2PDownloaderMock()
		{
			super();
			lastSubPiece = new SubPiece(0, 1023);
		}
		
		public function get rid():RID
		{
			return _rid;
		}
		
		public function get priority():uint
		{
			return 10;
		}
		
		public function getNextSubPiece():SubPiece
		{
			if (subpiece.compare(lastSubPiece) <= 0)
			{
				var currentSubPiece:SubPiece = com.pplive.mx.ObjectUtil.copy(subpiece) as SubPiece;
				subpiece.moveToNextSubPiece();
				return currentSubPiece;	
			}
			else
				return null;
		}
		
		public function onSubPiece(subpiece:SubPiece, data:ByteArray):void
		{
			logger.info("onSubPiece: " + subpiece);
			record("onSubPiece");
			if (subpiece.compare(lastSubPiece) == 0)
				dispatchEvent(new Event("onSubPiece"));
		}
	}
}