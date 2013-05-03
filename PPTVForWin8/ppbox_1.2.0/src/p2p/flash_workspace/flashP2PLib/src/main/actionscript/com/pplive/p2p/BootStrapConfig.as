// ActionScript file

package com.pplive.p2p
{
	public class BootStrapConfig
	{
		
		//下面的4个变量，用来控制查询trackerproxy的频率，典型的配置可能是：50,1，30,120
		
		//如果未尝试过的peer的个数，小于leastUntriedPeerCountForList，那么才会考虑重新查询tracker
		public static var leastUntriedPeerCountForList:uint = 50;
		//未尝试的peer不够时，频繁查询的最大次数
		public static var frequentPeerListTimes:uint = 1;		
		
		//频繁查询和非频繁查询的时间间隔,在一个分段刚开始下载的时候，前几次查询tracker的时间间隔可能比较短，当查询次数达到frequentPeerListTimes
		//的时候，查询的频率就要降下来，因为这个情况很可能是服务器确实查不到足够多的资源了。开始几次的查询，叫做频繁查询，后几次的查询叫非频繁查询。
		
		//这个值设置为30，而不是比较小的数值的原因是，减少短视频的不必要的查询。
		public static var frequentTimeIntervalPeerList:uint = 30;
		public static var nofrequentTimeIntervalPeerList:uint = 120;	
		
		//当剩余时间达到startP2pRestPlayTime的时候，启用p2p
		public static var startP2pRestPlayTime:uint=50;
		//当剩余时间小于resumeHttpRestPlayTime的时候，恢复http
		public static var resumeHttpRestPlayTime:uint=20;
		
		public static var useP2p:Boolean = true;
		
		
	}
}