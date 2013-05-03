package com.pplive
{
	import flash.display.DisplayObjectContainer;
	import flash.events.KeyboardEvent;
	import flash.system.Capabilities;
	import flash.text.TextField;
	
	import org.as3commons.logging.api.LOGGER_FACTORY;
	import org.as3commons.logging.setup.LevelTargetSetup;
	import org.as3commons.logging.setup.LogSetupLevel;
	import org.as3commons.logging.setup.target.TextFieldTarget;
	import org.as3commons.logging.setup.target.TraceTarget;
	import org.as3commons.logging.setup.target.AlconTarget;
	import org.as3commons.logging.setup.target.MergedTarget;
	import org.as3commons.logging.setup.*;
	import org.as3commons.logging.setup.target.*;
	
	public class LogTarget
	{
		private static var textTarget:TextFieldTarget;
		
		public function LogTarget()
		{
		}
		
		public static function set container(dc:DisplayObjectContainer):void
		{
			if(dc != null)
			{
				textTarget = new TextFieldTarget;
				textTarget.visible = false;
				textTarget.x = - Capabilities.screenResolutionX / 2;
				textTarget.y = - Capabilities.screenResolutionY / 2;
				textTarget.width = Capabilities.screenResolutionX;
				textTarget.height = Capabilities.screenResolutionY;
				
				if (dc.stage)
				{
					dc.stage.addEventListener(KeyboardEvent.KEY_DOWN, keyEventHandler);
				}
				
				LOGGER_FACTORY.setup = new LevelTargetSetup(textTarget, LogSetupLevel.INFO);
				
				//var alconTarget:AlconTarget = new AlconTarget;
				//var mergeTarget:MergedTarget = new MergedTarget(textTarget,alconTarget);				
				//LOGGER_FACTORY.setup = new LevelTargetSetup(mergeTarget, LogSetupLevel.INFO);
				
				dc.addChild(textTarget);
			}
			else
			{
				var traceTarget:TraceTarget = new TraceTarget;
				LOGGER_FACTORY.setup = new LevelTargetSetup(traceTarget, LogSetupLevel.DEBUG);	
			}
		}
		
		private static function keyEventHandler(event:KeyboardEvent):void 
		{
			if (event.altKey && event.ctrlKey && event.keyCode == 80)
			{
				textTarget.visible = !textTarget.visible;
			}
		}
	}
}