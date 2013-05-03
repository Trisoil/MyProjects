<%@ Page Language="C#" AutoEventWireup="true" CodeFile="TrackerCooperators.aspx.cs"
    Inherits="Manager_Cooperators" %>

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
            CssFilePath="~/App_Themes/Glass/{0}/styles.css" CssPostfix="Glass" KeyFieldName="CooperatorId">
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
                    <EditButton Visible="True" Text="编辑">
                    </EditButton>
                    <NewButton Visible="True" Text="新建">
                    </NewButton>
                    <DeleteButton Visible="True" Text="删除">
                    </DeleteButton>
                    <ClearFilterButton Visible="True" Text="重置过滤">
                    </ClearFilterButton>
                </dxwgv:GridViewCommandColumn>
                <dxwgv:GridViewDataTextColumn FieldName="CooperatorId" Caption="地域码" Width="40px"
                    VisibleIndex="1" ReadOnly="true">
                    <EditFormSettings Visible="False" />
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataComboBoxColumn FieldName="CooperatorName" Caption="合作商(机房名称)"
                    Width="120px" VisibleIndex="2">
                    <PropertiesComboBox DataSourceID="sdsServerRooms" ValueField="GroupName" TextField="GroupName" EnableIncrementalFiltering="true">
                    </PropertiesComboBox>
                    <DataItemTemplate>
                        <%# Eval("CooperatorName") %>
                    </DataItemTemplate>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataTextColumn FieldName="UpPicPb" VisibleIndex="3" Width="80px">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataTextColumn FieldName="CollectPb" VisibleIndex="4" Width="80px">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="DacReportMinute" VisibleIndex="5" Width="80px">
                </dxwgv:GridViewDataSpinEditColumn>
            </Columns>
        </dxwgv:ASPxGridView>
        <asp:SqlDataSource ID="sds" runat="server" ConnectionString="<%$ ConnectionStrings:BootStrapServers %>"
            SelectCommand="SELECT [CooperatorId], [CooperatorName], [UpPicPb], [CollectPb], [DacReportMinute] FROM [bss_TrackerCooperators]"
            DeleteCommand="DELETE FROM bss_TrackerCooperators WHERE (CooperatorId = @CooperatorId)"
            InsertCommand="INSERT INTO bss_TrackerCooperators(CooperatorName, UpPicPb, CollectPb, DacReportMinute) VALUES (@CooperatorName, @UpPicPb, @CollectPb, @DacReportMinute)"
            UpdateCommand="UPDATE bss_TrackerCooperators SET CooperatorName = @CooperatorName, UpPicPb = @UpPicPb, CollectPb = @CollectPb, DacReportMinute = @DacReportMinute WHERE (CooperatorId = @CooperatorId)">
            <DeleteParameters>
                <asp:Parameter Name="CooperatorId" />
            </DeleteParameters>
            <UpdateParameters>
                <asp:Parameter Name="CooperatorName" />
                <asp:Parameter Name="UpPicPb" />
                <asp:Parameter Name="CollectPb" />
                <asp:Parameter Name="DacReportMinute" />
                <asp:Parameter Name="CooperatorId" />
            </UpdateParameters>
            <InsertParameters>
                <asp:Parameter Name="CooperatorName" />
                <asp:Parameter Name="UpPicPb" />
                <asp:Parameter Name="CollectPb" />
                <asp:Parameter Name="DacReportMinute" />
            </InsertParameters>
        </asp:SqlDataSource>
        <asp:SqlDataSource ID="sdsServerRooms" runat="server" ConnectionString="<%$ ConnectionStrings:OpenService_Files %>"
          ProviderName="<%$ ConnectionStrings:OpenService_Files.ProviderName %>" SelectCommand="select GroupName from ServerGroup order by GroupName"></asp:SqlDataSource>
    </div>
    </form>
</body>
</html>
