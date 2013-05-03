package
{
	public class SegmentInfo
	{
		private var _rid:String;
		private var _duration:Number;
		private var _fileLength:uint;
		private var _headLength:uint;
		private var _offset:uint;
		
		public function SegmentInfo(rid:String, duration:Number, fileLength:uint, headLength:uint, offset:uint)
		{
			_rid = rid;
			_duration = duration;
			_fileLength = fileLength;
			_headLength = headLength;
			_offset = offset;
		}
		
		public function get rid():String {return _rid;}
		public function get duration():Number {return _duration;}
		public function get fileLength():uint {return _fileLength;}
		public function get headLength():uint {return _headLength;}
		public function get offset():uint {return _offset;}
		public function get endOffset():uint { return _offset + _fileLength - _headLength; }
	}
}
