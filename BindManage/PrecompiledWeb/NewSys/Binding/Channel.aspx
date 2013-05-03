<%@ page language="C#" masterpagefile="~/Binding/BindingMaster.master" autoeventwireup="true" inherits="Binding_Default, App_Web_channel.aspx.e1b12084" title="推广渠道管理" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolder1" Runat="Server">
    <script type="text/javascript" src="Js/Channel.js"></script>
    <asp:ScriptManager ID="channelsm" runat="server" >
        <Services>
            <asp:ServiceReference Path="ChannelManage.asmx" />
        </Services>
    </asp:ScriptManager>
    <div id="channelmange_grid_div" style="width:100%; height:100%; position:absolute;"></div>
</asp:Content>

