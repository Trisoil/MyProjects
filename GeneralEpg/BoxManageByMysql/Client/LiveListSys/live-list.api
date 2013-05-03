<%@ WebHandler Language="C#" Class="live_list" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_list : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.type = ClientUtils.FormatInt(context.Request["type"]);
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.flag = context.Request["flag"];
        filter.order = context.Request["order"];
        filter.nowplay = ClientUtils.FormatBool(context.Request["nowplay"]);
        filter.rankingspan = ClientUtils.FormatRankingSpan(context.Request["rankingspan"]);
        filter.beginletter = ClientUtils.FormatBeginLetter(context.Request["beginletter"], "beginletter");
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var live = ServiceInvoke<ILive>.CreateContract("boxlive");
        context.Response.Write(ServiceInvoke<ILive>.Invoke<string>(live, proxy => proxy.LiveLists(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}