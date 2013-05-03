

PPLive.PPInstall.StatisticManage=function(){
    var ppem;
    var grid;
    var pp;
    return{
        pageInit:function(em){
            ppem=em;
            pp=this;
            this.gridInit();
            this.pageLayout();
            ppem.editInit(grid);
            this.loadData();
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'StatisticManage.asmx',methodName:'GetAllStatistic'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ID'
                },[
                    'ID','ProductID','CustomID','ChannelID','SoftID','StatDate','StatCount','LoadCount','DownCount','ProductCnName','ProductEnName','CustomEnName','CustomCnName','ChannelEnName','ChannelCnName','SofgEnName','SoftCnName'
                ]),
                remoteSort: true
            });
            store.setDefaultSort('StatDate', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
                {type: 'date',  dataIndex: 'StatDate',dateFormat:'yyyy-MM-dd HH:mm:ss'},
                {type: 'numeric',  dataIndex: 'StatCount'},
			    {type: 'string',  dataIndex: 'ProductEnName'},
			    {type: 'string',  dataIndex: 'CustomEnName'},
			    {type: 'string',  dataIndex: 'ChannelEnName'},
			    {type: 'string',  dataIndex: 'SoftCnName'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'统计日期',width:100,sortable:true,dataIndex:'StatDate',renderer:Ext.util.Format.dateRenderer('Y-m-d')},  //H:i:s
                {header:'产品英文名称',width:100,sortable:true,dataIndex:'ProductEnName'}, 
                {header:'定制英文名称',width:120,sortable:true,dataIndex:'CustomEnName'},
                {header:'定制中文名称',width:120,sortable:true,dataIndex:'CustomCnName'},
                {header:'渠道英文名称',width:100,sortable:true,dataIndex:'ChannelEnName'},
                {header:'渠道中文名称',width:100,sortable:true,dataIndex:'ChannelCnName'},
                {header:'软件中文名称',width:100,sortable:true,dataIndex:'SoftCnName'},
                {header:'安装数',width:90,sortable:true,dataIndex:'StatCount'},
                {header:'加载统计数',width:90,sortable:true,dataIndex:'LoadCount'},
                {header:'下载统计数',width:90,sortable:true,dataIndex:'DownCount'}
            ]);
//            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
//            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
//            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
//            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            var lblSFilter = new Ext.form.Label({html:'&nbsp;&nbsp;开始时间：&nbsp;'});
            var startTimeFilter=new Ext.form.DateField({fieldLabel:'开始时间',format:'Y-m-d 00:00:00',id:'stastarttime',value:new Date().format('yyyy-MM-dd 00:00:00'),width:145,listeners:{select:ppem.filterHandler}});//,menu:new DatetimeMenu()});
            var lblEFilter = new Ext.form.Label({html:'&nbsp;&nbsp;结束时间：&nbsp;'});
            var endTimeFilter=new Ext.form.DateField({fieldLabel:'结束时间',format:'Y-m-d 23:59:59',width:145,id:'staendtime',value:new Date().format('yyyy-MM-dd 23:59:59'),listeners:{select:ppem.filterHandler}});//,menu:new DatetimeMenu()});
            var lblProName = new Ext.form.Label({html:'&nbsp;&nbsp;产品：&nbsp;'});
            var proName = new Ext.form.ComboBox({id:'staproname',mode:'local',forceSelection: true,displayField:'Name',valueField:'Id',editable:false,triggerAction:'all',width:100,store:new Ext.data.Store({autoLoad:true,proxy: new Ext.data.WebServiceProxy({servicePath: 'StatisticManage.asmx',methodName:'GetAllProductName'}),reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'Id'},[{name:'Id'},{name:'Name'}])}),listeners:{select:ppem.proFilterHandler}})
            var lblCusName = new Ext.form.Label({html:'&nbsp;&nbsp;产品：&nbsp;'});
            var cusStore = new Ext.data.Store({proxy: new Ext.data.WebServiceProxy({servicePath: 'StatisticManage.asmx',methodName:'GetALLCustom'}),reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'Id'},[{name:'Id'},{name:'Name'}])});
            var cusName = new Ext.form.ComboBox({id:'stacustomname',mode:'local',forceSelection: true,displayField:'Name',valueField:'Id',editable:false,triggerAction:'all',width:100,store:cusStore,listeners:{select:ppem.filterHandler}})
            var lblChaName = new Ext.form.Label({html:'&nbsp;&nbsp;产品：&nbsp;'});
            var chaName = new Ext.form.ComboBox({id:'stachannelname',mode:'local',forceSelection: true,displayField:'Name',valueField:'Id',editable:false,triggerAction:'all',width:150,store:new Ext.data.Store({autoLoad:true,proxy: new Ext.data.WebServiceProxy({servicePath: 'StatisticManage.asmx',methodName:'GetAllChannel'}),reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'Id'},[{name:'Id'},{name:'Name'}])}),listeners:{select:ppem.filterHandler}})
            var lblSoftName = new Ext.form.Label({html:'&nbsp;&nbsp;产品：&nbsp;'});
            var softName = new Ext.form.ComboBox({id:'stasoftname',mode:'local',forceSelection: true,displayField:'Name',valueField:'Id',editable:false,triggerAction:'all',width:100,store:new Ext.data.Store({autoLoad:true,proxy: new Ext.data.WebServiceProxy({servicePath: 'StatisticManage.asmx',methodName:'GetAllSoft'}),reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'Id'},[{name:'Id'},{name:'Name'}])}),listeners:{select:ppem.filterHandler}})
            var btnFilter = new Ext.Button({text:'查询',id:'btnstaselect',iconCls:'find',handler:ppem.filterHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'statisticmanage_grid_div',
                id:'statisticmanage_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',lblSFilter,'-',startTimeFilter,'-',lblEFilter,'-',endTimeFilter,'-',proName,'-',cusName,'-',chaName,'-',softName,'-',btnFilter,'-'],
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
                    contextmenu:function(e){
                        var menu = new Ext.menu.Menu({
					        id: 'basicMenu',
					        minWidth :20,
					        shadow : "sides",
					        items: [
					            //{text:'增加',iconCls:'add',listeners:{click:ppem.adddHandler}},
					            {text:'刷新',iconCls:'refresh',listeners:{click:ppem.refreshHandler}}
					        ]
				        });
		              e.preventDefault();
		              menu.showAt(e.getXY());
                    }
//                    rowcontextmenu:function(g,rowindex,e){
//                        var menu = new Ext.menu.Menu({
//					        id: 'rowMenu',
//					        minWidth :20,
//					        shadow : "sides",
//					        items: [
//					            {text:'增加',iconCls:'add',listeners:{click:ppem.adddHandler}},
//					            {text:'编辑',iconCls:'edit',listeners:{click:ppem.editHandler.createDelegate(this,[rowindex])}},
//					            {text:'删除',iconCls:'remove',listeners:{click:ppem.deleteHandler.createDelegate(this,[rowindex])}},
//					            {text:'刷新',iconCls:'refresh',listeners:{click:ppem.refreshHandler}}
//					        ]
//				        });
//		              e.preventDefault();
//		              menu.showAt(e.getXY());
//                    }
                }
            });
        },
        pageLayout:function(){
            var viewport=new Ext.Viewport({layout:'border',items:[grid]});
        },
        loadData:function(){
            grid.render();
            //grid.store.load({params:{start: 0, limit: ppem.pageSize,filters:[],inputFilters:[]}});
            //Ext.getCmp('staproname').setValue(0);
            ppem.customStoreLoad();
            Ext.getCmp('staproname').setValue('全部');
            Ext.getCmp('stacustomname').setValue('全部');
            Ext.getCmp('stachannelname').setValue('全部');
            Ext.getCmp('stasoftname').setValue('全部');
            Ext.getCmp('stacustomname').disable();
            ppem.filterHandler();
//            Ext.getDom('staproname').value='全部';
//            Ext.getDom('stacustomname').value='全部';
//            Ext.getDom('stachannelname').value='全部';
//            Ext.getDom('stasoftname').value='全部';
        }
    }
}();

