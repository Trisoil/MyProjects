Ext.grid.CheckboxSelectionModel.override({   
    handleMouseDown : Ext.emptyFn      
});   
Ext.grid.RowSelectionModel.override({   
    initEvents : function() {   
        if(!this.grid.enableDragDrop && !this.grid.enableDrag){   
            this.grid.on("rowmousedown", this.handleMouseDown, this);   
        }else{ // allow click to work like normal   
            this.grid.on("rowclick", function(grid, rowIndex, e) {   
                var target = e.getTarget();                   
                if(target.className !== 'x-grid3-row-checker' && e.button === 0 && !e.shiftKey && !e.ctrlKey) {   
                    this.selectRow(rowIndex, false);   
                    grid.view.focusRow(rowIndex);   
                }   
            }, this);   
        }   
  
        this.rowNav = new Ext.KeyNav(this.grid.getGridEl(), {   
            "up" : function(e){   
                if(!e.shiftKey){   
                    this.selectPrevious(e.shiftKey);   
                }else if(this.last !== false && this.lastActive !== false){   
                    var last = this.last;   
                    this.selectRange(this.last,  this.lastActive-1);   
                    this.grid.getView().focusRow(this.lastActive);   
                    if(last !== false){   
                        this.last = last;   
                    }   
                }else{   
                    this.selectFirstRow();   
                }   
            },   
            "down" : function(e){   
                if(!e.shiftKey){   
                    this.selectNext(e.shiftKey);   
                }else if(this.last !== false && this.lastActive !== false){   
                    var last = this.last;   
                    this.selectRange(this.last,  this.lastActive+1);   
                    this.grid.getView().focusRow(this.lastActive);   
                    if(last !== false){   
                        this.last = last;   
                    }   
                }else{   
                    this.selectFirstRow();   
                }   
            },   
            scope: this  
        });   
  
        var view = this.grid.view;   
        view.on("refresh", this.onRefresh, this);   
        view.on("rowupdated", this.onRowUpdated, this);   
        view.on("rowremoved", this.onRemove, this);           
    }   
});   


