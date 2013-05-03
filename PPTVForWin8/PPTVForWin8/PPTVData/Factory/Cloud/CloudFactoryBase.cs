using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;

namespace PPTVData.Factory.Cloud
{
    using PPTVData.Utils;
    using PPTVData.Entity.Cloud;

    public abstract class CloudFactoryBase : HttpFactoryBase<CloudArgs>
    {
        protected static string _md5Key = "BJAS90G02GN20G92J093GH209HGSA";

        public static long TimeInterval { get; set; }

        public string Etag { get; set; }

        public string Type { get; set; }

        protected override CloudArgs AnalysisData(System.Xml.XmlReader reader)
        {
            if (_webClient.ResponseHeaders != null)
            {
                string date = _webClient.ResponseHeaders["Date"];
                if (date != null)
                {
                    DateTime serverTime = DateTime.Parse(date);
                    TimeInterval = DateTime.Now.Subtract(serverTime).Ticks;
                }
            }

            var args = new CloudArgs();
            args.Datas = new List<CloudDataInfo>();
            while (reader.Read())
            {
                if (reader.Name == "root")
                {
                    if (Method == HttpMethod.Get)
                    {
                        reader.MoveToAttribute("etag");
                        Etag = reader.ReadContentAsString();
                    }
                    reader.MoveToAttribute("ret");
                    int ret = reader.ReadContentAsInt();
                    reader.MoveToAttribute("msg");
                    string msg = reader.ReadContentAsString();
                    if (ret == 0)
                    {
                        args.message = msg;
                        return args;
                    }
                }
                if (reader.Name == Type)
                {
                    while (reader.Read())
                    {
                        if (reader.Name == "Item")
                        {
                            try
                            {
                                var infoSync = new CloudDataInfo();

                                if (reader.MoveToAttribute("UUID"))
                                    infoSync.UUID = reader.ReadContentAsString();

                                if (reader.MoveToAttribute("Device"))
                                    infoSync.Device = reader.ReadContentAsInt();

                                if (reader.MoveToAttribute("DeviceHistory"))
                                    infoSync.DeviceHistory = reader.ReadContentAsInt();

                                if (reader.MoveToAttribute("Id"))
                                    infoSync.Id = reader.ReadContentAsInt();
                                //reader.MoveToAttribute("ClId");
                                //infoSync.ClId = reader.ReadContentAsInt();
                                if (reader.MoveToAttribute("Name"))
                                    infoSync.Name = reader.ReadContentAsString();

                                if (reader.MoveToAttribute("SubName"))
                                    infoSync.SubName = reader.ReadContentAsString();

                                if (reader.MoveToAttribute("SubId"))
                                    infoSync.SubId = reader.ReadContentAsInt();

                                if (reader.MoveToAttribute("Pos"))
                                    infoSync.Pos = reader.ReadContentAsInt();

                                if (reader.MoveToAttribute("Duration"))
                                    infoSync.Duration = reader.ReadContentAsInt();

                                if (reader.MoveToAttribute("ModifyTime"))
                                    infoSync.ModifyTime = reader.ReadContentAsLong();
                                //reader.MoveToAttribute("Property");
                                //infoSync.Property = reader.ReadContentAsInt();
                                //reader.MoveToAttribute("VideoType");
                                //infoSync.VideoType = reader.ReadContentAsInt();
                                //reader.MoveToAttribute("Bt");
                                //infoSync.Bt = reader.ReadContentAsInt();
                                //reader.MoveToAttribute("Mode");
                                //infoSync.Mode = reader.ReadContentAsInt();
                                args.Datas.Add(infoSync);
                            }
                            catch { }
                        }
                    }
                }
            }
            return args;
        }

        protected abstract override string CreateUri(params object[] paras);
    }

    /// <summary>
    /// HTTP请求回调实体表示类
    /// </summary>
    public class CloudArgs
    {
        public string message { get; set; }

        public List<CloudDataInfo> Datas { get; set; }
    }
}
