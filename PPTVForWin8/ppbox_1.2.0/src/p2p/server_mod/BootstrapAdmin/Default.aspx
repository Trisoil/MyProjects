<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/Managers/IndexServers.aspx"
            Target="_blank">IndexServers</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink2" runat="server" NavigateUrl="~/Managers/StunServers.aspx"
            Target="_blank">StunServers</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink3" runat="server" NavigateUrl="~/Managers/TrackerCooperators.aspx"
            Target="_blank">TrackerCooperators</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink4" runat="server" NavigateUrl="~/Managers/TrackerServers.aspx"
            Target="_blank">TrackerServers</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink9" runat="server" NavigateUrl="~/Managers/BsConfig.aspx"
            Target="_blank">BsConfig</asp:HyperLink>
        <br />
        <br />
        <asp:HyperLink ID="HyperLink5" runat="server" NavigateUrl="~/Services/IndexServers.ashx"
            Target="_blank">配置:IndexServers</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink6" runat="server" NavigateUrl="~/Services/StunServers.ashx"
            Target="_blank">配置:StunServers</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink7" runat="server" NavigateUrl="~/Services/TrackerServers.ashx"
            Target="_blank">配置:TrackerServers</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink8" runat="server" NavigateUrl="~/Services/CooperatorIPs.ashx"
            Target="_blank">配置:CooperatorIPs</asp:HyperLink>
        <br />
        <asp:HyperLink ID="HyperLink10" runat="server" NavigateUrl="~/Services/Bsconfig.ashx"
            Target="_blank">配置:BsConfig</asp:HyperLink>
    </div>
    </form>
</body>
</html>
