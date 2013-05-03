package com.pplive.play
{
	public class SegmentInfo
	{
		private var _rid:String;
		private var _duration:Number;
		private var _fileLength:uint;
		private var _headLength:uint;
		
		public function SegmentInfo(rid:String, duration:Number, fileLength:uint, headLength:uint)
		{
			_rid = rid;
			_duration = duration;
			_fileLength = fileLength;
			_headLength = headLength;
		}
		
		public function get rid():String {return _rid;}
		public function set rid(ridString:String):void {_rid = ridString;}
		public function get duration():Number {return _duration;}
		public function get fileLength():uint {return _fileLength;}
		public function get headLength():uint {return _headLength;}
	}
}
