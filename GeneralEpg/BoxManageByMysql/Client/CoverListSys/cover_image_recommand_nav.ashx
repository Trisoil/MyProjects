<%@ WebHandler Language="C#" Class="cover_image_recommand_nav" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Cover;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class cover_image_recommand_nav : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new CoverFilter();
        filter.auth = context.Request["auth"];
        filter.navmode = ClientUtils.FormatInt(context.Request["mode"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);

        var cover = ServiceInvoke<ICover>.CreateContract("coverservice");
        context.Response.Write(ServiceInvoke<ICover>.Invoke<string>(cover, proxy => proxy.ImageNav(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}