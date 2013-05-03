package com.pplive.events
{
	import flash.events.Event;
	
	//这个类的产生，是为了兼容非p2p版本上报vodserver状态相关的信息使用的。其中，	interval表示从开始请求到事件发生的时间。
	//如果是连接失败，那么这个时间就是开始连接到发现连不上的时间间隔，如果是先成功，下载一段再失败，这个时间就表示从开始连接到失败的时间间隔。
	public class PlayResultEvent extends Event
	{
		public static const PLAY_RESULT:String = "PLAY_RESULT_REPORT";
		private var _timeLoaded:uint;
		private var _bufferLength:uint;
		
		//m（int），5表示无插播放失败，6表示有插成功，7表示无插成功，8有插拖动成功，9无插拖动成功
		//10,11,12,13表示有http连接有重试过。10表示有插成功，11表示无插成功，12有插拖动成功，13无插拖动成功。
		private var _m:int;
		
		//URL（string），当前成功或者失败的url
		private var _url:String;
		
		//成功或者失败的耗时(毫秒)
		//第一次播放或者拖动之后，按照bufferfull为条件来抛出事件，正常播放到下一段的时候，以收到新段的第一个数据为条件来抛出事件。
		private var _interval:uint;
		
		public function PlayResultEvent(m:int,url:String,interval:uint)
		{
			super(PLAY_RESULT);
			_m = m;
			_url = url;
			_interval = interval;
		}
		
		public function get m():int {return _m;}
		public function get url():String {return _url;}
		public function get interval():uint {return _interval;}
	}
}