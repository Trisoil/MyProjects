<%@ WebHandler Language="C#" Class="MISUserTransfer" %>

using System;
using System.Web;

public class MISUserTransfer : IHttpHandler
{

    private void RequestForLogin(string returnUrl, HttpContext context) {        
        context.Response.Redirect(string.Format("{0}?ReturnUrl={1}MISUserTransfer.ashx&DestUrl={2}", System.Web.Security.FormsAuthentication.DefaultUrl,
            HttpUtility.UrlEncode(string.Format("http://{0}:{1}/", context.Request.Url.Host, context.Request.Url.Port.ToString())),
            HttpUtility.UrlEncode(returnUrl)));
    }

    #region IHttpHandler 成员

    public bool IsReusable
    {
        get { return false; }
    }

    public void ProcessRequest(HttpContext context)
    {
        string ru = context.Request.QueryString["ReturnUrl"];
        if (string.IsNullOrEmpty(ru))
        {
            context.Response.ContentType = "text/plain";
            context.Response.Write("当前页面提供分布式用户集中登录功能, 请选择菜单访问其它功能页面.");
            return;
        }
        string username, token;
        //分析跳转参数
        username = context.Request.QueryString["UserName"];
        if (string.IsNullOrEmpty(username))
        {
            RequestForLogin(ru, context);
            return;
        }
        if (username.Equals(context.User.Identity.Name))
        {//非匿名用户
            context.Response.ContentType = "text/plain";
            context.Response.Write("用户 ");
            context.Response.Write(username);
            context.Response.Write(", 您当前已登录, 如果本页未按预期跳转, 可能您无权访问目标页面\r\n    ");
            context.Response.Write(ru);
            context.Response.Write("\r\n如果是因为浏览器缓存, 请在当前窗口强刷目标页面.\r\n或者, 请注销后更换账户重新登录进行重试!");
            return;
        }
        token = context.Request.QueryString["Token"];
        if (string.IsNullOrEmpty(token))
        {
            RequestForLogin(ru, context);
            return;
        }
        if (MISClient.Membership.ValidateCenterUser(username, token))
        {
            System.Web.Security.FormsAuthentication.SetAuthCookie(username, false);
            context.Response.Redirect(ru);
            return;
        }
        context.Response.ContentType = "text/plain";
        context.Response.Write("对不起, 中心授权认证失败!\r\n请注销后重新登录进行重试!");
    }

    #endregion
}