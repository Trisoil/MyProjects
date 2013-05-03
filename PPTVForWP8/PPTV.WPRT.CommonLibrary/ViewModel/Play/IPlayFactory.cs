using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Play
{
    using PPTVData.Entity;

    using PPTV.WPRT.CommonLibrary.DataModel.Play;

    /// <summary>
    /// 播放工厂通用接口
    /// </summary>
    public interface IPlayFactory
    {
        /// <summary>
        /// 设置播放信息
        /// </summary>
        Action<PlayMessageInfo> PlayMessageAction { get; set; }

        /// <summary>
        /// 播放失败
        /// </summary>
        Action<string> PlayErrorAction { get; set; }

        /// <summary>
        /// HTTP失败或超时
        /// </summary>
        Action HttpFailOrTimeoutAction { get; set; }

        /// <summary>
        /// 是否支持拖动
        /// </summary>
        bool SupportSeek { get; }

        /// <summary>
        /// DAC信息
        /// </summary>
        DACPlayInfo DacPlayInfo { get; }

        /// <summary>
        /// 播放
        /// </summary>
        void Play(MediaElement mediaPlayer);

        /// <summary>
        /// 结束播放
        /// </summary>
        void End();

        /// <summary>
        /// 播放前一集
        /// </summary>
        void PreProgram();

        /// <summary>
        /// 播放后一集
        /// </summary>
        void NextProgram();

        /// <summary>
        /// 多码流选择
        /// </summary>
        /// <param name="fileType"></param>
        void BtirateSelect(PlayFileType fileType);
    }
}
