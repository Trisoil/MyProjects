
PPLive.PPInstall.SoftManageVar = {
    treeJson:null,
    uploadUrlPrefix:'http://img-ins.pplive.cn',
    areaCombo:[[0,'无控制'],[1,'禁止优先'],[2,'允许优先']]
}

PPLive.PPInstall.SoftManage=function(){
    var ppem;
    var grid;
    var bid;
    var pp;
    return{
        pageInit:function(em){
            pp=this;
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'SoftManage.asmx',methodName:'GetAllSoft'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'SoftID'
                },[
                    'SoftID','Type','CnName','EnName','CnDescription','EnDescription','Url','ImageUrl','Parameter','MainPage','ForcePage','ExcludePage','IconDownloadUrl','IconLinkUrl','IconName','DesktopName','DesktopEnName','IsChecked','CountPerDay','AllCPDOfChannel','BindEnable','BlackCityCodes','WhiteCityCodes','AreaControlType'
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
                {header:'全局每天下发量',width:100,sortable:true,dataIndex:'CountPerDay'},
                {header:'所有渠道每天下发量总和',width:140,sortable:true,dataIndex:'AllCPDOfChannel'},
                {header:'软件类型',width:90,sortable:true,dataIndex:'Type',renderer:pp.showType}, 
                {header:'软件中文名称',width:120,sortable:true,dataIndex:'CnName'}, 
                {header:'软件英文名称',width:120,sortable:true,dataIndex:'EnName'},
                {header:'软件中文描述',width:200,sortable:true,dataIndex:'CnDescription',renderer:ppem.showTip},
                {header:'软件英文描述',width:200,sortable:true,dataIndex:'EnDescription',renderer:ppem.showTip},
                {header:'默认是否选择安装',width:120,sortable:true,dataIndex:'IsChecked',renderer:ppem.showBoolValue},
                {header:'绑定是否有效',width:100,sortable:true,dataIndex:'BindEnable',renderer:ppem.showBoolValue},
                {header:'软件下载地址',width:120,sortable:true,dataIndex:'Url',renderer:ppem.showTip},
                {header:'软件介绍图片',width:120,sortable:true,dataIndex:'ImageUrl',renderer:ppem.showTip},
                {header:'软件启动参数',width:120,sortable:true,dataIndex:'Parameter'},
                {header:'软件主页',width:120,sortable:true,dataIndex:'MainPage',renderer:ppem.showTip},
                {header:'需要强制的主页',width:120,sortable:true,dataIndex:'ForcePage',renderer:ppem.showTip},
                {header:'需要排除的主页',width:120,sortable:true,dataIndex:'ExcludePage',renderer:ppem.showTip},
                {header:'桌面图标下载地址',width:120,sortable:true,dataIndex:'IconDownloadUrl',renderer:ppem.showTip},
                {header:'桌面图标链接地址',width:120,sortable:true,dataIndex:'IconLinkUrl',renderer:ppem.showTip},
                {header:'桌面图标本地名称',width:120,sortable:true,dataIndex:'IconName'},
                {header:'桌面图标快捷方式名称',width:120,sortable:true,dataIndex:'DesktopName'},
                {header:'桌面图标快捷方式英文名称',width:120,sortable:true,dataIndex:'DesktopEnName'}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnBindTrue = new Ext.Button({text:'绑定有效',iconCls:'pageuser',handler:ppem.bindTrueHandler});
            var btnBindFalse = new Ext.Button({text:'绑定无效',iconCls:'deathhead',handler:ppem.bindFalseHandler});
            var btnArea = new Ext.Button({text:'地域设置',iconCls:'start',handler:ppem.areaSettingHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'softmange_grid_div',
                id:'softmange_grid',
                region:'center',
                store: store,
                cm: cm,
                sm:sm,
                frame:true,
                plugins: filters,
                trackMouseOver:true,
                loadMask: true,
                tbar:['-',btnAdd,'-',btnEdit,'-',btnDelete,'-',btnBindTrue,'-',btnBindFalse,'-',btnArea,'-',btnRefresh,'-'],
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
					            {text:'绑定有效',iconCls:'pageuser',listeners:{click:ppem.bindTrueHandler.createDelegate(this,[rowindex])}},
					            {text:'绑定无效',iconCls:'deathhead',listeners:{click:ppem.bindFalseHandler.createDelegate(this,[rowindex])}},
					            {text:'地域设置',iconCls:'start',listeners:{click:ppem.areaSettingHandler.createDelegate(this,[rowindex])}},
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
                case 1:
                    return '捆绑软件';
                case 2:
                    return '设置主页';
                case 3:
                    return '桌面图标';
                default:
                    return value;
            }
        }
    }
}();

PPLive.PPInstall.SoftEdit=function(){
    var pp;
    var grid;
    var keys;
    var bindtruekeys;
    var bindfalsekeys;
    var bid;
    var ppvars;
    var checkeNodes=[];
    return{
        editInit:function(gridPanel,bindid){
            pp=this;
            grid=gridPanel;
            bid=bindid;
            ppvars=PPLive.PPInstall.SoftManageVar;
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
                    bodyStyle:'padding:0 15 0 5',
                    width:450,
                    height:415,
                    deferredRender : false,
                    labelWidth:130,
                    frame:true,
                    items:[
                        {
                            layout: 'column',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:135,
                                        text:'软件类型：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softtypea',
                                        checked:true,
                                        boxLabel:'捆绑软件',
                                        inputValue:1,
                                        name:'softtype_radio',
                                        listeners:{
                                            check:pp.panelInit
                                        }
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softtypeb',
                                        boxLabel:'设置主页',
                                        inputValue:2,
                                        name:'softtype_radio',
                                        listeners:{
                                            check:pp.panelInit
                                        }
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softtypec',
                                        boxLabel:'桌面图标',
                                        inputValue:3,
                                        name:'softtype_radio',
                                        listeners:{
                                            check:pp.panelInit
                                        }
                                    }]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件中文名称',
                            allowBlank:false,
                            id:'softcname'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件英文名称',
                            id:'softename'
                        },
                        {
                            xtype:'textarea',
                            id:'softcdes',
                            width:250,
                            maxLength:24,
                            height:40,
                            fieldLabel:'软件中文描述'
                        },
                        {
                            xtype:'textarea',
                            id:'softedes',
                            width:250,
                            height:40,
                            maxLength:48,
                            fieldLabel:'软件英文描述'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件下载地址',
                            id:'softdownurl'
                        },
                        {
                            layout: 'column',
                            border: false,
                            columnWidth:1,
                            items:[
                                {
                                    layout: 'form',
                                    items:[
                                        {
                                            xtype: 'textfield',
                                            width:220,
                                            bodyStyle:'padding:0 0 5 0',
                                            fieldLabel: '软件介绍图片',
                                            id:'softimageurl'
                                        }
                                    ]
                                },
                                {
                                    bodyStyle: 'padding-left:5px;',
                                    items:[{
                                        xtype:'button',
                                        fieldLabel:'',
                                        text:'上传',
                                        id:'btn_soft_imageurl',
                                        handler:pp.imageurlUpload.createDelegate(pp,[id])
                                    }]
                                }
                            ]
                        },
//                        {
//                            xtype: 'textfield',
//                            width:250,
//                            bodyStyle:'padding:0 0 5 0',
//                            fieldLabel: '软件介绍图片',
//                            id:'softimageurl'
//                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件启动参数',
                            id:'softparameter'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '软件主页',
                            id:'softmainpage'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '需要强制的主页',
                            id:'softfmainpage'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '需要排除的主页',
                            id:'softexmainpage'
                        },
                        {
                            layout: 'column',
                            border: false,
                            columnWidth:1,
                            items:[
                                {
                                    layout: 'form',
                                    items:[{
                                        xtype: 'textfield',
                                        width:220,
                                        bodyStyle:'padding:0 0 5 0',
                                        fieldLabel: '图标下载地址',
                                        id:'softicondownurl'
                                    }]
                                },
                                {
                                    bodyStyle: 'padding-left:5px;',
                                    items:[{
                                        xtype:'button',
                                        fieldLabel:'',
                                        text:'上传',
                                        id:'btn_soft_icondownurl',
                                        handler:pp.icondownUpload.createDelegate(pp,[id])
                                    }]
                                }
                            ]
                        },
//                        {
//                            xtype: 'textfield',
//                            width:250,
//                            bodyStyle:'padding:0 0 5 0',
//                            fieldLabel: '软件桌面图标下载地址',
//                            id:'softicondownurl'
//                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '图标链接地址',
                            id:'softiconlinkurl'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '图标本地名称',
                            regex:/.+(\.ico)$/,
                            regexText:'该输入项必须以.ico为后缀名',
                            allowBlank:false,
                            id:'softiconname'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '图标快捷方式名称',
                            regex:/.+(\.lnk)$/,
                            regexText:'该输入项必须以.lnk为后缀名',
                            allowBlank:false,
                            id:'softdesktopname'
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '图标快捷方式英文名称',
                            regex:/.+(\.lnk)$/,
                            regexText:'该输入项必须以.lnk为后缀名',
                            allowBlank:false,
                            id:'softdesktopenname'
                        },
                        {
                            layout: 'column',
                            id:'columnsoftcheck_radio',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:135,
                                        text:'默认是否选择安装：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softcheckyes',
                                        checked:true,
                                        boxLabel:'是',
                                        inputValue:true,
                                        name:'softcheck_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softcheckno',
                                        boxLabel:'否',
                                        inputValue:false,
                                        name:'softcheck_radio'
                                    }]
                        },
                        {
                            layout: 'column',
                            id:'columnsoftbind_radio',
                            columnWidth:1,
                            //bodyStyle:'padding:0 0 5 0',
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:135,
                                        text:'绑定是否有效：'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softbindyes',
                                        checked:true,
                                        boxLabel:'是',
                                        inputValue:true,
                                        name:'softbind_radio'
                                    },
                                    {
                                        xtype:'radio',
                                        id:'softbindno',
                                        boxLabel:'否',
                                        inputValue:false,
                                        name:'softbind_radio'
                                    }]
                        },
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '全局每天下发量',
                            value:0,
                            allowBlank:false,
                            regex:pp.zeroRegex,
                            regexText:pp.zeroRegexText,
                            maxLength:9,
                            id:'softcountperday'
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btnsoftconfirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:455,
                    id:'bindsoftmanagewin',
                    height:435,
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
                pp.controlInit();
        },
        controlInit:function(){
            Ext.getCmp('softdownurl').getEl().up('.x-form-item').setDisplayed( true );
            Ext.getCmp('softimageurl').getEl().up('.x-form-item').setDisplayed( true );
            Ext.getCmp('btn_soft_imageurl').show();
            Ext.getCmp('softparameter').getEl().up('.x-form-item').setDisplayed( true );
            Ext.getCmp('softmainpage').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('softfmainpage').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('softexmainpage').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('softicondownurl').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('btn_soft_icondownurl').hide();
            Ext.getCmp('softiconlinkurl').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('softiconname').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('softdesktopname').getEl().up('.x-form-item').setDisplayed( false );
            Ext.getCmp('softdesktopenname').getEl().up('.x-form-item').setDisplayed( false );
        },
        panelInit:function(r,c){
            if(r.id=='softtypea'&&c)
            {
                Ext.getCmp('softdownurl').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softimageurl').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('btn_soft_imageurl').show();
                Ext.getCmp('softparameter').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softmainpage').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softfmainpage').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softexmainpage').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softicondownurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('btn_soft_icondownurl').hide();
                Ext.getCmp('softiconlinkurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softiconname').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softdesktopname').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softdesktopenname').getEl().up('.x-form-item').setDisplayed( false );
            }
            else if(r.id=='softtypeb'&&c)
            {
                Ext.getCmp('softdownurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softimageurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('btn_soft_imageurl').hide();
                Ext.getCmp('softparameter').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softmainpage').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softfmainpage').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softexmainpage').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softicondownurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('btn_soft_icondownurl').hide();
                Ext.getCmp('softiconlinkurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softiconname').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softdesktopname').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softdesktopenname').getEl().up('.x-form-item').setDisplayed( false );
            }
            else if(r.id=='softtypec'&&c)
            {
                Ext.getCmp('softdownurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softimageurl').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('btn_soft_imageurl').hide();
                Ext.getCmp('softparameter').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softmainpage').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softfmainpage').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softexmainpage').getEl().up('.x-form-item').setDisplayed( false );
                Ext.getCmp('softicondownurl').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('btn_soft_icondownurl').show();
                Ext.getCmp('softiconlinkurl').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softiconname').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softdesktopname').getEl().up('.x-form-item').setDisplayed( true );
                Ext.getCmp('softdesktopenname').getEl().up('.x-form-item').setDisplayed( true );
            }
        },
        adddHandler:function(){
            pp.initPanel('add','增加');
        },
        editHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                pp.initPanel('edit','编辑',record.data.SoftID);
                pp.initSoftType(record.data.Type);
                pp.initSoftCheck(record.data.IsChecked);
                pp.initSoftBind(record.data.BindEnable);
                Ext.getCmp('softcname').setValue(record.data.CnName);
                Ext.getCmp('softename').setValue(record.data.EnName);
                Ext.getCmp('softcdes').setValue(record.data.CnDescription);
                Ext.getCmp('softedes').setValue(record.data.EnDescription);
                Ext.getCmp('softdownurl').setValue(record.data.Url);
                Ext.getCmp('softimageurl').setValue(record.data.ImageUrl);
                Ext.getCmp('softparameter').setValue(record.data.Parameter);
                Ext.getCmp('softmainpage').setValue(record.data.MainPage);
                Ext.getCmp('softfmainpage').setValue(record.data.ForcePage);
                Ext.getCmp('softexmainpage').setValue(record.data.ExcludePage);
                Ext.getCmp('softicondownurl').setValue(record.data.IconDownloadUrl);
                Ext.getCmp('softiconlinkurl').setValue(record.data.IconLinkUrl);
                Ext.getCmp('softiconname').setValue(record.data.IconName);
                Ext.getCmp('softdesktopname').setValue(record.data.DesktopName);
                Ext.getCmp('softdesktopenname').setValue(record.data.DesktopEnName);
                Ext.getCmp('softcountperday').setValue(record.data.CountPerDay);
            }
        },
        operateHandler:function(o,id){
            var cname = Ext.getCmp('softcname');
            var countperday = Ext.getCmp('softcountperday');
            var type = pp.getSoftType();
            var iconnamecmp = Ext.getCmp('softiconname');
            var desktopnamecmp = Ext.getCmp('softdesktopname');
            var deskennamecmp = Ext.getCmp('softdesktopenname');
            var cdes = Ext.getCmp('softcdes');
            var edes = Ext.getCmp('softedes');
            var url = Ext.getCmp('softdownurl').getValue();
            if((cname.validate()&&countperday.validate()&&type!=3&&cdes.validate()&&edes.validate())||(type==3&&iconnamecmp.validate()&&desktopnamecmp.validate()&&deskennamecmp.validate()&&cname.validate()&&countperday.validate()&&cdes.validate()&&edes.validate()))
            {
                var ischeck = pp.getSoftCheck();
                var bindenable = pp.getSoftBind();
                var ename = Ext.getCmp('softename').getValue();
                var imagurl = Ext.getCmp('softimageurl').getValue();
                var par = Ext.getCmp('softparameter').getValue();
                var mainpage = Ext.getCmp('softmainpage').getValue();
                var fpage = Ext.getCmp('softfmainpage').getValue();
                var expage = Ext.getCmp('softexmainpage').getValue();
                var iconname = iconnamecmp.getValue();
                var desktopname = desktopnamecmp.getValue();
                var desktopenname = deskennamecmp.getValue();
                var icondownurl = Ext.getCmp('softicondownurl').getValue(); 
                var iconlinkurl = Ext.getCmp('softiconlinkurl').getValue();
                if(o=='add')
                    SoftManage.AddSoft(type,cname.getValue(),ename,cdes.getValue(),edes.getValue(),url,imagurl,par,mainpage,fpage,expage,icondownurl,iconlinkurl,iconname,desktopname,desktopenname,ischeck,bindenable,countperday.getValue(),pp.callbackSucess,pp.callbackError);
                else
                    SoftManage.EditSoft(id,type,cname.getValue(),ename,cdes.getValue(),edes.getValue(),url,imagurl,par,mainpage,fpage,expage,icondownurl,iconlinkurl,iconname,desktopname,desktopenname,ischeck,bindenable,countperday.getValue(),pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btnsoftconfirm');
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
                SoftManage.DeleteSofts(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        initSoftType:function(v){
            switch(v)
            {
                case 1:
                    Ext.getCmp('softtypea').setValue(true);Ext.getCmp('softtypeb').setValue(false);Ext.getCmp('softtypec').setValue(false);return;
                case 2:
                    Ext.getCmp('softtypea').setValue(false);Ext.getCmp('softtypeb').setValue(true);Ext.getCmp('softtypec').setValue(false);return;
                case 3:
                    Ext.getCmp('softtypea').setValue(false);Ext.getCmp('softtypeb').setValue(false);Ext.getCmp('softtypec').setValue(true);return;
            }
        },
        initSoftCheck:function(v){
            if(v)
            {
                Ext.getCmp('softcheckyes').setValue(true);
                Ext.getCmp('softcheckno').setValue(false);  
            }
            else
            {
                Ext.getCmp('softcheckyes').setValue(false);
                Ext.getCmp('softcheckno').setValue(true);  
            }
        },
        initSoftBind:function(v){
            if(v)
            {
                Ext.getCmp('softbindyes').setValue(true);
                Ext.getCmp('softbindno').setValue(false);  
            }
            else
            {
                Ext.getCmp('softbindyes').setValue(false);
                Ext.getCmp('softbindno').setValue(true);  
            }
        },
        getSoftType:function(){
            if(Ext.getCmp('softtypea').checked)return 1;
            else if(Ext.getCmp('softtypeb').checked)return 2;
            else return 3;
        },
        getSoftCheck:function(){
            if(Ext.getCmp('softcheckyes').checked)return true;
            return false;
        },
        getSoftBind:function(){
            if(Ext.getCmp('softbindyes').checked)return true;
            return false;
        },
        bindTrueHandler:function(r){
            bindtruekeys = pp.getKeys(grid,r);
            if(bindtruekeys&&bindtruekeys.length>0)
            {
                Ext.MessageBox.confirm('提示','确定要将选定的'+bindtruekeys.length+'条记录的绑定状态置为有效吗？',pp.trueBindTrueHandler);
            }
        },
        bindFalseHandler:function(r){
            bindfalsekeys = pp.getKeys(grid,r);
            if(bindfalsekeys&&bindfalsekeys.length>0)
            {
                Ext.MessageBox.confirm('提示','确定要将选定的'+bindfalsekeys.length+'条记录的绑定状态置为无效吗？',pp.trueBindFalseHandler);
            }
        },
        trueBindTrueHandler:function(btn){
            if(btn=='yes')
            {
                SoftManage.BindSoftEnable(bindtruekeys,true,pp.callbackSucess,pp.callbackError);
            }
        },
        trueBindFalseHandler:function(btn){
            if(btn=='yes')
            {
                SoftManage.BindSoftEnable(bindfalsekeys,false,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('bindsoftmanagewin');
            if(win)win.close();
            var owin = Ext.getCmp('soft_areas_win');
            if(owin)owin.close();
            pp.refreshHandler();
        },
        imageurlUpload:function(id){
            var filename = Ext.getCmp('softimageurl').getValue().replace(PPLive.PPInstall.SoftManageVar.uploadUrlPrefix,'');
            pp.initUploadPanel(id,filename,'imageurlupload');
        },
        icondownUpload:function(id){
            var filename = Ext.getCmp('softicondownurl').getValue().replace(PPLive.PPInstall.SoftManageVar.uploadUrlPrefix,'');
            pp.initUploadPanel(id,filename,'icondownupload');
        },
        areaSettingHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                var panel = new Ext.form.FormPanel({
                    lableAlign:'left',
                    buttonAlign:'right',   
                    bodyStyle:'padding:5 15 0 5',
                    width:420,
                    height:240,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            xtype:'combo',
                            id:'s_areas_com',
                            width:120,
                            fieldLabel: '地域控制类型',
                            disabled:true,
                            store:new Ext.data.SimpleStore({fields:['typeValue','typeName'],data:ppvars.areaCombo}),
                            displayField:'typeName',valueField:'typeValue',mode:'local',forceSelection:true,editable:false,triggerAction:'all'
                        },
                        {
                            layout: 'column',
                            border: false,
                            columnWidth:1,
                            items:[
                                {
                                    layout: 'form',
                                    items:[{
                                        xtype: 'textarea',
                                        width:220,
                                        bodyStyle:'padding:0 0 5 0',
                                        fieldLabel: '白名单',
                                        readOnly:true,
                                        id:'soft_white'
                                    }]
                                },
                                {
                                    layout: 'form',
                                    items:[
                                        {
                                            bodyStyle: 'padding-left:5px;',
                                            items:[{
                                                xtype:'button',
                                                fieldLabel:'',
                                                text:'编辑',
                                                id:'btn_soft_white',
                                                handler:pp.initIPAreaTree.createDelegate(pp,[record,'soft_white'])
                                            }]
                                        },
                                        {
                                            bodyStyle: 'padding-left:5px;padding-top:5px;',
                                            items:[{
                                                xtype:'button',
                                                fieldLabel:'',
                                                text:'地域组',
                                                id:'btn_soft_Mwhite',
                                                handler:pp.initAreaGroup.createDelegate(pp,[record,'soft_white'])
                                            }]
                                        }
                                    ]
                                }
                            ]
                        },
                        {
                            layout: 'column',
                            border: false,
                            columnWidth:1,
                            items:[
                                {
                                    layout: 'form',
                                    items:[{
                                        xtype: 'textarea',
                                        width:220,
                                        bodyStyle:'padding:0 0 5 0',
                                        fieldLabel: '黑名单',
                                        readOnly:true,
                                        id:'soft_black'
                                    }]
                                },
                                {
                                    layout: 'form',
                                    items:[
                                        {
                                            bodyStyle: 'padding-left:5px;',
                                            items:[{
                                                xtype:'button',
                                                fieldLabel:'',
                                                text:'编辑',
                                                id:'btn_soft_black',
                                                handler:pp.initIPAreaTree.createDelegate(pp,[record,'soft_black'])
                                            }]
                                        },
                                        {
                                            bodyStyle: 'padding-left:5px;padding-top:5px;',
                                            items:[{
                                                xtype:'button',
                                                fieldLabel:'',
                                                text:'地域组',
                                                id:'btn_soft_Mblack',
                                                handler:pp.initAreaGroup.createDelegate(pp,[record,'soft_black'])
                                            }]
                                        }
                                    ]
                                }
                            ]
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btn_soft_areas',handler:pp.saveAreaSettingHandler.createDelegate(this,[record.get('SoftID')])},
                        {text:'取消',handler:function(){win.close();}}
                    ]
                });
                var win = new Ext.Window({
                    title:'管理地域控制',
                    closable:true,
                    width:425,
                    id:'soft_areas_win',
                    height:260,
                    layout:'border',
                    resizable:false,
                    modal:true,
                    shadow:false,
                    items:[{
                        region:'center',
                        items:[panel]
                    }]
                });
                win.setAnimateTarget(Ext.getBody());
                win.show();
                Ext.getCmp('s_areas_com').setValue(record.get('AreaControlType'));
                Ext.getCmp('soft_white').setValue(record.get('WhiteCityCodes'));
                Ext.getCmp('soft_black').setValue(record.get('BlackCityCodes'));
            }
        },
        initAreaGroup:function(r,cid){
            var areas = Ext.getCmp(cid).getValue();
            Ext.MessageBox.show({title:'请稍后',msg: '正在加载菜单，请稍后。。。',width:230,wait:true});
            SoftManage.GetAllIPS(function(result){
                var serviceschc=[];
                for(var i=0;i<result.length;i++)
                {
                    var check=false;
                    var index = areas.indexOf(result[i].AreaIDList);
                    if(index>=0)
                        check=true;
                    serviceschc.push(new Ext.form.Checkbox({id:String.format('mcomsfi{0}',i),width:100,ctCls:'daycolumn',boxLabel:result[i].AreaGroupName,inputValue:result[i].AreaIDList,checked:check}));
                }
                var panel = new Ext.form.FormPanel({
                    lableAlign:'left',
                    buttonAlign:'right',   
                    bodyStyle:'padding:5 15 0 5',
                    width:350,
                    height:180,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                        {
                            layout:'column',
                            id:'mccomservice_checkgroup',
                            columnWidth:.9,
                            bodyStyle:'padding:0 2 0 0;float:left;overflow-y:auto;',
                            height:128,
                            width:328,
                            items:serviceschc
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btn_mccomserviceflag',handler:pp.saveAreaGroup.createDelegate(this,[r.get('SoftID'),cid])},
                        {text:'取消',handler:function(){win.close();}}
                    ]
                });
                var win = new Ext.Window({
                    title:'设置地域组',
                    closable:true,
                    width:355,
                    id:'mccomservice_win',
                    height:200,
                    layout:'border',
                    resizable:false,
                    modal:true,
                    shadow:false,
                    items:[{
                        region:'center',
                        items:[panel]
                    }]
                });
                win.setAnimateTarget(Ext.getBody());
                win.show();
                Ext.MessageBox.hide();
            },function(result){Ext.MessageBox.hide();pp.callbackError(result);});
        },
        saveAreaGroup:function(sid,cid){
            var checks = Ext.getCmp('mccomservice_checkgroup');
            var areas='';
            var offareas=[];
            for(var i=0;i<checks.items.items.length;i++)
            {
                if(Ext.getCmp(String.format('mcomsfi{0}',i))&&Ext.getCmp(String.format('mcomsfi{0}',i)).checked)//if(checks.items.items[i].checked)
                {
                    if(areas=='')
                        areas=Ext.getCmp(String.format('mcomsfi{0}',i)).inputValue;
                    else
                        areas+=String.format(' {0}',Ext.getCmp(String.format('mcomsfi{0}',i)).inputValue);
                }
                else if(Ext.getCmp(String.format('mcomsfi{0}',i))&&Ext.getCmp(String.format('mcomsfi{0}',i)).checked==false)
                {
                    offareas.push(Ext.getCmp(String.format('mcomsfi{0}',i)).inputValue);
                }
            }
            SoftManage.SaveIPAreaGroup(sid,areas,offareas,cid,function(result){
                Ext.getCmp('mccomservice_win').close();
                Ext.getCmp(cid).setValue(result);
            },pp.callbackError);
            pp.btnDisable('btn_mccomserviceflag');
        },
        saveAreaSettingHandler:function(sid){
            pp.btnDisable('btn_soft_areas');
            SoftManage.SaveAreaType(sid,Ext.getCmp('s_areas_com').getValue(),function(result){
                Ext.getCmp('soft_areas_win').close();
                pp.refreshHandler();
            },pp.callbackError);
        },
        initIPAreaTree:function(record,cid){
            var areas = Ext.getCmp(cid).getValue();
            var tree;
            checkeNodes=[];
            Ext.MessageBox.show({title:'请稍后',msg: '正在加载菜单，请稍后。。。',width:230,wait:true});
            var filterFiled = new Ext.form.TextField({
                width:300,
                id:'s_area_filter',
                emptyText:'快速检索',
                enableKeyEvents: true,
	            listeners:{
			        render: function(f){
                        this.filter = new QM.ux.TreeFilter(tree,{
					        clearAction : 'expand',
					        ignoreFolder:false
				        });
			        },
                    keydown: {
                        fn:function(t,e){
                            if(e.getKey() == e.ENTER)
                                t.filter.filter(t.getValue());
                        }
                        //,buffer: 350
                    }
		        }
            });
            SoftManage.LoadIPAreaTree(function(result){
                PPLive.PPInstall.SoftManageVar.treeJson=result;
                tree = new Ext.tree.TreePanel({
                    tbar:[filterFiled],
                    rootVisible:false,
                    autoScroll:true,
                    animate:false,
                    id:'s_areatree',
                    width:320,
                    height:Ext.getCmp('softmange_grid').height-120,
                    root: new Ext.tree.AsyncTreeNode({
                       loader: new PPLive.PPInstall.SoftManageTreeLoader({
                            json:true,
                            baseAttrs: { uiProvider: Ext.tree.TreeCheckNodeUI }
                        })
                    }),
                    listeners:{
                        check:function(node,checked){
                            if(checked)
                            {
                                checkeNodes.push(node);
                            }
                            else
                            {
                                Array.remove(checkeNodes,node);
                            }
                        }
                    }
                });
                var panel = new Ext.form.FormPanel({
                    lableAlign:'left',
                    buttonAlign:'right',   
                    bodyStyle:'padding:5 15 0 5',
                    width:350,
                    height:Ext.getCmp('softmange_grid').height-70,
                    deferredRender : false,
                    labelWidth:95,
                    frame:true,
                    items:[
                       tree
                    ],
                    buttons:[
                        {text:'确定',id:'btn_soft_areas',handler:pp.saveAreasHandler.createDelegate(this,[record.get('SoftID'),cid])},
                        {text:'取消',handler:function(){win.close();}}
                    ]
                });
                var win = new Ext.Window({
                    title:'地域',
                    closable:true,
                    width:355,
                    id:'soft_IPArea_win',
                    height:Ext.getCmp('softmange_grid').height-50,
                    layout:'border',
                    resizable:false,
                    modal:true,
                    shadow:false,
                    items:[{
                        region:'center',
                        items:[panel]
                    }]
                });
                Ext.MessageBox.hide();
                win.setAnimateTarget(Ext.getBody());
                win.show();
                if(areas)
                {
                    var res = areas.split(' ');
                    for(var i=0;i<res.length;i++)
                    {
                        var parentNode = tree.root;
                        var currNode;
                        var atext='';
                        var re = res[i].split('/');
                        for(var j=0;j<re.length;j++)
                        {
                            if(atext=='')
                                atext=re[j];
                            else
                                atext=atext+'/'+re[j];
                            if(parentNode)
                                parentNode = parentNode.findChild('areatext',atext);
                            currNode = parentNode;
                        }
                        if(currNode)
                        {
                            currNode.ensureVisible();
                            tree.getSelectionModel().select(currNode);
                            tree.fireEvent('check', currNode, true);
                            currNode.ui.checkbox.checked=true;
                        }
                    }
                }
            },function(result){Ext.MessageBox.hide();pp.callbackError(result)});
        },
        saveAreasHandler:function(sid,cid){
            var areas = '';
            for(var i=0;i<checkeNodes.length;i++)
            {
                if(areas=='')
                    areas=checkeNodes[i].attributes.areatext;
                else
                    areas+=String.format(' {0}',checkeNodes[i].attributes.areatext);
            }
            SoftManage.SaveIPAreas(sid,areas,cid,function(result){
                Ext.getCmp('soft_IPArea_win').close();
                Ext.getCmp(cid).setValue(areas);
            },pp.callbackError);
            pp.btnDisable('btn_soft_areas');
        },
        initUploadPanel:function(id,filename,operatetype){
            var panel = new Ext.form.FormPanel({
                id:'soft_upload_panel',
                lableAlign:'left',
                buttonAlign:'right',   
                bodyStyle:'padding:5 15 0 5',
                width:400,
                height:80,
                fileUpload: true,
                deferredRender : false,
                labelWidth:95,
                frame:true,
                items:[
                    {
                        xtype:'fileuploadfield',
                        id: 'soft_upload_field',
                        emptyText: '请选择一个上传文件',
                        fieldLabel: '安装包',
                        allowBlank:false,
                        msgTarget: 'side',
                        anchor: '95%',
                        buttonCfg: {
                            text: '',
                            iconCls: 'channelid'
                        }
                    }
                ],
                buttons:[
                    {text:'确定',id:"btn_soft_upload_confirm",handler:pp.trueUpload.createDelegate(this,[id,filename,operatetype])},
                    {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                ]
            });
            var win = new Ext.Window({
                title:'上传',
                closable:true,
                width:405,
                id:'soft_upload_win',
                height:100,
                layout:'border',
                resizable:false,
                shadow:false,
                modal:true,
                items:[{
                    region:'center',
                    items:[panel]
                }]
            });
            //win.setAnimateTarget(Ext.getBody());
            win.show();
            win.alignTo(document, 'c-c');
            win.el.slideIn('t',{ duration: 1 });
        },
        trueUpload:function(id,filename,otype){
            var up = Ext.getCmp('soft_upload_field');
            if(up.validate())
            {
                var upv = up.getValue();
                var upf = upv.substring(upv.lastIndexOf('\\')+1);
                //var ftype = upf.substring (upf.lastIndexOf('.')+1);
                //if(ftype.toLowerCase()!='swf'){Ext.MessageBox.alert('提示','选择的文件必须是swf文件！');return;}
                var fp = Ext.getCmp('soft_upload_panel');
                var button = Ext.getCmp("btn_soft_upload_confirm");
                button.disable();
                Ext.MessageBox.show({title:'请稍后',msg: '正在上传，请稍后。。。',width:250,wait:true});
                if(fp.getForm().isValid()){
                    fp.getForm().submit({
                        url: 'UpLoad/SoftUploadHandler.ashx',
                        params:{id:id,filename:filename,otype:otype,apptype:'bindad'},
                        success: function(form, o){
                            Ext.MessageBox.hide();
                            Ext.getCmp('soft_upload_win').close();
                            if(otype=='imageurlupload')
                            {
                                Ext.getCmp('softimageurl').setValue(String.format("{0}/{1}/{2}{3}",PPLive.PPInstall.SoftManageVar.uploadUrlPrefix,o.result.directoryName,o.result.filePrefix,upf));
                            }
                            else if(otype=='icondownupload')
                            {
                                Ext.getCmp('softicondownurl').setValue(String.format("{0}/{1}/{2}{3}",PPLive.PPInstall.SoftManageVar.uploadUrlPrefix,o.result.directoryName,o.result.filePrefix,upf));
                            }
                            //pp.refreshHandler();
                        },
                        failure: function(form,o){
                            Ext.MessageBox.hide();
                            Ext.MessageBox.alert('提示',decodeURIComponent(o.result.message));
                            button.enable();
                        }
                    });
                }
            }
        }
    }
}();

Ext.apply(PPLive.PPInstall.SoftEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.SoftManage.pageInit.createDelegate(
        PPLive.PPInstall.SoftManage,
        [PPLive.PPInstall.SoftEdit]
    )
);

PPLive.PPInstall.SoftManageTreeLoader = Ext.extend(Ext.tree.TreeLoader, {  
     load : function(node, callback){
        if(this.clearOnLoad){
            while(node.firstChild){
                node.removeChild(node.firstChild);
            }
        }
        if(this.doPreload(node)){ 
            if(typeof callback == "function"){
                callback();
            }
        }else if(this.dataUrl||this.url){
            this.requestData(node, callback);
        }
        else if(this.json){
            this.processResponse(null,node,callback);
        }
    },
    processResponse : function(response, node, callback){
        var root = PPLive.PPInstall.SoftManageVar.treeJson;
        try{
            node.beginUpdate();
            if(root&&root.length>0)
            {
                for(var i=0;i<root.length;i++)
                {       
                    node.appendChild(this.parseJson(root[i]));
                }
            }
            node.endUpdate();
            
            if(typeof callback == "function"){
                callback(this, node);
            }
        }catch(e){
            //this.handleFailure(response);
            alert(e);
        }
        //this.fireEvent("load", this, node, root);
    },
    handleFailure : function(response){
        //this.transId = false;
        var a = response.argument;
        this.processResponse(response, a.node, a.callback);
        //this.fireEvent("loadexception", this, a.node, response);
        if(typeof a.callback == "function"){
            a.callback(this, a.node);
        }
    },
    parseJson : function(node) {
        var nodes = [];
        var treeNode = this.createNode(node);
        if(node.Children&&node.Children.length>0)
        {
            for(var i=0;i<node.Children.length;i++)
            {
                var n = node.Children[i];
                var child = this.parseJson(n);
                treeNode.appendChild(child);
                //nodes.push(treeNode);
            }
        }
        //else
            return treeNode;
        //return nodes;
    },
    createNode : function(node){
        var name ;
        var attr={};
        if(node.attributes)
        {
            if(!node.Children||node.Children.length<=0)
            {
                attr['leaf']=true;
            }
            else
            {
                attr['loaded']=true;
            }
            name=node.text;
            attr.text=name;
            attr.areatext=node.AreaText;
            //attr.checked=false;
            for(var a in node.attributes)
            {
                attr[a]=node.attributes[a];
            }
        }
        return PPLive.PPInstall.SoftManageTreeLoader.superclass.createNode.call(this, attr);
    }
});








