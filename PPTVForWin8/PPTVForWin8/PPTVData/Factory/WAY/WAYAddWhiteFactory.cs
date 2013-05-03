using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;

namespace PPTVData.Factory.WAY
{
    using PPTVData.Utils;
    using PPTVData.Entity.WAY;

    public class WAYAddWhiteFactory : WAYFactoryBase
    {
        private string _name;
        private string _device;

        protected override string CreateUri(params object[] paras)
        {
            _name = paras[0] as string;
            _device = paras[1] as string;

            return string.Format("{0}addwhite/winrt", EpgUtils.WAYHost);
        }

        protected override System.Net.Http.HttpMethod Method
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
                _index = new Random().Next(1, 10);
                var dstr = Get3DESClientKeys(_device, _index);
                var time = Convert.ToInt64(DateTime.UtcNow.Subtract(DateTime.Parse("1970,1,1")).TotalMilliseconds);
                if (!string.IsNullOrEmpty(_name))
                {
                    var ustr = Get3DESClientKeys(_name, _index);
                    var md5 = MD5Helper.GetMd5String(string.Format("{0}{1}{2}{3}", ustr, dstr, time, _md5Key));
                    return string.Format("ustr={0}&dstr={1}&time={2}&md5={3}&index={4}", WebUtility.UrlEncode(ustr), WebUtility.UrlEncode(dstr), time, md5, _index);
                }
                else
                {
                    var md5 = MD5Helper.GetMd5String(string.Format("{0}{1}{2}", dstr, time, _md5Key));
                    return string.Format("dstr={0}&md5={1}&time={2}&index={3}", WebUtility.UrlEncode(dstr), time, md5, _index);
                }
            }
        }
    }
}
