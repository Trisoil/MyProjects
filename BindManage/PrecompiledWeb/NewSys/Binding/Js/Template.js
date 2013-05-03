
PPLive.PPInstall.TemplateManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'TemplateManage.asmx',methodName:'GetAllTemplate'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'TemplateID'
                },[
                    'TemplateID','Type','EnableAll','SoftCount','Description','Name'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
                {type: 'string',  dataIndex: 'Name'},
			    {type: 'numeric',  dataIndex: 'SoftCount'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'模版名',width:120,sortable:true,dataIndex:'Name'}, 
                {header:'模版类型',width:120,sortable:true,dataIndex:'Type',renderer:pp.showType}, 
                {header:'是否显示全选',width:120,sortable:true,dataIndex:'EnableAll',renderer:pp.showAll}, 
                {header:'最多软件个数',width:150,sortable:true,dataIndex:'SoftCount'},
                {header:'模版描述',width:250,sortable:true,dataIndex:'Description',renderer:ppem.showTip}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'templatemange_grid_div',
                id:'templatemange_grid',
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
        showType:function(value, meta, rec, rowIdx, colIdx, ds){
            switch(value)
            {
                case 0:
                    return '强制模板';
                case 1:
                    return '模板A';
                case 2:
                    return '模板B';
                case 3:
                    return '模板C';
                case 4:
                    return '模版D';
                case 5:
                    return '模版E';
                case 6:
                    return '模版F';
                default:
                    return value;
            }
        },
        showAll:function(value, meta, rec, rowIdx, colIdx, ds){
            if(value)
                return "是";
            return "否";
        }
    }
}();

