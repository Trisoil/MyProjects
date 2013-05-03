<%@ WebHandler Language="C#" Class="TrackerServers" %>

using System;
using System.Web;

public class TrackerServers : IHttpHandler
{

    public void ProcessRequest(HttpContext context)
    {
        /*
            ##################################################
            ##             PPVA 服务器分组信息
            ##################################################
            [tracker_commit]
            0 UDP 192.168.1.1 8000
            1 UDP 172.16.11.1 8000
            [tracker_list.0]
            0 UDP 192.168.1.2 8001
            1 UDP 172.16.11.2 8000
            [tracker_list.1]
            0 UDP 192.168.1.3 8002
            1 UDP 172.16.11.3 8000
            [tracker_list.2]
            0 UDP 192.168.1.4 8003
            1 UDP 172.16.11.4 8000
            [cache.0]
            0 TCP 192.168.1.5 8004
            [cache.1]
            0 TCP 192.168.1.6 8005
            [collecton]
            0 UDP 192.168.1.8 8007
            [other]
            up_pic_pb = 0.001
            collect_pb = 1
            dac_report_minute = 5
         */
        context.Response.ContentType = "text/plain";
        int cooperatorId;
        int.TryParse(context.Request.QueryString["id"], out cooperatorId);
        if (cooperatorId <= 0)
        {
            context.Response.Write("CooperatorId required, parameter name \"id\".");
            return;
        }
        context.Response.Write("#PPVA 服务器分组信息\r\n");
        using (System.Data.IDataReader reader = DbHelper.BootStrapServers.ExecuteReader("select ServerId, Category, ModValue, Protocol, IP, Port from bss_SimpleTrackerServers where CooperatorId = @id order by Category, ModValue", "id", cooperatorId))
        {
            string category = string.Empty;
            while (reader.Read())
            {
                if (!category.Equals(reader.GetString(1)))
                {
                    category = reader.GetString(1);
                    context.Response.Write(string.Format("[{0}]\r\n", category));
                }
                context.Response.Write(string.Format("{0} {1} {2} {3}\r\n", reader.GetInt32(2).ToString(), reader.GetString(3), reader.GetString(4), reader.GetInt32(5).ToString()));
            }
        } 
        using (System.Data.IDataReader reader = DbHelper.BootStrapServers.ExecuteReader("select UpPicPb, CollectPb, DacReportMinute from bss_TrackerCooperators where CooperatorId = @id", "id", cooperatorId))
        {
            if (reader.Read())
            {
                context.Response.Write(string.Format(@"[other]
up_pic_pb = {0}
collect_pb = {1}
dac_report_minute = {2}", reader.GetString(0), reader.GetString(1), reader.GetString(2)));
            }
        }
    }

    public bool IsReusable
    {
        get
        {
            return false;
        }
    }

}