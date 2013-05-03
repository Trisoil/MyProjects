using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

using Synacast.ProviderServices;

public partial class Login : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!string.IsNullOrEmpty(Context.User.Identity.Name) && !string.IsNullOrEmpty(Request["ReturnUrl"]))
        {
            Response.Write("您没有权限访问该页面！");
            return;
        }
        string str = Request["AudiStr"];
        if (string.IsNullOrEmpty(str))
        {
            Response.Write("用户名为空！");
            return;
        }

        try
        {
            string userName = CryptHelp.DecryptTripleDES(str.Split('$')[0]).Split('$')[0];
            if (string.IsNullOrEmpty(userName))
            {
                Response.Write("用户名为空！");
                return;
            }

            if (userName.Equals(Context.User.Identity.Name))
            {
                Response.Write("您没有权限访问该页面！");
                return;
            }
            string pwd = Membership.Provider.GetPassword(userName, "");
            if (Membership.ValidateUser(userName, pwd))
            {
                //string[] aus = Roles.GetRolesForUser(userName.Value);
                FormsAuthenticationTicket ticket = new FormsAuthenticationTicket(1, userName,
                                DateTime.Now, DateTime.Now.AddMinutes(double.Parse(System.Web.Configuration.WebConfigurationManager.AppSettings["UserExpireMin"])), false,
                                  userName, FormsAuthentication.FormsCookiePath);

                string cryptTicket = FormsAuthentication.Encrypt(ticket);
                HttpCookie ck = new HttpCookie(FormsAuthentication.FormsCookieName, cryptTicket);

                Response.AppendCookie(ck);

                HttpCookie userNameCookie = new HttpCookie("BT_LOCAL_USER", HttpContext.Current.Server.UrlEncode(userName));
                HttpContext.Current.Response.Cookies.Add(userNameCookie);
                Session["CurUserName"] = userName;

                string ss = this.Request["ReturnUrl"] ?? "Default.aspx";

                Response.Redirect(ss);

                Response.Write(ss);
            }
            else
            {
                Response.Write("用户名或密码错误！");
            }
        }
        catch (Exception ex)
        {
            Response.Write("请与系统管理员联系！（" + ex.Message + "）");
        }
    }
}
