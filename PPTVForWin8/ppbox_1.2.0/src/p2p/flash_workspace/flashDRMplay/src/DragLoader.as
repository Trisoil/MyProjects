package
{
	import com.pplive.net.LoadFailedEvent;
	import com.pplive.net.UrlLoaderWithRetry;
	
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
		private var fileName:String;
		
		private var dragLoader:UrlLoaderWithRetry;
		private var dragRequestStartTime:int;
		private var _requestDuration:uint;
		
		private var _segments:Vector.<SegmentInfo>;
		private var _dragPoints:Vector.<DragPoints>;
		private var _videoHeight:uint = 0;
		private var _videoWidth:uint = 0;
		private var _videoDuration:Number = 0;
		
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
			dragLoader.load(Utils.DRAG_HOST + fileName + "0drag0");
			dragRequestStartTime = getTimer();
		}
		
		public function get requestDuration():uint {return _requestDuration;}
		public function get segments():Vector.<SegmentInfo> {return _segments;}
		public function get dragPoints():Vector.<DragPoints> {return _dragPoints;}
		public function get videoHeight():uint {return _videoHeight;}
		public function get videoWidth():uint {return _videoWidth;}
		public function get videoDuration():Number {return _videoDuration;}
		
		private function onDragComplete(event:Event):void
		{
			_requestDuration = getTimer() - dragRequestStartTime;
			
			try
			{
				var dragXml:XML = new XML(dragLoader.data);
				logger.debug(dragXml);
				
				_videoWidth = dragXml.video.width.children()[0].toString();
				_videoHeight = dragXml.video.height.children()[0].toString();
				_videoDuration = dragXml.video.duration.children()[0].toString();
				
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
					var offset:uint = uint(segmentList[i].attribute("offset"));
					
					logger.info("segment " + i + " duration:" + duration + ", offset:" + offset 
						+ ", fileLength:" + fileLength + ", headLength:" + headLength);
					segments.push(new SegmentInfo(rid, duration, fileLength, headLength, offset));
				}
				_dragPoints = new Vector.<DragPoints>;
				var dragPointList:String = dragXml.drag;
				logger.info(dragPointList);
				var dragPointElements:Array = dragPointList.split("|");
				logger.info("array: " + dragPointElements);
				for (var j:uint = 0; j < dragPointElements.length; ++j) {
					var dragElements:Array = (dragPointElements[j] as String).split(":");
					logger.info("dragPoints: " + dragElements.toString());
					if (dragElements.length >= 3) {
						_dragPoints.push(new DragPoints(dragElements[0], dragElements[1], dragElements[2]));
					}
				}
				
				dispatchEvent(new Event(Event.COMPLETE));
			}
			catch(e:TypeError)
			{
				logger.error("onDragComplete parse error:" + e);
				dispatchEvent(new DRMLoadFailedEvent(DRMFailedReason.DRAG_ERROR));
			}
		}
		
		private function onDragSecurityError(event:SecurityErrorEvent):void
		{
			dispatchEvent(new DRMLoadFailedEvent(DRMFailedReason.DRAG_ERROR));
		}
		
		private function onDragLoadFailed(event:LoadFailedEvent):void
		{
			dispatchEvent(new DRMLoadFailedEvent(DRMFailedReason.DRAG_ERROR));
		}
		
	}
}