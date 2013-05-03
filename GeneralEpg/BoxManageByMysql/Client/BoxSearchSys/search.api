<%@ WebHandler Language="C#" Class="search" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Lists;
using Synacast.BoxManage.List.Vod.Serach;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class search : IHttpHandler {
    
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
        
        var list = ServiceInvoke<ISearch>.CreateContract("vodsearch");
        context.Response.Write(ServiceInvoke<ISearch>.Invoke<string>(list, proxy => proxy.VideoSearch(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}