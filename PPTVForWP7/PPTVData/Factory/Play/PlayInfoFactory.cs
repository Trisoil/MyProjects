using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;

namespace PPTVData.Factory
{
    using PPTVData.Entity;
    using System.Xml;
    using System.Diagnostics;

    public class PlayInfoFactory : HttpFactoryBase<PlayInfo>
    {

        protected override string CreateUri(params object[] paras)
        {
            return String.Format("http://play.api.pptv.com/boxplay.api?id={0}&auth={1}&platform={2}", paras[0], EPGSetting.Auth, EPGSetting.PlatformName);
        }

        protected override PlayInfo AnalysisData(System.Xml.XmlReader reader)
        {
            reader.ReadToFollowing("channel");

            Dictionary<int, PlayInfo.Item> playInfoItems = new Dictionary<int, PlayInfo.Item>();
            while (reader.Read())
            {
                if (reader.Name == "item")
                {
                    if (reader.GetAttribute("bitrate") == null || reader.GetAttribute("ft") == null)
                        continue;
                    int bitrate = Convert.ToInt32(reader.GetAttribute("bitrate"));
                    int ft = Convert.ToInt32(reader.GetAttribute("ft"));
                    string rid = reader.GetAttribute("rid");
                    PlayInfo.Item playInfo = new PlayInfo.Item(bitrate, ft, rid, null);
                    playInfoItems.Add(ft, playInfo);
                }

                if (reader.Name == "dt" && reader.NodeType != XmlNodeType.EndElement)
                {
                    int ft = Convert.ToInt32(reader.GetAttribute("ft"));
                    PlayInfo.DTInfo dt = new PlayInfo.DTInfo();
                    reader.ReadToFollowing("sh");
                    reader.Read();
                    dt.Sh = reader.Value;
                    reader.ReadToFollowing("st");
                    reader.Read();
                    dt.St = reader.Value;
                    reader.ReadToFollowing("bwt");
                    reader.Read();
                    dt.Bwt = Convert.ToInt32(reader.Value);
                    if (playInfoItems.ContainsKey(ft))
                        playInfoItems[ft].DtInfo = dt;
                }
            }

            PlayInfo retPlayInfo = new PlayInfo();
            foreach (PlayInfo.Item item in playInfoItems.Values)
            {
                retPlayInfo.Add(item);
            }
            
            return retPlayInfo;
        }
    }
}
