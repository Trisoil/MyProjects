package com.pplive.p2p
{
	import com.pplive.p2p.events.SegmentCompleteEvent;
	import com.pplive.p2p.kernel.KernelDescription;
	
	import flash.events.IEventDispatcher;
	import flash.utils.ByteArray;
	
	public interface IP2PStream extends IEventDispatcher
	{
		// baseTimeStamp is the start TimeStamp of this segment. 
		function start(offset:uint, restPlayTime:uint,completeEvent:SegmentCompleteEvent = null):void;
		function destory():void;
		function reportKernelStatus(exist:Boolean, detectTime:int, description:KernelDescription = null):void;
		function readBytes():ByteArray;
		function seek(time:uint):void;
		function setRestPlayTime(time:uint):void;
		function getPlayMode():int;
		function setRid(rid:String):void;
		
		//http连接失败的次数
		function httpFailTimes():int 
	}
}