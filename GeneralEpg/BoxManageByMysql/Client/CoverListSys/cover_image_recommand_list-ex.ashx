<%@ WebHandler Language="C#" Class="cover_image_recommand_list_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Cover;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class cover_image_recommand_list_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new CoverFilter();
        filter.c = ClientUtils.FormatDefaultInt(context.Request["c"], 10);
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        filter.img_height = ClientUtils.FormatInt(context.Request["img_height"]);
        filter.img_width = ClientUtils.FormatInt(context.Request["img_width"]);
        filter.navid = ClientUtils.FormatInt(context.Request["nav_id"]);

        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var cover = ServiceInvoke<ICover>.CreateContract("coverservice");
        context.Response.Write(ServiceInvoke<ICover>.Invoke<string>(cover, proxy => proxy.ImageRecommandEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}