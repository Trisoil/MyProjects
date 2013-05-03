using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Threading.Tasks;
using System.Runtime.Serialization.Json;

namespace PPTVData.Factory.WAY
{
    using PPTVData.Entity.WAY;
    using PPTVData.Utils;

    public class WAYGetFactory : WAYFactoryBase
    {
        public static WAYGetInfo WayGetInfo;

        static WAYGetFactory()
        {
            WayGetInfo = new WAYGetInfo();
        }

        public static int GetUserLevel
        {
            get {
                return WayGetInfo.UserType > 1 ? 1 : 0;
            }
        }

        protected override bool IsBackThreadCallBack
        {
            get
            {
                return true;
            }
        }

        protected override string CreateUri(params object[] paras)
        {
            _index = new Random().Next(1, 10);
            var dstr = Get3DESClientKeys(paras[1] as string, _index);
            var userName = paras[0] as string;
            if (!string.IsNullOrEmpty(userName))
            {
                var ustr = Get3DESClientKeys(userName, _index);
                var md5 = MD5Helper.GetMd5String(string.Format("{0}{1}{2}", ustr, dstr, _md5Key));
                return string.Format("{0}get/winrt?ustr={1}&dstr={2}&md5={3}&index={4}",
                    EpgUtils.WAYHost, WebUtility.UrlEncode(ustr), WebUtility.UrlEncode(dstr), md5, _index);
            }
            else
            {
                var md5 = MD5Helper.GetMd5String(string.Format("{0}{1}", dstr, _md5Key));
                return string.Format("{0}get/winrt?dstr={1}&md5={2}&index={3}",
                    EpgUtils.WAYHost, WebUtility.UrlEncode(dstr), md5, _index);
            }
        }

        protected override WAYGetInfo AnalysisContent(Stream content)
        {
            var result = string.Empty;
            using (var reader = new StreamReader(content))
            {
                result = reader.ReadToEnd();
            }

            WAYGetInfo resultInfo = null;
            using (var ms = new MemoryStream(DESCrypt(result)))
            { 
                resultInfo = (WAYGetInfo)new DataContractJsonSerializer(typeof(WAYGetInfo)).ReadObject(ms);
            }
            _getTime = DateTime.UtcNow;
            _getTimeMillSeconds = resultInfo.Time;

            return resultInfo;
        }
    }
}
