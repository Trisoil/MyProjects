package org.as3commons.concurrency.thread
{
	import org.libspark.thread.Thread;
	import org.libspark.thread.EnterFrameThreadExecutor;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class FramePseudoThread extends Thread implements IThread
	{
		Thread.initialize(new EnterFrameThreadExecutor());
		
		private static var logger:ILogger = getLogger(FramePseudoThread);
		private var _runnable:IRunnable;
		private var _isRunning:Boolean = false;
		public function FramePseudoThread(runnable:IRunnable)
		{
			super();
			_runnable = runnable;
		}
		
		public function destroy():void
		{
			_isRunning = false;
			if (_runnable)
			{
				_runnable.cleanup();
				_runnable = null;
			}			
		}
		
		override public function start():void
		{
			super.start();
			_isRunning = true;
		}
		
		protected override function run():void
		{
			
			if (_isRunning && _runnable)
			{
				try
				{
					_runnable.process();
				}				
				catch(e:Object)
				{
					logger.error("FramePseudoThread error:" + e);
				}
			}			
			
			next(run);
		}
		
		public function pause():void
		{
			_isRunning = false;
		}
		
		public function resume():void
		{
			_isRunning = true;
		}
		
		public function isRunning():Boolean
		{
			return _isRunning;
		}
		
		public function stop():void
		{
			destroy();
		}
	}
}