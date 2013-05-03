package com.pplive.play
{
	import com.pplive.events.*;
	import com.pplive.p2p.struct.Constants;
	
	import flash.events.AsyncErrorEvent;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.NetStatusEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.media.SoundTransform;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.NetStream;
	import flash.net.NetStreamAppendBytesAction;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class PlayManager extends EventDispatcher implements IPlayManager
	{
		private static var logger:ILogger = getLogger(PlayManager);
		protected static var MAX_BUFFER_LENGTH:uint = 300;
		protected static var STREAM_BUFFER_TIME:uint = 4;

		protected var connection:NetConnection;
		protected var video:Video;
		protected var _stream:NetStream;
		protected var seekTime:uint = 0;
		protected var sessionId:String;
		protected var haveDispatchPlayStartEvent:Boolean = false;
		protected var secondTimer:Timer;

		//0 不用上报，1，stream append的时候上报，2 刚开始buffer full的时候上报 3，拖动后bufferfull上报
		protected const PLAY_RESULT_APPEND_REPORT:uint = 1;
		protected const PLAY_RESULT_START_BFULL_REPORT:uint = 2;
		protected const PLAY_RESULT_SEEK_BFULL_REPORT:uint = 3;		
		protected const PLAY_RESULT_NOT_REPORT:uint = 0;	

		protected var reportPlayResultStauts:uint = PLAY_RESULT_START_BFULL_REPORT;
		protected var playResultStartTime:uint = 0; 
		
		public function PlayManager()
		{
			connection = new NetConnection;
			connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler, false, 0, true);
			connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler, false, 0, true);
			connection.connect(null);
			
			secondTimer = new Timer(1000);
			secondTimer.addEventListener(TimerEvent.TIMER, onSecondTimer, false, 0, true);
			
			sessionId = new Date().valueOf().toString() + Math.random();
		}

		public function destroy():void
		{
			logger.info("PlayManager destory");

			video.attachNetStream(null);
			video = null;
			
			connection.close();
			connection.removeEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			connection.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection = null;
			
			_stream.close();
			_stream.removeEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			_stream.removeEventListener(IOErrorEvent.IO_ERROR, errorHandler);
			_stream.removeEventListener(AsyncErrorEvent.ASYNC_ERROR, errorHandler);
			_stream = null;

			secondTimer.stop();
			secondTimer.removeEventListener(TimerEvent.TIMER, onSecondTimer);
			secondTimer = null;

			sessionId = null;
		}

		public function attachVideo(video:Video):void
		{
			this.video = video;
			video.attachNetStream(_stream);
		}
		
		public function get volume():Number
		{
			return _stream.soundTransform.volume;
		}
		
		public function get bufferTime():uint
		{
			return _stream.bufferTime;
		}
		
		public function set volume(volume:Number):void
		{
			if (volume < 0)
			{
				volume = 0;
			}
			
			logger.info("set volume:" + volume);
			_stream.soundTransform = new SoundTransform(volume);
		}
		
		public function play(startTime:uint = 0):void
		{
			logger.info("play from " + startTime + "seconds.");
			reportPlayResultStauts = PLAY_RESULT_START_BFULL_REPORT;
			playResultStartTime = getTimer();
			
			secondTimer.start();
		}
		
		public function seek(seekTime:uint):void
		{
			logger.info("seek to " + seekTime + "seconds.");
			reportPlayResultStauts = PLAY_RESULT_SEEK_BFULL_REPORT;
			playResultStartTime = getTimer();
			
			// notify player buffer empty to display buffer animation.
			dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_EMPTY, false));
			
			_stream.seek(0);
		}
		
		public function pause():void
		{
			logger.info("pause");
			_stream.pause();
		}
		
		public function resume():void
		{
			logger.info("resume");
			_stream.resume();
		}
		
		public function get droppedFrame():Number
		{
			return _stream.info.droppedFrames;
		}
		
		public function get time():uint
		{
			return seekTime + _stream.time;
		}

		public function get stream():NetStream { return _stream; }
		
		public function appendBytes(bytes:ByteArray):void
		{
			if (!haveDispatchPlayStartEvent)
			{
				haveDispatchPlayStartEvent = true;

				logger.info("NetStream.Play.Start: send play start event.playmode:" + getPlayMode());
				dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_START, getPlayMode()));
				
				// notify player buffer empty to display buffer animation.
				dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_EMPTY, false));
			}
			
			_stream.appendBytes(bytes);
		}

		protected function getPlayMode():uint { return Constants.PLAY_MODE_DIRECT; }
		protected function getPlayResult():uint { return 0; }
		protected function isComplete():Boolean { return false; }
		
		protected function onSecondTimer(event:TimerEvent):void
		{
			dispatchEvent(new PlayProgressEvent(time, _stream.bufferLength));

			if (_stream.bufferLength >= _stream.bufferTime)
			{
				if (reportPlayResultStauts == PLAY_RESULT_START_BFULL_REPORT||
					reportPlayResultStauts == PLAY_RESULT_SEEK_BFULL_REPORT)
				{
					logger.info("send buffer full event");
					DispatchPlayResultEvent();
				}
			}
		}

		protected function onSeekNotify():void
		{
			logger.info("onSeekNotify");
			_stream.appendBytesAction(NetStreamAppendBytesAction.RESET_BEGIN);
			
			dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_SEEK_NOTIFY));
		}

		private function createNetStream():void
		{
			_stream = new NetStream(connection);
			_stream.inBufferSeek = false;
			_stream.checkPolicyFile = true;
			_stream.bufferTime = STREAM_BUFFER_TIME;
			_stream.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler, false, 0, true);
			_stream.addEventListener(IOErrorEvent.IO_ERROR, errorHandler, false, 0, true);
			_stream.addEventListener(AsyncErrorEvent.ASYNC_ERROR, errorHandler, false, 0, true);
			_stream.play(null);
		}

		protected function netStatusHandler(event:NetStatusEvent):void
		{
			switch (event.info.code) {
				case "NetConnection.Connect.Success":
					createNetStream();
					break;
					
				case "NetStream.Buffer.Full":
					logger.info("NetStream.Buffer.Full " + new Date().toLocaleTimeString());
					dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_FULL));
					
					if (reportPlayResultStauts == PLAY_RESULT_START_BFULL_REPORT||
						reportPlayResultStauts == PLAY_RESULT_SEEK_BFULL_REPORT)
					{
						DispatchPlayResultEvent();
					}
					break;
				
				case "NetStream.Buffer.Empty":
					if (isComplete())
					{
						logger.info("NetStream.Buffer.Empty movie complete.");
						dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_COMPLETE));
					}
					else
					{
						logger.info("NetStream.Buffer.Empty movie buffering.");
						dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_EMPTY, true));
					}					
					break;
					
				case "NetStream.Seek.Notify":
					onSeekNotify();
					break;

				case "NetStream.Pause.Notify":
					dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_PAUSED));
					break;
			}
		}
		
		protected function getPlayURL():String { return ""; }
		
		protected function DispatchPlayResultEvent():void
		{
			//PlayResultEvent(m:int,url:String,interval:uint);
			//6表示有插成功，7表示无插成功，8有插拖动成功，9无插拖动成功。
			logger.info("reportPlayResultStauts:"+reportPlayResultStauts);
			
			var m:int = getPlayResult();
			var url:String = getPlayURL();
			var interval:uint =getTimer()- playResultStartTime;

			logger.info("dispatch play result event,m:"+m+" url:"+url+" interval:"+interval);
			dispatchEvent(new PlayResultEvent(m,url,interval));
			
			//这里不需要将playResultStartTime 设置为0，因为之后的 onVodHttpError 和 onKernelHttpError可能还要用
			//playResultStartTime = 0;
			reportPlayResultStauts = PLAY_RESULT_NOT_REPORT;	
		}
		
		private function securityErrorHandler(event:SecurityErrorEvent):void {
			logger.info("securityErrorHandler: " + event);
		}
		
		private function errorHandler(event:Event):void {
			logger.info("errorHandler: " + event);
		}
	}
}
