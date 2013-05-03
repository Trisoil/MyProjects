package com.pplive.p2p.mp4
{
	import com.pplive.p2p.mp4.Mp4Header;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.utils.ByteArray;
	
	import org.flexunit.Assert;
	import org.flexunit.async.Async;

	public class Mp4HeaderTest
	{
		private var urlStream:URLStream;
		private var mp4Header:Mp4Header;
		
		public function Mp4HeaderTest()
		{
		}
		
		[Before(async)]
		public function setup():void
		{
			urlStream = new URLStream;
			mp4Header = new Mp4Header;
			
			// relative path to directory where swf file locate.
			var request:URLRequest = new URLRequest("../../../TestResource/header.bin");
			var asyncCompleteHandler:Function = Async.asyncHandler( this, onComplete, 500, null, onTimeOut );
			urlStream.addEventListener(Event.COMPLETE, asyncCompleteHandler);
			urlStream.addEventListener(IOErrorEvent.IO_ERROR, onIOError, false, 0, true);
			urlStream.load(request);
		}
		
		[After]
		public function teardown():void
		{
			urlStream = null;
			mp4Header = null;
		}
		
		public function onComplete(event:Event, passThroughData:Object):void
		{
			trace("onComplete bytesAvailable:" + urlStream.bytesAvailable);
			while(urlStream.bytesAvailable > 0)
			{
				var bytes:ByteArray = new ByteArray;
				if (urlStream.bytesAvailable > 1024)
					urlStream.readBytes(bytes, 0, 1024);
				else
					urlStream.readBytes(bytes);
				Assert.assertEquals(0, bytes.position);
				mp4Header.appendBytes(bytes);
			}
		}
		
		[Test]
		public function testBasicInfo():void
		{
			Assert.assertTrue(mp4Header.isComplete());
			Assert.assertFalse(mp4Header.hasError());
			Assert.assertEquals(20285, mp4Header.getSampleCount());
			Assert.assertEquals(302458, mp4Header.getDurationInMs());
			// getSample: test first 8 sample 
			var sample:Object;
			sample = mp4Header.getSample(0);
			Assert.assertEquals(Mp4Header.AUDIO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(0, sample.index);
			sample = mp4Header.getSample(1);
			Assert.assertEquals(Mp4Header.VIDEO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(0, sample.index);
			sample = mp4Header.getSample(2);
			Assert.assertEquals(Mp4Header.AUDIO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(1, sample.index);
			sample = mp4Header.getSample(3);
			Assert.assertEquals(Mp4Header.AUDIO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(2, sample.index);
			sample = mp4Header.getSample(4);
			Assert.assertEquals(Mp4Header.VIDEO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(1, sample.index);
			sample = mp4Header.getSample(5);
			Assert.assertEquals(Mp4Header.AUDIO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(3, sample.index);
			sample = mp4Header.getSample(6);
			Assert.assertEquals(Mp4Header.AUDIO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(4, sample.index);
			sample = mp4Header.getSample(7);
			Assert.assertEquals(Mp4Header.VIDEO_SAMPLE_TYPE, sample.type);
			Assert.assertEquals(2, sample.index);
			
			Assert.assertEquals(0, mp4Header.calcVideoTimeStamp(0));
			Assert.assertEquals(41, mp4Header.calcVideoTimeStamp(1));
			Assert.assertEquals(666, mp4Header.calcVideoTimeStamp(16));
			Assert.assertEquals(5125, mp4Header.calcVideoTimeStamp(123));
			Assert.assertEquals(56250, mp4Header.calcVideoTimeStamp(1350));
			Assert.assertEquals(145041, mp4Header.calcVideoTimeStamp(3481));
			Assert.assertEquals(291708, mp4Header.calcVideoTimeStamp(7001));
			
			Assert.assertEquals(0, mp4Header.getVideoSampleTime(0));
			Assert.assertEquals(0.041, mp4Header.getVideoSampleTime(1));
			Assert.assertEquals(0.666, mp4Header.getVideoSampleTime(16));
			Assert.assertEquals(5.125, mp4Header.getVideoSampleTime(123));
			Assert.assertEquals(56.250, mp4Header.getVideoSampleTime(1350));
			Assert.assertEquals(145.041, mp4Header.getVideoSampleTime(3481));
			Assert.assertEquals(291.708, mp4Header.getVideoSampleTime(7001));
			
			Assert.assertEquals(0, mp4Header.calcAudioTimeStamp(0));
			Assert.assertEquals(23, mp4Header.calcAudioTimeStamp(1));
			Assert.assertEquals(371, mp4Header.calcAudioTimeStamp(16));
			Assert.assertEquals(2856, mp4Header.calcAudioTimeStamp(123));
			Assert.assertEquals(31346, mp4Header.calcAudioTimeStamp(1350));
			Assert.assertEquals(80828, mp4Header.calcAudioTimeStamp(3481));
			Assert.assertEquals(162562, mp4Header.calcAudioTimeStamp(7001));
			Assert.assertEquals(302439, mp4Header.calcAudioTimeStamp(13025));
			
			Assert.assertEquals(201775, mp4Header.getSampleOffset(Mp4Header.AUDIO_SAMPLE_TYPE, 0));
			Assert.assertEquals(230689, mp4Header.getSampleOffset(Mp4Header.AUDIO_SAMPLE_TYPE, 1));
			Assert.assertEquals(236594, mp4Header.getSampleOffset(Mp4Header.AUDIO_SAMPLE_TYPE, 5));
			Assert.assertEquals(21238621, mp4Header.getSampleOffset(Mp4Header.AUDIO_SAMPLE_TYPE, 13025));
			
			Assert.assertEquals(201949, mp4Header.getSampleOffset(Mp4Header.VIDEO_SAMPLE_TYPE, 0));
			Assert.assertEquals(231033, mp4Header.getSampleOffset(Mp4Header.VIDEO_SAMPLE_TYPE, 1));
			Assert.assertEquals(243291, mp4Header.getSampleOffset(Mp4Header.VIDEO_SAMPLE_TYPE, 5));
			Assert.assertEquals(21238020, mp4Header.getSampleOffset(Mp4Header.VIDEO_SAMPLE_TYPE, 7258));
			
			Assert.assertEquals(174, mp4Header.getSampleSize(Mp4Header.AUDIO_SAMPLE_TYPE, 0));
			Assert.assertEquals(172, mp4Header.getSampleSize(Mp4Header.AUDIO_SAMPLE_TYPE, 1));
			Assert.assertEquals(144, mp4Header.getSampleSize(Mp4Header.AUDIO_SAMPLE_TYPE, 5));
			Assert.assertEquals(150, mp4Header.getSampleSize(Mp4Header.AUDIO_SAMPLE_TYPE, 13025));
			
			Assert.assertEquals(28740, mp4Header.getSampleSize(Mp4Header.VIDEO_SAMPLE_TYPE, 0));
			Assert.assertEquals(3681, mp4Header.getSampleSize(Mp4Header.VIDEO_SAMPLE_TYPE, 1));
			Assert.assertEquals(4417, mp4Header.getSampleSize(Mp4Header.VIDEO_SAMPLE_TYPE, 5));
			Assert.assertEquals(601, mp4Header.getSampleSize(Mp4Header.VIDEO_SAMPLE_TYPE, 7258));
			
			Assert.assertFalse(mp4Header.isVideoSyncSample(115));
			Assert.assertTrue(mp4Header.isVideoSyncSample(116));
			Assert.assertFalse(mp4Header.isVideoSyncSample(117));
			Assert.assertTrue(mp4Header.isVideoSyncSample(2059));
			Assert.assertFalse(mp4Header.isVideoSyncSample(2060));
			Assert.assertTrue(mp4Header.isVideoSyncSample(4752));
			Assert.assertFalse(mp4Header.isVideoSyncSample(4753));
			Assert.assertTrue(mp4Header.isVideoSyncSample(6884));
			Assert.assertFalse(mp4Header.isVideoSyncSample(6885));
			
			Assert.assertEquals(41, mp4Header.getVideoSampleCompositionTimeInMs(0));
			Assert.assertEquals(83, mp4Header.getVideoSampleCompositionTimeInMs(1));
			Assert.assertEquals(0, mp4Header.getVideoSampleCompositionTimeInMs(2));
			Assert.assertEquals(41, mp4Header.getVideoSampleCompositionTimeInMs(7256));
			Assert.assertEquals(83, mp4Header.getVideoSampleCompositionTimeInMs(7257));
			Assert.assertEquals(0, mp4Header.getVideoSampleCompositionTimeInMs(7258));
			
			var AVCDecoderConfiguration:ByteArray = mp4Header.getAVCDecoderConfigurationRecord();
			Assert.assertEquals(40, AVCDecoderConfiguration.length);
			Assert.assertEquals(0x01, AVCDecoderConfiguration[0]);
			Assert.assertEquals(0x64, AVCDecoderConfiguration[1]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[2]);
			Assert.assertEquals(0x15, AVCDecoderConfiguration[3]);
			Assert.assertEquals(0xFF, AVCDecoderConfiguration[4]);
			Assert.assertEquals(0xE1, AVCDecoderConfiguration[5]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[6]);
			Assert.assertEquals(0x19, AVCDecoderConfiguration[7]);
			Assert.assertEquals(0x67, AVCDecoderConfiguration[8]);
			Assert.assertEquals(0x64, AVCDecoderConfiguration[9]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[10]);
			Assert.assertEquals(0x15, AVCDecoderConfiguration[11]);
			Assert.assertEquals(0xAC, AVCDecoderConfiguration[12]);
			Assert.assertEquals(0xD9, AVCDecoderConfiguration[13]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[14]);
			Assert.assertEquals(0xA0, AVCDecoderConfiguration[15]);
			Assert.assertEquals(0x23, AVCDecoderConfiguration[16]);
			Assert.assertEquals(0xB0, AVCDecoderConfiguration[17]);
			Assert.assertEquals(0x11, AVCDecoderConfiguration[18]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[19]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[20]);
			Assert.assertEquals(0x03, AVCDecoderConfiguration[21]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[22]);
			Assert.assertEquals(0x01, AVCDecoderConfiguration[23]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[24]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[25]);
			Assert.assertEquals(0x03, AVCDecoderConfiguration[26]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[27]);
			Assert.assertEquals(0x30, AVCDecoderConfiguration[28]);
			Assert.assertEquals(0x8F, AVCDecoderConfiguration[29]);
			Assert.assertEquals(0x16, AVCDecoderConfiguration[30]);
			Assert.assertEquals(0x2D, AVCDecoderConfiguration[31]);
			Assert.assertEquals(0x12, AVCDecoderConfiguration[32]);
			Assert.assertEquals(0x01, AVCDecoderConfiguration[33]);
			Assert.assertEquals(0x00, AVCDecoderConfiguration[34]);
			Assert.assertEquals(0x04, AVCDecoderConfiguration[35]);
			Assert.assertEquals(0x68, AVCDecoderConfiguration[36]);
			Assert.assertEquals(0xEB, AVCDecoderConfiguration[37]);
			Assert.assertEquals(0xEF, AVCDecoderConfiguration[38]);
			Assert.assertEquals(0x2C, AVCDecoderConfiguration[39]);
			
			var AudioSpecificConfig:ByteArray = mp4Header.getAudioSpecificConfig();
			Assert.assertEquals(2, AudioSpecificConfig.length);
			Assert.assertEquals(0x12, AudioSpecificConfig[0]);
			Assert.assertEquals(0x10, AudioSpecificConfig[1]);
		}
		
		[Test]
		public function testSeekInfo():void
		{
			// 测试和拖动相关的接口
			Assert.assertEquals(0, mp4Header.getVideoSampleIndexFromTime(0));
			Assert.assertEquals(0, mp4Header.getVideoSampleIndexFromTime(1));
			Assert.assertEquals(116, mp4Header.getVideoSampleIndexFromTime(4));
			Assert.assertEquals(653, mp4Header.getVideoSampleIndexFromTime(28));
			Assert.assertEquals(693, mp4Header.getVideoSampleIndexFromTime(29));
			Assert.assertEquals(7210, mp4Header.getVideoSampleIndexFromTime(301));
			
			Assert.assertEquals(0, mp4Header.getAudioSampleIndexFromVideoSampleIndex(0));
			Assert.assertEquals(1, mp4Header.getAudioSampleIndexFromVideoSampleIndex(1));
			Assert.assertEquals(7, mp4Header.getAudioSampleIndexFromVideoSampleIndex(4));
			Assert.assertEquals(50, mp4Header.getAudioSampleIndexFromVideoSampleIndex(28));
			Assert.assertEquals(52, mp4Header.getAudioSampleIndexFromVideoSampleIndex(29));
			Assert.assertEquals(540, mp4Header.getAudioSampleIndexFromVideoSampleIndex(301));
		}
		
		public function onTimeOut(passThroughData:Object):void
		{
			trace("onTimeOut");
			Assert.fail("Timeout reached before event");
		}
		
		public function onIOError(event:IOErrorEvent):void
		{
			trace("onIOError " + event);
			Assert.fail("onIOError");
		}
	}
}