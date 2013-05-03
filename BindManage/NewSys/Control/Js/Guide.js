
PPLive.PPInstall.GuideVars = {
    desEntry:'\\r\\n',
    desRegex:/\\r\\n/g,
    imagePrefix:'http://img-ins.pplive.cn',
    imageSplit:'$'
};

PPLive.PPInstall.GuideManage=function(){
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
                proxy: new Ext.data.WebServiceProxy({servicePath: 'GuideManage.asmx',methodName:'GetAllGuide'}),
                reader:new Ext.data.JsonReader({
                    root: 'CurrentPage',
                    totalProperty: 'TotalCount',
                    id: 'GuideID'
                },[
                    'GuideID','CustomID','Description','Interval','ImageArray','CustomCnName','CustomEnName','ProductCnName','ProductEnName'
                ]),
                remoteSort: true
            });
            //store.setDefaultSort('MakeCompleteTime', 'desc');
            var filters = new Ext.ux.grid.GridFilters({filters:[
				    {type: 'string',  dataIndex: 'CustomCnName'},
				    {type: 'string',  dataIndex: 'CustomEnName'}
			]});
            var sm = new Ext.grid.CheckboxSelectionModel();
            var cm = new Ext.grid.ColumnModel([sm,
                {header:'图片轮询间隔',width:100,sortable:true,dataIndex:'Interval'}, 
                {header:'中文描述',width:320,sortable:true,dataIndex:'Description',renderer:ppem.showTip}, 
                {header:'所属定制中文名',width:100,sortable:true,dataIndex:'CustomCnName',renderer:ppem.showTip},
                {header:'所属定制英文名',width:100,sortable:true,dataIndex:'CustomEnName',renderer:ppem.showTip},
                {header:'所属产品英文名',width:100,sortable:true,dataIndex:'ProductEnName',renderer:ppem.showTip},
                {header:'所属产品中文名',width:100,sortable:true,dataIndex:'ProductCnName',renderer:ppem.showTip}
            ]);
            var btnAdd=new Ext.Button({text:'增加',iconCls:'add',handler:ppem.adddHandler});
            var btnEdit = new Ext.Button({text:'编辑',iconCls:'edit',handler:ppem.editHandler});
            var btnDelete = new Ext.Button({text:'删除',iconCls:'remove',handler:ppem.deleteHandler});
            var btnRefresh = new Ext.Button({text:'刷新',iconCls:'refresh',handler:ppem.refreshHandler});
            cm.defaultSortable = true;
            grid = new Ext.grid.GridPanel({
                el:'guidemanage_grid_div',
                id:'guidemanage_grid',
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

PPLive.PPInstall.GuideEdit=function(){
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
                id:'guide_main_formpanel',
                lableAlign:'left',
                buttonAlign:'right',   
                //bodyStyle:'padding:5 15 0 5',
                width:570,
                height:350,
                deferredRender : false,
                labelWidth:90,
                frame:true,
                items:[
                    {
                        layout:'column',
                        columnWidth:.1,
                        style:'padding:0px 0px 3px 0px',
                        items:[
                            {
                                xtype:'label',
                                fieldLabel:'',
                                width:95,
                                text:'所属定制：'
                            },
                            {
                                xtype: 'combo',
                                allowBlank:false,
                                fieldLabel: '所属定制',
                                id:'guide_panel_custom',
                                mode:'local',
                                selectOnFocus:true,
                                forceSelection: true,
                                displayField:'Name',
                                editable:false,
                                valueField:'ID',typeAhead: true,showTip:true,triggerAction:'all',width:250,
                                store:new Ext.data.Store({
                                    autoLoad:true,
                                    proxy: new Ext.data.WebServiceProxy({servicePath: 'VersionManage.asmx',methodName:'GetAllCustoms'}),
                                    reader:new Ext.data.JsonReader({ root: 'CurrentPage',totalProperty: 'TotalCount',id: 'ID'},[{name:'ID'},{name:'Name'}])
                                })
                            }
                        ]
                    },
                    {
                        xtype: 'textfield',
                        width:425,
                        //bodyStyle:'padding:0 0 5 0',
                        fieldLabel: '图片轮询间隔',
                        regex:pp.zeroRegex,
                        regexText:pp.zeroRegexText,
                        allowBlank:false,
                        id:'guide_panel_interval'
                    },
                    {
                        xtype:'textarea',
                        id:'guide_panel_des',
                        width:425,
                        fieldLabel: '中文描述'
                    },
                    {
                        xtype:"itemselector",
                        id:"guide_panel_imagearray",
                        fieldLabel:"图片地址集合",
                        dataFields:["code", "desc"],
                        toData:[],
                        msWidth:425,
                        msHeight:175,
                        valueField:"code",
                        anchor: '100%',
                        displayField:"desc",
                        imagePath:"MuliteSelect/images/",
                        //toLegend:"Selected",
                        //fromLegend:"Available",
                        switchToFrom:true,
                        showFrom:false,
                        drawLeftIcon:false,
                        drawRightIcon:false,
                        fromData:[],
                        toTBar:[
                            {text:"上传",iconCls:'pageup',handler:function(){pp.initUploadPanel(id);}},
                            {text:"删除",iconCls:'remove',handler:pp.imagesDeleteHandler}
                        ],
                        listeners:{
                            rowdblclick:function(vw, index, node, e){
                                var imageurl = node.textContent;
                                if(Ext.isIE)
                                    imageurl = node.innerText;
                                if(imageurl)
                                    window.open(encodeURI(imageurl));
                            }
                        }
                    }
                ],
                buttons:[
                    {text:'确定',id:'btn_guide_confirm',handler:pp.operateHandler.createDelegate(this,[operate,id])},
                    {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                ]
            });
            var win = new Ext.Window({
                title:title,
                closable:true,
                width:575,
                id:'guide_main_win',
                height:370,
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
                pp.initPanel('edit','编辑',record.data.GuideID);
                Ext.getCmp('guide_panel_custom').store.on('load',function(){
                    Ext.getCmp('guide_panel_custom').setValue(record.data.CustomID);
                });
                Ext.getCmp('guide_panel_interval').setValue(record.data.Interval);
                if(Ext.isIE)
                    Ext.getCmp('guide_panel_des').setValue(record.data.Description.replace(PPLive.PPInstall.GuideVars.desRegex,'\r'));
                else
                    Ext.getCmp('guide_panel_des').setValue(record.data.Description.replace(PPLive.PPInstall.GuideVars.desRegex,'\n'));
                //alert(record.data.Description);
                //Ext.getCmp('guide_panel_des').setValue(record.data.Description);
                var images = record.data.ImageArray.split(PPLive.PPInstall.GuideVars.imageSplit);
                for(var i=0;i<images.length;i++)
                {
                    Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store.add([new Ext.data.Record({code:i+1,desc:images[i]})]);
                }
                Ext.getCmp('guide_panel_imagearray').toMultiselect.view.refresh();
            }
        },
        operateHandler:function(o,id){
            var customcmp = Ext.getCmp('guide_panel_custom');
            var intervalcmp = Ext.getCmp('guide_panel_interval');
            if(customcmp.validate()&&intervalcmp.validate())
            {
                var des = Ext.getCmp('guide_panel_des').getValue().replace(/\n|\r\n/g,PPLive.PPInstall.GuideVars.desEntry);
                //var des = Ext.getCmp('guide_panel_des').getValue();
                //alert(des);
                var images = pp.getImagesArray();
                if(o=='add')
                    GuideManage.AddGuide(customcmp.getValue(),intervalcmp.getValue(),des,images,pp.callbackSucess,pp.callbackError);
                else
                    GuideManage.EditGuide(id,customcmp.getValue(),intervalcmp.getValue(),des,images,pp.callbackSucess,pp.callbackError);
                pp.btnDisable('btn_guide_confirm');
            }
        },
        getImagesArray:function(){
            var cmpstore = Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store;
            var image='';
            if(cmpstore.data.items && cmpstore.data.items.length > 0)
            {
                for(var i=0;i<cmpstore.data.items.length;i++)
                {
                    if(cmpstore.data.items[i].data.desc)
                    {
                        if(i!=cmpstore.data.items.length-1)
                            image+=String.format("{0}{1}",cmpstore.data.items[i].data.desc,PPLive.PPInstall.GuideVars.imageSplit);
                        else
                            image+=String.format("{0}",cmpstore.data.items[i].data.desc);
                    }
                }
                return image;
            }
            return null;
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
                GuideManage.DeleteGuides(keys,pp.callbackSucess,pp.callbackError);
            }
        },
        callbackSucess:function(result){
            var win = Ext.getCmp('guide_main_win');
            if(win)win.close();
            pp.refreshHandler();
        },
        initUploadPanel:function(id,filename,operatetype){
            var panel = new Ext.form.FormPanel({
                id:'guide_upload_panel',
                lableAlign:'left',
                buttonAlign:'right',   
                bodyStyle:'padding:5 15 0 5',
                width:300,
                height:80,
                fileUpload: true,
                deferredRender : false,
                labelWidth:60,
                frame:true,
                items:[
                    {
                        xtype:'fileuploadfield',
                        id: 'guide_upload_field',
                        emptyText: '请选择一张图片',
                        fieldLabel: '图片文件',
                        allowBlank:false,
                        msgTarget: 'side',
                        anchor: '92%',
                        buttonCfg: {
                            text: '',
                            iconCls: 'channelid'
                        }
                    }
                ],
                buttons:[
                    {text:'确定',id:"btn_guide_upload_confirm",handler:pp.trueUpload.createDelegate(this,[id,filename,operatetype])},
                    {text:'取消',handler:function(){win.close();grid.selModel.clearSelections(); }}
                ]
            });
            var win = new Ext.Window({
                title:'上传',
                closable:true,
                width:305,
                id:'guide_upload_win',
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
            var up = Ext.getCmp('guide_upload_field');
            if(up.validate())
            {
                var upv = up.getValue();
                var upf = upv.substring(upv.lastIndexOf('\\')+1);
                //var ftype = upf.substring (upf.lastIndexOf('.')+1);
                //if(ftype.toLowerCase()!='swf'){Ext.MessageBox.alert('提示','选择的文件必须是swf文件！');return;}
                var fp = Ext.getCmp('guide_upload_panel');
                var button = Ext.getCmp("btn_guide_upload_confirm");
                button.disable();
                Ext.MessageBox.show({title:'请稍后',msg: '正在上传，请稍后。。。',width:250,wait:true});
                if(fp.getForm().isValid()){
                    fp.getForm().submit({
                        url: '../Binding/UpLoad/SoftUploadHandler.ashx',
                        params:{id:id,filename:filename,otype:otype,apptype:'bindad'},
                        success: function(form, o){
                            Ext.MessageBox.hide();
                            Ext.getCmp('guide_upload_win').close();
                            var imageurl = String.format("{0}/{1}/{2}{3}",PPLive.PPInstall.GuideVars.imagePrefix,o.result.directoryName,o.result.filePrefix,upf);
                            //Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store.add([new Ext.data.Record({code:imageurl,desc:imageurl})]);
                            Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store.insert(Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store.getCount(),new Ext.data.Record({code:imageurl,desc:imageurl}));
                            Ext.getCmp('guide_panel_imagearray').toMultiselect.view.refresh();
                        },
                        failure: function(form,o){
                            Ext.MessageBox.hide();
                            Ext.MessageBox.alert('提示',decodeURIComponent(o.result.message));
                            button.enable();
                        }
                    });
                }
            }
        },
        imagesDeleteHandler:function(){
            var images = Ext.getCmp('guide_panel_imagearray').toMultiselect.view.getSelectedIndexes();
            if(images.length > 0)
            {
                for(var i=0;i<images.length;i++)
                {
                    var rec = Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store.getAt(images[i]);
                    Ext.getCmp('guide_panel_imagearray').toMultiselect.view.store.remove(rec);
                }
                Ext.getCmp('guide_panel_imagearray').toMultiselect.view.refresh();
            }
        }
    }
}();

Ext.apply(PPLive.PPInstall.GuideEdit,PPLive.PPInstall.EditBase);

Ext.onReady(
    PPLive.PPInstall.GuideManage.pageInit.createDelegate(
        PPLive.PPInstall.GuideManage,
        [PPLive.PPInstall.GuideEdit]
    )
);



