<%@ WebHandler Language="C#" Class="blueeay_nav" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class blueeay_nav : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        NavFilter filter = new NavFilter();
        filter.auth = context.Request["auth"];
        filter.mode = -1;
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        INav nav = ServiceInvoke<INav>.CreateContract("boxnav");
        context.Response.Write(ServiceInvoke<INav>.Invoke<string>(nav, proxy => proxy.Nav(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}