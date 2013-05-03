<%@ WebService Language="C#" Class="IPAreaGroupManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;

using Synacast.PPInstall;
using Synacast.NHibernateActiveRecord.New;

/// <summary>
/// IPAreaGroupManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class IPAreaGroupManage : System.Web.Services.WebService
{

    public IPAreaGroupManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
    }

    [WebMethod]
    public CommonJson<IPAreaGroupInfo> GetAllIP(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        CommonJson<IPAreaGroupInfo> json = new CommonJson<IPAreaGroupInfo>();
        json.CurrentPage = IPAreaGroupInfo.Execute(new SqlQuery<IPAreaGroupInfo>("*", start, limit, sort, dir, IPAreaGroupInfo.TableName, IPAreaGroupInfo.PrimaryKey, filters.ToArray(), null)) as IPAreaGroupInfo[];
        json.TotalCount = (int)IPAreaGroupInfo.Execute(new SqlCount<IPAreaGroupInfo>(start, limit, sort, dir, IPAreaGroupInfo.TableName, IPAreaGroupInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddIP(string name, string areas)
    {
        IPAreaGroupInfo info = new IPAreaGroupInfo();
        info.AreaGroupName = name;
        info.AreaIDList = areas;
        info.Save();
    }

    [WebMethod]
    public void EditIP(int id, string name, string areas)
    {
        IPAreaGroupInfo info = IPAreaGroupInfo.FindById(id);
        if (info != null)
        {
            info.AreaGroupName = name;
            info.AreaIDList = areas;
            info.Save();
        }
    }

    [WebMethod]
    public void DeleteIPs(int[] ids)
    {
        foreach (int id in ids)
        {
            IPAreaGroupInfo info = IPAreaGroupInfo.FindById(id);
            if (info != null)
                info.Delete();
        }
    }
}


