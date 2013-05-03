package com.pplive.p2p.mp4
{
	import com.pplive.p2p.events.SegmentCompleteEvent;
	import com.pplive.util.EventUtil;
	
	import de.polygonal.ds.ArrayUtil;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.ProgressEvent;
	import flash.utils.ByteArray;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;

	public class Mp4Stream extends EventDispatcher
	{
		private static var logger:ILogger = getLogger(Mp4Stream);
		
		private var hasError:Boolean = false;
		private var inputBuffer:ByteArray = new ByteArray;
		private var outputBuffer:ByteArray = new ByteArray;
		private var outputBufferPosition:uint;
		private var inputDataOffset:uint = 0;
		
		private var header:Mp4Header;
		private var hasMdatBoxHeader:Boolean = false;
		private var mdatBoxSize:uint;
		
		private var currentSample:Object;
		private var currentSampleIndex:uint = 0;
		private var currentSampleOffset:uint = 0;
		private var audioSyncSample:uint = 0;
		
		private var isSeek:Boolean = false;
		private var hasSendFlvHeader:Boolean = false;
		private var baseVideoTimeStamp:uint = 0;
		private var baseAudioTimeStamp:uint = 0;
		
		static private const AVC_SEQUENCE_HEADER_PACKET:uint = 0;
		static private const AVC_NALU_PACKET:uint = 1;
		static private const AAC_SEQUENCE_HEADER_PACKET:uint = 0;
		static private const AAC_RAW_PACKET:uint = 1;
		
		public function Mp4Stream()
		{
			header = new Mp4Header;
		}
		
		public function destory():void
		{
			inputBuffer = null;
			outputBuffer = null;
			header.destory();
			header = null;
			currentSample = null;
		}
		
		public function appendBytes(bytes:ByteArray):void
		{
			logger.debug("appendBytes " + bytes.length + " bytes");
			if (!header.isComplete())
				header.appendBytes(bytes);
			
			if (header.hasError())
			{
				//TODO(herain):need dispatch error event when mp4 header has error.
				logger.error("header has Error!!!");
				return;
			}
			
			if (!isSeek && !hasMdatBoxHeader && bytes.bytesAvailable != 0)
			{
				readAtMost(bytes, inputBuffer, inputBuffer.length, 8-inputBuffer.length);				
				if (inputBuffer.length == 8)
				{
					inputBuffer.position = 0;
					mdatBoxSize = inputBuffer.readUnsignedInt();
					hasMdatBoxHeader = true;
					inputBuffer.clear();
				}
			}
			
			while (bytes.bytesAvailable != 0)
			{
				readOneSample(bytes);
			}
			
			inputDataOffset += bytes.length;
		}
		
		public function readBytes():ByteArray
		{
			var outputBytes:ByteArray = outputBuffer;
			outputBytes.position = 0;
			outputBuffer = new ByteArray;
			return outputBytes;
		}
		
		public function setBaseTimeStamp(event:SegmentCompleteEvent):void
		{
			this.baseVideoTimeStamp = event.videoTimeStamp;
			this.baseAudioTimeStamp = event.audioTimeStamp;
			
			// BaseTimeStamp not 0 indicate that it is change segment smoothly. Hence, flv header can't be send.
			hasSendFlvHeader = true;
		}
		
		public function hasMp4Header():Boolean
		{
			return header.isComplete();
		}
		
		public function getRealSeekTime(start:uint):Number
		{
			return header.getVideoSampleTime(header.getVideoSampleIndexFromTime(start));
		}
		
		public function seek(start:uint):uint
		{
			// can't call seek before hasMp4Header return true.			
			inputBuffer.clear();
			outputBuffer.clear();
			hasSendFlvHeader = false;
			currentSample = null;
			isSeek = true;
			
			// find video sync sample around start second
			var seekVideoSampleIndex:uint = header.getVideoSampleIndexFromTime(start);
			
			// find audio sample corresponding to video sync sample
			var seekAudioSampleIndex:int = header.getAudioSampleIndexFromVideoSampleIndex(seekVideoSampleIndex);
			audioSyncSample = seekAudioSampleIndex;
			
			var videoSampleOffset:uint = header.getSampleOffset(Mp4Header.VIDEO_SAMPLE_TYPE, seekVideoSampleIndex);
			var audioSampleOffset:uint = header.getSampleOffset(Mp4Header.AUDIO_SAMPLE_TYPE, seekAudioSampleIndex); 
			if (videoSampleOffset < audioSampleOffset)
			{
				// Video seek sample is before audio seek sample.
				// We need to find the first audio sample before the video seek sample since
				// we need the total sample index of the video seek sample.
				// Read the Example in line 110 for more detail.
				while (audioSampleOffset > videoSampleOffset && seekAudioSampleIndex > 0)
				{
					--seekAudioSampleIndex;
					audioSampleOffset = header.getSampleOffset(Mp4Header.AUDIO_SAMPLE_TYPE, seekAudioSampleIndex);
				}
				
				if (seekAudioSampleIndex == 0)
				{
					currentSampleIndex = seekVideoSampleIndex;
				}
				else
				{
					currentSampleIndex = seekVideoSampleIndex + seekAudioSampleIndex + 1;
				}
				
				inputDataOffset = videoSampleOffset;
				return videoSampleOffset;
			}
			else
			{
				// Audio seek sample is before video seek sample.
				// We need to find the first video sample before the audio seek sample since
				// we need the total sample index of the audio seek sample.
				// Example: 
				// 		video seek sample is video sample 116 and offset is A5F04
				// 		audio seek sample is audio sample 208 and offset is A5D8B
				// 		audio seek sample is before video seek sample
				//		we find the first video sample before audio seek sample is 
				//		video sample 115.
				//		so we get the total sample index of audio seek sample is 115+208+1=324
				while (videoSampleOffset > audioSampleOffset && seekVideoSampleIndex > 0)
				{
					--seekVideoSampleIndex;
					videoSampleOffset = header.getSampleOffset(Mp4Header.VIDEO_SAMPLE_TYPE, seekVideoSampleIndex);
				}
				
				if (seekVideoSampleIndex == 0)
				{
					currentSampleIndex = seekAudioSampleIndex;
				}
				else
				{
					currentSampleIndex = seekVideoSampleIndex + seekAudioSampleIndex + 1;
				}
				
				inputDataOffset = audioSampleOffset;
				return audioSampleOffset;
			}
		}
		
		private function readOneSample(bytes:ByteArray):void
		{
			if (!hasSendFlvHeader)
			{
				assembleFlvHeader();
				hasSendFlvHeader = true;
			}
			
			if (currentSampleIndex < header.getSampleCount())
			{
				if (!currentSample)
				{
					currentSample = header.getSample(currentSampleIndex);
					currentSampleOffset = header.getSampleOffset(currentSample.type, currentSample.index);
				}

				var bytesOffset:uint = inputDataOffset + bytes.position;
				if (bytesOffset < currentSampleOffset)
				{
					if (bytesOffset + bytes.bytesAvailable <= currentSampleOffset)
					{
						logger.debug("throw rubbish data " + bytes.bytesAvailable + " bytes");
						bytes.position = bytes.length;
						return;
					}
					else
					{
						logger.debug("throw rubbish data " + (currentSampleOffset - bytesOffset) + " bytes");
						bytes.position += currentSampleOffset - bytesOffset;
					}
				}

				var sampleSize:uint = header.getSampleSize(currentSample.type, currentSample.index);
				readAtMost(bytes, inputBuffer, inputBuffer.length, sampleSize - inputBuffer.length);
				
				if (inputBuffer.length == sampleSize)
				{
					if (currentSample.type == Mp4Header.VIDEO_SAMPLE_TYPE)
						assembleVideoTag();
					else
						assembleAudioTag();
					
					inputBuffer.clear();
					currentSample = null;
					dispatchEvent(new ProgressEvent(ProgressEvent.PROGRESS, false, false));
					logger.debug("send sample " + currentSampleIndex + "/" + header.getSampleCount());
					
					++currentSampleIndex;
					if (currentSampleIndex == header.getSampleCount())
					{
						reportSegmentCompelete();
					}
				}
			}
			else
			{
				// all sample has been read already, ignore redundant bytes
				bytes.position = bytes.length;
			}
		}
		
		private function reportSegmentCompelete():void
		{
			var event:SegmentCompleteEvent = new SegmentCompleteEvent(SegmentCompleteEvent.SEGMENT_COMPLETE
				, baseVideoTimeStamp + header.getDurationInMs(), baseAudioTimeStamp + header.getDurationInMs());
			logger.info("moive complete! timestamp: " + event.videoTimeStamp + ":" + event.audioTimeStamp);
			EventUtil.deferDispatch(this, event, 50);
		}
		
		private function saveOutputPosition():void
		{
			outputBufferPosition = outputBuffer.position;
			outputBuffer.position = outputBuffer.length;
		}
		
		private function restoreOutputPosition():void
		{
			outputBuffer.position = outputBufferPosition;
		}
		
		private function assembleFlvHeader():void
		{
			logger.info("assembleFlvHeader");
			saveOutputPosition();
			outputBuffer.writeByte(70);
			outputBuffer.writeByte(76);
			outputBuffer.writeByte(86);
			outputBuffer.writeByte(1);
			outputBuffer.writeByte(5);
			outputBuffer.writeInt(9);
			outputBuffer.writeInt(0);
			restoreOutputPosition();
		}
		
		private function assembleVideoTag():void
		{
			var isSyncSample:Boolean = header.isVideoSyncSample(currentSample.index);
			var timeStamp:uint = baseVideoTimeStamp + header.calcVideoTimeStamp(currentSample.index);		
			if (isSyncSample)
			{
				//VideoHeaderTag
				assembleFlvTag(9, getAvcTagHeader(true, AVC_SEQUENCE_HEADER_PACKET, 0
					, header.getAVCDecoderConfigurationRecord()), timeStamp);
			}
			
			logger.debug("video sample " + currentSample.index + " ready, timestamp:" + timeStamp + "ms.");
			assembleFlvTag(9, getAvcTagHeader(isSyncSample, AVC_NALU_PACKET
				, header.getVideoSampleCompositionTimeInMs(currentSample.index), inputBuffer), timeStamp);
		}
		
		private function getAvcTagHeader(isSync:Boolean, type:uint, compositionTime:uint
										 , buffer:ByteArray):ByteArray
		{
			var avcHeader:ByteArray = new ByteArray();
			
			// FrameType=1, indicated key frame(for AVC, a seekable frame)
			// CodecID=7, indicated AVC
			if (isSync)
				avcHeader.writeByte(0x17);
			else
				avcHeader.writeByte(0x27);
			
			// AVCPacketType: AVC_SEQUENCE_HEADER
			avcHeader.writeByte(type);
			
			// Composite Type: 0
			avcHeader.writeByte(compositionTime >> 16);
			avcHeader.writeByte(compositionTime >> 8);
			avcHeader.writeByte(compositionTime);
			
			avcHeader.writeBytes(buffer);
			return avcHeader;
		}
		
		private function assembleAudioTag():void
		{	
			var timeStamp:uint = baseAudioTimeStamp + header.calcAudioTimeStamp(currentSample.index);
			if (currentSample.index == audioSyncSample)
			{
				//AudioHeaderTag
				assembleFlvTag(8, getAacTagHeader(AAC_SEQUENCE_HEADER_PACKET
					, header.getAudioSpecificConfig()), timeStamp);
			}
			
			logger.debug("audio sample " + currentSample.index + " ready, timestamp:" + timeStamp + "ms.");
			assembleFlvTag(8, getAacTagHeader(AAC_RAW_PACKET, inputBuffer), timeStamp);
		}
		
		private function getAacTagHeader(type:uint, buffer:ByteArray):ByteArray
		{
			var aac_header:ByteArray = new ByteArray();
			
			// abobe_flash_video_file_format_spec_v10_1:P71
			//If the SoundFormat indicates AAC, the SoundType should be 1 (stereo) and the SoundRate should  be 3 (44 kHz). 
			//However, this does not mean that AAC audio in FLV is always stereo, 44 kHz data. Instead, the Flash Player ignores 
			//these values and extracts the channel and sample rate data is encoded in the AAC bit stream.
			aac_header.writeByte((10 << 4) | (3 << 2) | 0x02 | 1);
			aac_header.writeByte(type);
			aac_header.writeBytes(buffer);
			return aac_header;
		}
		
		private function assembleFlvTag(type:uint, body:ByteArray, timeStamp:uint):void
		{			
			saveOutputPosition();
			
			// Reserved, filter, TagType
			outputBuffer.writeByte(type);
			
			// DataSize
			outputBuffer.writeByte(body.length >> 16);
			outputBuffer.writeByte(body.length >> 8);
			outputBuffer.writeByte(body.length >> 0);
			
			// TimeStamp, TimeStampExtended
			outputBuffer.writeByte(timeStamp >> 16);
			outputBuffer.writeByte(timeStamp >> 8);
			outputBuffer.writeByte(timeStamp >> 0);
			outputBuffer.writeByte(timeStamp >> 24);
			
			// StreamID
			outputBuffer.writeByte(0);
			outputBuffer.writeByte(0);
			outputBuffer.writeByte(0);
			
			// AudioTagHeader or VideoTagHeader + AudioData or VideoData 
			outputBuffer.writeBytes(body);
			
			// TagSize
			outputBuffer.writeUnsignedInt(body.length + 11);
			
			restoreOutputPosition();
			return;
		}
		
		private function readAtMost(srcBytes:ByteArray, destBytes:ByteArray, destOffset:uint, maxLength:uint):void
		{
			if (srcBytes.bytesAvailable > maxLength)
			{
				srcBytes.readBytes(destBytes, destOffset, maxLength);
			}
			else
			{
				srcBytes.readBytes(destBytes, destOffset);
			}
		}
	}
}