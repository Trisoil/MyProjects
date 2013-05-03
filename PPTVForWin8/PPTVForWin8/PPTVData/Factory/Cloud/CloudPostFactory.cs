using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace PPTVData.Factory.Cloud
{
    using PPTVData.Utils;
    using PPTVData.Entity.Cloud;

    public class CloudPostFactory : CloudFactoryBase
    {
        /// <summary>
        /// 用于保存需删除的数据集
        /// </summary>
        private List<CloudDataInfo> _needDeletes = new List<CloudDataInfo>(30);

        protected override string CreateUri(params object[] paras)
        {
            Type = paras[0] as string;
            return string.Format("{0}/post/", EpgUtils.SyncHost);
        }

        protected override HttpMethod Method
        {
            get
            {
                return HttpMethod.Post;
            }
        }

        protected override string PostArgs
        {
            get
            {
                var index = new Random().Next(1, 10);
                var ustr = Get3DESClientKeys(
                    PersonalFactory.Instance.DataInfos[0].UserStateInfo.UserName, index);
                var millisecond = Convert.ToInt64((DateTime.UtcNow - DateTime.Parse("1970,1,1")).TotalMilliseconds);
                var content = CreatePost();

                var list = new List<APIParameter>(5);
                list.Add(new APIParameter("ustr", WebUtility.UrlEncode(ustr)));
                list.Add(new APIParameter("content", content));
                list.Add(new APIParameter("time", millisecond.ToString()));
                list.Add(new APIParameter("md5", MD5Helper.GetMd5String(string.Format("{0}{1}{2}{3}", ustr, content, millisecond, _md5Key))));
                list.Add(new APIParameter("index", index.ToString()));
                return HttpUtils.GetQueryFromParas(list);
            }
        }

        public void AddDeleteInfo(CloudDataInfo deleteInfo)
        {
            _needDeletes.Add(deleteInfo);
        }

        private string CreatePost()
        {
            var build = new StringBuilder(100);
            using (var xmlWriter = XmlWriter.Create(build))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteStartElement("root");
                var infos = Type == CloudType.Recent ? LocalRecentFactory.Instance.DataInfos :
                    LocalFavoritesFactory.Instance.DataInfos;
                CreateContent(xmlWriter, infos);
                xmlWriter.WriteEndElement();
                xmlWriter.Flush();
            }
            return build.ToString();
        }

        private void CreateContent(XmlWriter xmlWriter, ObservableCollection<CloudDataInfo> datas)
        {
            xmlWriter.WriteStartElement(Type);

            xmlWriter.WriteStartElement("Modify");
            var updates = datas.Where(v =>
                v.DataType == CloudInfoEnum.Update || v.DataType == CloudInfoEnum.Add);
            foreach (var update in updates)
            {
                WriteXmlItem(xmlWriter, update);
                update.DataType = CloudInfoEnum.Normal;
            }
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("Delete");
            foreach (var delete in _needDeletes)
            {
                xmlWriter.WriteStartElement("Item");
                xmlWriter.WriteAttributeString("UUID", delete.UUID);
                xmlWriter.WriteEndElement();
            }
            _needDeletes.Clear();
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();
        }

        private void WriteXmlItem(XmlWriter xmlWriter, CloudDataInfo item)
        {
            xmlWriter.WriteStartElement("Item");
            xmlWriter.WriteAttributeString("UUID", item.UUID);
            xmlWriter.WriteAttributeString("Device", item.Device.ToString());
            xmlWriter.WriteAttributeString("DeviceHistory", item.DeviceHistory.ToString());
            xmlWriter.WriteAttributeString("Id", item.Id.ToString());
            xmlWriter.WriteAttributeString("ClId", item.ClId.ToString());
            xmlWriter.WriteAttributeString("Name", item.Name);
            xmlWriter.WriteAttributeString("SubName", item.SubName);
            xmlWriter.WriteAttributeString("SubId", item.SubId.ToString());
            xmlWriter.WriteAttributeString("Pos", item.Pos.ToString());
            xmlWriter.WriteAttributeString("Duration", item.Duration.ToString());
            xmlWriter.WriteAttributeString("ModifyTime", item.ModifyTime.ToString());
            xmlWriter.WriteAttributeString("Property", item.Property.ToString());
            xmlWriter.WriteAttributeString("VideoType", item.VideoType.ToString());
            xmlWriter.WriteAttributeString("Bt", item.Bt.ToString());
            xmlWriter.WriteAttributeString("Mode", item.Mode.ToString());
            xmlWriter.WriteEndElement();
        }
    }
}
