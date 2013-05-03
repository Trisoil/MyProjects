<%@ Page Language="C#" MasterPageFile="~/Control/ProductMaster.master" AutoEventWireup="true" CodeFile="Custom.aspx.cs" Inherits="Product_Default" Title="定制信息管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="../SwfUpload/swfupload.js"></script>
    <script type="text/javascript" src="../SwfUpload/swfupload.speed.js"></script>
    <script type="text/javascript" src="../SwfUpload/mode.js"></script>
    <script type="text/javascript" src="../SwfUpload/handlers.js"></script>
    <script type="text/javascript" src="Js/UploadGrid.js"></script>
    <script type="text/javascript" src="Js/Custom.js"></script>
    <asp:ScriptManager ID="customsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="CustomManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <asp:HiddenField ID="curusername" runat="server" />
    <div id="custommanage_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

