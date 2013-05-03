<%@ WebHandler Language="C#" Class="live_recommand_list_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_recommand_list_ex : IHttpHandler {
    
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
        
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var nav = ServiceInvoke<ILiveNav>.CreateContract("boxlivenav");
        context.Response.Write(ServiceInvoke<ILiveNav>.Invoke<string>(nav, proxy => proxy.LiveRecommandListEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}