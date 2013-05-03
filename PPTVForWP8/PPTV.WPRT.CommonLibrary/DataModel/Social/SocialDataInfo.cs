using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.DataModel.Social
{
    public class SocialDataInfo
    {
        public SocialType Type { get; set; }

        public bool IsBinded { get; set; }

        public string AcessToken { get; set; }

        public string RefreshToken { get; set; }

        public long Expire { get; set; }
    }

    public enum SocialType
    {
        SINA,
        TENCENT,
        RENREN
    }
}
