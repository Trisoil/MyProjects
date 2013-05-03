<%@ page language="C#" masterpagefile="~/Binding/BindingMaster.master" autoeventwireup="true" inherits="Binding_Default, App_Web_soft.aspx.e1b12084" title="捆绑软件管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/TreeCheckNodeUI.js"></script>
    <script type="text/javascript" src="Js/PinyinFilter.js"></script>
    <script type="text/javascript" src="Js/TreeFilter.js"></script>
    <script type="text/javascript" src="Js/Soft.js"></script>
    <asp:ScriptManager ID="softsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="SoftManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="softmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

