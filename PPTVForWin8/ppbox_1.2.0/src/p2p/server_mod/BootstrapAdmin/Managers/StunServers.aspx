<%@ Page Language="C#" AutoEventWireup="true" CodeFile="StunServers.aspx.cs" Inherits="Manager_StunServers" %>

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
                                <dxe:ListEditItem Value="stun" Text="stun" />
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
                    <td>
                    </td>
                    <td>
                        <asp:Button ID="btInsert" runat="server" Text="插入" OnClick="btInsert_Click" />
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
                <dxwgv:GridViewDataComboBoxColumn FieldName="Category" Caption="分类" VisibleIndex="1"
                    Width="200px">
                    <PropertiesComboBox>
                        <Items>
                            <dxe:ListEditItem Value="stun" Text="stun" />
                        </Items>
                    </PropertiesComboBox>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="ModValue" Caption="模值" Width="40px"
                    VisibleIndex="2">
                </dxwgv:GridViewDataSpinEditColumn>
                <dxwgv:GridViewDataComboBoxColumn FieldName="Protocol" Caption="协议" Width="40px"
                    VisibleIndex="3">
                    <PropertiesComboBox>
                        <Items>
                            <dxe:ListEditItem Value="udp" Text="udp" />
                            <dxe:ListEditItem Value="tcp" Text="tcp" />
                        </Items>
                    </PropertiesComboBox>
                </dxwgv:GridViewDataComboBoxColumn>
                <dxwgv:GridViewDataTextColumn FieldName="IP" VisibleIndex="4" Width="120px">
                </dxwgv:GridViewDataTextColumn>
                <dxwgv:GridViewDataSpinEditColumn FieldName="Port" Caption="端口" Width="40px" VisibleIndex="5">
                </dxwgv:GridViewDataSpinEditColumn>
            </Columns>
        </dxwgv:ASPxGridView>
        <asp:SqlDataSource ID="sds" runat="server" ConnectionString="<%$ ConnectionStrings:BootStrapServers %>"
            SelectCommand="SELECT [ServerId], [Category], [ModValue], [Protocol], [IP], [Port] FROM [bss_SimpleStunServers]"
            InsertCommand="INSERT INTO [bss_SimpleStunServers](Protocol, IP, Port, Category, ModValue)VALUES(@Protocol, @IP, @Port, @Category, @ModValue)"
            UpdateCommand="UPDATE [bss_SimpleStunServers] SET Protocol = @Protocol, IP = @IP, Port = @Port, Category = @Category, ModValue = @ModValue WHERE ServerId = @ServerId"
            DeleteCommand="DELETE FROM [bss_SimpleStunServers] WHERE ServerId = @ServerId">
            <InsertParameters>
                <asp:Parameter Name="Protocol" />
                <asp:Parameter Name="IP" />
                <asp:Parameter Name="Port" />
                <asp:Parameter Name="Category" />
                <asp:Parameter Name="ModValue" />
            </InsertParameters>
            <UpdateParameters>
                <asp:Parameter Name="Protocol" />
                <asp:Parameter Name="IP" />
                <asp:Parameter Name="Port" />
                <asp:Parameter Name="Category" />
                <asp:Parameter Name="ModValue" />
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
