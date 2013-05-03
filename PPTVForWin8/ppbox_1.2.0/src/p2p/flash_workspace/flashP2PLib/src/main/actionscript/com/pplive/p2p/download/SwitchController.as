package com.pplive.p2p.download
{
	import com.pplive.p2p.struct.Constants;
	import com.pplive.p2p.BootStrapConfig;
	
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	
	public class SwitchController implements ISwitchController
	{
		private static var logger:ILogger = getLogger(SwitchController);
		private var kernelMode:Boolean;
		private var httpDownloaders:Vector.<HttpDownloader>;
		private var p2pDownloader:IDownloader;
		private var restPlayTime:uint = 0;
		private var controlTimer:Timer;
		private var bwType:uint;
		
		public function SwitchController(kernelMode:Boolean)
		{
			this.kernelMode = kernelMode;
			httpDownloaders = new Vector.<HttpDownloader>;
			
			if (!kernelMode)
			{
				controlTimer = new Timer(1000);
				controlTimer.addEventListener(TimerEvent.TIMER, onControlTimer, false, 0, true);	
			}
		}
		
		public function start():void
		{
			if (kernelMode)
			{
				httpDownloaders[0].resume();
			}
			else
			{
				onControlTimer(null);
				controlTimer.start();	
			}
		}
		
		public function stop():void
		{
			if (controlTimer)
			{
				controlTimer.stop();
				controlTimer.removeEventListener(TimerEvent.TIMER, onControlTimer);
				controlTimer = null;
			}
			
			if (p2pDownloader)
			{
				p2pDownloader.pause();
				p2pDownloader = null;
			}
			
			var httpDownloadersLength:uint = httpDownloaders.length;
			for (var i:uint = 0; i < httpDownloadersLength; ++i)
			{
				httpDownloaders[i].pause();
			}
			httpDownloaders.length = 0;
			httpDownloaders = null;
		}
		
		public function addHttpDownloader(httpDownloader:HttpDownloader):void
		{
			httpDownloaders.push(httpDownloader);
		}
		
		public function addP2PDownloader(p2pDownloader:IDownloader):void
		{
			this.p2pDownloader = p2pDownloader;
		}
		
		public function setRestPlayTime(restPlayTime:uint):void
		{
			this.restPlayTime = restPlayTime;
		}
		
		public function setBWType(bwType:uint):void
		{
			this.bwType = bwType;
		}
		
		// Subclasses of SwitchController should override onControlTimer to implement other control algorithm 
		protected function onControlTimer(event:TimerEvent):void
		{
			if (bwType == Constants.BWTYPE_HTTP_ONLY|| bwType == Constants.BWTYPE_HTTP_PREFERRED  || !BootStrapConfig.useP2p)
			{
				httpDownloaders[0].resume();
			}
			else if (bwType == Constants.BWTYPE_P2P_ONLY)
			{
				if (p2pDownloader != null)
				{
					p2pDownloader.resume();
				}
			}
			else
			{
				// 暂时不区别对待BWTYPE_NORMAL，BWTYPE_HTTP_MORE，BWTYPE_HTTP_PREFERRED三种情况
				if (p2pDownloader == null)
				{
					httpDownloaders[0].resume();
				}
				else
				{
					if (restPlayTime > BootStrapConfig.startP2pRestPlayTime)
					{
						// TODO(herain):multi cdn support havn't implement now.
						httpDownloaders[0].pause();
						p2pDownloader.resume();
						logger.debug("restPlayTime enough, resume p2p download.restPlayTime:"+restPlayTime);
					}
					else if(restPlayTime < BootStrapConfig.resumeHttpRestPlayTime)
					{
						p2pDownloader.pause();
						httpDownloaders[0].resume();
						logger.debug("restPlayTime not enough, resume http download.restPlayTime:"+restPlayTime);
					}
				}
			}
		}
	}
}