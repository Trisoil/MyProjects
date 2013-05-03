using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    public class PlayInfoHelp
    {
        /// <summary>
        /// For详情页
        /// </summary>
        /// <param name="channelDetailInfo"></param>
        /// <param name="programIndex"></param>
        public PlayInfoHelp(ChannelDetailInfo channelDetailInfo, int programIndex)
        {
            ChannelVID = channelDetailInfo.Id;
            ChannelDetailInfo = channelDetailInfo;
            ProgramIndex = programIndex;
        }

        public int ChannelVID { get; private set; }

        public ChannelDetailInfo ChannelDetailInfo { get; private set; }

        /// <summary>
        /// 值小于0为单集,值大于等于0表示连续集
        /// </summary>
        public int ProgramIndex { get; private set; }

        /// <summary>
        /// For Fav,History
        /// </summary>
        /// <param name="channelVID"></param>
        /// <param name="programIndex"></param>
        public PlayInfoHelp(int channelVID, int programIndex)
        {
            ChannelDetailInfo = null;
            ChannelVID = channelVID;
            ProgramIndex = programIndex;
        }

        public void UpdateChannelDetailInfo(ChannelDetailInfo channelDetailInfo)
        {
            ChannelDetailInfo = channelDetailInfo;
        }

        public void UpdateProgramIndex(int programIndex)
        {
            ProgramIndex = programIndex;
        }
    }
}
