<%@ WebService Language="C#" Class="GuideManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;

using Synacast.PPInstall;
using Synacast.PPInstall.Repository;
using Synacast.NHibernateActiveRecord.New;

/// <summary>
/// GuideManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class GuideManage : System.Web.Services.WebService
{

    GuideRepository _repository;
    public GuideManage()
    {
        _repository = new GuideRepository();
        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
    }

    [WebMethod]
    public GuideJson GetAllGuide(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        GuideJson json = new GuideJson();
        json.CurrentPage = GuideInfo.Execute(new SqlQuery<GuideInfo>("*", start, limit, sort, dir, GuideInfo.ViewName, GuideInfo.PrimaryKey, filters.ToArray(), null)) as GuideInfo[];
        json.TotalCount = (int)GuideInfo.Execute(new SqlCount<GuideInfo>(start, limit, sort, dir, GuideInfo.ViewName, GuideInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddGuide(int customId, int interval, string des, string imageArray)
    {
        _repository.JudgeCustom(customId);
        GuideInfo info = new GuideInfo();
        info.CustomID = customId;
        info.Interval = interval;
        info.Description = des;
        info.ImageArray = imageArray;
        info.Save();
    }

    [WebMethod]
    public void EditGuide(int id, int customId, int interval, string des, string imageArray)
    {
        GuideInfo info = GuideInfo.FindById(id);
        if (info != null)
        {
            if (info.CustomID != customId)
                _repository.JudgeCustom(customId);
            info.CustomID = customId;
            info.Interval = interval;
            info.Description = des;
            info.ImageArray = imageArray;
            info.Save();
        }
    }

    [WebMethod]
    public void DeleteGuides(int[] ids)
    {
        foreach (int id in ids)
        {
            GuideInfo info = GuideInfo.FindById(id);
            if (info != null)
            {
                info.Delete();
            }
        }
    }
    
    [Serializable]
    public class GuideJson
    {
        public int TotalCount;
        public GuideInfo[] CurrentPage;
    }

}


