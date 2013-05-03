<%@ WebHandler Language="C#" Class="search_hot_keywords" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Vod.Serach;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class search_hot_keywords : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new SimpleSearchFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.auth = context.Request["auth"];
        filter.rankingspan = ClientUtils.FormatRankingSpan(context.Request["rankingspan"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        var words = ServiceInvoke<ISearch>.CreateContract("vodsearch");
        context.Response.Write(ServiceInvoke<ISearch>.Invoke<string>(words, proxy => proxy.HotSearchWord(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}