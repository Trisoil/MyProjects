PPLive.PPInstall.Vars={
    downLinkPrefix:'http://bindad.bss.synacast.com/client'   //http://192.168.21.65:3721/client
};

PPLive.PPInstall.BindManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'BindManage.asmx',methodName:'GetAllBind'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'BindID'
                },[
                    'BindID','PageCount','ForceCount','PopupPage','ClientFullPath','GuideClientFullPath','SilentClientFullPath','IsStart','IsStartWithWindows','BindMode','ChannelID','CustomID','CustomCnName','CustomEnName','CustomProductID','ProductCnName','ProductEnName','ChannelCnName','ChannelEnName','ChannelType'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'numeric',  dataIndex: 'PageCount'},
				    {type: 'numeric',  dataIndex: 'ForceCount'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'捆绑软件页个数',width:100,sortable:true,dataIndex:'PageCount'}, 
                {header:'强制软件个数',width:100,sortable:true,dataIndex:'ForceCount'}, 
                {header:'完成后弹出页面',width:150,sortable:true,dataIndex:'PopupPage',renderer:ppem.showTip},
                //{header:'渠道安装包路径',width:150,sortable:true,dataIndex:'ClientFullPath',renderer:ppem.showTip},
                {header:'是否立即启动',width:90,sortable:true,dataIndex:'IsStart',renderer:pp.renderBool},
                {header:'是否随Window启动',width:110,sortable:true,dataIndex:'IsStartWithWindows',renderer:pp.renderBool},
                {header:'捆绑模式',width:90,sortable:true,dataIndex:'BindMode',renderer:pp.renderBindMode},
                {header:'所属定制中文名称',width:120,sortable:true,dataIndex:'CustomCnName'},
                {header:'所属定制英文名称',width:120,sortable:true,dataIndex:'CustomEnName'},
                {header:'所属产品中文名称',width:120,sortable:true,dataIndex:'ProductCnName'},
                {header:'所属产品英文名称',width:120,sortable:true,dataIndex:'ProductEnName'},
                {header:'所属渠道中文名称',width:120,sortable:true,dataIndex:'ChannelCnName'},
                {header:'所属渠道英文名称',width:120,sortable:true,dataIndex:'ChannelEnName'},
                {header:'所属渠道类型',width:120,sortable:true,dataIndex:'ChannelType'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnPage = new Ext.Button({text:'捆绑页',iconCls:'bindPage',handler:ppem.bindPageHandler});
            var btnChannel = new Ext.Button({text:'生成渠道安装包',iconCls:'pagecomponent',handler:ppem.makeChannelHandler});
            var btndownChannel = new Ext.Button({text:'下载完整包',iconCls:'pagedown',handler:ppem.downChannelHandler});
            var btncopydownlink = new Ext.Button({text:'复制完整包链接',iconCls:'copy',handler:ppem.copyDownLinkHandler});
            var btndownGuide = new Ext.Button({text:'下载引导包',iconCls:'pagedown',handler:ppem.downGuideHandler});
            var btncopyGuide = new Ext.Button({text:'复制引导包链接',iconCls:'copy',handler:ppem.copyGuideHandler});  
            var btndownSilent = new Ext.Button({text:'下载静默包',iconCls:'pagedown',handler:ppem.downSilentHandler});
            var btncopySilent = new Ext.Button({text:'复制静默包链接',iconCls:'copy',handler:ppem.copySilentHandler});
            var btncopycustomen = new Ext.Button({text:'复制定制英文名',iconCls:'copy',handler:ppem.copyCustomENHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            var twoTbar = new Ext.Toolbar({
                items:['-',btndownChannel,'-',btncopydownlink,'-',btndownGuide,'-',btncopyGuide,'-',btndownSilent,'-',btncopySilent,'-']
            });   
            grid = new Ext.grid.GridPanel({
                el:'bindmange_grid_div',
                id:'bindmange_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnPage,'-',btnChannel,'-',btncopycustomen,'-',btnRefresh,'-'],
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
                    render:function(){
                         twoTbar.render(this.tbar);
                    },
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
					            //{text:'删除',iconCls:'remove',listeners:{click:ppem.deleteHandler.createDelegate(this,[rowindex])}},
					            {text:'捆绑页',iconCls:'bindPage',listeners:{click:ppem.bindPageHandler.createDelegate(this,[rowindex])}},
					            {text:'生成渠道安装包',iconCls:'pagecomponent',listeners:{click:ppem.makeChannelHandler.createDelegate(this,[rowindex])}},
					            {text:'复制定制英文名',iconCls:'copy',listeners:{click:ppem.copyCustomENHandler.createDelegate(this,[rowindex])}},
					            {text:'下载完整包',iconCls:'pagedown',listeners:{click:ppem.downChannelHandler.createDelegate(this,[rowindex])}},
					            {text:'复制完整包链接',iconCls:'copy',listeners:{click:ppem.copyDownLinkHandler.createDelegate(this,[rowindex])}},
					            {text:'下载引导包',iconCls:'pagedown',listeners:{click:ppem.downGuideHandler.createDelegate(this,[rowindex])}},
					            {text:'复制引导包链接',iconCls:'copy',listeners:{click:ppem.copyGuideHandler.createDelegate(this,[rowindex])}},
					            {text:'下载静默包',iconCls:'pagedown',listeners:{click:ppem.downSilentHandler.createDelegate(this,[rowindex])}},
					            {text:'复制静默包链接',iconCls:'copy',listeners:{click:ppem.copySilentHandler.createDelegate(this,[rowindex])}},
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
        renderBool:function(value, meta, rec, rowIdx, colIdx, ds){
            switch(value)
            {
                case 0:
                    return '否';
                default:   // 1
                    return '是';
            }
        },
        renderBindMode:function(value, meta, rec, rowIdx, colIdx, ds){
            switch(value)
            {
                case -1:
                    return '安装';
                case 0:   
                    return '静默升级';
                case 1:  
                    return '提示强升';
                case 2: 
                    return '推荐升级';
                case 3:
                    return '提示升级';
                case 4: 
                    return '自动强升';
                case 5:
                    return '主动升级';
            }
        }
    }
}();

PPLive.PPInstall.BindEdit=function(){
    var pp;
    var grid;
    var keys;
    var makechannelr;
    var bmodeldata=[[0,'静默升级'],[1,'提示强升'],[2,'推荐升级'],[3,'提示升级'],[4,'自动强升'],[5,'主动升级']];
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
                    labelWidth:115,
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
                                    width:120,
                                    text:'所属定制：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'bindcustom',
                                    allowBlank:false,
                                    fieldLabel: '所属定制',
                                    id:'bindcustom',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'Name',
                                    editable:false,
                                    valueField:'ID',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'BindManage.asmx',methodName:'GetAllCustoms'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'ID'},[{name:'ID'},{name:'Name'}])
                                    })}
                                ]
                        },
                        {
                            layout:'column',
                            columnWidth:.1,
                            //bodyStyle:'padding:1 0 5 0',
                            style:'padding:1px 0px 5px 0px',
                            items:[{
                                    xtype:'label',
                                    fieldLabel:'',
                                    width:120,
                                    text:'所属渠道：'
                                },
                                {
                                    xtype: 'combo',
                                    name: 'bindchannel',
                                    allowBlank:false,
                                    fieldLabel: '所属渠道',
                                    id:'bindchannel',
                                    mode:'local',
                                    selectOnFocus:true,
                                    forceSelection: true,
                                    displayField:'Name',
                                    editable:false,
                                    valueField:'ID',typeAhead: true,showTip:true,triggerAction:'all',width:150,
                                    store:new Ext.data.Store({
                                        autoLoad:true,
                                        proxy: new Ext.data.WebServiceProxy({servicePath: 'BindManage.asmx',methodName:'GetAllChannel'}),
                                        reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'ID'},[{name:'ID'},{name:'Name'}])
                                    })}
                                ]
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:120,
                                        text:'捆绑模式：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'bindmodel1',
                                        checked:true,
                                        boxLabel:'安装',
                                        inputValue:-1,
                                        name:'bindmodel_radio',
                                        listeners:{
                                            check:pp.panelInit
                                        }
                                    },
                                    {
                                        xtype:'radio',
                                        id:'bindmodel2',
                                        boxLabel:'升级',
                                        inputValue:0,
                                        name:'bindmodel_radio',
                                        listeners:{
                                            check:pp.panelInit
                                        }
                                    }
                                ]
                        },
                        {
                            xtype:'combo',
                            id:'bindmodelcombo',
                            fieldLabel: '捆绑模式',
                            mode:'local',
                            displayField:'typeName',value:0,
                            valueField:'typeValue',editable:false,typeAhead: true,showTip:true,triggerAction:'all',width:150,
                            store:new Ext.data.SimpleStore({fields:['typeValue','typeName'],data:bmodeldata})
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:120,
                                        text:'是否立即启动：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'bindstartyes',
                                        checked:true,
                                        boxLabel:'是',
                                        inputValue:1,
                                        name:'bindisstart_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'bindstartno',
                                        boxLabel:'否',
                                        inputValue:0,
                                        name:'bindisstart_radio'
                                    }]
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:120,
                                        text:'是否随Windows启动：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'bindwindowyes',
                                        checked:true,
                                        boxLabel:'是',
                                        inputValue:1,
                                        name:'bindiswindow_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'bindwindowno',
                                        boxLabel:'否',
                                        inputValue:0,
                                        name:'bindiswindow_radio'
                                    }]
                        },
                        {
                            xtype: 'textfield',
                            width:245,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '完成后弹出页面',
                            id:'bindpoppage'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnbindconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:405,
                    id:'bindwin',
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
                Ext.getCmp('bindmodelcombo').getEl().up('.x-form-item').setDisplayed( false );
        },
        panelInit:function(r,c){
            if(r.id=='bindmodel1'&&c)
            {
                Ext.getCmp('bindmodelcombo').getEl().up('.x-form-item').setDisplayed( false );
            }
            else if(r.id=='bindmodel2'&&c)
            {
                Ext.getCmp('bindmodelcombo').getEl().up('.x-form-item').setDisplayed( true );
            }
        },
        adddHandler:function(){
            pp.initPanel('add','增加');
        },
        editHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                pp.initPanel('edit','编辑',record.data.BindID);
                Ext.getCmp('bindcustom').store.on('load',function(){
                    Ext.getCmp('bindcustom').setValue(record.data.CustomID);
                });
                Ext.getCmp('bindchannel').store.on('load',function(){
                    Ext.getCmp('bindchannel').setValue(record.data.ChannelID);
                });
                Ext.getCmp('bindpoppage').setValue(record.data.PopupPage);
                pp.initIsStart(record.data.IsStart);
                pp.initIsWindowsStart(record.data.IsStartWithWindows);
                pp.initBindModel(record.get('BindMode'));
