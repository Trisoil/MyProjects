using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    public class DACPlayInfo
    {
        public int type;
        public int vid;
        public string title;
        public string vvid;

        /// <summary>
        /// I:当前频道的观看时间，以秒计
        /// </summary>
        public int playTime;

        /// <summary>
        /// K：观看来源，0,封面图；1,推荐；2,频道分类；3,排行榜；4,搜索；5，最近观看；6，收藏；7，追剧
        /// </summary>
        public int programSource;

        /// <summary>
        /// L:从频道启动到开始播放所花的时间，以秒为单位。
        /// </summary>
        public int prepareTime;

        /// <summary>
        /// M:播放过程中总共的缓冲时间，以秒为单位
        /// </summary>
        public int bufferTime;

        /// <summary>
        /// N：播放过程中总共缓冲的次数
        /// </summary>
        public int allBufferCount;

        /// <summary>
        /// O：拖动次数
        /// </summary>
        public int dragCount;

        /// <summary>
        /// P:拖动产生的缓冲时长（以秒为单位）
        /// </summary>
        public int dragBufferTime;

        /// <summary>
        /// Q：卡顿次数（非拖动引起的缓冲时间大于3秒的次数）
        /// </summary>
        public int playBufferCount;

        /// <summary>
        /// R：数据流量类型（0,wifi；1,3g）
        /// </summary>
        public int connType = 0;

        /// <summary>
        /// S：视频是否播放成功（0,失败；1,成功）
        /// </summary>
        public int isPlaySucceeded;

        /// <summary>
        /// V：平均下载速度（kb/s）
        /// </summary>
        public double averageDownSpeed;

        /// <summary>
        /// W：播放停止原因（0，播放完成；1，手动；2，异常）
        /// </summary>
        public int stopReason;

        /// <summary>
        /// J：mp4文件名称
        /// </summary>
        public string mp4Name;

        /// <summary>
        /// FM: 影片的实际时长,浮点数,以秒计
        /// </summary>
        public int allTime;

        /// <summary>
        /// 码率标准：0，流畅版；1，高清版；2，超清版；3，全高清版；5，Mobile（300K)；6，Mobile（150K）；
        /// </summary>
        public int ft;

        /// <summary>
        /// 提交影片的实际码率值，数值
        /// </summary>
        public int fn;
    }
}
