<%@ Page Language="C#" AutoEventWireup="true" CodeFile="TrackerServers.aspx.cs" Inherits="Manager_TrackerServers" %>

<%@ Register Assembly="DevExpress.Web.ASPxGridView.v8.1, Version=8.1.3.0, Culture=neutral, PublicKeyToken=9b171c9fd64da1d1"
    Namespace="DevExpress.Web.ASPxGridView" TagPrefix="dxwgv" %>
<%@ Register Assembly="DevExpress.Web.ASPxEditors.v8.1, Version=8.1.3.0, Culture=neutral, PublicKeyToken=9b171c9fd64da1d1"
    Namespace="DevExpress.Web.ASPxEditors" TagPrefix="dxe" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>

    <script type="text/javascript">
        window.selectAll = function(checked) {
            var v = document.getElementsByTagName('span');
            for (var i = 0; i < v.length; i++) {
                v[i].childNodes[0].checked = checked;
            };
        };
        window.search = function() {
            var v = document.getElementById('tbSearch');
            if (!v.value) {
                alert('请输入关键字!');
                return false;
            }
            if (v.value.charAt(0) != '%')
                v.value = '%' + v.value;
            if (v.value.charAt(v.value.length - 1) != '%')
                v.value = v.value + '%';
            return true;
        };
        window.textChanged = function(v) {
            if (window.event.keyCode == 13) {
                return search();
            }
            return true;
        };
    </script>

