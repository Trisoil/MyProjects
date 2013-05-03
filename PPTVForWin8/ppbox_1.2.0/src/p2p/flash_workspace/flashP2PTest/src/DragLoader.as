package
{
	import com.pplive.net.LoadFailedEvent;
	import com.pplive.net.UrlLoaderWithRetry;
	import com.pplive.play.PlayInfo;
	import com.pplive.play.SegmentInfo;
	import com.pplive.util.URI;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLRequest;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class DragLoader extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(DragLoader);
		private static const dragHost:String = "http://drag.synacast.com/";
		private var fileName:String;
		
		private var dragLoader:UrlLoaderWithRetry;
		private var dragRequestStartTime:int;
		private var _requestDuration:uint;
		
		private var _segments:Vector.<SegmentInfo>;
		private var _videoHeight:uint = 0;
		private var _videoWidth:uint = 0;
		
		public function DragLoader(fileName:String)
		{
			this.fileName = fileName;
		}
		
		public function load():void
		{
			logger.info("PlayInfoLoader load");			
			dragLoader = new UrlLoaderWithRetry(2);
			dragLoader.addEventListener(Event.COMPLETE, onDragComplete, false, 0, true);
			dragLoader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onDragSecurityError, false, 0, true);
			dragLoader.addEventListener(LoadFailedEvent.LOAD_FAILED, onDragLoadFailed, false, 0, true);
			dragLoader.load(URI.transferToPPFixUrl(dragHost + fileName + "0drag"));
			dragRequestStartTime = getTimer();
		}
		
		public function get requestDuration():uint {return _requestDuration;}
		public function get segments():Vector.<SegmentInfo> {return _segments;}
		public function get videoHeight():uint {return _videoHeight;}
		public function get videoWidth():uint {return _videoWidth;}		
		
		private function onDragComplete(event:Event):void
		{
			_requestDuration = getTimer() - dragRequestStartTime;
			
			try
			{
				var dragXml:XML = new XML(dragLoader.data);
				logger.debug(dragXml);
				
				_videoWidth = dragXml.video.width.children()[0].toString();
				_videoHeight = dragXml.video.height.children()[0].toString();
				
				logger.info("videoWidth:" +　videoWidth + ", videoHeight:" + videoHeight);
				
				_segments = new Vector.<SegmentInfo>;
				var segmentList:XMLList = dragXml.segments.segment;
				for (var i:uint = 0; i < segmentList.length(); ++i)
				{
					var duration:Number = Number(segmentList[i].attribute("duration"));
					var ridString:String = segmentList[i].attribute("varid");
					var ridElements:Array = ridString.split("&");
					var rid:String = (ridElements[0] as String).split("=")[1];
					var fileLength:uint = uint((ridElements[1] as String).split("=")[1]);
					var headLength:uint = uint(segmentList[i].attribute("headlength"));
					
					logger.info("segment " + i + " duration:" + duration + ", rid:" + rid 
						+ ", fileLength:" + fileLength + ", headLength:" + headLength);
					segments.push(new SegmentInfo(rid, duration, fileLength, headLength));
				}
				
				//playInfo = new PlayInfo(_host, fileName, key, bwType, segments, null);
				dispatchEvent(new Event(Event.COMPLETE));
			}
			catch(e:TypeError)
			{
				logger.error("onDragComplete parse error:" + e);
				dispatchEvent(new PlayInfoLoadFailedEvent(PlayFailedReason.DRAG_ERROR));
			}
		}
		
		private function onDragSecurityError(event:SecurityErrorEvent):void
		{
			dispatchEvent(new PlayInfoLoadFailedEvent(PlayFailedReason.DRAG_ERROR));
		}
		
		private function onDragLoadFailed(event:LoadFailedEvent):void
		{
			dispatchEvent(new PlayInfoLoadFailedEvent(PlayFailedReason.DRAG_ERROR));
		}
		
	}
}