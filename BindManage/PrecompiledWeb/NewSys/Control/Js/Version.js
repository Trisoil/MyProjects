
PPLive.PPInstall.VersionManage=function(){
    var ppem;
    var grid;
    return{
        pageInit:function(em){
            ppem=em;
            this.gridInit();
            this.pageLayout();
            this.loadData();
            ppem.editInit(grid);
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'VersionManage.asmx',methodName:'GetAllVersion'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'VersionID'
                },[
                    'VersionID','EndVersion','DownloadUrl','SoftwareCount','CustomID','CustomCnName','CustionEnName','CustomProductID','ProductEnName','ProductCnName'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'string',  dataIndex: 'EndVersion'},
				    {type: 'string',  dataIndex: 'DownloadUrl'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'最低版本号',width:100,sortable:true,dataIndex:'EndVersion'}, 
                {header:'提示下载地址',width:160,sortable:true,dataIndex:'DownloadUrl',renderer:ppem.showTip}, 
                {header:'冲突软件个数',width:100,sortable:true,dataIndex:'SoftwareCount'},
                {header:'所属定制中文名称',width:120,sortable:true,dataIndex:'CustomCnName'},
                {header:'所属定制英文名称',width:120,sortable:true,dataIndex:'CustionEnName'},
                {header:'所属产品中文名称',width:120,sortable:true,dataIndex:'ProductCnName'},
                {header:'所属产品英文名称',width:120,sortable:true,dataIndex:'ProductEnName'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnSoft = new Ext.Button({text:'附加冲突软件',iconCls:'bindPage',handler:ppem.softHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'versionmange_grid_div',
                id:'versionmanage_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnSoft,'-',btnRefresh,'-'],
                bbar: new Ext.PagingToolbar({
                    pageSize: ppem.pageSize,
                    store: store,
                    fitToFrame:true,
                    plugins: filters,
                    displayInfo: true,
                    displayMsg: '{0} - {1}/{2}',
                    emptyMsg: "没有相关记录!"
                }),
                listeners:{
                    rowdblclick:function(g,r,e){
                        ppem.editHandler(r);
                    },
                    contextmenu:function(e){
                        var menu = new Ext.menu.Menu({
					        id: 'basicMenu',
					        minWidth :20,
					        shadow : "sides",
					        items: [
					            {text:'增加',iconCls:'add',listeners:{click:ppem.adddHandler}},
					            {text:'刷新',iconCls:'refresh',listeners:{click:ppem.refreshHandler}}
					        ]
				        });
		              e.preventDefault();
		              menu.showAt(e.getXY());
                    },
                    rowcontextmenu:function(g,rowindex,e){
                        var menu = new Ext.menu.Menu({
					        id: 'rowMenu',
					        minWidth :20,
					        shadow : "sides",
					        items: [
					            {text:'增加',iconCls:'add',listeners:{click:ppem.adddHandler}},
					            {text:'编辑',iconCls:'edit',listeners:{click:ppem.editHandler.createDelegate(this,[rowindex])}},
					            //{text:'删除',iconCls:'remove',listeners:{click:ppem.deleteHandler.createDelegate(this,[rowindex])}},
					            {text:'附加冲突软件',iconCls:'bindPage',listeners:{click:ppem.softHandler.createDelegate(this,[rowindex])}},
					            {text:'刷新',iconCls:'refresh',listeners:{click:ppem.refreshHandler}}
					        ]
				        });
		              e.preventDefault();
		              menu.showAt(e.getXY());
                    }
                }
            });
        },
        pageLayout:function(){
            var viewport=new Ext.Viewport({layout:'border',items:[grid]});
        },
        loadData:function(){
            grid.render();
            grid.store.load({params:{start: 0, limit: ppem.pageSize,sort:'',dir:'',filters:[],inputFilters:[]}});
        }
    }
}();

