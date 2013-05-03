using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Cloud
{
    using PPTVData.Utils;
    using PPTVData.Entity.Cloud;

    public class CloudGetFactory : CloudFactoryBase
    {
        protected override int TimerOut
        {
            get
            {
                return 20;
            }
        }
        
        protected override string CreateUri(params object[] paras)
        {
            var index = new Random().Next(1, 10);
            var ustr = Get3DESClientKeys(paras[0] as string, index);
            Type = paras[1] as string ;
            var millisecond = Convert.ToInt64((DateTime.UtcNow - DateTime.Parse("1970,1,1")).TotalMilliseconds);
            string md5 = MD5Helper.GetMd5String(string.Format("{0}{1}{2}", ustr, millisecond, _md5Key));
            return string.Format("{0}get/?ustr={1}&type={2}&time={3}&md5={4}&index={5}&etag={6}",
                EpgUtils.SyncHost, System.Net.WebUtility.UrlEncode(ustr), paras[1], millisecond, md5, index, Etag);
        }

    }
}
