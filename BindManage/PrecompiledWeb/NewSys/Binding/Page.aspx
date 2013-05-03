<%@ page language="C#" masterpagefile="~/Binding/BindingMaster.master" autoeventwireup="true" inherits="Binding_Default, App_Web_page.aspx.e1b12084" title="捆绑页管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Page.js"></script>
    <asp:ScriptManager ID="pagesm" runat="server" >
        <Services>
            <asp:ServiceReference Path="PageManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="pagemange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

