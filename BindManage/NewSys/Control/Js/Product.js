
PPLive.PPInstall.ProductManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'ProductManage.asmx',methodName:'GetAllProduct'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ProductID'
                },[
                    'ProductID','CnName','EnName','CnDescription','EnDescription'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'string',  dataIndex: 'CnName'},
				    {type: 'string',  dataIndex: 'EnName'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'产品中文名称',width:100,sortable:true,dataIndex:'CnName'}, 
                {header:'产品英文名称',width:100,sortable:true,dataIndex:'EnName'}, 
                {header:'产品中文描述',width:250,sortable:true,dataIndex:'CnDescription',renderer:ppem.showTip},
                {header:'产品英文描述',width:250,sortable:true,dataIndex:'EnDescription',renderer:ppem.showTip}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'productmange_grid_div',
                id:'productmanage_grid',
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
        }
    }
}();

PPLive.PPInstall.ProductEdit=function(){
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
                    height:280,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '产品中文名字',
                            id:'productcname',
                            allowBlank:false
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '产品英文名字',
                            id:'productename',
                            maxLength:50,
                            allowBlank:false
                        },
                        {
                            xtype:'textarea',
                            id:'cremark',
                            width:250,
                            height:80,
                            fieldLabel:'产品中文描述'
                        },
                        {
                            xtype:'textarea',
                            id:'eremark',
                            width:250,
                            height:80,
                            fieldLabel:'产品英文描述'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnproductconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections();}}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:405,
                    id:'productmanagewin',
                    height:300,
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
                pp.initPanel('edit','编辑',record.data.ProductID);
                Ext.getCmp('productcname').setValue(record.data.CnName);
                Ext.getCmp('productename').setValue(record.data.EnName);
                Ext.getCmp('cremark').setValue(record.data.CnDescription);
                Ext.getCmp('eremark').setValue(record.data.EnDescription);
            }
        },
        operateHandler:function(o,id){
            var cname = Ext.getCmp('productcname');
            var ename = Ext.getCmp('productename');
            if(cname.validate()&&ename.validate())
            {
                var cdes = Ext.getCmp('cremark').getValue();
                var edes = Ext.getCmp('eremark').getValue();
                if(o=='add')
                    ProductManage.AddProduct(cname.getValue(),ename.getValue(),cdes,edes,pp.callbackSucess,pp.callbackError);
                else
                    ProductManage.EditProduct(id,cname.getValue(),ename.getValue(),cdes,edes,pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnproductconfirm');
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
                ProductManage.DeleteProducts(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('productmanagewin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.ProductEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.ProductManage.pageInit.createDelegate(
        PPLive.PPInstall.ProductManage,
        [PPLive.PPInstall.ProductEdit]
    )
);

