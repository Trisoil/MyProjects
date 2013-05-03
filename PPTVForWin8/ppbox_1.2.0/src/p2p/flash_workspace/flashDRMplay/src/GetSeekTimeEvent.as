package
{
	import flash.events.Event;
	
	public class GetSeekTimeEvent extends Event
	{
		public static const GET_SEEK_TIME:String = "GET_SEEK_TIME";
		public var seekTime:Number;
		public function GetSeekTimeEvent(type:String, seekTime:Number, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			this.seekTime = seekTime;
		}
	}
}