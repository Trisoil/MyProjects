<%@ WebHandler Language="C#" Class="tagsearch_ch" %>

using System;
using System.Web;

using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;
using Synacast.BoxManage.List.Vod.Serach;

public class tagsearch_ch : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new TagSearchFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 200, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        filter.k = ClientUtils.FormatSimpleChinese(context.Request["k"], "k");
        filter.dimension = ClientUtils.FormatStrV(context.Request["dimension"], "dimension");
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        var list = ServiceInvoke<ITagSearch>.CreateContract("tagsearch");
        context.Response.Write(ServiceInvoke<ITagSearch>.Invoke<string>(list, proxy => proxy.TagSearchCH(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}