<%@ WebHandler Language="C#" Class="live_parade_search_smart_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live.Search;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_parade_search_smart_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveSearchFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.k = ClientUtils.FormatSimpleChinese(context.Request["k"], "k");
        filter.auth = context.Request["auth"];
        filter.flag = context.Request["flag"];
        filter.platform = context.Request["platform"];
        filter.lastday = ClientUtils.FormatLiveParadeSearchDate(context.Request["lastday"]);
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var search = ServiceInvoke<ILiveSearch>.CreateContract("boxlivesearch");
        context.Response.Write(ServiceInvoke<ILiveSearch>.Invoke<string>(search, proxy => proxy.LiveParadeSearchEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}