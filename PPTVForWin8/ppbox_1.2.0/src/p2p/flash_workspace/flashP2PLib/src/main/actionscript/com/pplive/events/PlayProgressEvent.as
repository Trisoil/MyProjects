package com.pplive.events
{
	import flash.events.Event;
	
	public class PlayProgressEvent extends Event
	{
		public static const PLAY_PROGRESS:String = "PLAY_PROGRESS_REPORT";
		private var _timeLoaded:uint;
		private var _bufferLength:uint;
		public function PlayProgressEvent(timeLoaded:uint, bufferLength:uint)
		{
			super(PLAY_PROGRESS);
			_timeLoaded = timeLoaded;
			_bufferLength = bufferLength;
		}
		
		public function get timeLoaded():uint {return _timeLoaded;}
		public function get bufferLength():uint {return _bufferLength;}
	}
}