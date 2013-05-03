<%@ WebHandler Language="C#" Class="live_list_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_list_ex : IHttpHandler {
    
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
        
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var live = ServiceInvoke<ILive>.CreateContract("boxlive");
        context.Response.Write(ServiceInvoke<ILive>.Invoke<string>(live, proxy => proxy.LiveListsEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}