package
{
	//import com.pplive.events.PieceCompleteEvent;
	import com.pplive.util.EventUtil;
	import com.pplive.util.StringConvert;
	
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
		
		private var _sessionID:String;
		
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
		/* DEBUGSTR */
		public var enableDebugStr:Boolean = true;
		private var earlyTermination:int = 100000;
		public var debugstr:String;
		
		static private const AVC_SEQUENCE_HEADER_PACKET:uint = 0;
		static private const AVC_NALU_PACKET:uint = 1;
		static private const AAC_SEQUENCE_HEADER_PACKET:uint = 0;
		static private const AAC_RAW_PACKET:uint = 1;
		
		static private const ENC_AU_HEADER_INBODY17:uint = 17;
		static private const ENC_AU_HEADER_INBODY1:uint = 1;
		static private const ENC_AU_HEADER_NONE:uint = 2;
		
		public function Mp4Stream(sessionID:String)
		{
			header = new Mp4Header;
			_sessionID = sessionID;
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
			if (!header.isComplete())
				header.appendBytes(bytes);
			
			if (header.hasError())
			{
				//TODO(herain):need dispatch error event when mp4 header has error.
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
		
		public function readBytes(bytes:ByteArray):void
		{
			outputBuffer.readBytes(bytes);
			
			outputBuffer.position = 0;
			if (enableDebugStr) {
				if (debugstr == null) {
					debugstr = new String(StringConvert.byteArray2HexString(outputBuffer));
				}else{
					debugstr += StringConvert.byteArray2HexString(outputBuffer);
				}
			}
			outputBuffer.clear();
		}
		
		public function setBaseTimeStamp(event:SegmentCompleteEvent):void
		{
			this.baseVideoTimeStamp = event.videoTimeStamp;
			this.baseAudioTimeStamp = event.audioTimeStamp;
			
			// BaseTimeStamp not 0 indicate that it is change piece smoothly. Hence, flv header can't be send.
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
				logger.info("send FlvHeader in readOneSample");
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
						logger.info("throw rubbish data " + bytes.bytesAvailable + " bytes");
						bytes.position = bytes.length;
						return;
					}
					else
					{
						logger.info("throw rubbish data " + (currentSampleOffset - bytesOffset) + " bytes");
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
					logger.info("send sample " + currentSampleIndex + "/" + header.getSampleCount());
					
					if (currentSampleIndex >=49) {
						logger.info("check");
					}
					
					/*DEBUG for Binary Interception */
					if (enableDebugStr) {
						if (currentSampleIndex > earlyTermination) {
							logger.info(debugstr);
							//the following code cause the player to crash.
							var er:ByteArray;
							logger.info(er.length);
						}
					}
					
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
			
			assembleFlvTag(18, getEncryptionHeader(header.adrmMetadata), null, 0, ENC_AU_HEADER_NONE);
			assembleFlvTag(18, header.adrmMetaHeader.getMetaHeaderArray(), null, 0, ENC_AU_HEADER_NONE);
		}
		
		private function getEncryptionHeader(meta:String):ByteArray
		{
			return header.adrmFlvHeader.getEncryptedHeaderArray();
		}
		
		private function assembleVideoTag():void
		{
			var isSyncSample:Boolean = header.isVideoSyncSample(currentSample.index);
			var timeStamp:uint = baseVideoTimeStamp + header.calcVideoTimeStamp(currentSample.index);
			var auHeader:uint = inputBuffer.readUnsignedByte();
			var isEncrypted:Boolean = (auHeader == 128);
			var IV:ByteArray = new ByteArray;
			var type:uint = (9+32);	//encrypted Video. 
			if (isEncrypted) {
				//this tag is encrypted.
				inputBuffer.readBytes(IV, 0, 16);
			}
			if (isSyncSample)
			{
				//VideoHeaderTag
				assembleFlvTag(type, getAvcTagHeader(true, AVC_SEQUENCE_HEADER_PACKET, 0),
						header.getAVCDecoderConfigurationRecord(), timeStamp, ENC_AU_HEADER_NONE);
			}
			
			logger.info("video sample " + currentSample.index + " ready, timestamp:" + timeStamp + "ms.");
			// type (if Encrypted) combined with ENC_AU_HEADER_INBODY enabled us to tell the difference
			//   assemble a normal video/audio tag or SCRIPTDATA header (since SCRIPTDATA is not encrypted).
			if (isEncrypted) {
				assembleFlvTag(type, getAvcTagHeader(isSyncSample, AVC_NALU_PACKET
					, header.getVideoSampleCompositionTimeInMs(currentSample.index)), inputBuffer, timeStamp, ENC_AU_HEADER_INBODY17);
			}else{
				assembleFlvTag(type, getAvcTagHeader(isSyncSample, AVC_NALU_PACKET
					, header.getVideoSampleCompositionTimeInMs(currentSample.index)), inputBuffer, timeStamp, ENC_AU_HEADER_INBODY1);				
			}
			inputBuffer.clear();
		}
		
		private function getAvcTagHeader(isSync:Boolean, type:uint, compositionTime:uint):ByteArray
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
			logger.info("comptime: " + compositionTime);
			avcHeader.writeByte(compositionTime >> 16);
			avcHeader.writeByte(compositionTime >> 8);
			avcHeader.writeByte(compositionTime);
			
			return avcHeader;
		}
		
		private function assembleAudioTag():void
		{	
			var timeStamp:uint = baseAudioTimeStamp + header.calcAudioTimeStamp(currentSample.index);
			var auHeader:uint = inputBuffer.readUnsignedByte();
			var isEncrypted:Boolean = (auHeader == 128);
			var IV:ByteArray = new ByteArray;
			var type:uint = (8+32);	//encrypted Video.
			if (isEncrypted) {
				//this tag is encrypted.
				inputBuffer.readBytes(IV, 0, 16);
			}
			if (currentSample.index == audioSyncSample)
			{
				//AudioHeaderTag
				assembleFlvTag(type, getAacTagHeader(AAC_SEQUENCE_HEADER_PACKET),
					header.getAudioSpecificConfig(), timeStamp, ENC_AU_HEADER_NONE);
			}
			
			logger.info("audio sample " + currentSample.index + " ready, timestamp:" + timeStamp + "ms.");
			if (isEncrypted) {
				assembleFlvTag(type, getAacTagHeader(AAC_RAW_PACKET), inputBuffer, timeStamp, ENC_AU_HEADER_INBODY17);
			}else{
				assembleFlvTag(type, getAacTagHeader(AAC_RAW_PACKET), inputBuffer, timeStamp, ENC_AU_HEADER_INBODY1);				
			}
			inputBuffer.clear();
		}
		
		private function getAacTagHeader(type:uint):ByteArray
		{
			var aac_header:ByteArray = new ByteArray();
			// AudioData
			//   SoundFormat: 10 AAC
			//   SoundRate: 3 44kHz
			//   SoundSize: 1 16-bit samples
			//   SoundType: 1 Stereo sound
			aac_header.writeByte(0xAF);
			aac_header.writeByte(type);
			return aac_header;
		}
		
		private function assembleFlvTag(type:uint, header:ByteArray, body:ByteArray, timeStamp:uint, sEnc:uint):void
		{			
			saveOutputPosition();
			
			// Reserved, filter, TagType
			outputBuffer.writeByte(type);
			
			var length:uint = header.length;
			var lengthH:uint = length;
			var lengthIV:uint = 0;
			var lengthB:uint = 0;
			var isEnc:Boolean = ((type == 41) || (type == 40));
			if (isEnc) {
				/*
				if (IV == null) {
					length += (1+3+3+1);
				}else{
					length += (1+3+3+17);
				}*/
				if ( (sEnc == ENC_AU_HEADER_INBODY17) || (sEnc == ENC_AU_HEADER_INBODY1)) {
					//the Encrypted Tag Header is fixed length without the following SelectiveEncParams
					length += 7;
				}else{
					length += 8; 	//we have another BYTE to simulate the NotEncryted.
				}
				lengthIV = length - lengthH;
			}
			if (body != null) {
				//the body here already contains the AU header. i.e., the SelectiveEncParams.
				length += body.length;
				lengthB = body.length;
			}
			// DataSize
			outputBuffer.writeByte(length >> 16);
			outputBuffer.writeByte(length >> 8);
			outputBuffer.writeByte(length >> 0);
			
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
			outputBuffer.writeBytes(header);
			
			//now we need to attach the EncryptionHeader and FilterParams if necessary.
			if ( isEnc ) {
				outputBuffer.writeByte(1);
				//outputBuffer.writeShort(2);
				outputBuffer.writeUTFBytes("SE");
				outputBuffer.writeByte(0);
				outputBuffer.writeShort(0);
				if (sEnc == ENC_AU_HEADER_INBODY17) {
					outputBuffer.writeByte(17);
					// we don't need to write the SelectiveEncryptionFilterParams, 
					//   as it is the same as the AU Header from encrypted f4v. 
					//outputBuffer.writeByte(128);
					//outputBuffer.writeBytes(IV, 0, 16);
				}else if (sEnc == ENC_AU_HEADER_INBODY1) {
					outputBuffer.writeByte(1);
					// same as above: we don't need to write the SelectiveEncryptionFilterParams, 
					//   as it is the same as the AU Header from encrypted f4v. 
					//outputBuffer.writeByte(0);
				}else if (sEnc == ENC_AU_HEADER_NONE) {
					// however, if we are writing the SYNC tag or Encrypted Header. we do need another Byte.
					outputBuffer.writeByte(1);
					outputBuffer.writeByte(0);
				}else{
					logger.info("Error:::: invalid sEnc Parameter " + sEnc);
					outputBuffer.writeByte(1);
					outputBuffer.writeByte(0);
				}
			} 
			if (body){
				outputBuffer.writeBytes(body);
			}
			
			// TagSize
			outputBuffer.writeUnsignedInt(length + 11);
			
			restoreOutputPosition();
			
			logger.info("tag: " + type + " " + lengthH + "+" + lengthIV + "+" + lengthB + "=" + length + " enc:" + isEnc);
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