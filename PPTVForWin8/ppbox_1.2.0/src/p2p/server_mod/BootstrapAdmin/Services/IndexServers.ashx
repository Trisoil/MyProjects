<%@ WebHandler Language="C#" Class="IndexServers" %>

using System;
using System.Web;

public class IndexServers : IHttpHandler
{

    public void ProcessRequest(HttpContext context)
    {
        /*
            ##################################################
            ##             Index Server配置文件 样例
            ##################################################
            [other]
            0   = 0
            ...
            255  = 1
            [index]                
            # index server
            0 udp 60.28.216.199 4010
            ...
            4 udp 60.28.216.155 4010
         */
        context.Response.ContentType = "text/plain";
        context.Response.Write("#Index Server 配置文件\r\n");
        context.Response.Write("[other]\r\n");
        System.Text.StringBuilder sb = new System.Text.StringBuilder();
        using (System.Data.IDataReader reader = DbHelper.BootStrapServers.ExecuteReader("select ServerId, ModMin, ModMax, Protocol, IP, Port from bss_SimpleIndexServers order by ModMin"))
        {
            int index = 0;
            while (reader.Read())
            {
                for (int i = reader.GetInt32(1); i <= reader.GetInt32(2); i++)
                {
                    context.Response.Write(string.Format("{0} = {1}\r\n", i.ToString(), index.ToString()));
                }
                sb.AppendFormat("{0} {1} {2} {3}\r\n", index.ToString(), reader.GetString(3), reader.GetString(4), reader.GetInt32(5).ToString());
                index++;
            }
        }
        context.Response.Write("[index]\r\n");
        context.Response.Write(sb.ToString());
    }

    public bool IsReusable
    {
        get
        {
            return false;
        }
    }

}