<%@ WebHandler Language="C#" Class="recommand_list_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class recommand_list_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new NavFilter();
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
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var nav = ServiceInvoke<INav>.CreateContract("boxnav");
        context.Response.Write(ServiceInvoke<INav>.Invoke<string>(nav, proxy => proxy.NavListsEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}