PPLive.PPInstall.VersionEdit=function(){
    var pp;
    var grid;
    var keys;
    return{
        editInit:function(gridPanel){
            pp=this;
            grid=gridPanel;
        },
        refreshHandler:function(){
            grid.store.reload();
            grid.selModel.clearSelections(); 
        },
        filterHandler:function(){
            var fs=[];
            var cfilter = Ext.getDom('auchannelcbo').value;
            if(cfilter&&cfilter!='全部') fs.push(['string','eq','ChannelType',cfilter]);
            grid.store.reload({params:{start: 0, limit: pp.pageSize,sort:'',dir:'',filters:fs,inputFilters:fs}});
        },
        initPanel:function(operate,title,id){
            var panel = new Ext.form.FormPanel({
                    lableAlign:'left',
                    buttonAlign:'right',   
                    bodyStyle:'padding:5 15 0 5',
                    width:400,
                    height:140,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            layout:'column',
                            columnWidth:.1,
                            //bodyStyle:'padding:1 0 5 0',
                            style:'padding:1px 0px 5px 0px',
                            items:[{
                                    xtype:'label',
                                    fieldLabel:'',
                                    width:100,
                                    text:'所属定制：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'versioncustom',
                                    allowBlank:false,
                                    fieldLabel: '所属定制',
                                    id:'versioncustom',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'Name',
                                    editable:false,
                                    valueField:'ID',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'VersionManage.asmx',methodName:'GetAllCustoms'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'ID'},[{name:'ID'},{name:'Name'}])
                                    })}
                                ]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '最低版本号',
                            id:'versionendversion'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '提示下载地址',
                            id:'versiondownurl'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnversionconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:405,
                    id:'versionwin',
                    height:160,
                    layout:'border',
                    resizable:false,
                    modal:true,
                    items:[{
                        region:'center',
                        items:[panel]
                    }]
                });
                win.setAnimateTarget(Ext.getBody());
                win.show();
        },
        adddHandler:function(){
            pp.initPanel('add','增加');
        },
        editHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                pp.initPanel('edit','编辑',record.data.VersionID);
                Ext.getCmp('versioncustom').store.on('load',function(){
                    Ext.getCmp('versioncustom').setValue(record.data.CustomID);
                });
                //Ext.getCmp('versioncustom').setValue(record.data.CustomID);
                //Ext.getDom('versioncustom').value=String.format('{0}|{1}',record.data.ProductEnName,record.data.CustionEnName);
                Ext.getCmp('versionendversion').setValue(record.data.EndVersion);
                Ext.getCmp('versiondownurl').setValue(record.data.DownloadUrl);
            }
        },
        operateHandler:function(o,id){
            var vc = Ext.getCmp('versioncustom');
            if(vc.validate())
            {
                var vsc = Ext.getCmp('versionendversion').getValue();
                var url = Ext.getCmp('versiondownurl').getValue();
                if(o=='add')
                    VersionManage.AddVersion(vsc,url,vc.getValue(),pp.callbackSucess,pp.callbackError);
                else
                    VersionManage.EditVersion(id,vsc,url,vc.getValue(),pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnversionconfirm');
            }
        },
        deleteHandler:function(r){
            keys = pp.getKeys(grid,r);
            if(keys&&keys.length)
            {
                Ext.MessageBox.confirm('提示','确定要删除选定的'+keys.length+'条记录吗？',pp.deleteSaveHandler);
            }  
        },
        deleteSaveHandler:function(btn){
            if(btn=='yes')
            {
                VersionManage.DeleteVersions(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        softHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var vid=record.data.VersionID;
                Ext.Dialog.init({title:"附加冲突软件",width:document.body.clientWidth*0.75,height:document.body.clientHeight*0.8,fullMask:false,maximizable:true});
                Ext.Dialog.open("VersionUninstall.aspx?versionid="+vid,pp.closeCallback);
            }
        },
        closeCallback:function(returnValue){
//            if(returnValue=='OK')
//            {
                grid.store.reload();
                grid.selModel.clearSelections();
//            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('versionwin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.VersionEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.VersionManage.pageInit.createDelegate(
        PPLive.PPInstall.VersionManage,
        [PPLive.PPInstall.VersionEdit]
    )
);



