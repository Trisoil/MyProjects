<%@ page language="C#" masterpagefile="~/Control/ProductMaster.master" autoeventwireup="true" inherits="Control_Default, App_Web_guide.aspx.cc961b29" title="引导包管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <link rel="stylesheet" type="text/css" href="MuliteSelect/multiselect.css"/>
    <script type="text/javascript" src="MuliteSelect/DDView.js"></script>
    <script type="text/javascript" src="MuliteSelect/MultiSelect.js"></script>
    <script type="text/javascript" src="MuliteSelect/ItemSelector.js"></script>
    <script type="text/javascript" src="Js/Guide.js"></script>
    <asp:ScriptManager ID="productsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="GuideManage.asmx" />
            <asp:ServiceReference Path="VersionManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="guidemanage_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

