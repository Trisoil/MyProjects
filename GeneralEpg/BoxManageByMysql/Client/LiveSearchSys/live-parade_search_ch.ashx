<%@ WebHandler Language="C#" Class="live_parade_search_ch" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live.Search;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_parade_search_ch : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveSearchFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.k = ClientUtils.FormatSimpleChinese(context.Request["k"], "k");
        filter.lastday = ClientUtils.FormatLiveParadeSearchDate(context.Request["lastday"]);
        filter.auth = context.Request["auth"];
        filter.flag = context.Request["flag"];
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var search = ServiceInvoke<ILiveSearch>.CreateContract("boxlivesearch");
        context.Response.Write(ServiceInvoke<ILiveSearch>.Invoke<string>(search, proxy => proxy.LiveParadeSearchCh(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}