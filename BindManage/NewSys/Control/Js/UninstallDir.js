

PPLive.PPInstall.UninstallDirManage=function(){
    var ppem;
    var grid;
    var sid;
    return{
        pageInit:function(em){
            ppem=em;
            sid = ppem.getParameterByName(window.location.href,'softid');
            this.gridInit();
            this.pageLayout();
            this.loadData();
            ppem.editInit(grid,sid);
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'UninstallDirManage.asmx',methodName:'GetUninstallDirById'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ID'
                },[
                    'ID','SoftwareID','DelDir','DelFolder','DelValue'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'string',  dataIndex: 'DelDir'},
				    {type: 'string',  dataIndex: 'DelFolder'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'软件目标目录',width:200,sortable:true,dataIndex:'DelDir'}, 
                {header:'软件目标文件夹',width:200,sortable:true,dataIndex:'DelFolder'},
                {header:'默认值',width:150,sortable:true,dataIndex:'DelValue'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'uninstalldirmanage_grid_div',
                id:'uninstalldirmanage_grid',
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
            grid.store.load({params:{start: 0, limit: ppem.pageSize,sort:'',dir:'',filters:[],inputFilters:[],softId:sid}});
        }
    }
}();

PPLive.PPInstall.UninstallDirEdit=function(){
    var pp;
    var grid;
    var keys;
    var sid;
    return{
        editInit:function(gridPanel,softId){
            pp=this;
            grid=gridPanel;
            sid=softId;
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
                    height:140,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件目标目录',
                            allowBlank:false,
                            id:'undirdir'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件目标文件夹',
                            allowBlank:false,
                            id:'undirfolder'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '默认值',
                            id:'undirvalue'
                        }
                    ],
                    buttons:[
                        {text:'确定',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:425,
                    id:'uninstalldirwin',
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
                pp.initPanel('edit','编辑',record.data.ID);
                Ext.getCmp('undirdir').setValue(record.data.DelDir);
                Ext.getCmp('undirfolder').setValue(record.data.DelFolder);
                Ext.getCmp('undirvalue').setValue(record.data.DelValue);
            }
        },
        operateHandler:function(o,id){
            var undirdir = Ext.getCmp('undirdir');
            var undirfolder = Ext.getCmp('undirfolder');
            if(undirdir.validate()&&undirfolder.validate())
            {
                
                var undirvalue = Ext.getCmp('undirvalue').getValue();
                if(o=='add')
                    UninstallDirManage.AddUninstallDir(sid,undirdir.getValue(),undirfolder.getValue(),undirvalue,pp.callbackSucess,pp.callbackError);
                else
                    UninstallDirManage.EditUninstallDir(id,sid,undirdir.getValue(),undirfolder.getValue(),undirvalue,pp.callbackSucess,pp.callbackError);
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
                UninstallDirManage.DeleteUninstallDirs(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('uninstalldirwin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.UninstallDirEdit,PPLive.PPInstall.EditBase);

window.onload=function(){
    Ext.onReady(
        PPLive.PPInstall.UninstallDirManage.pageInit.createDelegate(
            PPLive.PPInstall.UninstallDirManage,
            [PPLive.PPInstall.UninstallDirEdit]
        )
    );
}










