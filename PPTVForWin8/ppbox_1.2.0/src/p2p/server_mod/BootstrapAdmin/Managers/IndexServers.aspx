<%@ Page Language="C#" AutoEventWireup="true" CodeFile="IndexServers.aspx.cs" Inherits="IndexServers" %>

<%@ Register Assembly="DevExpress.Web.ASPxGridView.v8.1, Version=8.1.3.0, Culture=neutral, PublicKeyToken=9b171c9fd64da1d1"
    Namespace="DevExpress.Web.ASPxGridView" TagPrefix="dxwgv" %>
<%@ Register Assembly="DevExpress.Web.ASPxEditors.v8.1, Version=8.1.3.0, Culture=neutral, PublicKeyToken=9b171c9fd64da1d1"
    Namespace="DevExpress.Web.ASPxEditors" TagPrefix="dxe" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <dxwgv:ASPxGridView ID="agv" runat="server" DataSourceID="sds" AutoGenerateColumns="False"
            CssFilePath="~/App_Themes/Glass/{0}/styles.css" CssPostfix="Glass" KeyFieldName="ServerId">
            <Styles CssFilePath="~/App_Themes/Glass/{0}/styles.css" CssPostfix="Glass">
                <Header ImageSpacing="5px" SortingImageSpacing="5px">
                </Header>
            </Styles>
            <Images ImageFolder="~/App_Themes/Glass/{0}/">
                <CollapsedButton Height="12px" Width="11px" />
                <DetailCollapsedButton Height="9px" Width="9px" />
                <PopupEditFormWindowClose Height="17px" Width="17px" />
            </Images>
            <Settings ShowFilterRow="True" ShowGroupPanel="True" />
            <SettingsEditing Mode="Inline" />
            <SettingsPager PageSize="30" />
            <Columns>
                <dxwgv:GridViewCommandColumn VisibleIndex="0" Width="80px">
                    <EditButton Visible="true" Text="编辑">
                    </EditButton>
                    <NewButton Visible="True" Text="新建">
                    </NewButton>
                    <DeleteButton Visible="True" Text="删除">
                    </DeleteButton>
                    <ClearFilterButton Visible="True" Text="重置过滤">
                    </ClearFilterButton>
                </dxwgv:GridViewCommandColumn>
                <dxwgv:GridViewDataTextColumn FieldName="ServerId" VisibleIndex="1" Visible="false"
                    ReadOnly="True">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="ModMin" Caption="起始模" VisibleIndex="2"
                    Width="40px">
                </dxwgv:GridViewDataSpinEditColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="ModMax" Caption="结束模" VisibleIndex="3"
                    Width="40px">
                </dxwgv:GridViewDataSpinEditColumn>
                <dxwgv:GridViewDataComboBoxColumn FieldName="Protocol" Caption="协议" Width="40px"
                    VisibleIndex="4">
                    <PropertiesComboBox>
                        <Items>
                            <dxe:ListEditItem Value="udp" Text="udp" />
                            <dxe:ListEditItem Value="tcp" Text="tcp" />
                        </Items>
                    </PropertiesComboBox>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataTextColumn FieldName="IP" VisibleIndex="5" Width="120px">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="Port" Caption="端口" VisibleIndex="6"
                    Width="40px">
                </dxwgv:GridViewDataSpinEditColumn>
            </Columns>
        </dxwgv:ASPxGridView>
        <asp:SqlDataSource ID="sds" runat="server" ConnectionString="<%$ ConnectionStrings:BootStrapServers %>"
            SelectCommand="SELECT [ServerId], [ModMin], [ModMax], [Protocol], [IP], [Port] FROM [bss_SimpleIndexServers]"
            InsertCommand="INSERT INTO bss_SimpleIndexServers(Protocol, IP, Port, ModMin, ModMax)VALUES(@Protocol, @IP, @Port, @ModMin, @ModMax)"
            UpdateCommand="UPDATE [bss_SimpleIndexServers] SET ModMin = @ModMin, ModMax = @ModMax, Protocol = @Protocol, IP = @IP, Port = @Port WHERE ServerId = @ServerId"
            DeleteCommand="DELETE FROM [bss_SimpleIndexServers] WHERE ServerId = @ServerId">
            <InsertParameters>
                <asp:Parameter Name="Protocol" />
                <asp:Parameter Name="IP" />
                <asp:Parameter Name="Port" />
                <asp:Parameter Name="ModMin" />
                <asp:Parameter Name="ModMax" />
            </InsertParameters>
            <UpdateParameters>
                <asp:Parameter Name="Protocol" />
                <asp:Parameter Name="IP" />
                <asp:Parameter Name="Port" />
                <asp:Parameter Name="ModMin" />
                <asp:Parameter Name="ModMax" />
                <asp:Parameter Name="ServerId" />
            </UpdateParameters>
            <DeleteParameters>
                <asp:Parameter Name="ServerId" />
            </DeleteParameters>
        </asp:SqlDataSource>
    </div>
    </form>
</body>
</html>
