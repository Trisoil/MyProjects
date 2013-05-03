<%@ page language="C#" masterpagefile="~/Control/ProductMaster.master" autoeventwireup="true" inherits="Control_Default, App_Web_uninstalldir.aspx.cc961b29" title="卸载软件目录信息" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/UninstallDir.js"></script>
    <asp:ScriptManager ID="uninstalldirsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="UninstallDirManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="uninstalldirmanage_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

