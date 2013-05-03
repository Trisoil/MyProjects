<%@ WebHandler Language="C#" Class="blueray_list" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class blueray_list : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        NavFilter filter = new NavFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.navid = ClientUtils.FormatIntV(context.Request["navid"], "navid");
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        if (filter.ver > 1)
        {
            filter.screen = context.Request["screen"];
        }
        INav nav = ServiceInvoke<INav>.CreateContract("boxnav");
        context.Response.Write(ServiceInvoke<INav>.Invoke<string>(nav, proxy => proxy.NavLists(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}