package
{
	import flash.events.Event;
	
	public class DRMLoadFailedEvent extends Event
	{
		public static const DRM_LOAD_ERROR:String = "DRM_LOAD_ERROR";
		public var reason:uint;
		
		public function DRMLoadFailedEvent(reason:uint, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(DRM_LOAD_ERROR, bubbles, cancelable);
			this.reason = reason;
		}
		
		override public function toString():String
		{
			return super.toString() + " [reason:" + reason + "]";
		}
	}
}