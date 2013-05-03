<%@ Page Language="C#" MasterPageFile="~/Control/ProductMaster.master" AutoEventWireup="true" CodeFile="Product.aspx.cs" Inherits="Product_Default" Title="旗下产品信息管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Product.js"></script>
    <asp:ScriptManager ID="productsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="ProductManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="productmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

