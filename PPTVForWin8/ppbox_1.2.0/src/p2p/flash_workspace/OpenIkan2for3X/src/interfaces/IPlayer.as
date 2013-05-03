package interfaces
{
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	
	public interface IPlayer
	{
		
		public function IPlayer();
		
		/**
		 * 切换播放/暂停
		 * @return 该函数无返回值
		 */
		function togglevideo():void;
		
		/**
		 * 停止播放
		 * @return 该函数无返回值
		 */	
		function stopvideo():void;
		
		
		/**
		 * 传入播放信息
		 * @return 该函数无返回值
		 */	
		function injectObject(obj:Object = null):void;
		/**
		 * 视频需要播放的时间点，以秒为单位
		 * @param v:Number - 拖动的时间点
		 * @return 该函数无返回值
		 */
		
		function seekvideo(v:Number):void;
		
		/**
		 * 重新设置播放器的大小
		 * @param firstParam w:Number - 设置宽度
		 * @param secondParam h:Number - 设置高度
		 * @return 该函数无返回值
		 */
		function resize(w:Number,h:Number):void;
		
		/**
		 * 设置播放器的音量
		 * @param v:Number - 声音值 0-100
		 * @return 该函数无返回值
		 */
		function setvolumn(v:Number):void;
		
		/**
		 * 得到播放器宽度
		 * @return 该函数数值型
		 */
		function getWidth():Number;
		
		/**
		 * 得到播放器高度
		 * @return 该函数数值型
		 */
		function getHeight():Number;
		
		/**
		 * 视频时长，以秒为单位
		 * @return 该函数无返回值
		 */
		function get duration():Number;
		
		/**
		 * 加载字节百分比
		 * @return 数值型
		 */
		function get byteloaded():Number;
		
		/**
		 * 缓冲百分比
		 * @return 数值型
		 */
		function get bufferloaded():Number;
		
		/**
		 * 已经播放的时间，单位为秒
		 * @return 数值型
		 */
		function get timeloaded():Number;
		
		/**
		 * 播放状态
		 * @return 字符型
		 */
		function get playstate():String;
		
		/**
		 * 设置播放器亮度
		 * @param v:Number - 值 -100-100
		 * @return 该函数无返回值
		 */
		function setHightLight(v:int,c:int):void;
		
		/**
		 * 返回当前播放视频截图
		 * @return 截图
		 */
		function getSnapshotPic():BitmapData;
		
	}
	
}