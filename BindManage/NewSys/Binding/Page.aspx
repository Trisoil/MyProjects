<%@ Page Language="C#" MasterPageFile="~/Binding/BindingMaster.master" AutoEventWireup="true" CodeFile="Page.aspx.cs" Inherits="Binding_Default" Title="捆绑页管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Page.js"></script>
    <asp:ScriptManager ID="pagesm" runat="server" >
        <Services>
            <asp:ServiceReference Path="PageManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="pagemange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