PPLive.PPInstall.StatisticEdit=function(){
    var pp;
    var grid;
    var keys;
    return{
        editInit:function(gridPanel){
            pp=this;
            pp.pageEntry(pp.filterHandler);
            grid=gridPanel;
        },
        refreshHandler:function(){
            grid.store.reload();
            grid.selModel.clearSelections(); 
        },
        customStoreLoad:function(){
            var proid = Ext.getCmp('staproname').getValue();
            if(!proid||typeof proid!='number')proid=0;
            Ext.getCmp('stacustomname').clearValue();
            Ext.getCmp('stacustomname').store.reload({params:{productId:proid}});
            Ext.getCmp('stacustomname').setValue('全部');
//            Ext.getDom('stacustomname').value='全部';
        },
        proFilterHandler:function(){
            pp.customStoreLoad();
            Ext.getCmp('stacustomname').disable();
            if(Ext.getCmp('staproname').getValue()!='全部'&&Ext.getCmp('staproname').getValue()!=0)
                Ext.getCmp('stacustomname').enable();
            pp.filterHandler();
        },
        filterHandler:function(){
            var fs=[];
            var sTime=Ext.getCmp('stastarttime');
            if(sTime&&sTime.validate()&&sTime.getValue())fs.push(['date','gt','StatDate',sTime.getValue().format('yyyy-MM-dd 00:00:00')]);
            var eTime=Ext.getCmp('staendtime');
            if(eTime&&eTime.validate()&&eTime.getValue())fs.push(['date','lt','StatDate',eTime.getValue().format('yyyy-MM-dd 23:59:59')]);
            var proid = Ext.getCmp('staproname').getValue();
            if(proid&&proid!=0&&proid!='全部') fs.push(['numeric','eq','ProductID',proid]);
            var cusid = Ext.getCmp('stacustomname').getValue();
            if(cusid&&cusid!=0&&cusid!='全部')fs.push(['numeric','eq','CustomID',cusid]);
            var chanid = Ext.getCmp('stachannelname').getValue();
            if(chanid&&chanid!=0&&chanid!='全部')fs.push(['numeric','eq','ChannelID',chanid]);
            var softid = Ext.getCmp('stasoftname').getValue();
            if(softid&&softid!=0&&softid!='全部')fs.push(['numeric','eq','SoftID',softid]);
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
                        {text:'确定',handler:pp.operateHandler.createDelegate(this,[operate,id])},
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
                Ext.getCmp('versionuninstallcombo').setValue(record.data.SoftwareID);
                Ext.getDom('versionuninstallcombo').value=record.data.Name;
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

Ext.apply(PPLive.PPInstall.StatisticEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.StatisticManage.pageInit.createDelegate(
        PPLive.PPInstall.StatisticManage,
        [PPLive.PPInstall.StatisticEdit]
    )
);












