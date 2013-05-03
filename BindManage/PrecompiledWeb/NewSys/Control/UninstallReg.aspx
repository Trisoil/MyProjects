<%@ page language="C#" masterpagefile="~/Control/ProductMaster.master" autoeventwireup="true" inherits="Control_Default, App_Web_uninstallreg.aspx.cc961b29" title="卸载软件注册表管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/UninstallReg.js"></script>
    <asp:ScriptManager ID="uninstallregsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="UninstallRegManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="uninstallregmanage_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

