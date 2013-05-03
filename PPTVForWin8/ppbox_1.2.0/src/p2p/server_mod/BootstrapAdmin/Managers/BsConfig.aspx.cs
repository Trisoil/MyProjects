using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

public partial class Managers_BsConfig : System.Web.UI.Page
{
    private static string _fileName;
    public static string FileName
    {
        get
        {
            if (string.IsNullOrEmpty(_fileName))
            {
                _fileName = string.Format("{0}Services/BsConfig.txt", HttpContext.Current.Request.PhysicalApplicationPath);
            }
            return _fileName;
        }
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!IsPostBack)
        {
            try
            {
                this.TextBox1.Text = System.IO.File.ReadAllText(FileName);
                this.ltlMessage.Text = "配置文件读取完成。";
            }
            catch (Exception E)
            {
                this.ltlMessage.Text = string.Format("<div style='color:red'>异常信息: {0}, {1}</div>", E.GetType().Name, E.Message);
            }
        }
    }

    protected void btReset_Click(object sender, EventArgs e)
    {
        try
        {
            this.TextBox1.Text = System.IO.File.ReadAllText(FileName, System.Text.Encoding.UTF8);
            this.ltlMessage.Text = "配置文件读取完成。";
        }
        catch (Exception E)
        {
            this.ltlMessage.Text = string.Format("<div style='color:red'>异常信息: {0}, {1}</div>", E.GetType().Name, E.Message);
        }
    }

    protected void btConfirm_Click(object sender, EventArgs e)
    {
        try
        {
            System.IO.File.WriteAllText(FileName, this.TextBox1.Text, System.Text.Encoding.UTF8);
            this.ltlMessage.Text = "配置文件保存完成。";
        }
        catch (Exception E)
        {
            this.ltlMessage.Text = string.Format("<div style='color:red'>异常信息: {0}, {1}</div>", E.GetType().Name, E.Message);
        }
    }

}
