<%@ WebHandler Language="C#" Class="cover_splashscreen" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Cover;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class cover_splashscreen : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new CoverFilter();
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        filter.img_height = ClientUtils.FormatInt(context.Request["img_height"]);
        filter.img_width = ClientUtils.FormatInt(context.Request["img_width"]);
        filter.mode = ClientUtils.FormatString(context.Request["mode"], "mode");
        var cover = ServiceInvoke<ICover>.CreateContract("coverservice");
        context.Response.Write(ServiceInvoke<ICover>.Invoke<string>(cover, proxy => proxy.SplashScreeen(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}