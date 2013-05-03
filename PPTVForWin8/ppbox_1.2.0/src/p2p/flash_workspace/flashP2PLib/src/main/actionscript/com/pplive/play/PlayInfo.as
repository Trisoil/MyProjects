package com.pplive.play
{
	public class PlayInfo
	{
		private var _host:String;
		private var _fileName:String;
		private var _key:String;
		private var _bwType:uint;
		private var _segments:Vector.<SegmentInfo>;
		private var _backupHostVector:Vector.<String>;
		private var _duration:uint;
		private var _isVip:Boolean;
		
		public function PlayInfo(host:String, fileName:String, key:String, bwType:uint
								 , segments:Vector.<SegmentInfo>, backupHostVector:Vector.<String>,isVip:Boolean=false)
		{
			_host = host;
			_fileName = fileName;
			_key = key;
			_bwType = bwType;
			_segments = segments;
			_backupHostVector = backupHostVector;
			_isVip = isVip;
			calcDuration();
		}
		
		private function calcDuration():void
		{
			var totalDuration:Number = 0;
			var segmentsLength:uint = segments.length; 
			for (var i:uint = 0; i < segmentsLength; ++i)
			{
				totalDuration += segments[i].duration;	
			}
			_duration = totalDuration;
		}
		
		public function constructUrl(segmentIndex:uint):String
		{
			var baseUrl:String = "http://" + host + "/" + segmentIndex + "/" + fileName + "?key=" + key;
			//var baseUrl:String = "http://" + host + ":8080/" + segmentIndex + "/" + fileName + "?key=" + key;			
			//var baseUrl:String = "http://" + host + ":8081/" + segmentIndex + "/" + fileName + "?key=" + key;
			return baseUrl;
		}
		
		public function get host():String { return _host;}
		public function get fileName():String { return _fileName;}
		public function get key():String { return _key;}
		public function get bwType():uint { return _bwType;}
		public function get segments():Vector.<SegmentInfo> { return _segments;}
		public function get backupHostVector():Vector.<String> { return _backupHostVector;}
		public function get duration():uint {return _duration;}
		public function get isVip():Boolean {return _isVip;}
		
	}
}