//                Ext.getCmp('bindcustom').setValue(record.data.CustomID);
//                //Ext.getCmp('bindcustom').selectByValue(record.data.CustomID,true);
//                Ext.getDom('bindcustom').value=String.format('{0}|{1}',record.data.ProductEnName,record.data.CustomEnName);
//                Ext.getCmp('bindchannel').setValue(record.data.ChannelID);
//                Ext.getDom('bindchannel').value=record.data.ChannelEnName;
//                Ext.getCmp('bindpoppage').setValue(record.data.PopupPage);
//                pp.initIsStart(record.data.IsStart);
//                pp.initIsWindowsStart(record.data.IsStartWithWindows);
            }
        },
        operateHandler:function(o,id){
            var custom = Ext.getCmp('bindcustom');
            var channel = Ext.getCmp('bindchannel');
            if(custom.validate()&&channel.validate())
            {
                var page = Ext.getCmp('bindpoppage').getValue();
                var isstart = pp.getIsStart();
                var iswindows = pp.getIsWindowStart();
                var bmodel = pp.getBindModel();
                if(o=='add')
                    BindManage.AddBind(page,custom.getValue(),channel.getValue(),isstart,iswindows,bmodel,pp.callbackSucess,pp.callbackError);
                else
                    BindManage.EditBind(id,page,custom.getValue(),channel.getValue(),isstart,iswindows,bmodel,pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnbindconfirm');
            }
        },
        initBindModel:function(v){
            if(v==-1)
            {
                Ext.getCmp('bindmodel1').setValue(true);Ext.getCmp('bindmodel2').setValue(false);
            }
            else
            {
                Ext.getCmp('bindmodel1').setValue(false);Ext.getCmp('bindmodel2').setValue(true);
                Ext.getCmp('bindmodelcombo').setValue(v);
            }
        },
        initIsStart:function(v){
            switch(v)
            {
                case 0:
                    Ext.getCmp('bindstartyes').setValue(false);Ext.getCmp('bindstartno').setValue(true);return;
                case 1:
                    Ext.getCmp('bindstartyes').setValue(true);Ext.getCmp('bindstartno').setValue(false);return;
            }
        },
        initIsWindowsStart:function(v){
            switch(v)
            {
                case 0:
                    Ext.getCmp('bindwindowyes').setValue(false);Ext.getCmp('bindwindowno').setValue(true);return;
                case 1:
                    Ext.getCmp('bindwindowyes').setValue(true);Ext.getCmp('bindwindowno').setValue(false);return;
            }
        },
        getBindModel:function(){
            if(Ext.getCmp('bindmodel1').checked)return -1;
            return Ext.getCmp('bindmodelcombo').getValue();
        },
        getIsStart:function(){
            if(Ext.getCmp('bindstartyes').checked)return 1;
            return 0;
        },
        getIsWindowStart:function(){
            if(Ext.getCmp('bindwindowyes').checked)return 1;
            return 0;
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
                BindManage.DeleteBinds(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        bindPageHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var bindid=record.data.BindID;
                Ext.Dialog.init({title:"捆绑页",width:document.body.clientWidth*0.75,height:document.body.clientHeight*0.8,fullMask:false,maximizable:true});
                Ext.Dialog.open("Page.aspx?bindid="+bindid,pp.closeCallback);
            }
        },
        closeCallback:function(returnValue){
//            if(returnValue=='OK')
//            {
                grid.store.reload();
                grid.selModel.clearSelections();
//            }
        },
        makeChannelHandler:function(r){
            makechannelr=r;
            Ext.MessageBox.confirm('提示','确定要生成选定记录的渠道安装包吗？',pp.trueMakeChannel);
        },
        trueMakeChannel:function(btn){
            if(btn=='yes')
            {
                var record = pp.getRecord(grid,makechannelr);
                if(record)
                {
                    Ext.MessageBox.show({title:'请稍后',msg: '正在生成，请稍后。。。',width:250,wait:true});
                    BindManage.MakeChannel(record.data.CustomID,record.data.ChannelID,pp.makeChannelSucess,pp.makeChannelError);
                }
            }
        },
        makeChannelSucess:function(result){
            Ext.MessageBox.hide();
            pp.refreshHandler();
        },
        makeChannelError:function(result){
            Ext.MessageBox.hide();
            Ext.MessageBox.show({title: '信息',msg: result.get_message(),buttons: Ext.MessageBox.OK,icon:Ext.MessageBox.INFO,fn:function(){pp.refreshHandler();}});
        },        
        copyCustomENHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                pp.copyBoard(record.data.CustomEnName);
            }
        },
        initDownLink:function(rec,col){
            if(!rec.get(col))
            {
                Ext.MessageBox.alert('提示','链接地址为空，请生成渠道安装包后使用该功能！');
                return;
            }
            return String.format("{0}/{1}",PPLive.PPInstall.Vars.downLinkPrefix,rec.get(col)).replace(/\\/g,"/");  
        },
        initDownLoad:function(r,col){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var url = pp.initDownLink(record,col);
                if(url)
                    window.open(encodeURI(url));
            }
        },
        initCopy:function(r,col){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var url = pp.initDownLink(record,col);
                if(url)
                    pp.copyBoard(url);
            }
        },
        downChannelHandler:function(r){
            pp.initDownLoad(r,'ClientFullPath');
        },
        copyDownLinkHandler:function(r){
            pp.initCopy(r,'ClientFullPath');
        },
        downGuideHandler:function(r){
            pp.initDownLoad(r,'GuideClientFullPath');
        },
        copyGuideHandler:function(r){
            pp.initCopy(r,'GuideClientFullPath');
        },
        downSilentHandler:function(r){
            pp.initDownLoad(r,'SilentClientFullPath');
        },
        copySilentHandler:function(r){
            pp.initCopy(r,'SilentClientFullPath');
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('bindwin');
            if(win)win.close();
            pp.refreshHandler();
        }
    }
}();

Ext.apply(PPLive.PPInstall.BindEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.BindManage.pageInit.createDelegate(
        PPLive.PPInstall.BindManage,
        [PPLive.PPInstall.BindEdit]
    )
);