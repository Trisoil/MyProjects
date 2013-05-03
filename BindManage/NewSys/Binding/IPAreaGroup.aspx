<%@ Page Language="C#" MasterPageFile="~/Binding/BindingMaster.master" AutoEventWireup="true" CodeFile="IPAreaGroup.aspx.cs" Inherits="Binding_Default" Title="Untitled Page" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/TreeCheckNodeUI.js"></script>
    <script type="text/javascript" src="Js/PinyinFilter.js"></script>
    <script type="text/javascript" src="Js/TreeFilter.js"></script>
    <script type="text/javascript" src="Js/IPAreaGroup.js"></script>
    <asp:ScriptManager ID="softsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="IPAreaGroupManage.asmx" />
            <asp:ServiceReference Path="SoftManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="ipareagroup_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

