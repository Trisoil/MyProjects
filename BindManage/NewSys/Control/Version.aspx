<%@ Page Language="C#" MasterPageFile="~/Control/ProductMaster.master" AutoEventWireup="true" CodeFile="Version.aspx.cs" Inherits="Product_Default" Title="版本控制信息管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Version.js"></script>
    <asp:ScriptManager ID="versionsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="VersionManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="versionmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

