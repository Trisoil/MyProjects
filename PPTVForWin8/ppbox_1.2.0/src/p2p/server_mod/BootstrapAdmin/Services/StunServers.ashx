<%@ WebHandler Language="C#" Class="StunServers" %>

using System;
using System.Web;

public class StunServers : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        /*
            ##################################################
            ##             Stun Server配置文件 样例
            ##################################################
            [stun]
            0 udp         220.165.14.5       7000
            ...
            0 udp         221.235.191.179  7000
         */
        context.Response.ContentType = "text/plain";
        context.Response.Write("#Stun Server 配置文件\r\n");
        context.Response.Write("[stun]\r\n");
        using (System.Data.IDataReader reader = DbHelper.BootStrapServers.ExecuteReader("select ServerId, Category, ModValue, Protocol, IP, Port from bss_SimpleStunServers order by ModValue"))
        {
            while (reader.Read())
            {
                context.Response.Write(string.Format("{0} {1} {2} {3}\r\n", reader.GetInt32(2).ToString(), reader.GetString(3), reader.GetString(4), reader.GetInt32(5).ToString()));
            }
        }
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}