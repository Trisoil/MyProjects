<%@ WebHandler Language="C#" Class="cover_image_recommand_nav_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Cover;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class cover_image_recommand_nav_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new CoverFilter();
        filter.auth = context.Request["auth"];
        filter.navmode = ClientUtils.FormatInt(context.Request["mode"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);

        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);

        var cover = ServiceInvoke<ICover>.CreateContract("coverservice");
        context.Response.Write(ServiceInvoke<ICover>.Invoke<string>(cover, proxy => proxy.ImageNavEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}