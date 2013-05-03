using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;

namespace Synacast.BoxManage.List
{
    [Serializable]
    public class ExFilterBase
    {
        public int c { get; set; }

        public int s { get; set; }

        public string auth { get; set; }

        public string platform { get; set; }

        public int bitratemin { get; set; }

        public int bitratemax { get; set; }

        public int wmin { get; set; }

        public int wmax { get; set; }

        public int hmin { get; set; }

        public int hmax { get; set; }

        public int forbidvip { get; set; }

        public int conlen { get; set; }

        public string lang { get; set; }

        public int ver { get; set; }

        public string screen { get; set; }

        public bool nowplay { get; set; }

        public bool forbidden = Convert.ToBoolean(HttpContext.Current.Items["forbidden"]) && !string.IsNullOrEmpty(HttpContext.Current.Request.QueryString["fb"]);

        public string fbcode = HttpContext.Current.Request["fb"];

        public string commonfbcode;
    }
}