PPLive.PPInstall.PageSoftManage=function(){
    var ppem;
    var grid;
    var pid;
    var tid;
    var pp;
    return{
        pageInit:function(em){
            pp=this;
            ppem=em;
            pid = ppem.getParameterByName(window.location.href,'pageid');
            tid = ppem.getParameterByName(window.location.href,'tempid');
            pp.gridInit();
            pp.pageLayout();
            pp.loadData();
            pp.rowEvent();
            pp.dropGrid();
            ppem.editInit(grid,pid,tid);
        },
        gridInit:function(){
            Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
            var store = new Ext.data.Store({
                proxy: new Ext.data.WebServiceProxy({servicePath: 'PageSoftManage.asmx',methodName:'GetAllPageSoftById'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ID'
                },[
                    'ID','PageID','SoftID','CPDOfChannel','SoftCnName','SoftEnName','SoftCountPerDay','SoftAllCPDOfChannel','SoftBindEnable','ForceDescription','ForceName','Rank'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('Rank', 'asc');
//            var filters = new Ext.ux.grid.GridFilters({filters:[
//				    {type: 'numeric',  dataIndex: 'SoftCount'}
//			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'绑定是否有效',width:100,sortable:false,dataIndex:'SoftBindEnable',renderer:ppem.showBoolValue}, 
                {header:'渠道每天下发量',width:100,sortable:false,dataIndex:'CPDOfChannel'}, 
                {header:'软件中文名称',width:120,sortable:false,dataIndex:'SoftCnName'},
                {header:'软件英文名称',width:120,sortable:false,dataIndex:'SoftEnName'},
                {header:'软件全局每天下发量',width:120,sortable:false,dataIndex:'SoftCountPerDay'},
                {header:'软件所有渠道每天下发总量',width:120,sortable:false,dataIndex:'SoftAllCPDOfChannel'},
                {header:'强制安装名称',width:100,sortable:false,dataIndex:'ForceName'}, 
                {header:'强制安装描述',width:120,sortable:false,dataIndex:'ForceDescription',renderer:ppem.showTip}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnUp = new Ext.Button({text:'上移',iconCls:'arrowup',handler:ppem.upHandler});
            var btnDown = new Ext.Button({text:'下移',iconCls:'arrowdown',handler:ppem.downHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'pagesoftmange_grid_div',
                id:'pagesoftmange_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                //plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnUp,'-',btnDown,'-',btnRefresh,'-'],
//                bbar: new Ext.PagingToolbar({
//                    pageSize: ppem.pageSize,
//                    store: store,
//                    fitToFrame:true,
//                    plugins: filters,
//                    displayInfo: true,
//                    displayMsg: '{0} - {1}/{2}',
//                    emptyMsg: "没有相关记录!"
//                }),
                ddGroup:'pagesoftddgroup',
                enableDragDrop:true,
                getDragDropText:function(){
                    var count = grid.selModel.getCount();
                    if(count!=1)
                        return '只能选择一个记录进行移动！';
                    else
                        return grid.selModel.selections.items[0].data.SoftCnName;
                },
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
					            {text:'上移',iconCls:'arrowup',listeners:{click:ppem.upHandler.createDelegate(this,[rowindex])}},
					            {text:'下移',iconCls:'arrowdown',listeners:{click:ppem.downHandler.createDelegate(this,[rowindex])}},
					            {text:'刷新',iconCls:'refresh',listeners:{click:ppem.refreshHandler}}
					        ]
				        });
		              e.preventDefault();
		              menu.showAt(e.getXY());
                    }
                },
                buttons:[
                    {id:'btnpagesoftallsave',text:'保存',handler:ppem.allsaveHandler}
                    //{text:'取消',handler:function(){window.parent.Ext.Dialog.close('OK');}}
                ]
            });
        },
        pageLayout:function(){
            var viewport=new Ext.Viewport({layout:'border',items:[grid]});
        },
        loadData:function(){
            grid.render();
            //grid.store.load({params:{start: 0, limit: ppem.pageSize,sort:'',dir:'',filters:[],inputFilters:[],pageId:pid}});
            grid.store.load({params:{pageId:pid}});
        },
        mouseRow:null,
        rowEvent:function(){
            grid.view.mainBody.on('mouseover',function(e,t){
                var row;
                if((row = grid.view.findRowIndex(t)) !== false){
                    pp.mouseRow=row;
                }
            });
        },
        dropGrid:function(){
            var firstGridDropTargetEl =  grid.getView().el.dom.childNodes[0].childNodes[1];
	        var firstGridDropTarget = new Ext.dd.DropTarget(firstGridDropTargetEl, {
		        ddGroup    : 'pagesoftddgroup',
		        copy       : true,
		        notifyDrop : function(ddSource, e, data){
		            var length = ddSource.dragData.selections.length;
		            if(length==1)
		            {
        			    var record = ddSource.dragData.selections[0];
        			    if(record)
        			    {   
        			        if(pp.mouseRow>=0)
        			        {
        			            grid.store.remove(record);
                                grid.store.insert(pp.mouseRow,record);
                                grid.selModel.selectRow(pp.mouseRow);
        			        }
			                return(true);
			            }
			        }
		        }
	        }); 	
        }
    }
}();

