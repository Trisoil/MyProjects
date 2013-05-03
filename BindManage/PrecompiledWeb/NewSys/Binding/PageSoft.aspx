<%@ page language="C#" masterpagefile="~/Binding/BindingMaster.master" autoeventwireup="true" inherits="Binding_Default, App_Web_pagesoft.aspx.e1b12084" title="捆绑软件与捆绑页关系" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/PageSoft.js"></script>
    <asp:ScriptManager ID="pagesoftsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="PageSoftManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="pagesoftmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

