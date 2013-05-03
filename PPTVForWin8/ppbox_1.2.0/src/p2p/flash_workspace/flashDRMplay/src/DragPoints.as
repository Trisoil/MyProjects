package
{
	public class DragPoints
	{
		private var _time:Number;
		private var _segmentID:uint;
		private var _fileOffset:uint;
		
		public function DragPoints(time:Number, segmentID:uint, offset:uint)
		{
			_time = time;
			_segmentID = segmentID;
			_fileOffset = offset;
		}
		
		public function get time():Number {return _time;}
		public function get segmentID():uint {return _segmentID;}
		public function get offset():uint {return _fileOffset;}
	}
}
