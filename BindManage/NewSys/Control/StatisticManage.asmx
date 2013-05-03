<%@ WebService Language="C#" Class="StatisticManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;

using Synacast.NHibernateActiveRecord;
using Synacast.NHibernateActiveRecord.New;
using Synacast.PPInstall;

/// <summary>
/// StatisticManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class StatisticManage : System.Web.Services.WebService
{

    public StatisticManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
    }

    [WebMethod]
    public StatisticJson GetAllStatistic(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        StatisticJson json = new StatisticJson();
        json.CurrentPage = StatisticInfo.Execute(new SqlQuery<StatisticInfo>("*", start, limit, sort, dir, StatisticInfo.ViewName, StatisticInfo.PrimaryKey, filters.ToArray(), null)) as StatisticInfo[];
        json.TotalCount = (int)StatisticInfo.Execute(new SqlCount<StatisticInfo>(start, limit, sort, dir, StatisticInfo.ViewName, StatisticInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }
    
    [WebMethod]
    public ComboboxJson GetAllProductName()
    {
        ProductInfo[] result = ProductInfo.FindAll();
        ComboboxJson json = new ComboboxJson();
        List<ComboboxClass> list = new List<ComboboxClass>();
        ComboboxClass cc = new ComboboxClass();
        cc.Id = 0;
        cc.Name = "全部";
        list.Add(cc);
        for (int i = 0; i < result.Length; i++)
        {
            ComboboxClass c = new ComboboxClass();
            c.Id = result[i].ProductID;
            c.Name = result[i].EnName;
            list.Add(c);
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public ComboboxJson GetALLCustom(int productId)
    {
        CustomInfo[] result = null;
        if (productId != 0)
        {
            string sql = string.Format("select * from {0} where ProductID={1}", CustomInfo.TableName, productId);
            result = CustomInfo.Execute(new SqlCommonQuery<CustomInfo>(sql)) as CustomInfo[];
        }
        ComboboxJson json = new ComboboxJson();
        List<ComboboxClass> list = new List<ComboboxClass>();
        ComboboxClass cc = new ComboboxClass();
        cc.Id = 0;
        cc.Name = "全部";
        list.Add(cc);
        if (result != null)
        {
            for (int i = 0; i < result.Length; i++)
            {
                ComboboxClass c = new ComboboxClass();
                c.Id = result[i].CustomID;
                c.Name = result[i].EnName;
                list.Add(c);
            }
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public ComboboxJson GetAllChannel()
    {
        ChannelInfo[] result = ChannelInfo.FindAll();
        ComboboxJson json = new ComboboxJson();
        List<ComboboxClass> list = new List<ComboboxClass>();
        ComboboxClass cc = new ComboboxClass();
        cc.Id = 0;
        cc.Name = "全部";
        list.Add(cc);
        for (int i = 0; i < result.Length; i++)
        {
            ComboboxClass c = new ComboboxClass();
            c.Id = result[i].ChannelID;
            c.Name = result[i].EnName;
            list.Add(c);
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public ComboboxJson GetAllSoft()
    {
        SoftInfo[] result = SoftInfo.FindAll();
        ComboboxJson json = new ComboboxJson();
        List<ComboboxClass> list = new List<ComboboxClass>();
        ComboboxClass cc = new ComboboxClass();
        cc.Id = 0;
        cc.Name = "全部";
        list.Add(cc);
        for (int i = 0; i < result.Length; i++)
        {
            ComboboxClass c = new ComboboxClass();
            c.Id = result[i].SoftID;
            c.Name = result[i].CnName;
            list.Add(c);
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }
    
    [Serializable]
    public class StatisticJson
    {
        public int TotalCount;
        public StatisticInfo[] CurrentPage;
    }

    [Serializable]
    public class ComboboxJson
    {
        public int TotalCount;
        public ComboboxClass[] CurrentPage;
    }

    [Serializable]
    public class ComboboxClass
    {
        public int Id;
        public string Name;
    }
}


