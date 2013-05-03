<%@ WebService Language="C#" Class="PageManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;
using System.Text;

using Synacast.PPInstall;
using Synacast.NHibernateActiveRecord;
using Synacast.NHibernateActiveRecord.New;
using NewWorkFlowSys.HistoryRecord;

/// <summary>
/// PageManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class PageManage : System.Web.Services.WebService
{
    static string _eventFrom = "捆绑页信息";
    HistoryRecordManage _hr;
    
    public PageManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public PageJson GetPageByBind(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters, int bindId)
    {
        filters.AddRange(inputFilters);
        string where = string.Format(" and BindID={0}",bindId);
        PageJson json = new PageJson();
        json.CurrentPage = PageInfo.Execute(new SqlQuery<PageInfo>("*", start, limit, sort, dir, PageInfo.ViewName, PageInfo.PrimaryKey, filters.ToArray(),where)) as PageInfo[];
        json.TotalCount = (int)PageInfo.Execute(new SqlCount<PageInfo>(start, limit, sort, dir, PageInfo.ViewName, PageInfo.PrimaryKey, filters.ToArray(), where));
        return json;
    }

    [WebMethod]
    public void AddPage(int bindId, int templateId, string des,int pagesoftCount)
    {
        JudgePage(bindId, templateId);
        PageInfo info = new PageInfo();
        info.BindID = bindId;
        info.TemplateID = templateId;
        info.Description = des;
        info.SoftCount = pagesoftCount;
        info.Save();
        UpdateBindAdd(bindId,templateId);
        string eventLog = string.Format("增加了一个捆绑页信息,捆绑控制标识号为{0},捆绑模版标识号为{1}", bindId, templateId);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditPage(int id, int bindId, int templateId, string des,int pagesoftCount)
    {
        PageInfo info = PageInfo.FindById(id);
        if (info != null)
        {
            if (info.TemplateID != templateId)
                JudgePage(bindId, templateId);
            UpdateForceCount(bindId, templateId, info.TemplateID, info.SoftCount);
            info.BindID = bindId;
            info.TemplateID = templateId;
            info.Description = des;
            info.SoftCount = pagesoftCount;
            info.Save();
            string eventLog = string.Format("编辑了一个捆绑页信息,捆绑控制标识号为{0},捆绑模版标识号为{1}", bindId, templateId);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeletePages(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            PageInfo info = PageInfo.FindById(id);
            if (info != null)
            {
                UpdateBindDes(info);
                UpdateSoftDes(id);
                builder.Append(info.BindID);
                builder.Append("/");
                builder2.Append(info.TemplateID);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("删除了{0}个捆绑页信息,捆绑控制标识号为{1},捆绑模版标识号为{2}", ids.Length,builder.ToString(),builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    private void UpdateForceCount(int bindId, int newTemplateId, int oldTemplateId,int softCount)
    {
        //if(softCount == 0)
        //    return;
        if (newTemplateId == oldTemplateId)
            return;
        TemplateInfo info = TemplateInfo.FindById(newTemplateId);
        TemplateInfo info2 = TemplateInfo.FindById(oldTemplateId);
        if (info != null && info2 != null)
        {
            if (info.Type == info2.Type)
                return;
            else if (info.Type == 0 && info2.Type != 0)  //非强制模版换成强制模版
            {
                BindInfo bind = BindInfo.FindById(bindId);
                if (bind != null)
                {
                    bind.ForceCount += softCount;
                    bind.PageCount -= 1;  //由于强制模版不需增加控制的页数
                    bind.Save();
                }
            }
            else if (info.Type != 0 && info2.Type == 0)  //强制模版换成非强制模版
            {
                BindInfo bind = BindInfo.FindById(bindId);
                if (bind != null)
                {
                    bind.ForceCount -= softCount;
                    bind.PageCount += 1;
                    bind.Save();
                }
            }
        }
    }

    /// <summary>
    /// 判断是否存在属于同一控制的同一模版记录
    /// </summary>
    /// <param name="bindId"></param>
    /// <param name="templateId"></param>
    private void JudgePage(int bindId, int templateId)
    {
        string sql = string.Format("select count(*) from {0} where BindID={1} and TemplateID={2}",PageInfo.TableName,bindId,templateId);
        int count = (int)PageInfo.ExecuteScalarBySql(sql);
        if (count > 0) throw new ApplicationException("已经存在相同捆绑页模版的记录，请选择其他模版！");
    }
    
    /// <summary>
    /// 修改控制的页个数
    /// </summary>
    /// <param name="bindId"></param>
    private void UpdateBindAdd(int bindId,int templateId)
    {
        TemplateInfo template = TemplateInfo.FindById(templateId);
        if (template != null)
        {
            if (template.Type == 0)  //强制模版不需增加控制的页数
                return;
        }
        BindInfo bind = BindInfo.FindById(bindId);
        if (bind != null)
        {
            bind.PageCount += 1;
            bind.Save();
        }
    }

    /// <summary>
    /// 修改控制的页个数，如是强制模版，修改控制的强制软件个数
    /// </summary>
    /// <param name="info"></param>
    private void UpdateBindDes(PageInfo info)
    {
        BindInfo bind = BindInfo.FindById(info.BindID);
        if (bind != null)
        {
            TemplateInfo temp = TemplateInfo.FindById(info.TemplateID);
            if (temp != null)
            {
                if (temp.Type == 0)
                {
                    bind.ForceCount -= info.SoftCount;
                }
                else
                {
                    bind.PageCount -= 1;   //如删除的是废强制模版，控制页数-1，强制模版不需减
                }
            }
            bind.Save();
        }
    }

    /// <summary>
    /// 修改捆绑软件的所有渠道每天下发总量
    /// </summary>
    /// <param name="pageId"></param>
    private void UpdateSoftDes(int pageId)
    {
        string sql = string.Format("select * from {0} where PageID={1}",PageSoftInfo.ViewName,pageId);
        PageSoftInfo[] result = DbObject<PageSoftInfo>.Execute(new SqlCommonQuery<PageSoftInfo>(sql)) as PageSoftInfo[];
        foreach (PageSoftInfo info in result)
        {
            SoftInfo soft = SoftInfo.FindById(info.SoftID);
            if (soft != null)
            {
                soft.AllCPDOfChannel -= info.CPDOfChannel;
                soft.Save();
            }
        }
    }
    
    [WebMethod]
    public PageTemplateJson GetAllTemplate()
    {
        PageTemplateJson json = new PageTemplateJson();
        json.CurrentPage = TemplateInfo.FindAll();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public PageSoftComboJson GetAllSoft()
    {
        SoftInfo[] result = SoftInfo.FindAll();
        List<PageSoftComboInfo> list = new List<PageSoftComboInfo>();
        foreach (SoftInfo info in result)
        {
            PageSoftComboInfo s = new PageSoftComboInfo();
            s.Id = info.SoftID;
            s.Name = info.CnName;
            s.SoftCountPerDay = info.CountPerDay ;
            list.Add(s);
        }
        PageSoftComboJson json = new PageSoftComboJson();
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    /// <summary>
    /// 返回所有绑定有效的捆绑软件
    /// </summary>
    /// <returns></returns>
    [WebMethod]
    public PageSoftComboJson GetAllEnableSoft()
    {
        string sql = string.Format("select * from {0} where BindEnable=1", SoftInfo.TableName);
        SoftInfo[] result = SoftInfo.Execute(new SqlCommonQuery<SoftInfo>(sql)) as SoftInfo[];
        List<PageSoftComboInfo> list = new List<PageSoftComboInfo>();
        foreach (SoftInfo info in result)
        {
            PageSoftComboInfo s = new PageSoftComboInfo();
            s.Id = info.SoftID;
            s.Name = info.CnName;
            s.SoftCountPerDay = info.CountPerDay;
            list.Add(s);
        }
        PageSoftComboJson json = new PageSoftComboJson();
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [Serializable]
    public class PageJson
    {
        public int TotalCount;
        public PageInfo[] CurrentPage;
    }

    [Serializable]
    public class PageTemplateJson
    {
        public int TotalCount;
        public TemplateInfo[] CurrentPage;
    }

    [Serializable]
    public class PageSoftComboJson
    {
        public int TotalCount;
        public PageSoftComboInfo[] CurrentPage;
    }

    [Serializable]
    public class PageSoftComboInfo
    {
        public int Id;
        public string Name;
        public int SoftCountPerDay;
    }
    
}


