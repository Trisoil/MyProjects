<%@ WebHandler Language="C#" Class="BsConfig" %>

using System;
using System.Web;

public class BsConfig : IHttpHandler
{
    private static string _fileName;
    public static string FileName
    {
        get
        {
            if (string.IsNullOrEmpty(_fileName))
            {
                _fileName = string.Format("{0}Services/BsConfig.txt", HttpContext.Current.Request.PhysicalApplicationPath);
            }
            return _fileName;
        }
    }

    public void ProcessRequest(HttpContext context)
    {
        context.Response.ContentType = "text/plain";
        context.Response.Write(System.IO.File.ReadAllText(FileName, System.Text.Encoding.UTF8));
    }

    public bool IsReusable
    {
        get
        {
            return false;
        }
    }

}