using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.Utils
{
    using PPTVData.Entity;

    using PPTV.WPRT.CommonLibrary;
    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.ViewModel;

    public static class ChannelUtils
    {
        /// <summary>
        /// 当前进入频道的来源
        /// </summary>
        public static DACPageType DACPageType;

        /// <summary>
        /// 判断是否是单集
        /// </summary>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static bool JudgeSingle(ChannelDetailInfo channelInfo)
        {
            if (channelInfo.ProgramInfos.Count == 1
                && channelInfo.ProgramInfos[0].ChannelId == channelInfo.Id)
                return true;
            return false;
        }

        /// <summary>
        /// 判断分集索引是否相等，适配倒序情况
        /// </summary>
        /// <param name="sourceIndex"></param>
        /// <param name="programIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static bool JudgeProgreamIndex(int sourceIndex, int programIndex, ChannelDetailInfo channelInfo)
        {
            if (programIndex < 0 ||
                programIndex >= channelInfo.ProgramInfos.Count)
                programIndex = 0;
            if (programIndex != channelInfo.ProgramInfos[programIndex].Index)
                return sourceIndex == channelInfo.ProgramInfos[programIndex].Index;
            return sourceIndex == -1 ? true : sourceIndex == programIndex;
        }

        /// <summary>
        /// 生成频道名称：主集名称 第几集
        /// </summary>
        /// <param name="programIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static string CreateChannelTitle(int programIndex, ChannelDetailInfo channelInfo, bool noNumberShow)
        {
            if (JudgeSingle(channelInfo))
                return channelInfo.Title;

            if (programIndex < 0
                || programIndex >= channelInfo.ProgramInfos.Count)
                programIndex = 0;

            if (!noNumberShow)
                return channelInfo.IsNumber ? string.Format("{0} 第{1}集", channelInfo.Title, channelInfo.ProgramInfos[programIndex].Title) :
                    string.Format("{0} {1}", channelInfo.Title, channelInfo.ProgramInfos[programIndex].Title);
            else
                return channelInfo.IsNumber ? string.Format("{0} 第{1}集", channelInfo.Title, channelInfo.ProgramInfos[programIndex].Title) :
                    string.Format("{0} {1} 第{2}集", channelInfo.Title, channelInfo.ProgramInfos[programIndex].Title, programIndex + 1);
        }

        /// <summary>
        /// 生成频道Id
        /// </summary>
        /// <param name="programIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static int CreateChannelId(int programIndex, ChannelDetailInfo channelInfo)
        {
            if (programIndex < 0
                || programIndex >= channelInfo.ProgramInfos.Count)
                programIndex = 0;

            return channelInfo.ProgramInfos[programIndex].ChannelId;
        }

        /// <summary>
        /// 生成频道分集索引，适配倒序情况
        /// </summary>
        /// <param name="sourceIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static int CreateProgramIndex(int programIndex, ChannelDetailInfo channelInfo)
        {
            if (programIndex < 0
                || programIndex >= channelInfo.ProgramInfos.Count)
                programIndex = 0;

            if (programIndex != channelInfo.ProgramInfos[programIndex].Index)
                return channelInfo.ProgramInfos.Count - programIndex - 1;
            return programIndex;
        }

        /// <summary>
        /// 生成频道分集索引，不适配倒序情况
        /// </summary>
        /// <param name="programIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static int CreateComonIndex(int programIndex, ChannelDetailInfo channelInfo)
        {
            if (programIndex < 0
                || programIndex >= channelInfo.ProgramInfos.Count)
                return programIndex;
            return channelInfo.ProgramInfos[programIndex].Index;
        }

        /// <summary>
        /// 生成上一集索引，匹配倒序情况
        /// </summary>
        /// <param name="programIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static int CreatePreviousIndex(int programIndex, ChannelDetailInfo channelInfo)
        {
            if (programIndex < 0
                || programIndex >= channelInfo.ProgramInfos.Count)
                return -1;
            if (programIndex != channelInfo.ProgramInfos[programIndex].Index)
                return ++programIndex;
            else
                --programIndex;
            if (programIndex >= channelInfo.ProgramInfos.Count)
                return -1;
            return programIndex;
        }

        /// <summary>
        /// 生成下一集的索引，匹配倒序情况
        /// </summary>
        /// <param name="programIndex"></param>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        public static int CreateNextIndex(int programIndex, ChannelDetailInfo channelInfo)
        {
            if (programIndex < 0
                || programIndex >= channelInfo.ProgramInfos.Count)
                return -1;
            if (programIndex != channelInfo.ProgramInfos[programIndex].Index)
                return --programIndex;
            else
                ++programIndex;
            if (programIndex >= channelInfo.ProgramInfos.Count)
                return -1;
            return programIndex;
        }
    }
}
