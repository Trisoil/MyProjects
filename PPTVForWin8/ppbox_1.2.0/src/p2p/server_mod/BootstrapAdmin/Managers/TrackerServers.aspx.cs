using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

public partial class Manager_TrackerServers : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {

    }

    protected void btInsert_Click(object sender, EventArgs e)
    {
        /*
        0 udp          61.155.9.162           8001 # 南京电信
        0 udp          61.155.8.45            8000 # 南京电信
         */
        CheckBox ckb;
        foreach (ListViewItem item in this.lv.Items)
        {
            ckb = item.FindControl("ckb") as CheckBox;
            if (ckb.Checked)
            {
                if (this.acbCategories.SelectedIndex < 0)
                {
                    Page.Response.Write("<div style='color:red'>请选择分类！</div>");
                    return;
                }
                foreach (string line in this.tbInsert.Text.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries))
                {
                    string[] t = line.Split(new char[] { '\t', ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    if (t.Length > 3)
                    {
                        try
                        {
                            int i = DbHelper.BootStrapServers.ExecuteNonQuery(@"INSERT INTO [bss_SimpleTrackerServers](Protocol, IP, Port, CooperatorId, Category, ModValue)
VALUES(@Protocol, @IP, @Port, @CooperatorId, @Category, @ModValue);"
                                , "Protocol", t[1], "IP", t[2], "Port", t[3],
                                "CooperatorId", ckb.ToolTip, "Category", this.acbCategories.SelectedItem.Value, "ModValue", t[0]);
                            if (i > 0)
                            {
                                Page.Response.Write(string.Format("Done:{0},{1},{2},{3}<br/>", t[0], t[1], t[2], t[3]));
                            }
                            else
                            {
                                Page.Response.Write(string.Format("Error:{0},{1},{2},{3}, 数据库执行影响行数不大于 0<br/>", t[0], t[1], t[2], t[3]));
                            }
                        }
                        catch (Exception E)
                        {
                            Page.Response.Write(string.Format("Error: {0},{1}: {2}-{3}-{4}:{5}<br/>", E.GetType().Name, E.Message, t[0], t[1], t[2], t[3]));
                        }
                    }
                }
            }
        }
        this.agv.DataBind();
    }

    protected void btRemoveAll_Click(object sender, EventArgs e)
    {
        List<object> ls = this.agv.GetSelectedFieldValues("ServerId");
        if (ls.Count > 0)
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder();
            foreach (int id in ls)
            {
                sb.AppendFormat(@"DELETE FROM [bss_SimpleTrackerServers] WHERE ServerId = {0};
", id.ToString());
            }
            DbHelper.BootStrapServers.ExecuteNonQuery(sb.ToString());
            this.agv.DataBind();
            this.ltlMessage.Text = string.Empty;
        }
        else
        {
            this.ltlMessage.Text = "<b style='color:red'>请选择要从下线中移除的视频行!</b><br/>";
        }
    }
}
