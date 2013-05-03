using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

/// <summary>
/// 用户填充ExtGrid的Json结构
/// </summary>

[Serializable]
public class CommonJson<T> where T:new()
{
    public int TotalCount;
    public T[] CurrentPage;
}
