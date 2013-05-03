<%@ WebHandler Language="C#" Class="recommand_nav" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class recommand_nav : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new NavFilter();
        filter.auth = context.Request["auth"];
        filter.mode = ClientUtils.FormatInt(context.Request["mode"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var nav = ServiceInvoke<INav>.CreateContract("boxnav");
        context.Response.Write(ServiceInvoke<INav>.Invoke<string>(nav, proxy => proxy.Nav(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}