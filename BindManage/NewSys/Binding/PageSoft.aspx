<%@ Page Language="C#" MasterPageFile="~/Binding/BindingMaster.master" AutoEventWireup="true" CodeFile="PageSoft.aspx.cs" Inherits="Binding_Default" Title="捆绑软件与捆绑页关系" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/PageSoft.js"></script>
    <asp:ScriptManager ID="pagesoftsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="PageSoftManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="pagesoftmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

