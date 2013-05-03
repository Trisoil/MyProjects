<%@ WebHandler Language="C#" Class="SoftUploadHandler" %>

using System;
using System.Web;

public class SoftUploadHandler : IHttpHandler
{

    public void ProcessRequest(HttpContext context)
    {
        string message = "";
        string fileName = "";
        string directoryName = null;
        string filePrefix = null;
        try
        {
            if (context.Request.Files.Count > 0)
            {
                HttpPostedFile upFile = context.Request.Files[0];
                if (upFile.InputStream.Length > 0)
                {
                    string fileType = upFile.FileName.Substring(upFile.FileName.LastIndexOf(".") + 1);
                    string name = upFile.FileName.Substring(upFile.FileName.LastIndexOf("\\") + 1);
                    directoryName = DateTime.Now.ToString("yyyy-MM-dd");
                    filePrefix = DateTime.Now.ToString("HHmmss");
                    fileName = string.Format("{0}/{1}{2}", directoryName, filePrefix, name);
                    byte[] buffer = new byte[upFile.InputStream.Length];
                    upFile.InputStream.Read(buffer, 0, (int)upFile.InputStream.Length);
                    RemoteClient client = RemoteClient.GetInstance("WWW");
                    string fname = context.Request["filename"];
                    if (!string.IsNullOrEmpty(fname))
                    {
                        message = client.Video.Replace(context.Request["apptype"], fileName, buffer, fname);
                    }
                    else
                    {
                        message = client.Video.Save(context.Request["apptype"], fileName, buffer);
                    }
                }
            }
        }
        catch (Exception ex)
        {
            context.Response.StatusCode = 500;
            message = ex.Message;
        }
        //context.Response.ContentType = "text/plain";
        context.Response.Clear();
        if ("删除成功!".Equals(message) || "保存成功!".Equals(message))
        {
            context.Response.Write("{success:true,directoryName:'" + directoryName + "',filePrefix:'" + filePrefix + "'}");
        }
        else
        {
            context.Response.Write("{success:false,message:'" + context.Server.UrlEncode(message) + "'}");
        }
        message = "";
    }


    public bool IsReusable
    {
        get
        {
            return false;
        }
    }

}