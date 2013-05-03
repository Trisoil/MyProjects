package com.pplive.events
{
	import flash.events.Event;
	
	public class DacLogEvent extends Event
	{
		public static const P2P_DAC_LOG:String = "P2P_DAC_LOG";
		
		//在检测到内核存在的时候，才发送DETECT_KERNEL_LOG
		public static const DETECT_KERNEL_LOG:String = "DETECT_KERNEL_LOG";
		
		private var _logObject:Object;
		public function DacLogEvent(logObject:Object, type:String = P2P_DAC_LOG)
		{
			super(type);
			_logObject = logObject;
		}
		
		public function get logObject():Object
		{
			return _logObject;
		}
		
		override public function toString():String
		{
			var str:String = super.toString();
			var itemArray:Vector.<String> = new Vector.<String>;
			for(var item:String in logObject)
			{
				itemArray.push(item + ":" + logObject[item]);
			}
			
			str += ", " + itemArray.join();
			return str;
		}
	}
}