PPLive.PPInstall.PageSoftEdit=function(){
    var pp;
    var grid;
    var keys;
    var pid;
    var tid;
    var winid;
    return{
        editInit:function(gridPanel,pageid,tempid){
            pp=this;
            grid=gridPanel;
            pid=pageid;
            tid=tempid;
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
            var pwidth=205;var winwidth=225;winid='pagesoftwin';
            if(tid!=0)
            {
                pwidth=140;winwidth=160;winid='pagesofsmalltwin';
            }
            var panel = new Ext.form.FormPanel({
                    lableAlign:'left',
                    buttonAlign:'right',   
                    bodyStyle:'padding:5 15 0 5',
                    width:420,
                    height:pwidth,
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
                                    text:'捆绑软件中文名称：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'pagesoftcnamecombo',
                                    allowBlank:false,
                                    fieldLabel: '捆绑软件中文名称',
                                    id:'pagesoftcnamecombo',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'Name',
                                    //editable:false,
                                    valueField:'Id',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'PageManage.asmx',methodName:'GetAllEnableSoft'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'Id'},[{name:'Id'},{name:'Name'},{name:'SoftCountPerDay'}])
                                    }),
                                    listeners:{
                                        select:pp.pagesoftSlectHandler
                                    }
                                }]
                        },
                        {
                            xtype: 'textfield',
                            width:245,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '渠道每天下发量',
                            labelSeparator:'：',
                            value:0,
                            allowBlank:false,
                            regex:pp.zeroRegex,
                            regexText:pp.zeroRegexText,
                            maxLength:9,
                            id:'pagecountperday'
                        },
                        {
                            xtype:'label',
                            id:'pagecountwarn',
                            style:'padding:2px 0px 0px 0px',
                            hidden:true,
                            html:"<font style='color:red;padding-left:130px;'>以该软件全局每天下发量上限为准</font>"
                        },
                        {
                            xtype: 'textfield',
                            width:245,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '强制安装名称',
                            id:'forcename'
                        },
                        {
                            xtype:'textarea',
                            id:'forcedes',
                            style:'padding:2px 0px 0px 0px',
                            width:250,
                            height:50,
                            fieldLabel:'强制安装描述'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnpagesoftconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:425,
                    id:winid,
                    height:winwidth,
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
                pp.showPanelInit();
        },
        showPanelInit:function(){
            if(tid!=0)
            {
                Ext.getCmp('forcedes').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('forcename').getEl().up('.x-form-item').setDisplayed( false );
            }
            else
            {
                Ext.getCmp('forcedes').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('forcename').getEl().up('.x-form-item').setDisplayed( true );
            }
        },
        pagesoftSlectHandler:function(c,r,n){
            if(r.get("SoftCountPerDay")>0)
            {
                Ext.getCmp('pagecountperday').disable();
                Ext.getCmp('pagecountwarn').show();
            }
            else
            {
                Ext.getCmp('pagecountperday').enable();
                Ext.getCmp('pagecountwarn').hide();
            }
            Ext.getCmp('forcename').setValue(r.get('Name'));
        },
        adddHandler:function(){
            pp.initPanel('add','增加');
        },
        editHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                pp.initPanel('edit','编辑',record.data.ID);
                Ext.getCmp('pagesoftcnamecombo').store.on('load',function(){
                    Ext.getCmp('pagesoftcnamecombo').setValue(record.data.SoftID);
                });
                //Ext.getCmp('pagesoftcnamecombo').setValue(record.data.SoftID);
                //Ext.getDom('pagesoftcnamecombo').value=record.data.SoftCnName;
                Ext.getCmp('pagecountperday').setValue(record.data.CPDOfChannel);
                Ext.getCmp('forcename').setValue(record.data.ForceName);
                Ext.getCmp('forcedes').setValue(record.data.ForceDescription);
                if(record.data.SoftCountPerDay>0)
                {
                    Ext.getCmp('pagecountperday').disable();
                    Ext.getCmp('pagecountwarn').show();
                }
            }
        },
        operateHandler:function(o,id){
            var cname = Ext.getCmp('pagesoftcnamecombo');
            var count = Ext.getCmp('pagecountperday');
            if(cname.validate()&&count.validate())
            {
                var fname = Ext.getCmp('forcename').getValue();
                var fdes = Ext.getCmp('forcedes').getValue();
                if(o=='add')
                    PageSoftManage.AddPageSoft(pid,cname.getValue(),count.getValue(),fname,fdes,pp.callbackSucess,pp.callbackError);
                else
                    PageSoftManage.EditPageSoft(id,pid,cname.getValue(),count.getValue(),fname,fdes,pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnpagesoftconfirm');
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
                PageSoftManage.DeletePageSofts(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        upHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var index = grid.store.indexOf(record);
                var preindex = index-1;
                if(preindex>=0)
                {
                    grid.store.remove(record);
                    grid.store.insert(preindex,record);
                    grid.selModel.selectRow(preindex);
                }
            }
        },
        downHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var index = grid.store.indexOf(record);
                var nextindex = index+1;
                var count = grid.store.getCount();
                if(nextindex<count)
                {
                    grid.store.remove(record);
                    grid.store.insert(nextindex,record);
                    grid.selModel.selectRow(nextindex);
                }
            }
        },
        allsaveHandler:function(){
            var count = grid.store.getCount();
            var records = grid.store.getRange(0,count);
            if(records&&records.length>0)
            {
                var ids = [];
                for(var i=0;i<records.length;i++)
                {
                    ids.push(records[i].id)
                }
                PageSoftManage.RankPageSoft(ids,pp.allsaveCallback,pp.allsaveErrorback);
                pp.btnDisable('btnpagesoftallsave');
            }
            else
                pp.allsaveCallback();
        },
        allsaveCallback:function(result){
            window.parent.Ext.Dialog.close('OK');
        },
        allsaveErrorback:function(result){
            var btn = Ext.getCmp('btnpagesoftallsave');
            if(btn)
                btn.enable();
            Ext.MessageBox.show({title: '错误',msg: result.get_message(),buttons: Ext.MessageBox.OK,icon:Ext.MessageBox.ERROR});
        },
        callbackSucess:function(result){
            var win = Ext.getCmp(winid);
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.PageSoftEdit,PPLive.PPInstall.EditBase);

window.onload=function(){
    Ext.onReady(
        PPLive.PPInstall.PageSoftManage.pageInit.createDelegate(
            PPLive.PPInstall.PageSoftManage,
            [PPLive.PPInstall.PageSoftEdit]
        )
    );
}








