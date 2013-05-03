<%@ page language="C#" masterpagefile="~/Binding/BindingMaster.master" autoeventwireup="true" inherits="Binding_Default, App_Web_bind.aspx.e1b12084" title="捆绑控制信息管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Bind.js"></script>
    <asp:ScriptManager ID="bindsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="BindManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="bindmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

