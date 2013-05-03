

PPLive.PPInstall.UninstallSoftManage=function(){
    var ppem;
    var grid;
    var pp;
    return{
        pageInit:function(em){
            pp=this;
            ppem=em;
            this.gridInit();
            this.pageLayout();
            this.loadData();
            ppem.editInit(grid);
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'UninstallSoftManage.asmx',methodName:'GetAllUninstallSoft'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'SoftwareID'
                },[
                    'SoftwareID','Name','UninstallName','KillExe','ForceUninstall','IsMust','AppPath'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'string',  dataIndex: 'Name'},
				    {type: 'string',  dataIndex: 'KillExe'}
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
            var btnDir = new Ext.Button({text:'附加软件目录信息',iconCls:'bindPage',handler:ppem.addDirHandler});
            var btnReg = new Ext.Button({text:'附加软件注册表信息',iconCls:'bindPage',handler:ppem.addRegHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'uninstallmanage_grid_div',
                id:'uninstallmanage_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnDir,'-',btnReg,'-',btnRefresh,'-'],
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
					            {text:'附加软件目录信息',iconCls:'bindPage',listeners:{click:ppem.addDirHandler.createDelegate(this,[rowindex])}},
					            {text:'附加软件注册表信息',iconCls:'bindPage',listeners:{click:ppem.addRegHandler.createDelegate(this,[rowindex])}},
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

PPLive.PPInstall.UninstallSoftEdit=function(){
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
                    width:410,
                    height:210,
                    deferredRender : false,
                    labelWidth:115,
                    frame:true,
                    items:[
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件名称',
                            id:'unname',
                            allowBlank:false
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '卸载目录匹配名称',
                            id:'undirname'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '终止进程名',
                            id:'unprocess'
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 5px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:120,
                                        text:'是否强制卸载：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'unforceno',
                                        checked:true,
                                        boxLabel:'不强制',
                                        inputValue:0,
                                        name:'unsoftforce_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'unforceyes',
                                        boxLabel:'强制',
                                        inputValue:1,
                                        name:'unsoftforce_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'unforceyesno',
                                        boxLabel:'先普通后强制',
                                        inputValue:2,
                                        name:'unsoftforce_radio'
                                    }]
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 5px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:120,
                                        text:'是否必须卸载：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'unsoftmustyes',
                                        boxLabel:'是',
                                        inputValue:1,
                                        checked:true,
                                        name:'unsoftmust_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'unsoftmustno',
                                        boxLabel:'否',
                                        inputValue:0,
                                        name:'unsoftmust_radio'
                                    }]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '卸载删除目录',
                            id:'undir'
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
                    width:415,
                    id:'unsoftwin',
                    height:230,
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
                pp.initPanel('edit','编辑',record.data.SoftwareID);
                pp.initUninstallForce(record.data.ForceUninstall);
                pp.initIsMust(record.data.IsMust);
                Ext.getCmp('unname').setValue(record.data.Name);
                Ext.getCmp('undirname').setValue(record.data.UninstallName);
                Ext.getCmp('unprocess').setValue(record.data.KillExe);
                Ext.getCmp('undir').setValue(record.data.AppPath);
            }
        },
        operateHandler:function(o,id){
            var name = Ext.getCmp('unname');
            if(name.validate())
            {
                var undirname = Ext.getCmp('undirname').getValue();
                var unprocess = Ext.getCmp('unprocess').getValue();
                var undir = Ext.getCmp('undir').getValue();
                var unforce = pp.getUninstallForce();
                var unmust = pp.getIsMust();
                if(o=='add')
                    UninstallSoftManage.AddUninstallSoft(name.getValue(),undirname,unprocess,undir,unforce,unmust,pp.callbackSucess,pp.callbackError);
                else
                    UninstallSoftManage.EditUninstallSoft(id,name.getValue(),undirname,unprocess,undir,unforce,unmust,pp.callbackSucess,pp.callbackError);
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
                UninstallSoftManage.DeleteUninstallSofts(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('unsoftwin');
            if(win)win.close();
            pp.refreshHandler();
        },
        initUninstallForce:function(v){
            switch(v)
            {
                case 0:
                    Ext.getCmp('unforceno').setValue(true);Ext.getCmp('unforceyes').setValue(false);Ext.getCmp('unforceyesno').setValue(false);return;
                case 1:
                    Ext.getCmp('unforceno').setValue(false);Ext.getCmp('unforceyes').setValue(true);Ext.getCmp('unforceyesno').setValue(false);return;
                case 2:
                    Ext.getCmp('unforceno').setValue(false);Ext.getCmp('unforceyes').setValue(false);Ext.getCmp('unforceyesno').setValue(true);return;
            }
        },
        initIsMust:function(v){
            switch(v)
            {
                case 0:
                    Ext.getCmp('unsoftmustyes').setValue(false);Ext.getCmp('unsoftmustno').setValue(true);return;
                case 1:
                    Ext.getCmp('unsoftmustyes').setValue(true);Ext.getCmp('unsoftmustno').setValue(false);return;
            }
        },
        getUninstallForce:function(){
            if(Ext.getCmp('unforceno').checked) return 0;
            else if(Ext.getCmp('unforceyes').checked) return 1;
            else return 2;
        },
        getIsMust:function(){
            if(Ext.getCmp('unsoftmustyes').checked) return 1;
            return 0;
        },
        addDirHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var sid = record.data.SoftwareID;
                var name = record.data.Name;
                Ext.Dialog.init({title:name+"的卸载目录信息",width:document.body.clientWidth*0.75,height:document.body.clientHeight*0.8,fullMask:false,maximizable:true});
                Ext.Dialog.open("UninstallDir.aspx?softid="+sid,pp.closeCallback);
            }
        },
        addRegHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var sid=record.data.SoftwareID;
                var name = record.data.Name;
                Ext.Dialog.init({title:name+"的卸载注册表信息",width:document.body.clientWidth*0.75,height:document.body.clientHeight*0.8,fullMask:false,maximizable:true});
                Ext.Dialog.open("UninstallReg.aspx?softid="+sid,pp.closeCallback);
            }
        },
        closeCallback:function(returnValue){
//            if(returnValue=='OK')
//            {
                grid.store.reload();
                grid.selModel.clearSelections();
//            }
        }
    }
}();

Ext.apply(PPLive.PPInstall.UninstallSoftEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.UninstallSoftManage.pageInit.createDelegate(
        PPLive.PPInstall.UninstallSoftManage,
        [PPLive.PPInstall.UninstallSoftEdit]
    )
);



