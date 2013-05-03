package com.pplive.p2p.struct
{
	//放到constants里的东西，有的是常量，有的是只能设置一次的变量。
	public class Constants
	{
		public static const SUBPIECE_SIZE:uint = 1024;	// 1K
		public static const PIECE_SIZE:uint = 131072;	// SubPieceNumPerPiece * SubPieceSize
		public static const BLOCK_SIZE:uint = 2097152;	// PieceNumPerBlock * PieceSize
		
		public static const PIECE_NUM_PER_BLOCK:uint = 16;
		public static const SUBPIECE_NUM_PER_PIECE:uint = 128;
		public static const SUBPIECE_NUM_PER_BLOCK:uint = 2048;
		
		//public static const KERNEL_MAGIC_PORT:uint = 19765;
		//public static const KERNEL_MAGIC_PORT:uint = 9000;
		
		
		public static const KERNEL_MAGIC_PORT0:uint = 9000;
		public static const KERNEL_MAGIC_PORT1:uint = 9001;
		
		//KERNEL_MAGIC_PORT是一个取值为KERNEL_MAGIC_PORT0或者KERNEL_MAGIC_PORT1的变量，而且检测出来之后就不会变化了。所以放在Constants里
		public static var KERNEL_MAGIC_PORT:uint = KERNEL_MAGIC_PORT0;
		
		
		public static const BWTYPE_NORMAL:uint = 0;
		public static const BWTYPE_HTTP_MORE:uint = 1;
		public static const BWTYPE_HTTP_ONLY:uint = 2;
		public static const BWTYPE_HTTP_PREFERRED:uint = 3;
		public static const BWTYPE_P2P_ONLY:uint = 4;
		
		public static const KERNEL_STATUS_UNKNOWN:uint = 0;
		public static const KERNEL_STATUS_EXIST:uint = 1;
		public static const KERNEL_STATUS_NON_EXIST:uint = 2;
		
		public static const PLAY_MODE_UNKNOWN:int = -1;
		public static const PLAY_MODE_DIRECT:int = 0;
		public static const PLAY_MODE_KERNEL:int = 1;
		
		//有且只有内核检测之后，会设置这个port
		public static var   LOCAL_KERNEL_TCP_PORT:uint=16000;
		public static var	  IS_VIP:Boolean = false;
	}
}
