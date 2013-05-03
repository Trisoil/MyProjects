<%@ WebHandler Language="C#" Class="multitags_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Tags;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class multitags_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        TagsFilter filter = new TagsFilter();
        filter.dimension = ClientUtils.FormatStrV(context.Request["dimension"], "dimension");
        filter.auth = context.Request["auth"];
        filter.type = ClientUtils.FormatType(context.Request["type"]);
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        ITags tag = ServiceInvoke<ITags>.CreateContract("boxtags");
        context.Response.Write(ServiceInvoke<ITags>.Invoke<string>(tag, proxy => proxy.MultiTagsEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}