package
{
	import com.pplive.p2p.BootStrapModuleTest;
	import com.pplive.p2p.CacheManagerTest;
	import com.pplive.p2p.P2PModuleTest;
	import com.pplive.p2p.download.DownloaderDriverTest;
	import com.pplive.p2p.download.HttpDownloaderTest;
	import com.pplive.p2p.download.IPPoolTest;
	import com.pplive.p2p.download.P2PDownloaderTest;
	import com.pplive.p2p.download.PeerConnectionTest;
	import com.pplive.p2p.download.PeerConnectorTest;
	import com.pplive.p2p.download.SpeedMeterTest;
	import com.pplive.p2p.mp4.Mp4HeaderTest;
	import com.pplive.p2p.mp4.Mp4StreamTest;
	import com.pplive.p2p.network.TcpStreamTest;
	import com.pplive.p2p.struct.BlockMapTest;
	import com.pplive.p2p.struct.StructUtilTest;
	import com.pplive.p2p.struct.PeerDownloadInfoTest;
	import com.pplive.p2p.struct.PieceTest;
	import com.pplive.p2p.struct.RIDTest;
	import com.pplive.p2p.struct.SubPieceTest;
	import com.pplive.p2p.tracker.TrackerConnectionTest;
	import com.pplive.p2p.tracker.TrackerModuleTest;

	[Suite]
	[RunWith( "org.flexunit.runners.Suite" )]
	public class FlashP2PAllTest
	{
		public function FlashP2PAllTest()
		{
		}
		
		public var constantsTest:StructUtilTest;
		public var subPieceTest:SubPieceTest;
		public var pieceTest:PieceTest;
		public var cacheManagerTest:CacheManagerTest;
		public var mp4HeaderTest:Mp4HeaderTest;
		public var mp4StreamTest:Mp4StreamTest;
		public var downloadDriverTEst:DownloaderDriverTest;
//		public var httpDownloaderTest:HttpDownloaderTest;
//		public var speedMeterTest:SpeedMeterTest;
//		public var bootStrapModuleTest:BootStrapModuleTest;
//		public var ridTest:RIDTest;
//		public var trackerConnectionTest:TrackerConnectionTest;
//		public var trackerModuleTest:TrackerModuleTest;
//		public var p2pModuleTest:P2PModuleTest;
		public var ippoolTest:IPPoolTest;
//		public var peerConnectorTest:PeerConnectorTest;
		public var tcpStreamTest:TcpStreamTest;
		public var peerDownloadInfoTest:PeerDownloadInfoTest;
		public var blockMapTest:BlockMapTest;
//		public var peerConnectionTest:PeerConnectionTest;
//		public var p2pDownloaderTest:P2PDownloaderTest;
	}
}