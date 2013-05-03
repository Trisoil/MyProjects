package
{
	import flash.events.Event;
	
	public class PlayInfoLoadFailedEvent extends Event
	{
		public static const PLAYINFO_LOAD_ERROR:String = "PLAYINFO_LOAD_ERROR";
		public var reason:uint;
		
		public function PlayInfoLoadFailedEvent(reason:uint, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(PLAYINFO_LOAD_ERROR, bubbles, cancelable);
			this.reason = reason;
		}
		
		override public function toString():String
		{
			return super.toString() + " [reason:" + reason + "]";
		}
	}
}