package
{
	import flash.events.Event;
	
	public class DownloadEvent extends Event
	{
		public static const DOWNLOAD_EVENT:String = "DOWNLOAD_EVENT";
		public var downloadStatus:uint;
		public function DownloadEvent(type:String, status:uint,
											 bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			this.downloadStatus = status;
		}
	}
}