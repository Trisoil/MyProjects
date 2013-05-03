package com.pplive.profile
{	
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class FunctionProfiler
	{
		private static var defaultLogger:ILogger = getLogger(FunctionProfiler);
		private var _begin:int;
		private var _update:int;
		private var _section:int;
		private var _logger:ILogger;
		
		public function FunctionProfiler(logger:ILogger = null, functionName:String = null)
		{
			_begin = getTimer();
			_update = _begin 
			_section = 0;
			_logger = logger;
			
			if (functionName)
			{
				this.logger.debug(functionName);
			}
		}
		
		public function makeSection():void
		{
			var now:int = getTimer();
			logger.debug("Section" + (_section++) + " duration=" + (now - _update));
			_update = now;
		}
		
		public function end():void
		{
			var now:int = getTimer();
			if (_section != 0)
			{
				logger.debug("Section" + (_section++) + " duration=" + (now - _update));
			}
			
			logger.debug("End duration=" + (now - _begin));
		}
		
		private function get logger():ILogger
		{
			if (_logger)
			{
				return _logger;
			}
			else
			{
				return defaultLogger;
			}
		}
	}
}