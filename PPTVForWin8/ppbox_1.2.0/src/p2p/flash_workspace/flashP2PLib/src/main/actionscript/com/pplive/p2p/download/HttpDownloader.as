package com.pplive.p2p.download
{
	import com.pplive.mx.ObjectUtil;
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.struct.Piece;
	import com.pplive.p2p.struct.StructUtil;
	import com.pplive.p2p.struct.SubPiece;
	import com.pplive.profile.FunctionProfiler;
	import com.pplive.util.MathUtil;
	import com.pplive.util.URI;
	
	import flash.utils.ByteArray;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class HttpDownloader extends Downloader
	{
		private static const logger:ILogger = getLogger(HttpDownloader);

		// TODO(herain):2012-1-12:参数需要细调
		private static const MIN_REQUEST_PIECE_NUM:uint = 2;
		private static const MAX_REQUEST_PIECE_NUM:uint = 10;
		private static const DEFALUT_DOWNLOAD_TIME_PER_CONNECTION:uint = 5;
		
		//记录失败的次数，现在由于一个download只有一个host，将来可以考虑做成key为host的map
		private var _failtimes:int = 0;
		
		private var _connections:Vector.<HttpConnection> = new Vector.<HttpConnection>;
		private var _url:String;
		
		public function HttpDownloader(url:String)
		{
			logger.debug("HttpDownloader,url:"+url);
			_url = url;
		}
		
		override public function pause():void
		{
			if (isRunning)
			{
				logger.info("pause. isRunning:" + isRunning);
				super.pause();
				destoryConnections();
			}
		}
		
		override public function resume():void
		{
			if (!isRunning)
			{
				logger.info("resume. isRunning:" + isRunning);
				super.resume();
				if (downloadDriver != null)
				{
					createConnection();
				}
			}
		}
		
		override public function destory():void
		{
			super.destory();
			logger.info("destory");
			_url = null;
			destoryConnections();
		}
		
		public function onRecvSubPiece(subPiece:SubPiece, data:ByteArray):void
		{
			speedMeter.submitBytes(data.length);
			downloadDriver.onRecvSubPiece(subPiece, data, this);
		}
		
		public function onConnectionCompleted(connection:HttpConnection):void
		{
			var pieces:Vector.<Piece> = requestPieceTask();
			if (pieces.length != 0)
			{
				connection.reuse(pieces);
			}
			else
			{
				// no more task.
				connection.stop();
				_connections.splice(_connections.indexOf(connection), 1);
			}
		}
		
		private function createConnection():void
		{
			var pieces:Vector.<Piece> = requestPieceTask();
			if (pieces.length != 0)
			{
				_connections.push(new HttpConnection(this, _url, pieces, downloadDriver.getFileLength()));	
			}
		}
		
		private function destoryConnections():void
		{
			var connectionsLength:uint = _connections.length;
			for (var i:uint = 0; i < connectionsLength; ++i)
			{
				_connections[i].stop();
			}
			
			_connections.length = 0;
		}
		
		private function destoryConnection(connection:HttpConnection):void
		{
			var index:int = _connections.indexOf(connection,0);
			if (index != -1)
			{
				_connections[index].stop();
				_connections[index] = null;
				_connections.splice(index,1);
				logger.info("after delete index:"+index+" _connections length:"+ _connections.length);			
			}
			else
			{
				logger.info("not find,connnection length:"+_connections.length);
			}
		}
		
		private function requestPieceTask():Vector.<Piece>
		{
			// clear _pieceVector
			var pieces:Vector.<Piece> = new Vector.<Piece>;
			pieces.length = 0;
			
			var pieceNum:uint = calcRequestPieceNum();			
			while(pieces.length < pieceNum)
			{
				var piece:Piece = downloadDriver.requestNextPiece(this);
				if (piece == null)
				{
					// download to file end, no more piece
					break;
				}
				
				if (pieces.length == 0)
				{
					pieces.push(piece);
				}
				else
				{
					var lastPiece:Piece = com.pplive.mx.ObjectUtil.copy(pieces[pieces.length - 1]) as Piece;
					lastPiece.moveToNextPiece();
					if (lastPiece.blockIndex == piece.blockIndex && lastPiece.pieceIndex == piece.pieceIndex)
					{
						pieces.push(piece);
					}
					else
					{
						// no more continuous piece,cause we add the piece to requestingPieces in  requestNextPiece
						//and we actually don't download the piece,so we mus call stopPieceDownload to remove
						downloadDriver.stopPieceDownload(piece);
						logger.info("encounter not continuous piece");
						break;
					}
				}
			}	
			
			return pieces;
		}
		
		private function calcRequestPieceNum():uint
		{
			var pieceNum:uint = DEFALUT_DOWNLOAD_TIME_PER_CONNECTION * speedMeter.getRecentSpeedInKBPS() / Constants.SUBPIECE_NUM_PER_PIECE;
			return MathUtil.limitMinMaxUint(pieceNum, MIN_REQUEST_PIECE_NUM, MAX_REQUEST_PIECE_NUM);
		}
		
		public function onHttpError(connection:HttpConnection, error:uint,interval:uint):void
		{
			if(null == connection)
			{
				logger.error("httpconnection null,may not run here");
			}
			//TODO:做一些处理，例如踢掉不能用的连接，只保留可用的。目前只有一个连接，所以暂不处理。
			logger.info("http error:"+error+" interval:"+interval+" hasrecvdata:"+(error>=10000));
			
			//清理这个连接正在下载的块
			if(connection.piece)
			{
				var piecesize:uint = connection.piece.length;
				for (var i:uint = 0; i < piecesize; ++i)
				{
					downloadDriver.stopPieceDownload(connection.piece[i]);
				}
			}
			
			//暂定为重试2次
			if (++_failtimes <=2)
			{
				logger.info("onHttpError failed time:"+_failtimes);			
				destoryConnection(connection);
				createConnection();
				return;
			}			
			
			downloadDriver.onHttpError(error,interval);
		}
		
		public function get failtimes():int 
		{
			logger.debug("get failed time:"+_failtimes);
			return _failtimes;
		}
	}
}