</head>
<body>
    <form id="form1" runat="server">
    <div>
        <a href="javascript:void(0)" onclick="var v = document.getElementById('dvInsert');v.style.display = v.style.display == 'none'?'block':'none';">
            批量插入 </a>&nbsp;&nbsp;<asp:Button ID="btRemoveAll" runat="server" Text="批量删除" OnClientClick="return confirm('确定删除所选择的行吗？')"
                OnClick="btRemoveAll_Click" />
        <div id="dvInsert" style="display: none">
            <table>
                <tr>
                    <td>
                        <b>分类</b>
                    </td>
                    <td>
                        <dxe:ASPxComboBox ID="acbCategories" runat="server" CssFilePath="~/App_Themes/Glass/{0}/styles.css"
                            CssPostfix="Glass" ImageFolder="~/App_Themes/Glass/{0}/" ValueType="System.String"
                            SelectedIndex="0">
                            <ButtonStyle Cursor="pointer" Width="13px">
                            </ButtonStyle>
                            <ValidationSettings>
                                <ErrorImage Height="14px" Url="~/App_Themes/Glass/Editors/edtError.png" Width="14px" />
                                <ErrorFrameStyle ImageSpacing="4px">
                                    <ErrorTextPaddings PaddingLeft="4px" />
                                </ErrorFrameStyle>
                            </ValidationSettings>
                            <Items>
                                <dxe:ListEditItem Value="tracker_commit" />
                                <dxe:ListEditItem Value="tracker_list.0" />
                                <dxe:ListEditItem Value="tracker_list.1" />
                                <dxe:ListEditItem Value="tracker_list.2" />
                                <dxe:ListEditItem Value="cache.0" />
                                <dxe:ListEditItem Value="cache.1" />
                                <dxe:ListEditItem Value="collecton" />
                                <dxe:ListEditItem Value="notify" />
                                <dxe:ListEditItem Value="live_tracker_list.0" />
                            </Items>
                        </dxe:ASPxComboBox>
                    </td>
                </tr>
                <tr>
                    <td>
                        <b>批量数据</b><br />
                        0 udp 61.155.9.162 8001 # 南京电信<br />
                        0 udp 61.155.8.45 8000 # 南京电信
                    </td>
                    <td>
                        <asp:TextBox ID="tbInsert" runat="server" Height="278px" TextMode="MultiLine" Width="657px"></asp:TextBox>
                    </td>
                </tr>
                <tr>
                    <td colspan="2">
                        <asp:TextBox runat="server" ID="tbSearch" Text="%" Wrap="False" onkeydown="textChanged(this)"></asp:TextBox>
                        <asp:Button runat="server" ID="btSearch" Text="检索合作商" OnClientClick="return search();"
                            CausesValidation="False" />&nbsp;
                        <asp:Button ID="btInsert" runat="server" Text="插入到数据库" OnClick="btInsert_Click" />
                        <asp:ListView ID="lv" runat="server" DataSourceID="sdsSearch">
                            <EmptyDataTemplate>
                                <table cellspacing="0" cellpadding="4" border="0" id="gv" style="color: #333333;
                                    border-collapse: collapse;">
                                    <thead>
                                        <tr style="color: White; background-color: #1C5E55; font-weight: bold;">
                                            <th scope="col" style="width: 5px">
                                                #
                                            </th>
                                            <th scope="col" align="center" style="width: 5px">
                                                <input type="checkbox" onclick="selectAll(this.checked);" />
                                            </th>
                                            <th scope="col">
                                                <b>选择合作商</b>
                                            </th>
                                        </tr>
                                    </thead>
                                    <tbody>
                                        <tr>
                                            <td colspan="4" align="center">
                                                查询结果为空
                                            </td>
                                        </tr>
                                    </tbody>
                                </table>
                            </EmptyDataTemplate>
                            <LayoutTemplate>
                                <table cellspacing="0" cellpadding="4" border="0" id="gv" style="color: #333333;
                                    border-collapse: collapse;">
                                    <thead>
                                        <tr style="color: White; background-color: #1C5E55; font-weight: bold;">
                                            <th scope="col" style="width: 5px">
                                                #
                                            </th>
                                            <th scope="col" align="center" style="width: 5px">
                                                <input type="checkbox" onclick="selectAll(this.checked);" />
                                            </th>
                                            <th scope="col">
                                                <b>选择合作商</b>
                                            </th>
                                        </tr>
                                    </thead>
                                    <tbody runat="server" id="itemPlaceHolder">
                                    </tbody>
                                </table>
                            </LayoutTemplate>
                            <ItemTemplate>
                                <tr style="background-color: #E3EAEB;">
                                    <td align="left">
                                        <%# Container.DisplayIndex + 1 %>
                                    </td>
                                    <td align="left">
                                        <asp:CheckBox runat="server" ID="ckb" ToolTip='<%# Eval("CooperatorId") %>' />
                                    </td>
                                    <td align="left">
                                        <%# Eval("CooperatorName")%>
                                    </td>
                                </tr>
                            </ItemTemplate>
                            <AlternatingItemTemplate>
                                <tr style="background-color: White;">
                                    <td align="left">
                                        <%# Container.DisplayIndex + 1 %>
                                    </td>
                                    <td align="left">
                                        <asp:CheckBox runat="server" ID="ckb" ToolTip='<%# Eval("CooperatorId") %>' />
                                    </td>
                                    <td align="left">
                                        <%# Eval("CooperatorName")%>
                                    </td>
                                </tr>
                            </AlternatingItemTemplate>
                        </asp:ListView>
                        <asp:DataPager ID="DataPager1" runat="server" PagedControlID="lv" PageSize="30">
                            <Fields>
                                <asp:NextPreviousPagerField ButtonType="Button" ShowFirstPageButton="True" ShowNextPageButton="False"
                                    ShowPreviousPageButton="False" />
                                <asp:NumericPagerField />
                                <asp:NextPreviousPagerField ButtonType="Button" ShowLastPageButton="True" ShowNextPageButton="False"
                                    ShowPreviousPageButton="False" />
                            </Fields>
                        </asp:DataPager>
                    </td>
                </tr>
            </table>
        </div>
        <asp:Literal ID="ltlMessage" runat="server"></asp:Literal>
        <dxwgv:ASPxGridView ID="agv" ClientInstanceName="agv" runat="server" DataSourceID="sds" AutoGenerateColumns="False"
            CssFilePath="~/App_Themes/Glass/{0}/styles.css" CssPostfix="Glass" KeyFieldName="ServerId"
            Width="100%">
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
                <%--<dxwgv:GridViewCommandColumn VisibleIndex="0" Width="80px">
                    <EditButton Visible="true" Text="编辑">
                    </EditButton>
                    <NewButton Visible="True" Text="新建">
                    </NewButton>
                    <DeleteButton Visible="True" Text="删除">
                    </DeleteButton>
                    <ClearFilterButton Visible="True" Text="重置过滤">
                    </ClearFilterButton>
                </dxwgv:GridViewCommandColumn>--%>
                <dxwgv:GridViewCommandColumn VisibleIndex="0" Width="5px" ShowSelectCheckbox="true">
                    <HeaderCaptionTemplate>
                        <input id='ckbHeader' type="checkbox" value="全选" onclick="agv.SelectAllRowsOnPage(this.checked);document.getElementById('ckbFooter').checked = this.checked;" />
                    </HeaderCaptionTemplate>
                    <FooterTemplate>
                        <input id='ckbFooter' type="checkbox" value="全选" onclick="agv.SelectAllRowsOnPage(this.checked);document.getElementById('ckbHeader').checked = this.checked;" />
                    </FooterTemplate>
                    <ClearFilterButton Visible="true" Text="重置过滤" />
                </dxwgv:GridViewCommandColumn>
                <dxwgv:GridViewDataTextColumn FieldName="ServerId" VisibleIndex="0" ReadOnly="True"
                    Visible="false">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataComboBoxColumn FieldName="CooperatorId" VisibleIndex="2" Caption="合作商/地域码"
                    Width="120px">
                    <PropertiesComboBox DataSourceID="sdsCooperators" ValueField="CooperatorId" TextField="CooperatorName">
                    </PropertiesComboBox>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataComboBoxColumn FieldName="Category" VisibleIndex="3" Caption="分类"
                    Width="200px">
                    <PropertiesComboBox>
                        <Items>
                            <dxe:ListEditItem Value="tracker_commit" />
                            <dxe:ListEditItem Value="tracker_list.0" />
                            <dxe:ListEditItem Value="tracker_list.1" />
                            <dxe:ListEditItem Value="tracker_list.2" />
                            <dxe:ListEditItem Value="cache.0" />
                            <dxe:ListEditItem Value="cache.1" />
                            <dxe:ListEditItem Value="collecton" />
                            <dxe:ListEditItem Value="notify" />
                            <dxe:ListEditItem Value="live_tracker_list.0" />
                        </Items>
                    </PropertiesComboBox>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="ModValue" VisibleIndex="4" Caption="模值"
                    Width="40px">
                </dxwgv:GridViewDataSpinEditColumn>
                <dxwgv:GridViewDataComboBoxColumn FieldName="Protocol" Caption="协议" Width="40px"
                    VisibleIndex="5">
                    <PropertiesComboBox>
                        <Items>
                            <dxe:ListEditItem Value="udp" Text="udp" />
                            <dxe:ListEditItem Value="tcp" Text="tcp" />
                        </Items>
                    </PropertiesComboBox>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataTextColumn FieldName="IP" VisibleIndex="6" Width="120px">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="Port" VisibleIndex="7" Caption="端口"
                    Width="40px">
                </dxwgv:GridViewDataSpinEditColumn>
            </Columns>
        </dxwgv:ASPxGridView>
        <asp:SqlDataSource ID="sds" runat="server" ConnectionString="<%$ ConnectionStrings:BootStrapServers %>"
            SelectCommand="SELECT [ServerId], [CooperatorId], [CooperatorName], [Category], [ModValue], [Protocol], [IP], [Port] FROM [vw_bss_SimpleTrackerServers] order by [CooperatorId]"
            InsertCommand="INSERT INTO [bss_SimpleTrackerServers](Protocol, IP, Port, CooperatorId, Category, ModValue)VALUES(@Protocol, @IP, @Port, @CooperatorId, @Category, @ModValue)"
            UpdateCommand="UPDATE [bss_SimpleTrackerServers] SET CooperatorId = @CooperatorId, Category = @Category, ModValue = @ModValue, Protocol = @Protocol, IP = @IP, Port = @Port  WHERE ServerId = @ServerId"
            DeleteCommand="DELETE FROM [bss_SimpleTrackerServers] WHERE ServerId = @ServerId">
            <InsertParameters>
                <asp:Parameter Name="Protocol" />
                <asp:Parameter Name="IP" />
                <asp:Parameter Name="Port" />
                <asp:Parameter Name="CooperatorId" />
                <asp:Parameter Name="Category" />
                <asp:Parameter Name="ModValue" />
            </InsertParameters>
            <UpdateParameters>
                <asp:Parameter Name="Protocol" />
                <asp:Parameter Name="IP" />
                <asp:Parameter Name="Port" />
                <asp:Parameter Name="CooperatorId" />
                <asp:Parameter Name="Category" />
                <asp:Parameter Name="ModValue" />
                <asp:Parameter Name="ServerId" />
            </UpdateParameters>
            <DeleteParameters>
                <asp:Parameter Name="ServerId" />
            </DeleteParameters>
        </asp:SqlDataSource>
        <asp:SqlDataSource ID="sdsCooperators" runat="server" ConnectionString="<%$ ConnectionStrings:BootStrapServers %>"
            SelectCommand="SELECT [CooperatorId], [CooperatorName] FROM [bss_TrackerCooperators]">
        </asp:SqlDataSource>
        <asp:SqlDataSource ID="sdsSearch" runat="server" ConnectionString="<%$ ConnectionStrings:BootStrapServers %>"
            SelectCommand="SELECT [CooperatorId], [CooperatorName] FROM [bss_TrackerCooperators]
                WHERE [CooperatorName] like @Key">
            <SelectParameters>
                <asp:ControlParameter ControlID="tbSearch" PropertyName="Text" Name="Key" />
            </SelectParameters>
        </asp:SqlDataSource>
    </div>
    </form>
</body>
</html>
