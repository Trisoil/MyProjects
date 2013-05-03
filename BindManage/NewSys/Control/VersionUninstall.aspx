<%@ Page Language="C#" MasterPageFile="~/Control/ProductMaster.master" AutoEventWireup="true" CodeFile="VersionUninstall.aspx.cs" Inherits="Control_Default" Title="附加卸载软件" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/VersionUninstall.js"></script>
    <asp:ScriptManager ID="versionuninstallsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="VersionUninstallManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="versionuninstallmanage_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

