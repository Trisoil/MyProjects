<%@ WebHandler Language="C#" Class="multitags" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Tags;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class multitags : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        TagsFilter filter = new TagsFilter();
        filter.dimension = ClientUtils.FormatStrV(context.Request["dimension"], "dimension");
        filter.auth = context.Request["auth"];
        filter.type = ClientUtils.FormatType(context.Request["type"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        ITags tag = ServiceInvoke<ITags>.CreateContract("boxtags");
        context.Response.Write(ServiceInvoke<ITags>.Invoke<string>(tag, proxy => proxy.MultiTags(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}