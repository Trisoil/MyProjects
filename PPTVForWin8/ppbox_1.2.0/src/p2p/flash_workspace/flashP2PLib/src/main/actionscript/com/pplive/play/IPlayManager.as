package com.pplive.play
{
	import flash.events.IEventDispatcher;
	import flash.media.Video;
	import flash.net.NetStream;
	import flash.utils.ByteArray;

	public interface IPlayManager extends IEventDispatcher
	{
		function destroy():void;
		function attachVideo(video:Video):void;
		function play(startTime:uint = 0):void;
		function seek(seekTime:uint):void;
		function pause():void;
		function resume():void;
		function get volume():Number;
		function set volume(volume:Number):void;
		function get bufferTime():uint;
		function get droppedFrame():Number;
		function get time():uint;
		function get stream():NetStream;

		function appendBytes(bytes:ByteArray):void;
	}
}
