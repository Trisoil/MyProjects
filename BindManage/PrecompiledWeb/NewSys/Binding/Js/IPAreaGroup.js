
PPLive.PPInstall.IPAreaGroupVar = {
    treeJson:null
}

PPLive.PPInstall.IPAreaGroupManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'IPAreaGroupManage.asmx',methodName:'GetAllIP'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'ID'
                },[
                    'ID','AreaGroupName','AreaIDList'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'string',  dataIndex: 'AreaGroupName'},
				    {type: 'string',  dataIndex: 'AreaIDList'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'地区组名',width:180,sortable:true,dataIndex:'AreaGroupName'},
                {header:'地区组列表',width:390,sortable:true,dataIndex:'AreaIDList',renderer:ppem.showTip}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.addHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'ipareagroup_grid_div',
                id:'ipareagroup_grid',
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
					            {text:'增加',iconCls:'add',listeners:{click:ppem.addHandler}},
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
					            {text:'增加',iconCls:'add',listeners:{click:ppem.addHandler}},
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

PPLive.PPInstall.IPAreaGroupEdit=function(){
    var pp;
    var grid;
    var keys;
    var ppvars;
    var checkeNodes=[];
    return{
        editInit:function(gridPanel){
            pp=this;
            grid=gridPanel;
            ppvars=PPLive.PPInstall.IPAreaGroupVar;
        },
        refreshHandler:function(){
            grid.store.reload();
            grid.selModel.clearSelections(); 
        },
        initIPAreaTree:function(operate,title,id,areas,gname){
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
                ppvars.treeJson=result;
                tree = new Ext.tree.TreePanel({
                    tbar:[filterFiled],
                    rootVisible:false,
                    autoScroll:true,
                    animate:false,
                    id:'s_areatree',
                    width:250,
                    height:Ext.getCmp('ipareagroup_grid').height-153,
                    root: new Ext.tree.AsyncTreeNode({
                       loader: new PPLive.PPInstall.IPAreaGroupManageTreeLoader({
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
                    width:362,
                    height:Ext.getCmp('ipareagroup_grid').height-70,
                    deferredRender : false,
                    labelWidth:75,
                    frame:true,
                    items:[
                        {
                            xtype: 'textfield',
                            width:250,
                            bodyStyle:'padding:0 0 5 0',
                            fieldLabel: '地域组名称',
                            allowBlank:false,
                            id:'area_name'
                        },
                        {
                            layout: 'column',
                            columnWidth:1,
                            style:'padding:0px 0px 1px 0px',
                            items:[{
                                        xtype:'label',
                                        fieldLabel:'',
                                        width:80,
                                        text:'地域组设置:'
                                    },
                                    tree
                                ]
                        }
                    ],
                    buttons:[
                        {text:'确定',id:'btn_soft_areas',handler:pp.saveAreasHandler.createDelegate(this,[operate,id])},
                        {text:'取消',handler:function(){win.close();}}
                    ]
                });
                var win = new Ext.Window({
                    title:title,
                    closable:true,
                    width:367,
                    id:'soft_IPArea_win',
                    height:Ext.getCmp('ipareagroup_grid').height-50,
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
                if(operate=='edit')
                    Ext.getCmp('area_name').setValue(gname);
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
        addHandler:function(){
           pp.initIPAreaTree('add','增加');
        },
        editHandler:function(r){
            var record = pp.getRecord(grid,r);
            if(record)
            {
                pp.initIPAreaTree('edit','编辑',record.get('ID'),record.get('AreaIDList'),record.get('AreaGroupName'));
                
            }
        },
        deleteHandler:function(r){
            var keys = pp.getKeys(grid,r);
            if(keys&&keys.length>0)
            {
                Ext.MessageBox.confirm('提示','确定要删除选定的'+keys.length+'条记录吗？',function(btn){
                    if(btn=='yes')
                    {
                        IPAreaGroupManage.DeleteIPs(keys,pp.refreshHandler,pp.callbackError);
                    }
                });
            }
        },
        saveAreasHandler:function(operate,id){
            var namecon = Ext.getCmp('area_name');
            if(namecon.validate())
            {
                var areas = '';
                for(var i=0;i<checkeNodes.length;i++)
                {
                    if(areas=='')
                        areas=checkeNodes[i].attributes.areatext;
                    else
                        areas+=String.format(' {0}',checkeNodes[i].attributes.areatext);
                }
                if(operate=='add')
                    IPAreaGroupManage.AddIP(namecon.getValue(),areas,function(result){
                        Ext.getCmp('soft_IPArea_win').close();
                        pp.refreshHandler();
                    },pp.callbackError);
                else
                    IPAreaGroupManage.EditIP(id,namecon.getValue(),areas,function(result){
                        Ext.getCmp('soft_IPArea_win').close();
                        pp.refreshHandler();
                    },pp.callbackError);
                pp.btnDisable('btn_soft_areas');
            }
        }
    }
}();

Ext.apply(PPLive.PPInstall.IPAreaGroupEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.IPAreaGroupManage.pageInit.createDelegate(
        PPLive.PPInstall.IPAreaGroupManage,
        [PPLive.PPInstall.IPAreaGroupEdit]
    )
);

PPLive.PPInstall.IPAreaGroupManageTreeLoader = Ext.extend(Ext.tree.TreeLoader, {  
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
        var root = PPLive.PPInstall.IPAreaGroupVar.treeJson;
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
        return PPLive.PPInstall.IPAreaGroupManageTreeLoader.superclass.createNode.call(this, attr);
    }
});








