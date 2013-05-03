<%@ WebHandler Language="C#" Class="live_recommand_list" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_recommand_list : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveNavFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.nowplay = ClientUtils.FormatBool(context.Request["nowplay"]);
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.navid = ClientUtils.FormatIntV(context.Request["navid"], "navid");
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var nav = ServiceInvoke<ILiveNav>.CreateContract("boxlivenav");
        context.Response.Write(ServiceInvoke<ILiveNav>.Invoke<string>(nav, proxy => proxy.LiveRecommandList(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}