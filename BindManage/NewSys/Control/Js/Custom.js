
PPLive.PPInstall.CustomVars={
    fullbag:0,
    guidebag:1,
    silentbag:2,
    gsbag:3,
    uploadprefixname:'*.exe',
    uploadfiledes:'可执行文件'
};

PPLive.PPInstall.CustomManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'CustomManage.asmx',methodName:'GetAllCustom'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'CustomID'
                },[
                    'CustomID','CnName','EnName','Description','ClientPath','CurVersion','GuideClientPath','GuideCurVersion','SilentClientPath','SilentCurVersion','GSClientPath','GSCurVersion','VaRidInfo','ProductCnName','ProductEnName','ProductID'
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
                {header:'定制中文名称',width:100,sortable:true,dataIndex:'CnName'}, 
                {header:'定制英文名称',width:100,sortable:true,dataIndex:'EnName'}, 
                {header:'定制中文描述',width:200,sortable:true,dataIndex:'Description',renderer:ppem.showTip},
                //{header:'部分上传路径',width:150,sortable:true,dataIndex:'ClientPath',renderer:ppem.showTip},
                {header:'最新完整包版本',width:100,sortable:true,dataIndex:'CurVersion'},
                {header:'最新引导包版本',width:100,sortable:true,dataIndex:'GuideCurVersion'},
                {header:'最新静默包版本',width:100,sortable:true,dataIndex:'SilentCurVersion'},
                {header:'最新引导静默包版本',width:120,sortable:true,dataIndex:'GSCurVersion'},
                {header:'所属产品中文名称',width:120,sortable:true,dataIndex:'ProductCnName'},
                {header:'所属产品英文名称',width:120,sortable:true,dataIndex:'ProductEnName'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnUpLoad = new Ext.Button({text:'上传完整包',iconCls:'pageup',handler:ppem.upLoadHandler});
            var btnguideUpLoad = new Ext.Button({text:'上传引导包',iconCls:'pageup',handler:ppem.upGuideLoadHandler});
            var btnsilentUpLoad = new Ext.Button({text:'上传静默包',iconCls:'pageup',handler:ppem.upSilentLoadHandler});
            var btnGCUpLoad = new Ext.Button({text:'上传引导静默包',iconCls:'pageup',handler:ppem.upGCLoadHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'custommanage_grid_div',
                id:'custommanage_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnUpLoad,'-',btnguideUpLoad,'-',btnsilentUpLoad,'-',btnGCUpLoad,'-',btnRefresh,'-'],
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
					            {text:'上传完整包',iconCls:'pageup',listeners:{click:ppem.upLoadHandler.createDelegate(this,[rowindex])}},
					            {text:'上传引导包',iconCls:'pageup',listeners:{click:ppem.upGuideLoadHandler.createDelegate(this,[rowindex])}},
					            {text:'上传静默包',iconCls:'pageup',listeners:{click:ppem.upSilentLoadHandler.createDelegate(this,[rowindex])}},
					            {text:'上传引导静默包',iconCls:'pageup',listeners:{click:ppem.upGCLoadHandler.createDelegate(this,[rowindex])}},
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

PPLive.PPInstall.CustomEdit=function(){
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
                    height:230,
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
                                    text:'所属产品：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'playaddress',
                                    allowBlank:false,
                                    fieldLabel: '所属产品',
                                    id:'customproduct',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'EnName',
                                    editable:false,
                                    valueField:'ProductID',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'CustomManage.asmx',methodName:'GetAllProducts'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'ProductID'},[{name:'ProductID'},{name:'CnName'},{name:'EnName'}])
                                    })}
                                ]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '定制中文名字',
                            id:'customcname',
                            allowBlank:false
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '定制英文名字',
                            maxLength:50,
                            regex: pp.wordnumregex,
                            regexText:pp.wordnumregexText,
                            id:'customename',
                            allowBlank:false
                        },
                        {
                            xtype:'textarea',
                            id:'cremark',
                            width:250,
                            height:80,
                            fieldLabel:'定制中文描述'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btncustomconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:405,
                    id:'customwin',
                    height:250,
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
                pp.initPanel('edit','编辑',record.data.CustomID);
                Ext.getCmp('customproduct').store.on('load',function(){
                    Ext.getCmp('customproduct').setValue(record.data.ProductID);
                });
                //Ext.getCmp('customproduct').setValue(record.data.ProductID);
                //Ext.getDom('customproduct').value=record.data.ProductEnName;
                Ext.getCmp('customcname').setValue(record.data.CnName);
                Ext.getCmp('customename').setValue(record.data.EnName);
                Ext.getCmp('cremark').setValue(record.data.Description);
            }
        },
        operateHandler:function(o,id){
            var pro = Ext.getCmp('customproduct');
            var cname = Ext.getCmp('customcname');
            var ename = Ext.getCmp('customename');
            if(pro.validate()&&cname.validate()&&ename.validate())
            {
                var cdes = Ext.getCmp('cremark').getValue();
                if(o=='add')
                    CustomManage.AddCustom(cname.getValue(),ename.getValue(),cdes,pro.getValue(),pp.callbackSucess,pp.callbackError);
                else
                    CustomManage.EditCustom(id,cname.getValue(),ename.getValue(),cdes,pro.getValue(),pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btncustomconfirm');
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
                CustomManage.DeleteCustoms(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        initupLoadWin:function(r,bagtype){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var username = document.getElementById('ctl00_ContentPlaceHolder1_curusername').value;
                var ugrid=new UploadGrid(record.data.CustomID,username,bagtype,PPLive.PPInstall.CustomVars.uploadprefixname,PPLive.PPInstall.CustomVars.uploadfiledes);
                var win = new Ext.Window({
                    title:'上传',
                    closable:false,
                    width:document.body.clientWidth*0.7,
                    id:'customuploadwin',
                    height:document.body.clientHeight*0.5,
                    layout:'border',
                    resizable:false,
                    maximizable:true,
                    modal:true,
                    items:[{
                        region:'center',
                        items:[ugrid.grid]
                    }],
                    buttons:[
                        {text:'关闭',id:'customuploadclose',handler:function(){win.close();pp.refreshHandler();}}
                    ],
                    listeners:{
                        bodyresize:function(p,w,h){
                            ugrid.grid.setWidth(w);
                            ugrid.grid.setHeight(h-65);
                        }
                    }
                });
                win.setAnimateTarget(Ext.getBody());
                win.show();
            }
        },
        upLoadHandler:function(r){
//            var record = pp.getRecord(grid,r);
//            if(record)
//            {
//                var panel = new Ext.form.FormPanel({
//                    id:'customuppanel',
//                    lableAlign:'left',
//                    buttonAlign:'right',   
//                    bodyStyle:'padding:5 15 0 5',
//                    width:400,
//                    height:80,
//                    fileUpload: true,
//                    deferredRender : false,
//                    labelWidth:95,
//                    frame:true,
//                    items:[
//                        {
//                            xtype:'fileuploadfield',
//                            id: 'customfileload',
//                            name:'customfileload',
//                            emptyText: '请选择一个安装包文件',
//                            fieldLabel: '安装包',
//                            allowBlank:false,
//                            msgTarget: 'side',
//                            anchor: '95%',
//                            buttonCfg: {
//                                text: '',
//                                iconCls: 'channelid'
//                            }
//                        }
//                    ],
//                    buttons:[
//                        {text:'确定',id:"submit-button",handler:pp.trueUpload.createDelegate(this,[record.data.CustomID])},
//                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
//                    ]
//                });
//                var win = new Ext.Window({
//                    title:'上传',
//                    closable:true,
//                    width:405,
//                    id:'customuploadwin',
//                    height:100,
//                    layout:'border',
//                    resizable:false,
//                    modal:true,
//                    items:[{
//                        region:'center',
//                        items:[panel]
//                    }]
//                });
//                win.setAnimateTarget(Ext.getBody());
//                win.show();
                
//            }
            pp.initupLoadWin(r,PPLive.PPInstall.CustomVars.fullbag);
        },
        upGuideLoadHandler:function(r){
            pp.initupLoadWin(r,PPLive.PPInstall.CustomVars.guidebag);
        },
        upSilentLoadHandler:function(r){
            pp.initupLoadWin(r,PPLive.PPInstall.CustomVars.silentbag);
        },
        upGCLoadHandler:function(r){
            pp.initupLoadWin(r,PPLive.PPInstall.CustomVars.gsbag);
        },
        trueUpload:function(customId){
            var up = Ext.getCmp('customfileload');
            if(up.validate())
            {
                var upf = up.getValue();
                //var ftype = upf.substring (upf.lastIndexOf('.')+1);
                //if(ftype.toLowerCase()!='swf'){Ext.MessageBox.alert('提示','选择的文件必须是swf文件！');return;}
                var fp = Ext.getCmp('customuppanel');
                var button = Ext.getCmp("submit-button");
                button.disable();
                Ext.MessageBox.show({title:'请稍后',msg: '正在上传，请稍后。。。',width:250,wait:true});
                if(fp.getForm().isValid()){
                    fp.getForm().submit({
                        url: 'UpLoad/CustomUpLoad.aspx',
                        params:{customid:customId},
                        success: function(form, o){
                            Ext.MessageBox.hide();
                            Ext.getCmp('customuploadwin').close();
                            pp.refreshHandler();
                        },
                        failure: function(form,o){
                            Ext.MessageBox.hide();
                            Ext.MessageBox.alert('提示',o.result.message);
                            button.enable();
                        }
                    });
                }
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('customwin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.CustomEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.CustomManage.pageInit.createDelegate(
        PPLive.PPInstall.CustomManage,
        [PPLive.PPInstall.CustomEdit]
    )
);