PPLive.PPInstall.TemplateEdit=function(){
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
                    width:470,
                    height:240,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '模版名',
                            allowBlank:false,
                            id:'temname'
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 5px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:100,
                                        text:'模版类型：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtypea',
                                        checked:true,
                                        boxLabel:'模版A',
                                        inputValue:1,
                                        name:'temtype_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtypeb',
                                        boxLabel:'模版B',
                                        inputValue:2,
                                        name:'temtype_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtypec',
                                        boxLabel:'模版C',
                                        inputValue:3,
                                        name:'temtype_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtyped',
                                        boxLabel:'模版D',
                                        inputValue:4,
                                        name:'temtype_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtypee',
                                        boxLabel:'模版E',
                                        inputValue:5,
                                        name:'temtype_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtypef',
                                        boxLabel:'模版F',
                                        inputValue:6,
                                        name:'temtype_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temtypecore',
                                        boxLabel:'强制模版',
                                        inputValue:0,
                                        name:'temtype_radio'
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
                                        width:100,
                                        text:'是否显示全选：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temallyes',
                                        checked:true,
                                        boxLabel:'是',
                                        inputValue:true,
                                        name:'all_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'temallno',
                                        boxLabel:'否',
                                        inputValue:false,
                                        name:'all_radio'
                                    }]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '最多软件个数',
                            regex:pp.zeroRegex,
                            regexText:pp.zeroRegexText,
                            maxLength:9,
                            allowBlank:false,
                            id:'temsoftcount'
                        },
                        {
                            xtype:'textarea',
                            id:'temmark',
                            width:250,
                            height:80,
                            fieldLabel:'模版描述'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btntemplateconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:475,
                    id:'templatemanagewin',
                    height:260,
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
                pp.initPanel('edit','编辑',record.data.TemplateID);
                Ext.getCmp('temname').setValue(record.data.Name);
                Ext.getCmp('temmark').setValue(record.data.Description);
                Ext.getCmp('temsoftcount').setValue(record.data.SoftCount);
                pp.initTempType(record.data.Type);
                pp.initEnableAll(record.data.EnableAll)
            }
        },
        operateHandler:function(o,id){
            var name = Ext.getCmp('temname');
            var scount = Ext.getCmp('temsoftcount');
            if(name.validate()&&scount.validate())
            {
                var type = pp.getTempType();
                var all = pp.getEnableAll();
                var des = Ext.getCmp('temmark').getValue();
                if(o=='add')
                    TemplateManage.AddTemplate(name.getValue(),type,all,scount.getValue(),des,pp.callbackSucess,pp.callbackError);
                else
                    TemplateManage.EditTemplate(id,name.getValue(),type,all,scount.getValue(),des,pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btntemplateconfirm');
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
                TemplateManage.DeleteTemplates(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        initTempType:function(v){
            switch(v)
            {
                case 0:
                    Ext.getCmp('temtypecore').setValue(true);Ext.getCmp('temtypea').setValue(false);Ext.getCmp('temtypeb').setValue(false);Ext.getCmp('temtypec').setValue(false);Ext.getCmp('temtyped').setValue(false);Ext.getCmp('temtypee').setValue(false);Ext.getCmp('temtypef').setValue(false);return;
                case 1:
                    Ext.getCmp('temtypecore').setValue(false);Ext.getCmp('temtypea').setValue(true);Ext.getCmp('temtypeb').setValue(false);Ext.getCmp('temtypec').setValue(false);Ext.getCmp('temtyped').setValue(false);Ext.getCmp('temtypee').setValue(false);Ext.getCmp('temtypef').setValue(false);return;
                case 2:
                    Ext.getCmp('temtypecore').setValue(false);Ext.getCmp('temtypea').setValue(false);Ext.getCmp('temtypeb').setValue(true);Ext.getCmp('temtypec').setValue(false);Ext.getCmp('temtyped').setValue(false);Ext.getCmp('temtypee').setValue(false);Ext.getCmp('temtypef').setValue(false);return;
                case 3:
                    Ext.getCmp('temtypecore').setValue(false);Ext.getCmp('temtypea').setValue(false);Ext.getCmp('temtypeb').setValue(false);Ext.getCmp('temtypec').setValue(true);Ext.getCmp('temtyped').setValue(false);Ext.getCmp('temtypee').setValue(false);Ext.getCmp('temtypef').setValue(false);return;
                case 4:
                    Ext.getCmp('temtypecore').setValue(false);Ext.getCmp('temtypea').setValue(false);Ext.getCmp('temtypeb').setValue(false);Ext.getCmp('temtypec').setValue(false);Ext.getCmp('temtyped').setValue(true);Ext.getCmp('temtypee').setValue(false);Ext.getCmp('temtypef').setValue(false);return;
                case 5:
                    Ext.getCmp('temtypecore').setValue(false);Ext.getCmp('temtypea').setValue(false);Ext.getCmp('temtypeb').setValue(false);Ext.getCmp('temtypec').setValue(false);Ext.getCmp('temtyped').setValue(false);Ext.getCmp('temtypee').setValue(true);Ext.getCmp('temtypef').setValue(false);return;
                case 6:
                    Ext.getCmp('temtypecore').setValue(false);Ext.getCmp('temtypea').setValue(false);Ext.getCmp('temtypeb').setValue(false);Ext.getCmp('temtypec').setValue(false);Ext.getCmp('temtyped').setValue(false);Ext.getCmp('temtypee').setValue(false);Ext.getCmp('temtypef').setValue(true);return;
            }   
        },
        initEnableAll:function(v){
            if(v)
            {
                Ext.getCmp('temallyes').setValue(true);
                Ext.getCmp('temallno').setValue(false);
            }
            else
            {
                Ext.getCmp('temallyes').setValue(false);
                Ext.getCmp('temallno').setValue(true);
            }
        },
        getTempType:function(){
            if(Ext.getCmp('temtypecore').checked)return 0;
            else if(Ext.getCmp('temtypea').checked)return 1;
            else if(Ext.getCmp('temtypeb').checked)return 2;
            else if(Ext.getCmp('temtypec').checked)return 3;
            else if(Ext.getCmp('temtyped').checked)return 4;
            else if(Ext.getCmp('temtypee').checked)return 5;
            else return 6;
        },
        getEnableAll:function(){
            if(Ext.getCmp('temallyes').checked) return true;
            else return false;
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('templatemanagewin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.TemplateEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.TemplateManage.pageInit.createDelegate(
        PPLive.PPInstall.TemplateManage,
        [PPLive.PPInstall.TemplateEdit]
    )
);







