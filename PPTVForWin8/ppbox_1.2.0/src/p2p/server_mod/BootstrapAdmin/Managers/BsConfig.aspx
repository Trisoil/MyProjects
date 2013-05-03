<%@ Page Language="C#" AutoEventWireup="true" CodeFile="BsConfig.aspx.cs" Inherits="Managers_BsConfig" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div align="center">
        <h3>
            bs配置文件编辑
        </h3>
        <asp:TextBox ID="TextBox1" runat="server" Height="600px" TextMode="MultiLine" Width="700px"></asp:TextBox>
        <br />
        <asp:Literal runat="server" ID="ltlMessage"></asp:Literal>
        <br />
        <asp:Button ID="btReset" runat="server" OnClientClick="return confirm('将重新加载配置内容并替换输入框里的文本，确认吗？')"
            Text="重置文本" onclick="btReset_Click" />
        <asp:Button ID="btConfirm" runat="server" OnClientClick="return confirm('将输入框里的文本覆盖到配置文件并保存，确认吗？')"
            Text="确认更改" onclick="btConfirm_Click" /></div>
    </form>
</body>
</html>
