package com.pplive.p2p.events
{
	import flash.events.Event;
	
	public class SegmentCompleteEvent extends Event
	{
		public static const SEGMENT_COMPLETE:String = "SEGMENT_COMPLETE";
		public var videoTimeStamp:uint;
		public var audioTimeStamp:uint;
		public function SegmentCompleteEvent(type:String, videoTimeStamp:uint, audioTimeStamp:uint,
											 bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			this.videoTimeStamp = videoTimeStamp;
			this.audioTimeStamp = audioTimeStamp;
		}
	}
}