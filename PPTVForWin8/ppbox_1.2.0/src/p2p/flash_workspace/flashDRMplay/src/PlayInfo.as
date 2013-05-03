package
{	
	import de.polygonal.ds.ArrayUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;	
	
	public class PlayInfo
	{
		private static var logger:ILogger = getLogger(PlayInfo);
		private var _host:String;
		private var _fileName:String;
		private var _key:String;
		private var _segments:Vector.<SegmentInfo>;
		private var _duration:uint;
		private var _dragPoints:Vector.<DragPoints>;
		private var _dtLoader:DtLoader;
		private var _dragLoader:DragLoader;
		private var segmentTimeOffsetArray:Array = new Array;
		private var dragTimeOffsetArray:Array = new Array;
		private var _currentDragIndex:int;
		private var _dragResolution:Number;

		
		public function PlayInfo(fileName:String, dtLoader:DtLoader, dragLoader:DragLoader)
		{
			_fileName = fileName;
			_key = key;
			_segments = dragLoader.segments;
			_dragPoints = dragLoader.dragPoints;
			
			_dtLoader = dtLoader;
			_dragLoader = dragLoader;
			calcDuration();
			
			var timeOffset:Number = 0;
			for (var i:uint = 0; i < _segments.length; ++i)
			{
				segmentTimeOffsetArray.push(timeOffset);
				timeOffset += _segments[i].duration;
			}
			//important: we insert a zero at the beginning to ease our search.
			dragTimeOffsetArray.push(0);
			_dragResolution = _dragPoints[0].time;
			for (var j:uint = 0; j < _dragPoints.length; ++j)
			{
				//var dragP:Vector.<DragPoints> = playInfo.dragPoints;
				dragTimeOffsetArray.push(_dragPoints[j].time);
			}

		}
		
		private function destroy():void
		{
			segmentTimeOffsetArray.length = 0;
			segmentTimeOffsetArray = null;
			dragTimeOffsetArray.length = 0;
			dragTimeOffsetArray = null;
		}
		
		private function calcDuration():void
		{
			var totalDuration:Number = 0;
			for (var i:uint = 0; i < segments.length; ++i)
			{
				totalDuration += segments[i].duration;	
			}
			_duration = totalDuration;
		}
				
		private function getIndexFromTime(seekTime:Number):int
		{
			var dragIndex:Number;
			for (var i:uint = 0; i < dragTimeOffsetArray.length; i ++ ) {
				if (seekTime <= dragTimeOffsetArray[i]) {
					break;
				}
			}
			if (i >= (dragTimeOffsetArray.length - 1) ) {
				dragIndex = dragTimeOffsetArray.length - 1;
			}else{
				dragIndex = i;
				if (i > 0 && (Math.abs(seekTime - dragTimeOffsetArray[i-1]) < Math.abs(dragTimeOffsetArray[i] - seekTime))) {
					dragIndex --;
					logger.info("drag point is closer to the previous one " + dragIndex);
				}
			}
			
			return dragIndex - 1;
		}
		
		public function constructUrl(seekTime:Number):String
		{
			var dragIndex:int = getIndexFromTime(seekTime);
			var offset:uint = 0;
			var segmentIndex:uint = 0;
			
			logger.info("seek index in DRAG is " + dragIndex);
			//we are dragging only if we passed the first drag point.
			if (dragIndex >= 0) {
				segmentIndex = _dragPoints[dragIndex].segmentID;
				offset = _dragPoints[dragIndex].offset - _segments[segmentIndex].offset;
				logger.info("seek time in DRAG is " + _dragPoints[dragIndex].time);
			}
				
			_currentDragIndex = dragIndex;
			
			var baseUrl:String = _dtLoader.host + segmentIndex + "/" + fileName + ".mp4?key=" + _dtLoader.key + "&start=" + offset;
			
			logger.info(baseUrl);
			return baseUrl;
		}
		
		public function needSeek(targetTime:Number, currentTime:Number):Boolean
		{
			logger.info("Need seek: dragpoint -now " + currentTime + " vs. -new " +targetTime + " threshold: " + _dragResolution);
			var diff:Number = Math.abs(targetTime - currentTime);
			return (diff > _dragResolution);
		}
		
		public function getSeekTime():Number
		{
			var newSeek:Number = 0;

			if (_currentDragIndex >= 0) {
				newSeek = _dragPoints[_currentDragIndex].time;
			}
			
			logger.info("Get seektime: " + newSeek);
			return newSeek;
		}
		
		public function get lastDragPointIndex():uint { return _currentDragIndex;}
		public function get host():String { return _host;}
		public function get fileName():String { return _fileName;}
		public function get key():String { return _key;}
		public function get segments():Vector.<SegmentInfo> { return _segments;}
		public function get dragPoints():Vector.<DragPoints> { return _dragPoints;}
		public function get duration():uint {return _duration;}
	}
}