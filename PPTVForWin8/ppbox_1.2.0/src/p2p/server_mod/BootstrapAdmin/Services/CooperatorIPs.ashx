<%@ WebHandler Language="C#" Class="CooperatorIPs" %>

using System;
using System.Web;
using System.Data;
using System.Collections.Generic;

public class CooperatorIPs : IHttpHandler
{

    public void ProcessRequest(HttpContext context)
    {
        /*
            ##################################################
            ##             PPVA IP地址库
            ##################################################
            # IP库版本
            IPLibVersion = 0
            PublicID = 0

            #   起始IP地址		    结束IP地址		    地域码
            #   ---------------	-----------------	 ---------------
            #    127.0.0.1        127.0.0.1              10001
         */
        context.Response.ContentType = "text/plain";
        object o = DbHelper.BootStrapServers.ExecuteScalar("select CooperatorId from bss_TrackerCooperators where CooperatorName = '公网'");
        if (o.Equals(DBNull.Value))
        {
            context.Response.Write("#严重错误: 公网ip段缺失！");
            return;
        }
        int publicId = int.Parse(o.ToString());
        context.Response.Write("#PPVA IP地址库\r\n");
        context.Response.Write("IPLibVersion = 0\r\n");
        context.Response.Write("PublicID = ");
        context.Response.Write(o.ToString());
        context.Response.Write("\r\n\r\n");

        Dictionary<string, Room> Cooperators = new Dictionary<string, Room>();
        using (IDataReader reader = DbHelper.BootStrapServers.ExecuteReader("select CooperatorName, CooperatorId from bss_TrackerCooperators where CooperatorName <> '公网'"))
        {
            while (reader.Read())
            {
                Cooperators.Add(reader.GetString(0), new Room() { Id = reader.GetInt32(1), Name = reader.GetString(0) });
            }
        }

        Room room = null;
        Dictionary<int, Room> Rooms = new Dictionary<int, Room>();
        using (IDataReader reader = DbHelper.OpenService_Files.ExecuteReader("select GroupName, GroupID from ServerGroup"))
        {
            while (reader.Read())
            {
                if (Cooperators.TryGetValue(reader.GetString(0), out room))
                {
                    Rooms.Add(reader.GetInt32(1), room);
                }
            }
        }
        int i = -1;
        bool isValid = false;
        using (IDataReader reader = DbHelper.OpenService_Files.ExecuteReader("select GroupID, BeginIP, EndIP from Cooperate_ip order by GroupID"))
        {
            while (reader.Read())
            {
                if (i != reader.GetInt32(0))
                {
                    i = reader.GetInt32(0);
                    if (Rooms.TryGetValue(i, out room))
                    {
                        isValid = true;
                        //输出行
                        context.Response.Write(string.Format("\r\n#{0}\r\n", room.Name));
                        
                    }
                    else
                    {
                        isValid = false;
                    }
                }
                if (isValid)
                {
                    context.Response.Write(string.Format("{1} {2} {0}\r\n", room.Id, reader.GetString(1), reader.GetString(2)));
                }
            }
        }
    }

    class Room
    {
        public int Id { get; set; }
        public string Name { get; set; }
    }

    public bool IsReusable
    {
        get
        {
            return false;
        }
    }

}