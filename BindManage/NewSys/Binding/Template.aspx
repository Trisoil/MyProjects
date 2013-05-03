<%@ Page Language="C#" MasterPageFile="~/Binding/BindingMaster.master" AutoEventWireup="true" CodeFile="Template.aspx.cs" Inherits="Binding_Default" Title="捆绑页模版信息管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Template.js"></script>
    <asp:ScriptManager ID="bindsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="TemplateManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="templatemange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

