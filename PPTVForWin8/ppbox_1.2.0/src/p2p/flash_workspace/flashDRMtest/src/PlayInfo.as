package
{
	public class PlayInfo
	{
		private var _host:String;
		private var _fileName:String;
		private var _key:String;
		private var _segments:Vector.<SegmentInfo>;
		private var _duration:uint;
		
		public function PlayInfo(host:String, fileName:String, segments:Vector.<SegmentInfo>)
		{
			_host = host;
			_fileName = fileName;
			_key = key;
			_segments = segments;
			calcDuration();
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
		
		public function constructUrl(segmentIndex:uint):String
		{
			var baseUrl:String = "http://" + host + ":8080/" + segmentIndex + "/" + fileName + "?key=" + key;			
			return baseUrl;
		}
		
		public function get host():String { return _host;}
		public function get fileName():String { return _fileName;}
		public function get key():String { return _key;}
		public function get segments():Vector.<SegmentInfo> { return _segments;}
		public function get duration():uint {return _duration;}
	}
}