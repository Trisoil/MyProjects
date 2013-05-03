package com.pplive.play
{
	import com.pplive.events.*;
	import com.pplive.p2p.IP2PStream;
	import com.pplive.p2p.P2PMp4Stream;
	import com.pplive.p2p.events.GetSeekTimeEvent;
	import com.pplive.p2p.events.SegmentCompleteEvent;
	import com.pplive.p2p.kernel.KernelPreDownload;
	import com.pplive.p2p.struct.Constants;
	import com.pplive.profile.FunctionProfiler;
	import com.pplive.util.StringConvert;
	import com.pplive.util.URI;
	
	import de.polygonal.ds.ArrayUtil;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.NetStatusEvent;
	import flash.events.ProgressEvent;
	import flash.events.TimerEvent;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class P2PPlayManager extends PlayManager
	{
		private static var logger:ILogger = getLogger(P2PPlayManager);

		private var playInfo:PlayInfo;
		private var dragLoader:DragLoader;
		private var segmentTimeOffsetArray:Array = new Array;
		private var _currentSegment:int = -1;
		private var isLastSegmentComplete:Boolean = false;
		private var preDownloadUrlArray:Array = new Array;
		private var segmentStreamVector:Vector.<IP2PStream>;
		private var currentSegmentCompleteEvent:SegmentCompleteEvent;
		private var haveDispatchDetectKernelLogEvent:Boolean = false;

		private var kernelPreDownload:KernelPreDownload;

		public function P2PPlayManager(playInfo:PlayInfo)
		{
			super();

			Constants.IS_VIP=playInfo.isVip;
			this.playInfo = playInfo;			

			var timeOffset:Number = 0;
			var segmentsLength:uint = playInfo.segments.length;
			for (var i:uint = 0; i < segmentsLength; ++i)
			{
				segmentTimeOffsetArray.push(timeOffset);
				timeOffset += playInfo.segments[i].duration;
			}
			
			segmentStreamVector = new Vector.<IP2PStream>(playInfo.segments.length);
			
			if (playInfo.segments[0].rid == null)
			{
				dragLoader = new DragLoader(playInfo.fileName);
				dragLoader.addEventListener(Event.COMPLETE, onDragLoaded, false, 0, true);
				dragLoader.load();
			}
			
			addEventListener(PlayStatusEvent.PLAY_SEEK_NOTIFY, seekNotifyListener);

			logger.info("P2PPlayManager construct,play info:"+playInfo);			
		}

		public function destory():void
		{
			playInfo = null;
			segmentTimeOffsetArray.length = 0;
			segmentTimeOffsetArray = null;

			deleteP2PMp4Stream(_currentSegment);

			segmentStreamVector.length = 0;
			segmentStreamVector = null;

			currentSegmentCompleteEvent = null;
			
			removeEventListener(PlayStatusEvent.PLAY_SEEK_NOTIFY, seekNotifyListener);

			super.destroy();
		}

		override public function play(startTime:uint = 0):void
		{
			logger.info("play from " + startTime + "seconds.");

			var segmentIndex:uint = findSegmentIndexFromTime(startTime);
			_currentSegment = segmentIndex;
			secondTimer.start();
			
			playResultStartTime = getTimer();
			reportPlayResultStauts = PLAY_RESULT_START_BFULL_REPORT;			

			seekTime = startTime;

			createP2PMp4Stream(_currentSegment);
			var timeOffset:uint = startTime - segmentTimeOffsetArray[_currentSegment];
			segmentStreamVector[_currentSegment].start(timeOffset, _stream.bufferLength);
		}

		override public function seek(seekTime:uint):void
		{
			if (seekTime >= 0 && seekTime <= playInfo.duration)
			{
				logger.info("seek to " + seekTime + "seconds.");
				this.seekTime = seekTime;
				currentSegmentCompleteEvent = null;
				isLastSegmentComplete = false;
				_stream.seek(0);	
				
				// notify player buffer empty to display buffer animation.
				dispatchEvent(new PlayStatusEvent(PlayStatusEvent.BUFFER_EMPTY, false));
				
				playResultStartTime = getTimer();
				reportPlayResultStauts = PLAY_RESULT_SEEK_BFULL_REPORT;
			}
		}

		override protected function getPlayMode():uint { return segmentStreamVector[_currentSegment].getPlayMode(); }
		
		override protected function getPlayURL():String
		{
			return playInfo.constructUrl(_currentSegment);
		}
		
		override protected function isComplete():Boolean { return isLastSegmentComplete; }
		
		override public function appendBytes(bytes:ByteArray):void
		{
			if (reportPlayResultStauts == PLAY_RESULT_APPEND_REPORT)
			{
				DispatchPlayResultEvent();
			}
			
			super.appendBytes(bytes);
		}

		override protected function getPlayResult():uint
		{ 
			var m:uint = 0;

			if (Constants.PLAY_MODE_KERNEL == segmentStreamVector[_currentSegment].getPlayMode())
			{
				if (reportPlayResultStauts == PLAY_RESULT_SEEK_BFULL_REPORT )
				{
					m=8;
				}
				else
				{
					m=6;
				}
			}
			else
			{
				if (reportPlayResultStauts == PLAY_RESULT_SEEK_BFULL_REPORT )
				{
					m=9;
				}
				else
				{
					m=7;
				}
			}

			if (segmentStreamVector[_currentSegment].httpFailTimes() > 0)
			{
				//10,11,12,13表示有http连接有重试过。
				//10表示有插成功，11表示无插成功，12有插拖动成功，13无插拖动成功。
				m+=4;
			}

			return m;
		}

		protected function seekNotifyListener(e:PlayStatusEvent):void
		{
			var segmentIndex:uint = findSegmentIndexFromTime(seekTime);
			var timeOffset:uint;

			if (segmentIndex == _currentSegment)
			{
				logger.info("seek in the same segment:" + _currentSegment);
				timeOffset = seekTime - segmentTimeOffsetArray[_currentSegment];
				segmentStreamVector[_currentSegment].seek(timeOffset);
			}
			else
			{
				logger.info("seek in the different segment: " + _currentSegment + "->" + segmentIndex);

				//如果是内核模式，则需要清理预下载的资源
				if(Constants.PLAY_MODE_KERNEL == segmentStreamVector[_currentSegment].getPlayMode())
				{
					if(null == kernelPreDownload)
					{
						//没有启动过预下载，也无需关闭
					}
					else
					{
						kernelPreDownload.stop(constructKernelUrl(_currentSegment));
					}
				}

				deleteP2PMp4Stream(_currentSegment);
				_currentSegment = segmentIndex;
				createP2PMp4Stream(segmentIndex);
				timeOffset = seekTime - segmentTimeOffsetArray[_currentSegment];
				segmentStreamVector[_currentSegment].start(timeOffset, _stream.bufferLength);
			}
		}

		private function createP2PMp4Stream(segmentIndex:uint):void
		{
			if (segmentStreamVector[segmentIndex] == null)
			{
				var p2pStream:IP2PStream = new P2PMp4Stream(sessionId, playInfo.constructUrl(segmentIndex)
					, playInfo.bwType, playInfo.segments[segmentIndex], playInfo.backupHostVector);
				segmentStreamVector[segmentIndex] = p2pStream;
				p2pStream.addEventListener(ProgressEvent.PROGRESS, flvProgrossHandler, false, 0, true);
				p2pStream.addEventListener(GetSeekTimeEvent.GET_SEEK_TIME, getSeekTime, false, 0, true);
				p2pStream.addEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, segmentCompleteListener, false, 0, true);
				p2pStream.addEventListener(DacLogEvent.P2P_DAC_LOG, onDacLog, false, 0, true);
				p2pStream.addEventListener(DacLogEvent.DETECT_KERNEL_LOG, onDetectKernelLog, false, 0, true);
				p2pStream.addEventListener(HttpErrorEvent.VOD_HTTP_ERROR, onVodHttpError, false, 0, true);
				p2pStream.addEventListener(HttpErrorEvent.KERNEL_HTTP_ERROR, onKernelHttpError, false, 0, true);
			}
		}

		private function deleteP2PMp4Stream(segmentIndex:uint):void
		{
			if (segmentStreamVector[segmentIndex] != null)
			{
				var p2pStream:IP2PStream = segmentStreamVector[segmentIndex];
				segmentStreamVector[segmentIndex] = null;

				p2pStream.destory();
				p2pStream.removeEventListener(ProgressEvent.PROGRESS, flvProgrossHandler);
				p2pStream.removeEventListener(GetSeekTimeEvent.GET_SEEK_TIME, getSeekTime);
				p2pStream.removeEventListener(SegmentCompleteEvent.SEGMENT_COMPLETE, segmentCompleteListener);
				p2pStream.removeEventListener(DacLogEvent.P2P_DAC_LOG, onDacLog);
				p2pStream.removeEventListener(DacLogEvent.DETECT_KERNEL_LOG, onDetectKernelLog);
				p2pStream.removeEventListener(HttpErrorEvent.VOD_HTTP_ERROR, onVodHttpError);
				p2pStream.removeEventListener(HttpErrorEvent.KERNEL_HTTP_ERROR, onKernelHttpError);
			}
		}

		private function flvProgrossHandler(event:ProgressEvent):void 
		{
			// We must guarantee that progress event are dispatched by _currentSegment stream.
			var bytes:ByteArray = segmentStreamVector[_currentSegment].readBytes();
			logger.debug("flvProgrossHandler size:" + bytes.length);

			super.appendBytes(bytes);
		}

		private function getSeekTime(event:GetSeekTimeEvent):void 
		{
			// get accurate seekTime.
			seekTime = segmentTimeOffsetArray[_currentSegment] + event.seekTime;
			logger.info("getSeekTime " + seekTime);
		}

		private function segmentCompleteListener(event:SegmentCompleteEvent):void
		{
			if (_currentSegment != segmentStreamVector.length - 1)
			{
				// we have more segments.
				currentSegmentCompleteEvent = event;
				moveToNextSegment();
			}
			else
			{
				isLastSegmentComplete = true;
			}
		}

		override protected function onSecondTimer(event:TimerEvent):void
		{
			super.onSecondTimer(event);

			// update rest play time.
			segmentStreamVector[_currentSegment].setRestPlayTime(_stream.bufferLength);

			if (currentSegmentCompleteEvent != null)
			{
				// current segment is complete.
				moveToNextSegment();
			}
		}

		private function moveToNextSegment():void
		{
			if (_stream.bufferLength < MAX_BUFFER_LENGTH)
			{
				var profiler:FunctionProfiler = new FunctionProfiler(logger, "moveToNextSegment");
				deleteP2PMp4Stream(_currentSegment);
				profiler.makeSection();

				_currentSegment++;
				createP2PMp4Stream(_currentSegment);
				profiler.makeSection();

				segmentStreamVector[_currentSegment].start(0,_stream.bufferLength, currentSegmentCompleteEvent);
				currentSegmentCompleteEvent = null;

				playResultStartTime = getTimer();
				reportPlayResultStauts = PLAY_RESULT_APPEND_REPORT;

				profiler.end();
			}
			else if(Constants.PLAY_MODE_KERNEL == segmentStreamVector[_currentSegment].getPlayMode())
			{
				//内核空闲了，开始预下载				
				if(null == kernelPreDownload)
				{
					kernelPreDownload = new KernelPreDownload(Constants.LOCAL_KERNEL_TCP_PORT);
					constructAllKernelUrl();
				}

				if(preDownloadUrlArray[_currentSegment] != null)
				{
					kernelPreDownload.start(preDownloadUrlArray[_currentSegment],_stream.bufferLength * 1000);

					//设置到null，是为了使得每一段只会被预下载一次。来回拖动的就不考虑预下载了。
					preDownloadUrlArray[_currentSegment] = null;
				}
			}
		}

		private function constructAllKernelUrl():void
		{
			for(var i:uint =0; i< playInfo.segments.length;++i)
			{		
				//var url:String = constructKernelUrl(i);
				preDownloadUrlArray.push(constructKernelUrl(i));				
			}	
		}
		
		private function constructKernelUrl(index:uint):String
		{
			var url:String = playInfo.constructUrl(index+1);			
			var uri:URI = new URI(url);
			uri.port = Constants.KERNEL_MAGIC_PORT;
			//原来是连19765端口，被钩挂到9000端口，所以不用设置host，现在直连9000，会判断只接受本机来的ip	
			uri.host = "127.0.0.1";
			uri.path = "/ppvaplaybyopen";
			uri.variables.url = url;
			uri.variables.id = index+1;
			//uri.variables.headlength = segmentStreamVector[_currentSegment+1].headLength;
			uri.variables.BWType = playInfo.bwType;
			uri.variables.source = 1;
			url = uri.toString();
			uri	= null;
			return url;
		}
		
		protected function findSegmentIndexFromTime(time:uint):uint
		{
			var segmentIndex:int = ArrayUtil.bsearchFloat(segmentTimeOffsetArray, time, 0, segmentTimeOffsetArray.length - 1);
			if (segmentIndex < 0)
			{
				return ~segmentIndex - 1;
			}
			else
			{
				// Wow, you are so lucky to come this path!
				return segmentIndex;
			}
		}
		
		private function onDacLog(event:DacLogEvent):void
		{
			dispatchEvent(new DacLogEvent(event.logObject));
		}
		
		private function onDetectKernelLog(event:DacLogEvent):void
		{
			if (!haveDispatchDetectKernelLogEvent)
			{
				haveDispatchDetectKernelLogEvent = true;
				event.logObject.n = StringConvert.urldecodeGB2312(playInfo.fileName);
				dispatchEvent(new DacLogEvent(event.logObject,DacLogEvent.DETECT_KERNEL_LOG));
			}
		}
		
		private function onVodHttpError(event:HttpErrorEvent):void
		{
			var url:String = playInfo.constructUrl(_currentSegment);
			var interval:uint =getTimer()- playResultStartTime;
			
			logger.info("dispatch play result event"+" url:"+url+" interval playmanager::"+interval);
			
			if(event.error <10000 )				
			{
				//5表示服务器连接的时候失败,只有没收到数据的时候才发送			
				var m:int = 5;
				dispatchEvent(new PlayResultEvent(m,url,event.interval));
			}
			else
			{
				//播放过程中的http的问题，上报播放失败
				dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_FAILED));
			}
			
			//如果这里要重试，那么就要在重试前将playResultStartTime 设置为当前时间
		}
		
		private function onKernelHttpError(event:HttpErrorEvent):void
		{			
			var interval:uint =getTimer()- playResultStartTime;
			logger.info("dispatch PlayStatusEvent failed,interval1:"+interval+"interval2:"+event.interval);
			//dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_FAILED));
			
			if(event.error <10000 )				
			{
				//5表示服务器连接的时候失败,内核失败记录就好了，随便设置一个url，让内核去查吧		
				var m:int = 5;
				var url:String = "http://127.0.0.1/a.mp4";
				dispatchEvent(new PlayResultEvent(m,url,event.interval));
			}
			else
			{
				//播放过程中的http的问题，上报播放失败
				dispatchEvent(new PlayStatusEvent(PlayStatusEvent.PLAY_FAILED));
			}
		}
		
		protected function onDragLoaded(event:Event):void
		{
			var segmentsLength:uint= playInfo.segments.length;
			for (var i:uint = 0; i < segmentsLength; ++i)
			{
				//logger.info("segment:"+_currentSegment+" rid:"+ playInfo.segments[_currentSegment].rid);
				playInfo.segments[i].rid = dragLoader.segments[i].rid;
			}
			
			if (_currentSegment != -1  && segmentStreamVector[_currentSegment])
			{
				logger.info("segment:"+_currentSegment+" rid:"+ playInfo.segments[_currentSegment].rid);
				segmentStreamVector[_currentSegment].setRid(playInfo.segments[_currentSegment].rid);
			}
		}

		public function get currentSegment():int
		{
			return _currentSegment;
		}
	}
}
