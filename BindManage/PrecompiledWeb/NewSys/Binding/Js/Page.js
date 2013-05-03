

PPLive.PPInstall.PageManage=function(){
    var ppem;
    var grid;
    var bid;
    return{
        pageInit:function(em){
            ppem=em;
            bid = ppem.getParameterByName(window.location.href,'bindid');
            this.gridInit();
            this.pageLayout();
            this.loadData();
            ppem.editInit(grid,bid);
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'PageManage.asmx',methodName:'GetPageByBind'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'PageID'
                },[
                    'PageID','SoftCount','Description','TemplateID','BindID','CustomID','ChannelID','ChannelCnName','ChannelEnName','CustomCnName','CustomEnName','ProductID','ProductCnName','ProductEnName','TemplateType','TemplateName'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'numeric',  dataIndex: 'SoftCount'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'页软件个数',width:100,sortable:true,dataIndex:'SoftCount'}, 
                {header:'捆绑页描述',width:200,sortable:true,dataIndex:'Description',renderer:ppem.showTip}, 
                {header:'捆绑页模版名',width:100,sortable:true,dataIndex:'TemplateName'},
                {header:'所属渠道英文名称',width:120,sortable:true,dataIndex:'ChannelEnName'},
                {header:'所属定制英文名称',width:120,sortable:true,dataIndex:'CustomEnName'},
                //{header:'所属产品中文名称',width:120,sortable:true,dataIndex:'ProductCnName'},
                {header:'所属产品英文名称',width:120,sortable:true,dataIndex:'ProductEnName'}
                //{header:'所属渠道中文名称',width:120,sortable:true,dataIndex:'ChannelCnName'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnSoft = new Ext.Button({text:'捆绑软件',iconCls:'bindPage',handler:ppem.bindSoftHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'pagemange_grid_div',
                id:'pagemange_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnSoft,'-',btnRefresh,'-'],
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
					            {text:'捆绑软件',iconCls:'bindPage',listeners:{click:ppem.bindSoftHandler.createDelegate(this,[rowindex])}},
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
            grid.store.load({params:{start: 0, limit: ppem.pageSize,sort:'',dir:'',filters:[],inputFilters:[],bindId:bid}});
        }
    }
}();

PPLive.PPInstall.PageEdit=function(){
    var pp;
    var grid;
    var keys;
    var bid;
    return{
        editInit:function(gridPanel,bindid){
            pp=this;
            grid=gridPanel;
            bid=bindid;
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
                    height:200,
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
                                    text:'所属模板：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'pagetemplate',
                                    allowBlank:false,
                                    fieldLabel: '所属模板',
                                    id:'pagetemplatecombo',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'Name',
                                    editable:false,
                                    valueField:'TemplateID',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'PageManage.asmx',methodName:'GetAllTemplate'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'TemplateID'},[{name:'TemplateID'},{name:'Name'}])
                                    })}
                                ]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '页软件个数',
                            allowBlank:false,
                            value:0,
                            regex:pp.zeroRegex,
                            regexText:pp.zeroRegexText,
                            id:'ps_count'
                        },
                        {
                            xtype:'textarea',
                            id:'pagemark',
                            width:250,
                            height:80,
                            fieldLabel:'捆绑页描述'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnpageconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:405,
                    id:'pagewin',
                    height:220,
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
                pp.initPanel('edit','编辑',record.data.PageID);
                Ext.getCmp('pagetemplatecombo').store.on('load',function(){
                    Ext.getCmp('pagetemplatecombo').setValue(record.data.TemplateID);
                });
                //Ext.getCmp('pagetemplatecombo').setValue(record.data.TemplateID);
                //Ext.getDom('pagetemplatecombo').value=record.data.TemplateName;
                Ext.getCmp('ps_count').setValue(record.get('SoftCount'));
                Ext.getCmp('pagemark').setValue(record.data.Description);
            }
        },
        operateHandler:function(o,id){
            var tem = Ext.getCmp('pagetemplatecombo');
            var pcount = Ext.getCmp('ps_count');
            if(tem.validate()&&pcount.validate())
            {
                var des = Ext.getCmp('pagemark').getValue();
                if(o=='add')
                    PageManage.AddPage(bid,tem.getValue(),des,pcount.getValue(),pp.callbackSucess,pp.callbackError);
                else
                    PageManage.EditPage(id,bid,tem.getValue(),des,pcount.getValue(),pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnpageconfirm');
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
                PageManage.DeletePages(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        bindSoftHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var pageid=record.data.PageID;
                var tempid=record.data.TemplateType;
                Ext.Dialog.init({title:"捆绑软件",width:document.body.clientWidth*0.85,height:document.body.clientHeight*0.9,closable:false,fullMask:false,maximizable:true});
                Ext.Dialog.open("PageSoft.aspx?pageid="+pageid+"&tempid="+tempid,pp.closeCallback);
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
            var win = Ext.getCmp('pagewin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.PageEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.PageManage.pageInit.createDelegate(
        PPLive.PPInstall.PageManage,
        [PPLive.PPInstall.PageEdit]
    )
);






