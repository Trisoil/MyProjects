
PPLive.PPInstall.ChannelManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'ChannelManage.asmx',methodName:'GetAllChannel'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ChannelID'
                },[
                    'ChannelID','CnName','EnName','Description','Type'
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
                {header:'渠道中文名称',width:120,sortable:true,dataIndex:'CnName'}, 
                {header:'渠道英文名称',width:120,sortable:true,dataIndex:'EnName'}, 
                {header:'渠道中文描述',width:250,sortable:true,dataIndex:'Description',renderer:ppem.showTip},
                {header:'渠道类型',width:150,sortable:true,dataIndex:'Type'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            var lblcname = new Ext.form.Label({html:'&nbsp;&nbsp;中文名称：&nbsp;'});
            var txtcname = new Ext.form.TextField({id:'txt_channelfilter_cname',enableKeyEvents:true,listeners:{keydown:function(t,e){if(e.getKey() == e.ENTER){ppem.filterHandler();}}}});
            var lblename = new Ext.form.Label({html:'&nbsp;&nbsp;英文名称：&nbsp;'});
            var txtename = new Ext.form.TextField({id:'txt_channelfilter_ename',enableKeyEvents:true,listeners:{keydown:function(t,e){if(e.getKey() == e.ENTER){ppem.filterHandler();}}}});
            var btnFilter = new Ext.Button({id:'btn_channel_filter',text:'搜索',iconCls:'find',handler:ppem.filterHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'channelmange_grid_div',
                id:'channelmanage_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnRefresh,'-',lblcname,'-',txtcname,'-',lblename,'-',txtename,'-',btnFilter,'-'],
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

PPLive.PPInstall.ChannelEdit=function(){
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
            var cfilter = Ext.getCmp('txt_channelfilter_cname').getValue();
            if(cfilter) fs.push(['string','','CnName',cfilter]);
            var nfilter = Ext.getCmp('txt_channelfilter_ename').getValue();
            if(nfilter) fs.push(['string','','EnName',nfilter]);
            grid.store.lastOptions.params.inputFilters=fs;
            grid.store.reload();     
        },
        initPanel:function(operate,title,id){
            var panel = new Ext.form.FormPanel({
                    lableAlign:'left',
                    buttonAlign:'right',   
                    bodyStyle:'padding:5 15 0 5',
                    width:400,
                    height:170,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '渠道中文名称',
                            id:'channelcname',
                            allowBlank:false
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '渠道英文名称',
                            regex: pp.wordnumregex,
                            regexText:pp.wordnumregexText,
                            id:'channelename',
                            maxLength:36,
                            allowBlank:false
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '渠道中文描述',
                            id:'channeldes'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '渠道类型',
                            id:'channeltype'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btn_channel_confirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:405,
                    id:'channelwin',
                    height:190,
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
                pp.initPanel('edit','编辑',record.data.ChannelID);
                Ext.getCmp('channelcname').setValue(record.data.CnName);
                Ext.getCmp('channelename').setValue(record.data.EnName);
                Ext.getCmp('channeldes').setValue(record.data.Description);
                Ext.getCmp('channeltype').setValue(record.data.Type);
            }
        },
        operateHandler:function(o,id){
            var cname = Ext.getCmp('channelcname');
            var ename = Ext.getCmp('channelename');
            if(cname.validate()&&ename.validate())
            {
                var des = Ext.getCmp('channeldes').getValue();
                var type = Ext.getCmp('channeltype').getValue();
                if(o=='add')
                    ChannelManage.AddChannel(cname.getValue(),ename.getValue(),des,type,pp.callbackSucess,pp.callbackError);
                else
                    ChannelManage.EditChannel(id,cname.getValue(),ename.getValue(),des,type,pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btn_channel_confirm');
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
                ChannelManage.DeleteChannels(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('channelwin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.ChannelEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.ChannelManage.pageInit.createDelegate(
        PPLive.PPInstall.ChannelManage,
        [PPLive.PPInstall.ChannelEdit]
    )
);





