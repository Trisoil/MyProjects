package
{
	import de.polygonal.ds.ArrayUtil;
	
	import flash.utils.ByteArray;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	internal class Mp4Header
	{
		private static var logger:ILogger = getLogger(Mp4Header);
		private var _hasError:Boolean = false;
		private var _hasDRMError:Boolean = false;
		
		private var inputBuffer:ByteArray = new ByteArray;
		
		private var hasFtypBox:Boolean = false;
		private var hasMoovBox:Boolean = false;
		private var hasValidAdobeDRM:Boolean = false;
		
		private var trakTimeScale:uint;
		private var trakBoxType:int;
		static private const VIDEO_TRAK_BOX_TYPE:int = 1;
		static private const AUDIO_TRAK_BOX_TYPE:int = 2;
		
		private var videoSampleCount:uint;
		private var videoSampleDeltaArray:Array;
		private var videoSampleSize:Array = new Array;
		private var videoSampleCompositionTime:Array = new Array;
		private var videoSampleOffsetArray:Array = new Array;
		private var videoSyncSample:Array = new Array;
		private var videoChunkInfo:Array = new Array;
		private var videoChunks:Array = new Array;
		private var videoChunkOffset:Array = new Array;
		private var videoTimeScale:uint;
		private var videoDeltaSum:uint;
		private var videoDuration:uint;
		
		private var audioSampleCount:uint;
		private var audioSampleDeltaArray:Array;
		private var audioSampleSize:Array = new Array;
		private var audioSampleOffsetArray:Array = new Array;
		private var audioChunkInfo:Array = new Array;
		private var audioChunks:Array = new Array;
		private var audioChunkOffset:Array = new Array;
		private var audioTimeScale:uint;
		
		private var samples:Array = new Array;
		static public const VIDEO_SAMPLE_TYPE:uint = 1;
		static public const AUDIO_SAMPLE_TYPE:uint = 2;
		
		private var AVCDecoderConfigurationRecord:ByteArray;
		private var AudioSpecificConfig:ByteArray;
		
		//DRM related data
		private var adrmSchemeURI:String;
		private var adrmEncryptionAlgorithm:String;
		private var adrmKeyLength:uint;
		private var adrmFmrmsV3Metadata:String;
		private var adrmSelectiveEncryption:uint;
		public var adrmFlvHeader:FlvEncryptionHeader;
		public var adrmMetaHeader:FlvMetaDataHeader;
		
		public function Mp4Header()
		{
		}
		
		public function get adrmMetadata():String
		{
			return adrmFmrmsV3Metadata;
		}
		
		public function destory():void
		{
			inputBuffer = null;
			videoSampleSize.length = 0;
			videoSampleSize = null;
			videoSampleCompositionTime.length = 0;
			videoSampleCompositionTime = null;
			videoSampleOffsetArray.length = 0;
			videoSampleOffsetArray = null;
			videoSyncSample.length = 0;
			videoSyncSample = null;
			videoChunkInfo.length = 0;
			videoChunkInfo = null;
			videoChunks.length = 0;
			videoChunks = null;
			videoChunkOffset.length = 0;
			videoChunkOffset = null;
			
			audioSampleDeltaArray.length = 0;
			audioSampleDeltaArray = null;
			audioSampleSize.length = 0;
			audioSampleSize = null;
			audioSampleOffsetArray.length = 0;
			audioSampleOffsetArray = null;
			audioChunkInfo.length = 0;
			audioChunkInfo = null;
			audioChunks.length = 0;
			audioChunks = null;
			audioChunkOffset.length = 0;
			audioChunkOffset = null;
			
			samples.length = 0;
			samples = null;
			
			AVCDecoderConfigurationRecord = null;
			AudioSpecificConfig = null;
		}
		
		public function appendBytes(bytes:ByteArray):void
		{
			// bytes.position = 0; bytes
			if (!hasFtypBox && bytes.bytesAvailable != 0)
			{
				var ftypBoxLength:int = bytes.readInt();
				bytes.position += ftypBoxLength - 4;
				hasFtypBox = true;
			}
			
			if (!hasMoovBox && bytes.bytesAvailable != 0)
			{
				if (inputBuffer.length == 0)
				{
					// has no moov data in inputBuffer, write moov_box_length into inBuffer
					bytes.readBytes(inputBuffer, 0, 4);
				}
				
				inputBuffer.position = 0;
				var moovBoxLength:int = inputBuffer.readInt();
				inputBuffer.position = inputBuffer.length;
				
				var read_length:uint = moovBoxLength - inputBuffer.length;
				if (bytes.bytesAvailable > read_length)
				{
					bytes.readBytes(inputBuffer, inputBuffer.position, read_length);
				}
				else
				{
					bytes.readBytes(inputBuffer, inputBuffer.position);
				}
				
				if (inputBuffer.length == moovBoxLength)
				{
					hasMoovBox = true;
					if (!parseMoovBox())
					{
						_hasError = true;
					}
					if (adrmFmrmsV3Metadata == null) {
						_hasError = true;
						logger.info("DRM Header Parse has error");
					}else{
						adrmFlvHeader = new FlvEncryptionHeader(adrmFmrmsV3Metadata);
						adrmMetaHeader = new FlvMetaDataHeader();
					}
				}
			}
		}
		
		public function isComplete():Boolean
		{
			return hasMoovBox && !_hasError;
		}
		
		public function hasError():Boolean
		{
			return _hasError;
		}
		
		public function getSampleCount():uint
		{
			return samples.length;
		}
		
		public function getSample(index:uint):Object
		{
			return samples[index];
		}
		
		public function getVideoSampleIndexFromTime(time:uint):uint
		{
			var videoSampleIndex:int;
			var samplePos:int = ArrayUtil.bsearchInt(videoSampleDeltaArray, time * videoTimeScale, 0, videoSampleDeltaArray.length-1);
			if (samplePos >= 0)
			{
				videoSampleIndex = samplePos;				
			}
			else
			{
				videoSampleIndex = ~samplePos;
				if (videoSampleIndex > videoSampleDeltaArray.length - 1)
					videoSampleIndex = videoSampleDeltaArray.length - 1;
			}

			var syncSamplePos:int = ArrayUtil.bsearchInt(videoSyncSample, videoSampleIndex+1, 0, videoSyncSample.length-1);
			var realVideoSampleIndex:uint;
			if (syncSamplePos >= 0)
			{
				// sample at start second is sync sample
				realVideoSampleIndex = videoSampleIndex;
			}
			else
			{
				syncSamplePos = ~syncSamplePos;
				if (syncSamplePos > videoSyncSample.length - 1)
				{
					// 	start is bigger than the last sync sample.
					realVideoSampleIndex = videoSyncSample[videoSyncSample.length - 1] -1;
				}
				else
				{
					// find the sync sample nearest to sample at satrt second
					var diff1:uint = videoSampleIndex+1 - videoSyncSample[syncSamplePos-1];
					var diff2:uint = videoSyncSample[syncSamplePos]-(videoSampleIndex+1);
					if (diff1 < diff2)
						realVideoSampleIndex = videoSyncSample[syncSamplePos-1] - 1;
					else
						realVideoSampleIndex = videoSyncSample[syncSamplePos] - 1;
				}
			}
			
			return realVideoSampleIndex;
		}
		
		public function getVideoSampleTime(index:uint):Number
		{
			return calcVideoTimeStamp(index) / 1000;
		}
		
		public function getDurationInMs():uint
		{
			return videoDuration;
		}
		
		public function getAudioSampleIndexFromVideoSampleIndex(videoSampleIndex:uint):uint
		{
			var audioSampleDelta:int = calcVideoTimeStamp(videoSampleIndex) * audioTimeScale / 1000;
			var audioSmapleIndex:int = ArrayUtil.bsearchInt(audioSampleDeltaArray, audioSampleDelta, 0, audioSampleDeltaArray.length-1);
			if (audioSmapleIndex < 0)
			{			
				audioSmapleIndex = ~audioSmapleIndex;
				audioSmapleIndex -= 1;		// herain:在没有恰好命中一个音频sample的时候返回拖动点前的第一个音频sample
			}
			
			return audioSmapleIndex;
		}
		
		public function calcVideoTimeStamp(sampleIndex:uint):uint
		{
			return videoSampleDeltaArray[sampleIndex] * 1000 / videoTimeScale;
		}
		
		public function calcAudioTimeStamp(sampleIndex:uint):uint
		{
			return audioSampleDeltaArray[sampleIndex] * 1000 / audioTimeScale;
		}
		
		public function getSampleOffset(type:uint, index:uint):uint
		{
			if (type == VIDEO_SAMPLE_TYPE)
			{
				return videoSampleOffsetArray[index];
			}
			else
			{
				return audioSampleOffsetArray[index];
			}
		}
		
		public function getSampleSize(type:uint, index:uint):uint
		{
			if (type == VIDEO_SAMPLE_TYPE)
			{
				return videoSampleSize[index];
			}
			else
			{
				return audioSampleSize[index];
			}
		}
		
		public function isVideoSyncSample(index:uint):Boolean
		{
			return ArrayUtil.bsearchInt(videoSyncSample, index + 1, 0, videoSyncSample.length-1) >= 0;
		}
		
		public function getVideoSampleCompositionTimeInMs(index:uint):uint
		{
			return videoSampleCompositionTime[index] * 1000 / videoTimeScale;
		}
		
		public function getAVCDecoderConfigurationRecord():ByteArray
		{
			return AVCDecoderConfigurationRecord;
		}
		
		public function getAudioSpecificConfig():ByteArray
		{
			return AudioSpecificConfig;
		}
		
		private function parseMoovBox():Boolean
		{
			inputBuffer.position = 0;
			if (recusiveParseBox(inputBuffer.length))
			{
				assembleSamples();
				inputBuffer.clear();
				return true;
			}
			else
			{
				return false;
			}
		}
		
		private function recusiveParseBox(boxEndPosition:int):Boolean
		{
			while(inputBuffer.position < boxEndPosition && inputBuffer.bytesAvailable >= 8)
			{
				var boxBodyLength:uint = inputBuffer.readUnsignedInt() - 8;
				var box_name:String = inputBuffer.readUTFBytes(4);
				logger.info("MP4BOX: " + box_name + " Len: " + boxBodyLength + " Pos: " + inputBuffer.position);
				if (inputBuffer.bytesAvailable >= boxBodyLength - 8)
				{
					parseBox(box_name, inputBuffer.position + boxBodyLength);
				}
				else
				{
					return false;
				}
			}
			
			return inputBuffer.position == boxEndPosition;
		}
		
		private function parseBox(box_name:String, boxEndPosition:uint):void
		{
			if (box_name == "moov" || box_name == "trak" || box_name == "mdia" || box_name == "minf" || box_name == "stbl" ||
			    box_name == "sinf" || box_name == "schi" )
			{
				recusiveParseBox(boxEndPosition);
			} 
			else if (box_name == "mdhd")
			{
				parseMdhdBox(boxEndPosition);
			}
			else if (box_name == "hdlr")
			{
				parseHdlrBox(boxEndPosition);
			}
			else if (box_name == "stsd")
			{
				parseStsdBox(boxEndPosition);
			}
			else if (box_name == "avc1")
			{
				parseAvc1Box(boxEndPosition);
			}
			else if (box_name == "avcC")
			{
				parseAvccBox(boxEndPosition);
			}
			else if (box_name == "mp4a")
			{
				parseMp4aBox(boxEndPosition);
			}
			else if (box_name == "esds")
			{
				parseEsdsBox(boxEndPosition);
			}
			else if (box_name == "stts")
			{
				parseSttsBox(boxEndPosition);
			}
			else if (box_name == "ctts")
			{
				parseCttsBox(boxEndPosition);
			}
			else if (box_name == "stss")
			{
				parseStssBox(boxEndPosition);
			}
			else if (box_name == "stsz")
			{
				parseStszBox(boxEndPosition);
			}
			else if (box_name == "stsc")
			{
				parseStscBox(boxEndPosition);
			}
			else if (box_name == "stco")
			{
				parseStcoBox(boxEndPosition);
			}
			else if (box_name == "encv")
			{
				parseEncvBox(boxEndPosition);
			}
			else if (box_name == "enca")
			{
				parseEncaBox(boxEndPosition);
			}
			else if (box_name == "frma") 
			{
				parseFrmaBox(boxEndPosition);
			}
			else if (box_name == "schm") 
			{
				parseSchmBox(boxEndPosition);
			}
			else if (box_name == "adkm")
			{
				parseAdkmBox(boxEndPosition);
			}
			else if (box_name == "ahdr")
			{
				parseAhdrBox(boxEndPosition);
			}
			else if (box_name == "aprm")
			{
				parseAprmBox(boxEndPosition);
			}
			else if (box_name == "aeib")
			{
				parseAeibBox(boxEndPosition);
			}
			else if (box_name == "akey")
			{
				parseAkeyBox(boxEndPosition);
			}
			else if (box_name == "flxs")
			{
				parseFlxsBox(boxEndPosition);
			} 
			else if (box_name == "adaf")
			{
				parseAdafBox(boxEndPosition);
			}
			else 
			{
				inputBuffer.position = boxEndPosition;
			}
		}
		
		private function parseFrmaBox(boxEndPosition:uint):void
		{
			var unEncryptedDataFormat:uint = inputBuffer.readUnsignedInt();
		}
		
		private function parseSchmBox(boxEndPosition:uint):void
		{
			var version:uint = inputBuffer.readUnsignedByte();
			var flags1:uint = inputBuffer.readUnsignedByte();			
			var flags2:uint = inputBuffer.readUnsignedShort();
			var flags:uint = (flags1 << 16) | flags2; 
			var scheme_name:String = inputBuffer.readUTFBytes(4);
			logger.info("Drm Scheme is " + scheme_name + " flags is " + flags);
			if (scheme_name == "adkm") {
				var scheme_version:uint = inputBuffer.readUnsignedInt();
				if (flags == 1) {
					adrmSchemeURI = inputBuffer.readUTF();
					logger.info("Drm scheme url is " + adrmSchemeURI);
				}
			}else{
				_hasDRMError = true;
				inputBuffer.position += 4;
				if (flags == 1) {
					//passed the string SchemeURI.
					inputBuffer.readUTF();
				}	
			}
		}
		
		private function parseAdkmBox(boxEndPosition:uint):void
		{
			var version:uint = inputBuffer.readUnsignedByte();
			inputBuffer.position += 3;   //skip the flags UI24.
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseAhdrBox(boxEndPosition:uint):void
		{
			var version:uint = inputBuffer.readUnsignedByte();
			if (version == 1) {
				_hasDRMError = true;
			}
			inputBuffer.position += 3;
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseAprmBox(boxEndPosition:uint):void
		{
			var version:uint = inputBuffer.readUnsignedByte();
			inputBuffer.position += 3;
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseAeibBox(boxEndPosition:uint):void
		{
			var version:uint = inputBuffer.readUnsignedByte();
			var flag1:uint = inputBuffer.readUnsignedByte();
			var flag2:uint = inputBuffer.readUnsignedShort();
			adrmEncryptionAlgorithm = inputBuffer.readUTFBytes(8);
			adrmKeyLength = inputBuffer.readUnsignedByte();
			logger.info("Encryption algorithm is " + adrmEncryptionAlgorithm + " Key is " + adrmKeyLength);
		}
		
		private function parseAkeyBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseFlxsBox(boxEndPosition:uint):void
		{
			var index:uint = inputBuffer.position;
			while (index < boxEndPosition) {
				if (inputBuffer[index] != 0x00) {
					index ++;
				}else{
					break;
				}
			}
			index -= inputBuffer.position;
			adrmFmrmsV3Metadata = inputBuffer.readUTFBytes(index+1); //read in the ending 0x00
			logger.info("Fmrms length: " + index +  " Str = " + adrmFmrmsV3Metadata);
		}
		
		private function parseAdafBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;  //skip Version and Flags
			var encr:uint = inputBuffer.readUnsignedByte();
			adrmSelectiveEncryption = encr;
			inputBuffer.position += 1;
			var ivlength:uint = inputBuffer.readByte();
			logger.info("SelectedEncryption is " + adrmSelectiveEncryption + " ivlength " + ivlength);
		}
		
		private function parseEncvBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 78;  //VisualSampleEntry is 82 bytes + Box length.
										//however, based on actual example, BoxLength+BoxHeader
										//  do not exist inside the Encv box.
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseEncaBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 28;	//AudioSampleEntry is 32 Bytes + Header. (Same as above)
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseMdhdBox(boxEndPosition:uint):void
		{
			var version:uint = inputBuffer.readUnsignedInt();
			if (version == 0)
			{
				inputBuffer.position += 8;
				trakTimeScale = inputBuffer.readUnsignedInt();
				inputBuffer.position = boxEndPosition;
			}
			else if (version == 1)
			{
				inputBuffer.position += 16;
				trakTimeScale = inputBuffer.readUnsignedInt();
				inputBuffer.position = boxEndPosition;
			}
		}
		
		private function parseHdlrBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 8;
			var handler_type:String = inputBuffer.readUTFBytes(4);
			if (handler_type == "vide")
			{
				trakBoxType = VIDEO_TRAK_BOX_TYPE;
				videoTimeScale = trakTimeScale;
			}
			else if(handler_type == "soun")
			{
				trakBoxType = AUDIO_TRAK_BOX_TYPE;
				audioTimeScale = trakTimeScale;
			}
			else
			{
				trakBoxType = -1;
			}
			
			inputBuffer.position = boxEndPosition;
		}
		
		private function parseStsdBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 8;
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseAvc1Box(boxEndPosition:uint):void
		{
			inputBuffer.position += 78;
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseAvccBox(boxEndPosition:uint):void
		{
			// AVCDecoderConfigurationRecord contains the same information that would be stored in an avcC box 
			AVCDecoderConfigurationRecord = new ByteArray;
			inputBuffer.readBytes(AVCDecoderConfigurationRecord, 0, boxEndPosition - inputBuffer.position);
		}
		
		private function parseMp4aBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 28;
			recusiveParseBox(boxEndPosition);
		}
		
		private function parseEsdsBox(boxEndPosition:uint):void
		{
			// todo 
			// assume ES_Descriptor stream Dependence Flag, URL Flag, OCR stream Flag be 0.
			// reference:ISO_IEC_14496-1_2010 7.2.6.5 ES_Descriptor
			// 7.2.2.1: lists the box types.
			inputBuffer.readUnsignedInt();
			while(inputBuffer.position < boxEndPosition)
			{
				var tag:int = inputBuffer.readByte();
				switch(tag)
				{
					case 3:   //ES_DescrTag
						inputBuffer.position += 3;
						var flag:int = inputBuffer.readByte();
						var streamDependenceFlag:int = flag >> 7;
						var urlFlag:int = (flag | 0x00000040) >> 6;
						var ocrStreamFlag:int = (flag | 0x00000020) >> 5;
						if (streamDependenceFlag)
							inputBuffer.position += 2;
						if (urlFlag)
						{
							var urlLength:int = inputBuffer.readByte();
							inputBuffer.position += urlLength;
						}
						if (ocrStreamFlag)
							inputBuffer.position += 2;
						break;
					case 4:   //DecoderConfigDescrTag
						inputBuffer.position += 3;
						inputBuffer.position += 14;
						break;
					case 5:   //DecSpecificInfoTag
						inputBuffer.position += 3;   //wenjiewang: what the heck I am doing? skip 0x80 0x80 0x80
						var description_length:int = inputBuffer.readByte();
						AudioSpecificConfig = new ByteArray;
						inputBuffer.readBytes(AudioSpecificConfig, 0, description_length);
						inputBuffer.position = boxEndPosition;
						break;
					default:
						break;
				}
			}
		}
		
		private function parseSttsBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			var entryCount:uint = inputBuffer.readUnsignedInt();
			var sampleCount:uint = 0;
			var delta:uint;
			var sampleDeltaArray:Array = new Array;
			var deltaSum:uint = 0;
			
			for (var i:uint = 0; i < entryCount; ++i)
			{
				sampleCount += inputBuffer.readUnsignedInt();
				delta = inputBuffer.readUnsignedInt();
				for (var j:uint = 0; j < sampleCount; ++j)
				{
					sampleDeltaArray.push(deltaSum);
					deltaSum += delta;
				}
			}
			
			if (trakBoxType == VIDEO_TRAK_BOX_TYPE)
			{
				videoSampleCount = sampleCount;
				videoSampleDeltaArray = sampleDeltaArray;
				videoDeltaSum = deltaSum;
			}
			else if (trakBoxType == AUDIO_TRAK_BOX_TYPE)
			{
				audioSampleCount = sampleCount;
				audioSampleDeltaArray = sampleDeltaArray;
			}
		}
		
		private function parseCttsBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			var entryCount:uint = inputBuffer.readUnsignedInt();
			for (var i:uint = 0; i < entryCount; ++i)
			{
				var sampleCount:uint = inputBuffer.readUnsignedInt();
				var compositionTime:uint = inputBuffer.readUnsignedInt();
				for (var j:uint = 0; j < sampleCount; ++j)
				{
					videoSampleCompositionTime.push(compositionTime);
				}
			}
		}
		
		private function parseStssBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			var entryCount:uint = inputBuffer.readUnsignedInt();
			for (var i:uint = 0; i < entryCount; ++i)
			{
				videoSyncSample.push(inputBuffer.readUnsignedInt());
			}
		}
		
		private function parseStszBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			var sampleSize:uint = inputBuffer.readUnsignedInt();
			var sampleCount:uint = inputBuffer.readUnsignedInt();
			if (trakBoxType == VIDEO_TRAK_BOX_TYPE)
			{
				if (sampleSize == 0)
				{
					for (var i:uint = 0; i < sampleCount; ++i)
					{
						videoSampleSize.push(inputBuffer.readUnsignedInt());
					}
				}
				else
				{
					for (i = 0; i < sampleCount; ++i)
					{
						videoSampleSize.push(sampleSize);
					}
				}
			}
			else if (trakBoxType == AUDIO_TRAK_BOX_TYPE)
			{
				if (sampleSize == 0)
				{
					for (i = 0; i < sampleCount; ++i)
					{
						audioSampleSize.push(inputBuffer.readUnsignedInt());
					}
				}
				else
				{
					for (i = 0; i < sampleCount; ++i)
					{
						audioSampleSize.push(sampleSize);
					}
				}
			}
		}
		
		private function parseStscBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			var entryCount:uint = inputBuffer.readUnsignedInt();
			for (var i:uint = 0; i < entryCount; ++i)
			{
				var chunk:Object = new Object;
				chunk.firstChunk = inputBuffer.readUnsignedInt() - 1;	// transform index begin from 0
				chunk.samplesPerChunk = inputBuffer.readUnsignedInt();
				chunk.sampleDescriptionIndex = inputBuffer.readUnsignedInt();
				
				if (trakBoxType == VIDEO_TRAK_BOX_TYPE)
				{
					videoChunkInfo.push(chunk);
				}
				else if (trakBoxType == AUDIO_TRAK_BOX_TYPE)
				{
					audioChunkInfo.push(chunk);
				}
			}
		}
		
		private function parseStcoBox(boxEndPosition:uint):void
		{
			inputBuffer.position += 4;
			var entryCount:uint = inputBuffer.readUnsignedInt();
			for (var i:uint = 0; i < entryCount; ++i)
			{
				if (trakBoxType == VIDEO_TRAK_BOX_TYPE)
				{
					videoChunkOffset.push(inputBuffer.readUnsignedInt());
				}
				else if (trakBoxType == AUDIO_TRAK_BOX_TYPE)
				{
					audioChunkOffset.push(inputBuffer.readUnsignedInt());
				}
			}
		}
		
		private function assembleSamples():void
		{
			videoDuration = videoDeltaSum / videoTimeScale * 1000;
			
			assembleChunks(VIDEO_TRAK_BOX_TYPE);
			assembleChunks(AUDIO_TRAK_BOX_TYPE);
			
			var videoChunkIndex:uint = 0;
			var videoSampleIndex:uint = 0;
			var audioChunkIndex:uint = 0;
			var audioSampleIndex:uint = 0;
			var chunkIndex:uint = 0;
			for(; chunkIndex < videoChunks.length + audioChunks.length; ++chunkIndex)
			{
				var isVideoChunk:Boolean = true;
				if (videoChunkIndex == videoChunks.length)
					isVideoChunk = false;
				else if (audioChunkIndex != audioChunks.length &&
					audioChunks[audioChunkIndex].offset < videoChunks[videoChunkIndex].offset)
				{
					isVideoChunk = false;
				}
				
				var i:uint = 0;
				var sample:Object;
				var sampleOffset:uint;
				if (isVideoChunk)
				{
					sampleOffset = videoChunks[videoChunkIndex].offset;
					for (i = 0; i < videoChunks[videoChunkIndex].sampleCount; ++i)
					{
						videoSampleOffsetArray.push(sampleOffset);
						sampleOffset += videoSampleSize[videoSampleIndex];
						
						sample = new Object();
						sample.type = VIDEO_SAMPLE_TYPE;
						sample.index = videoSampleIndex++;
						samples.push(sample);
					}
					
					videoChunkIndex++;
				}
				else
				{
					sampleOffset = audioChunks[audioChunkIndex].offset;
					for (i = 0; i < audioChunks[audioChunkIndex].sampleCount; ++i)
					{
						audioSampleOffsetArray.push(sampleOffset);
						sampleOffset += audioSampleSize[audioSampleIndex];
						
						sample = new Object();
						sample.type = AUDIO_SAMPLE_TYPE;
						sample.index = audioSampleIndex++;
						samples.push(sample);
					}
					
					audioChunkIndex++;
				}
			}
			
			logMediaInfo();
		}
		
		private function logMediaInfo():void
		{
			logger.info("duration: " + videoDuration + " ms"
						+ "\ntotal sample count:" + samples.length
						+ "\nvideo sample count: " + videoSampleSize.length
						+ "\naudio sample count:" + audioSampleSize.length);
		}
		
		private function assembleChunks(type:uint):void
		{
			var chunkInfoArray:Array;
			var totalSampleCount:uint;
			var chunkOffsetArray:Array;
			var chunks:Array;
			if (type == VIDEO_TRAK_BOX_TYPE)
			{
				chunkInfoArray = videoChunkInfo;
				totalSampleCount = videoSampleCount;
				chunkOffsetArray = videoChunkOffset;
				chunks = videoChunks;
			}
			else
			{
				chunkInfoArray = audioChunkInfo;
				totalSampleCount = audioSampleCount;
				chunkOffsetArray = audioChunkOffset;
				chunks = audioChunks;
			}
			
			var sampleCount:uint = 0;
			var chunkIndex:uint = 0;
			for (var chunkInfoIndex:uint = 0; chunkInfoIndex < chunkInfoArray.length; ++chunkInfoIndex)
			{
				while((chunkInfoIndex != chunkInfoArray.length - 1) ?
					(chunkIndex < chunkInfoArray[chunkInfoIndex + 1].firstChunk) :
					(sampleCount < totalSampleCount))
				{
					var chunk:Object = new Object;
					chunk.firstSample = sampleCount;
					chunk.sampleCount = chunkInfoArray[chunkInfoIndex].samplesPerChunk;
					chunk.offset = chunkOffsetArray[chunkIndex];
					
					chunks.push(chunk);
					++chunkIndex;
					sampleCount += chunkInfoArray[chunkInfoIndex].samplesPerChunk;
				}
			}
		}
	}
}