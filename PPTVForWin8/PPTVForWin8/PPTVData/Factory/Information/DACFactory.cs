using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Storage;
using Windows.System.Threading;
using Windows.Devices.Enumeration;
using Windows.UI.Xaml;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;
    using PPTVData.Factory.WAY;

    public class DACFactory
    {
        /// <summary>
        /// //客户端版本 6、windowsphone7；8、windowsmetro； 9、windowsphone8；
        /// </summary>
        const string C = "8";

        /// <summary>
        /// 软件版本类别：0、 默认公共版本；
        /// </summary>
        const string C1 = "0";

        /// <summary>
        /// 设备ID
        /// </summary>
        static string D = DeviceInfoFactory.Instance.DataInfos[0].DeviceId;

        //渠道号
#if SONY
        const string Y1 = "20002";
#elif LENOVO
        const string Y1 = "20003";
#else
        const string Y1 = "20001";
#endif

        /// <summary>
        /// 设备型号
        /// </summary>
        const string Y2 = "Windows 8";

        /// <summary>
        /// 单例
        /// </summary>
        public static readonly DACFactory Instance = new DACFactory();

        private DACFactory()
        { }

        private string CreateUri(string content)
        {
            return string.Format("{0}{1}", EpgUtils.DACUri, DataCommonUtils.Encode(content, "pplive"));
        }

        public void StartUp()
        {
            var clientByStartUp = new WebClient();
            string content = string.Format("Action=0&A=1&B=1&C={0}&C1={1}&D={2}&E={3}&F={4}&G={5}&H={6}&I={7}",
                 C, C1, D, EpgUtils.ClientVersion, D, Y2, Y2, Y1);
            string uri = CreateUri(content);
            clientByStartUp.DownloadStringAsync(uri);
        }

        public void Quit()
        {
            var runTime = DateTime.Now.Subtract(DataCommonUtils.AppStartTime).Seconds;
            var clientByQuit = new WebClient();
            string content = string.Format("Action=0&A=2&B=1&C={0}&C1={1}&D={2}&E={3}&F={4}&G={5}&H={6}&I=0&J={7}&K={8}", C, C1, D, EpgUtils.ClientVersion, D, Y2, Y2, runTime, Y1);
            string uri = CreateUri(content);
            clientByQuit.DownloadStringAsync(uri);
        }

        public void Play(DACPlayInfo playInfo, int playType)
        {
            var clientByPlayEnd = new WebClient();

            var builder = new StringBuilder(100);
            builder.AppendFormat("Action=0&A={0}&B=1&C={1}&C1={2}&VVID={3}&D={4}", playType, C, C1, playInfo.vvid, D);

            if (PersonalFactory.Instance.Logined)
            {
                var d1 = PersonalFactory.Instance.DataInfos[0].UserStateInfo.VIP == 0 ? "1" : "2";
                builder.AppendFormat("&D1={0}&D2={1}", d1, PersonalFactory.Instance.DataInfos[0].UserStateInfo.UserName);
            }
            else
            {
                builder.Append("&D1=0");
            }
            builder.AppendFormat("&D3={0}", WAYGetFactory.WayGetInfo.UserType);
            builder.AppendFormat("&E={0}", EpgUtils.ClientVersion);
            builder.AppendFormat("&F={0}&F1=1", playInfo.type);
            builder.AppendFormat("&G={0}", playInfo.vid);
            builder.AppendFormat("&H={0}", playInfo.title);
            builder.AppendFormat("&I={0}", playInfo.playTime);
            builder.AppendFormat("&J={0}", playInfo.mp4Name);
            builder.AppendFormat("&FT={0}", playInfo.ft);
            builder.AppendFormat("&FN={0}", playInfo.fn);
            builder.AppendFormat("&FM={0}", playInfo.allTime);
            builder.AppendFormat("&K={0}", playInfo.programSource);
            builder.AppendFormat("&L={0}", playInfo.prepareTime);
            builder.AppendFormat("&M={0}", playInfo.bufferTime);
            builder.AppendFormat("&N={0}", playInfo.allBufferCount);
            builder.AppendFormat("&O={0}", playInfo.dragCount);
            builder.AppendFormat("&P={0}", playInfo.dragBufferTime);
            builder.AppendFormat("&Q={0}", playInfo.playBufferCount);
            builder.AppendFormat("&R={0}", playInfo.connType);
            builder.AppendFormat("&S={0}", playInfo.isPlaySucceeded);
            builder.AppendFormat("&T={0}", 1);
            builder.AppendFormat("&U={0}", string.Empty);
            builder.AppendFormat("&V={0}", playInfo.averageDownSpeed);
            builder.AppendFormat("&W={0}", playInfo.stopReason);
            builder.AppendFormat("&Y1={0}", Y1);
            builder.AppendFormat("&Y2={0}", Y2);
            builder.AppendFormat("&Y3={0}", Y2);

            var uri = CreateUri(builder.ToString());
            clientByPlayEnd.DownloadStringAsync(uri);
        }
    }

}
