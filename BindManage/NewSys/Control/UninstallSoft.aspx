<%@ Page Language="C#" MasterPageFile="~/Control/ProductMaster.master" AutoEventWireup="true" CodeFile="UninstallSoft.aspx.cs" Inherits="Control_Default" Title="卸载软件管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/UninstallSoft.js"></script>
    <asp:ScriptManager ID="uninstallsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="UninstallSoftManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="uninstallmanage_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

