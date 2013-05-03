<%@ WebHandler Language="C#" Class="list_by_tag" %>

using System;
using System.Web;

using Synacast.BoxManage.Client;
using Synacast.BoxManage.List.Lists;
using Synacast.BoxManage.Client.Help;
using Synacast.BoxManage.List.Vod.Lists;

public class list_by_tag : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new ListFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.auth = context.Request["auth"];
        filter.flag = context.Request["flag"];
        filter.platform = context.Request["platform"];
        filter.rankingspan = ClientUtils.FormatRankingSpan(context.Request["rankingspan"]);
        filter.beginletter = ClientUtils.FormatBeginLetter(context.Request["beginletter"], "beginletter");
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        filter.ntags = ClientUtils.FormatNTags(context.Request["ntags"], "ntags");
        filter.screen = context.Request["screen"];
        filter.order = ClientUtils.FormatOrder(context.Request["order"]);
        var list = ServiceInvoke<IListByTags>.CreateContract("listbytags");
        context.Response.Write(ServiceInvoke<IListByTags>.Invoke<string>(list, proxy => proxy.ListByTags(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}