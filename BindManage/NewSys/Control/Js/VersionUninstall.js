

PPLive.PPInstall.VersionUninstallManage=function(){
    var ppem;
    var grid;
    var vid;
    var pp;
    return{
        pageInit:function(em){
            ppem=em;
            pp=this;
            vid = ppem.getParameterByName(window.location.href,'versionid');
            this.gridInit();
            this.pageLayout();
            this.loadData();
            ppem.editInit(grid,vid);
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'VersionUninstallManage.asmx',methodName:'GetVersionUninstallById'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ID'
                },[
                    'ID','VersionID','SoftwareID','Name','UninstallName','KillExe','ForceUninstall','IsMust','AppPath'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'numeric',  dataIndex: 'SoftCount'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'软件名称',width:120,sortable:true,dataIndex:'Name'}, 
                {header:'卸载目录匹配名称',width:120,sortable:true,dataIndex:'UninstallName'}, 
                {header:'终止进程名',width:200,sortable:true,dataIndex:'KillExe',renderer:ppem.showTip},
                {header:'是否强制卸载',width:120,sortable:true,dataIndex:'ForceUninstall',renderer:pp.showForceUninstall},
                {header:'是否必须卸载',width:120,sortable:true,dataIndex:'IsMust',renderer:pp.showIsMust},
                {header:'卸载删除目录',width:120,sortable:true,dataIndex:'AppPath'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'versionuninstallmanage_grid_div',
                id:'versionuninstallmanage_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnRefresh,'-'],
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
					            {text:'删除',iconCls:'remove',listeners:{click:ppem.deleteHandler.createDelegate(this,[rowindex])}},
					            {text:'刷新',iconCls:'refresh',listeners:{click:ppem.refreshHandler}}
					        ]
				        });
		              e.preventDefault();
		              menu.showAt(e.getXY());
                    }
                },
                buttons:[
                    {text:'关闭',handler:function(){window.parent.Ext.Dialog.close('OK');}}
                ]
            });
        },
        pageLayout:function(){
            var viewport=new Ext.Viewport({layout:'border',items:[grid]});
        },
        loadData:function(){
            grid.render();
            grid.store.load({params:{start: 0, limit: ppem.pageSize,sort:'',dir:'',filters:[],inputFilters:[],versionId:vid}});
        },
        showForceUninstall:function(value, meta, rec, rowIdx, colIdx, ds){
            switch(value)
            {
                case 0:
                    return '不强制';
                case 1:
                    return '强制';
                case 2:
                    return '先普通后强制';
                default:
                    return value;
            }
        },
        showIsMust:function(value, meta, rec, rowIdx, colIdx, ds){
            switch(value)
            {
                case 0:
                    return '否';
                case 1:
                    return '是';
                default:
                    return value
            }
        }
    }
}();

PPLive.PPInstall.VersionUninstallEdit=function(){
    var pp;
    var grid;
    var keys;
    var vid;
    return{
        editInit:function(gridPanel,versionid){
            pp=this;
            grid=gridPanel;
            vid=versionid;
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
                    width:420,
                    height:80,
                    deferredRender : false,
                    labelWidth:125,
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
                                    width:130,
                                    text:'卸载软件名称：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'versionuninstallcombo',
                                    allowBlank:false,
                                    fieldLabel: '卸载软件名称',
                                    id:'versionuninstallcombo',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'Name',
                                    editable:false,
                                    valueField:'SoftwareID',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'VersionUninstallManage.asmx',methodName:'GetAllUninstallSoft'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'SoftwareID'},[{name:'SoftwareID'},{name:'Name'}])
                                    })}
                                ]
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnversionunconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:425,
                    id:'versionuninstallwin',
                    height:100,
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
                pp.initPanel('edit','编辑',record.data.ID);
                Ext.getCmp('versionuninstallcombo').store.on('load',function(){
                    Ext.getCmp('versionuninstallcombo').setValue(record.data.SoftwareID);
                });
                //Ext.getCmp('versionuninstallcombo').setValue(record.data.SoftwareID);
                //Ext.getDom('versionuninstallcombo').value=record.data.Name;
            }
        },
        operateHandler:function(o,id){
            var unname = Ext.getCmp('versionuninstallcombo');
            if(unname.validate())
            {
                if(o=='add')
                    VersionUninstallManage.AddVersionUninstall(vid,unname.getValue(),pp.callbackSucess,pp.callbackError);
                else
                    VersionUninstallManage.EditVersionUninstall(id,vid,unname.getValue(),pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnversionunconfirm');
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
                VersionUninstallManage.DeleteVersionUninstalls(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('versionuninstallwin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.VersionUninstallEdit,PPLive.PPInstall.EditBase);

window.onload=function(){
    Ext.onReady(
        PPLive.PPInstall.VersionUninstallManage.pageInit.createDelegate(
            PPLive.PPInstall.VersionUninstallManage,
            [PPLive.PPInstall.VersionUninstallEdit]
        )
    );
}










