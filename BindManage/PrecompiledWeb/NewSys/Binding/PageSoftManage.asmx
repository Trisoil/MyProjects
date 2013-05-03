<%@ WebService Language="C#" Class="PageSoftManage" %>

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
/// PageSoftManage ��ժҪ˵��
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class PageSoftManage : System.Web.Services.WebService
{

    static string _eventFrom = "�������������Ϣ";
    HistoryRecordManage _hr;
    
    public PageSoftManage()
    {

        //���ʹ����Ƶ��������ȡ��ע�������� 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public PageSoftJson GetPageSoftById(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters, int pageId)
    {
        filters.AddRange(inputFilters);
        string where = string.Format(" and PageID={0}",pageId);
        PageSoftJson json = new PageSoftJson();
        json.CurrentPage = PageSoftInfo.Execute(new SqlQuery<PageSoftInfo>("*", start, limit, sort, dir, PageSoftInfo.ViewName, PageSoftInfo.PrimaryKey, filters.ToArray(), where)) as PageSoftInfo[];
        json.TotalCount = (int)PageSoftInfo.Execute(new SqlCount<PageSoftInfo>(start, limit, sort, dir, PageSoftInfo.ViewName, PageSoftInfo.PrimaryKey, filters.ToArray(), where));
        return json;
    }

    [WebMethod]
    public PageSoftJson GetAllPageSoftById(int pageId)
    {
        string sql = string.Format("select * from {0} where PageID={1} order by Rank asc", PageSoftInfo.ViewName, pageId);
        PageSoftJson json = new PageSoftJson();
        json.CurrentPage = PageSoftInfo.Execute(new SqlCommonQuery<PageSoftInfo>(sql)) as PageSoftInfo[];
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public void AddPageSoft(int pageId, int softId, int count,string forceName,string forceDes)
    {
        JudgeSoft(pageId, softId);
        JudgeSoftCount(pageId, softId);
        JudgeSoftType(pageId, softId);
        PageSoftInfo info = new PageSoftInfo();
        info.PageID = pageId;
        info.SoftID = softId;
        info.CPDOfChannel = count;
        info.ForceName = forceName;
        info.ForceDescription = forceDes;
        info.Save();
        UpdateSoftCPDAdd(softId,pageId,count);
        string eventLog = string.Format("������һ�����������Ϣ,����ҳ��ʶ��Ϊ{0},���������ʶ��Ϊ{1}", pageId, softId);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditPageSoft(int id, int pageId, int softId, int count,string forceName, string forceDes)
    {
        PageSoftInfo info = PageSoftInfo.FindById(id);
        if (info != null)
        {
            if(info.SoftID != softId)
                JudgeSoft(pageId, softId);
            JudgeSoftType(pageId, softId);
            UpdateSoftCPD(softId, info.SoftID, count,info.CPDOfChannel);
            info.PageID = pageId;
            info.SoftID = softId;
            info.CPDOfChannel = count;
            info.ForceName = forceName;
            info.ForceDescription = forceDes;
            info.Save();
            string eventLog = string.Format("�༭��һ���������������Ϣ,����ҳ��ʶ��Ϊ{0},���������ʶ��Ϊ{1}", pageId, softId);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeletePageSofts(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            PageSoftInfo info = PageSoftInfo.FindById(id);
            if (info != null)
            {
                UpdateSoftCPDDes(info.SoftID,info.PageID,info.CPDOfChannel);
                builder.Append(info.PageID);
                builder.Append("/");
                builder2.Append(info.SoftID);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("ɾ����{0}���������������Ϣ,����ҳ��ʶ��Ϊ{1},���������ʶ��Ϊ{2}",ids.Length, builder.ToString(), builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void RankPageSoft(int[] ids)
    {
        for (int i = 0; i < ids.Length; i++)
        {
            PageSoftInfo info = PageSoftInfo.FindById(ids[i]);
            if (info != null)
            {
                info.Rank = i + 1;
                info.Save();
            }
        }
    }

    private void JudgeSoft(int pageId, int softId)
    {
        string sql = string.Format("select count(*) from {0} where PageID={1} and SoftID={2}",PageSoftInfo.TableName,pageId,softId);
        int count = (int)PageSoftInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("�Ѿ�������ͬ��������ļ�¼����ѡ�����������");
    }

    private void JudgeSoftCount(int pageId, int softId)
    {
        string temsql = string.Format("select TemplateSoftCount from {0} where PageID={1}", PageInfo.ViewName, pageId);
        int temcount = (int)PageInfo.ExecuteScalarBySql(temsql);
        if (temcount == 0)
            return;
        string pagesql = string.Format("select count(*) from {0} where PageID={1} ", PageSoftInfo.TableName, pageId);
        int pagecount = (int)PageSoftInfo.ExecuteScalarBySql(pagesql);
        if (pagecount >= temcount)
            throw new ApplicationException(string.Format("��ӵ�������ѳ���ģ��ҳ����������������������Ϊ{0}", temcount));
    }

    private void JudgeSoftType(int pageId, int softId)
    {
        string temsql = string.Format("select TemplateType from {0} where PageID={1}", PageInfo.ViewName, pageId);
        int temtype = int.Parse(PageInfo.ExecuteScalarBySql(temsql).ToString());
        if (temtype == 3)
        {
            SoftInfo soft = SoftInfo.FindById(softId);
            if (soft.Type != 1)
                throw new ApplicationException("ģ��Cֻ�������������ѡ�����������");
        }
    }
    
    private void UpdateSoftCPDAdd(int softId,int pageId,int count)
    {
        SoftInfo info = SoftInfo.FindById(softId);
        if (info != null)
        {
            info.AllCPDOfChannel += count;
            info.Save();
        }
        PageInfo page = PageInfo.FindById(pageId);
        if (page != null)
        {
            TemplateInfo temp = TemplateInfo.FindById(page.TemplateID);
            if (temp != null && temp.Type == 0)
            {
                BindInfo bind = BindInfo.FindById(page.BindID);
                if (bind != null)
                {
                    bind.ForceCount += 1;    //����ǿ�����+1
                    bind.Save();
                }
            }
            page.SoftCount += 1;   
            page.Save();
        }
    }

    private void UpdateSoftCPDDes(int softId, int pageId,int count)
    {
        SoftInfo info = SoftInfo.FindById(softId);
        if (info != null)
        {
            info.AllCPDOfChannel -= count;
            info.Save();
        }
        PageInfo page = PageInfo.FindById(pageId);
        if (page != null)
        {
            TemplateInfo temp = TemplateInfo.FindById(page.TemplateID);
            if (temp != null && temp.Type == 0)
            {
                BindInfo bind = BindInfo.FindById(page.BindID);
                if (bind != null)
                {
                    bind.ForceCount -= 1;    //����ǿ�����+1
                    bind.Save();
                }
            }
            page.SoftCount -= 1;
            page.Save();
        }
    }

    private void UpdateSoftCPD(int newId, int oldId, int newcount,int oldcount)
    {
        SoftInfo newinfo = SoftInfo.FindById(newId);
        if (newinfo != null)
        {
            newinfo.AllCPDOfChannel += newcount;
            newinfo.Save();
        }
        SoftInfo oldinfo = SoftInfo.FindById(oldId);
        if (oldinfo != null)
        {
            oldinfo.AllCPDOfChannel -= oldcount;
            oldinfo.Save();
        }
    }
    
    [Serializable]
    public class PageSoftJson
    {
        public int TotalCount;
        public PageSoftInfo[] CurrentPage;
    }

}


