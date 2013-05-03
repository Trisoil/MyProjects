<%@ Page Language="C#" MasterPageFile="~/Binding/BindingMaster.master" AutoEventWireup="true" CodeFile="Channel.aspx.cs" Inherits="Binding_Default" Title="推广渠道管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Channel.js"></script>
    <asp:ScriptManager ID="channelsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="ChannelManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="channelmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

