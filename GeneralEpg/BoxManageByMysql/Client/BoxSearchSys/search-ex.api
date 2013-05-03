<%@ WebHandler Language="C#" Class="search_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Lists;
using Synacast.BoxManage.List.Vod.Serach;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class search_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new ListFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.k = ClientUtils.FormatSimpleChinese(context.Request["k"], "k");
        filter.auth = context.Request["auth"];
        filter.flag = context.Request["flag"];
        filter.platform = context.Request["platform"];
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        if (filter.ver > 1)
        {
            filter.screen = context.Request["screen"];
        }
        filter.type = ClientUtils.FormatInt(context.Request["type"]);
        filter.shownav = ClientUtils.FormatInt(context.Request["shownav"]);
        
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var list = ServiceInvoke<ISearch>.CreateContract("vodsearch");
        context.Response.Write(ServiceInvoke<ISearch>.Invoke<string>(list, proxy => proxy.VideoSearchEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}