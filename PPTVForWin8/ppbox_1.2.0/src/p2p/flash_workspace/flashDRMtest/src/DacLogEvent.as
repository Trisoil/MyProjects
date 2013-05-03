package
{
	import flash.events.Event;
	
	public class DacLogEvent extends Event
	{
		public static const STOP_DAC_LOG:String = "STOP_DAC_LOG";
		private var _logObject:Object;
		public function DacLogEvent(type:String, logObject:Object = null, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			_logObject = logObject;
		}
		
		public function get logObject():Object
		{
			return _logObject;
		}
	}
}