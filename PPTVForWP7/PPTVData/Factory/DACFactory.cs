using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;
using System.Net;
using Microsoft.Phone.Info;
using Microsoft.Phone.BackgroundTransfer;

namespace PPTVData.Factory
{
    using PPTVData.Log;

    public class DACPlayInfo
    {
        public int castType;
        public double playTime;
        public int programSource;
        public double prepareTime;
        public double bufferTime;
        public int allBufferCount;
        public int dragCount;
        public double dragBufferTime;
        public int playBufferCount;
        public int connType;
        public int isPlaySucceeded;
        public int averageDownSpeed;
        public int stopReason;
        public string mp4Name;
    }

    public class DACFactory
    {
        private double _totalTimer = 0;  
        private HttpWebClient _clientSend = null;
        private static DACFactory _instance = null;

        public DACFactory()
        {
        }

        private static string GetUserId()
        {
            string userId = null;

            if (System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings.Contains("UserId"))
            {
                userId = System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings["UserId"] as string;
            }

            if (userId == null)
            {
                Guid guid = Guid.NewGuid();
                System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings.Add("UserId", guid.ToString());
                System.IO.IsolatedStorage.IsolatedStorageSettings.ApplicationSettings.Save();

                return guid.ToString();
            }
            else
                return userId;
        }
 
        public static void DACPlaying(DACPlayInfo playInfo, PPTVData.Entity.ChannelDetailInfo channelInfo,Version clientVersion)
        {
            string content = "Action=0&A=" + playInfo.castType.ToString() + "&B=1&C=6&D=" + GetUserId() + "&E=";
            content += (clientVersion==null?"":clientVersion.ToString());
            content += "&F=" + channelInfo.TypeID.ToString();
            content += "&G=" + channelInfo.VID.ToString();
            content += "&H=" + channelInfo.Title;
            content += "&I=" + playInfo.playTime.ToString("0");
            content += "&J=" + playInfo.mp4Name;
            content += "&K=" + playInfo.programSource.ToString();
            content += "&L=" + playInfo.prepareTime.ToString("0");
            content += "&M=" + playInfo.bufferTime.ToString("0");
            content += "&N=" + playInfo.allBufferCount.ToString();
            content += "&O=" + playInfo.dragCount.ToString();
            content += "&P=" + playInfo.dragBufferTime.ToString("0");
            content += "&Q=" + playInfo.playBufferCount.ToString();
            content += "&R=" + playInfo.connType.ToString();
            content += "&S=" + playInfo.isPlaySucceeded.ToString();
            content += "&T=";
            content += "&U=";
            content += "&V=" + playInfo.averageDownSpeed.ToString();
            content += "&W=" + playInfo.stopReason.ToString();
            content += "&Y1=0";
            content += "&Y2=" + DeviceExtendedProperties.GetValue("DeviceName").ToString();
            content += "&Y3=" + System.Environment.OSVersion.Version.ToString();
            _instance = new DACFactory();
            _instance.DACSend(content);
        }

        public static void DACStartup(Version clientVersion)
        {
            DACFactory dacFactory = new DACFactory();
            string content = "Action=0&A=1&B=1&C=6&D=" + GetUserId();
            content += "&E=";
            content += clientVersion == null ? "" : clientVersion.ToString();
            content += "&F=" + Utils.DeviceUniqueId;
            content += "&G=" + DeviceExtendedProperties.GetValue("DeviceName").ToString();
            content += "&H=" + System.Environment.OSVersion.Version.ToString() + "&I=0";
           
            _instance = new DACFactory();
            _instance.DACSend(content);
        }

        public static void DACQuit(Version clientVersion,int appLifeTime)
        {
            DACFactory dacFactory = new DACFactory();
            string content = "Action=0&A=2&B=1&C=6&D=" + GetUserId() + "&E=";
            content += (clientVersion == null ? "" : clientVersion.ToString()) + "&F=";
            content += Utils.DeviceUniqueId;
            content += "&G=" + DeviceExtendedProperties.GetValue("DeviceName").ToString();
            content += "&H=" + System.Environment.OSVersion.Version.ToString();
            content += "&I=";
            content += "&J=" + appLifeTime + "&K=0";
            dacFactory.DACSend(content);
        }

        public void Cancel()
        {
            if (_clientSend != null)
            {
                _clientSend.CancelAsync();
                _clientSend = null;
            }
        }      

        public void DACSend(string strContent)
        {
            Cancel();
            _clientSend = new HttpWebClient();


            strContent += "&Tick=" + DateTime.Now.Ticks.ToString();
            string encodeContent = Utils.Encode(strContent, "pplive");
            _clientSend.DownloadStringAsync(Utils._DACUrl + encodeContent);
        }


        void timer_Tick(object sender, EventArgs e)
        {
            _totalTimer = _totalTimer + 1;
        }
       
    }
}
