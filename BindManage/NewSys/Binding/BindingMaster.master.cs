using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

public partial class Binding_BindingMaster : System.Web.UI.MasterPage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        string css = null;
        string src = Request["AudiStr"];
        if (!string.IsNullOrEmpty(src))
        {
            css = src.Split('$')[2];
            Session["extcss"] = css;
        }
        if (string.IsNullOrEmpty(css))
            css = Session["extcss"] as string;
        string script = string.Format("extCss = '{0}';", css);
        this.Page.ClientScript.RegisterStartupScript(this.GetType(), "extcss", script, true);
    }
}
