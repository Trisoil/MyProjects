package com.pplive.p2p.network.protocol
{
	import com.pplive.p2p.struct.BlockMap;
	import com.pplive.p2p.struct.PeerDownloadInfo;
	import com.pplive.p2p.struct.RID;
	
	import flash.utils.ByteArray;

	public class AnnounceResponsePacket extends PeerResponsePacket
	{
		public static const ACTION:uint = 0xB1;
		public var peerDownloadInfo:PeerDownloadInfo;
		public var blockMap:BlockMap;
		
		public function AnnounceResponsePacket(bytes:ByteArray)
		{
			super(bytes);
			peerDownloadInfo = new PeerDownloadInfo(bytes);
			blockMap = new BlockMap(bytes);
		}
	}
}