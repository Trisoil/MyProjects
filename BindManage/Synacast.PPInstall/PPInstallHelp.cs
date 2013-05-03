using System;
using System.Collections.Generic;
using System.Text;
using System.Web;

namespace Synacast.PPInstall
{
    public class PPInstallHelp
    {
        public static string CurrentUserName
        {
            get
            {
                HttpCookie nameCookie = HttpContext.Current.Request.Cookies["BT_LOCAL_USER"];
                if (nameCookie != null)
                {
                    string userName = HttpContext.Current.Server.UrlDecode(nameCookie.Value);
                    return userName;
                }
                return string.Empty;
            }
        }

        public static string ClientIp
        {
            get { return HttpContext.Current.Request.UserHostAddress; }
        }
    }
}
