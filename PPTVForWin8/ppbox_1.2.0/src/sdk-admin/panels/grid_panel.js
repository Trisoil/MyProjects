
Ext.menu.RangeMenu.prototype.icons = {
  gt: 'img/greater_then.png', 
  lt: 'img/less_then.png',
  eq: 'img/equals.png'
};

Ext.grid.filter.StringFilter.prototype.icon = 'img/find.png';

var register_grid_panel = function (panel_def, panel_creator)
{
    
    // 注册数据集
    //var object_set = ("ref_keys" in panel_def) ? panel_def : new ObjectSet(panel_def);
    var object_set = ObjectSet.register_set(panel_def);
    
    panel_creator = panel_creator || create_grid_panel;
    
    var menu_item = {
        id      : panel_def.name,
        text    : panel_def.title, 
        path    : panel_def.menu_path, 
        icon    : panel_def.menu_icon, 
        leaf    : true, 
        panel   : panel_creator.createCallback(object_set.name)
    };
    
    menu_def.add(menu_item);
    
    return object_set;
}

var create_grid_panel = function (set_name)
{
    var object_set = ObjectSet.get_set(set_name);

    var grid = create_grid(object_set);
    
    var tab = create_tab(grid, object_set);
    
    var edit_win = create_edit_win(object_set);

    // 定义面板
    var panel = {
        title       : object_set.title,
        id          : object_set.name + '-panel',
        layout      : 'border',
        bodyBorder  : false,
        autoWidth   : true, 
        defaults    : {
            split           : true,
            useSplitTips    : true//,
            //bodyStyle       : 'padding:15px'
        },
        tbar        : [{
            pressed     : true,
            text        : '添加',
            disabled    : !object_set.initable, 
            handler     : add_item.createCallback(object_set, grid, edit_win)
        }, {
            pressed     : true,
            text        : '编辑',
            disabled    : !object_set.editable, 
            handler     : edit_item.createCallback(object_set, grid, edit_win)
        }, {
            pressed     : true,
            text        : '删除',
            disabled    : !object_set.deletable, 
            handler     : del_item.createCallback(object_set, grid)
        }],
        items       : [grid, tab], 
        listeners   : {
            beforeshow  : function (g) {
                grid.fireEvent('beforeshow', grid);
            }, 
            hide        : function (g) {
                grid.fireEvent('hide', grid);
            }
        }, 
        refresh     : function () {
            grid.refresh();
            var item = tab.getActiveTab();
            if (item && item.refresh)
                item.refresh();
        }
    };
    
    term_calls.push([
        grid.onStop,  
        grid
    ]);
    
    return panel;
};

var create_row_panel = function (record, index, object_set) {
    var items = [];
    for (var field in object_set.ref_keys) {
        var key = object_set.ref_keys[field];
        var set = ObjectSet.set_map[key];
        if (typeof set.create_store == 'undefined')
            continue;
        var grid = create_grid(set);
        
        grid.store.baseParams.filter = Ext.encode([{
            field   : set.outer_key, 
            data    : {
                type        : 'numeric', // 字符串也用等号
                comparison  : 'eq', 
                value       : record.data[field]
            }
        }]);
        //grid.store.load();
        
        grid.title = set.title;
        items.push(grid);
    }
    for (var key in object_set.ref_bys) {
        var set = ObjectSet.set_map[key];
        var attrs = object_set.ref_bys[key];
        for (var attr in attrs) {
            var grid = create_grid(set);
            
            grid.store.baseParams.filter = Ext.encode([{
                field   : attr.name, 
                data    : {
                    type        : 'numeric', // 字符串也用等号
                    comparison  : 'eq', 
                    value       : record.data[object_set.outer_key]
                }
            }]);
            //grid.on('activate', function () {
            //    this.fireEvent('beforeshow', this);
            //});
            //grid.on('deactivate', grid.onHide, grid);
            grid.title = set.title;
            if (attrs.length > 1)
                grid.title += '(' + attr.title + ')';
            items.push(grid);
        }
    }
    return new Ext.TabPanel({
        activeTab   : -1, 
        height      : 150, 
        //width       : 800, 
        plain       : true,
        defaults    : {
            autoScroll  : true
        },
        items       : items
    });
};

var create_tab = function (grid, object_set) {
    var items = [];
    for (var field in object_set.ref_keys) {
        var key = object_set.ref_keys[field];
        var set = ObjectSet.set_map[key];
        if (typeof set.create_store == 'undefined')
            continue;
        var grid1 = create_grid(set);
        grid1.store.filter = {
            dataIndex   : set.outer_key, 
            ref_field   : field, 
            data        : {
                type        : 'list', // 字符串也用等号
                value       : ''
            }
        };
        grid1.filters.addOtherFilter(grid1.store.filter);
        grid1.title = set.title;
        items.push(grid1);
    }
    for (var key in object_set.ref_bys) {
        var set = ObjectSet.set_map[key];
        var attrs = object_set.ref_bys[key];
        for (var idx =0; idx < attrs.length; ++idx) {
            var attr = attrs[idx];
            var grid1 = create_grid(set);
            grid1.store.filter = {
                dataIndex   : attr.name, 
                ref_field   : object_set.outer_key, 
                data        : {
                    type        : 'list', // 字符串也用等号
                    value       : ''
                }
            };
            grid1.filters.addOtherFilter(grid1.store.filter);
            grid1.title = set.title;
            if (attrs.length > 1)
                grid1.title += '(' + attr.title + ')';
            items.push(grid1);
        }
    }
    
    items.unshift({
        html: items.length == 0 ? '<p>没有关联数据</p>' : '<p>点击表单查看关联数据</p>',
        title: '*'
    });
    
    var tab = new Ext.TabPanel({
        activeTab       : 0, 
        //layout          : 'fit', 
        deferredRender  : false,
        autoWidth       : true, 
        height          : 250, 
        region          : 'south',
        plain           : true, 
        //autoScroll      : true, 
        defaults        : {
            //autoScroll      : true, 
            //autoWidth       : true, 
            //autoHeight      : true
        },
        items           : items
    });
    
    grid.getSelectionModel().on('selectionchange', function (sm) {
        var sels = sm.getSelections();
        for (var i = 1; i < items.length; i++) {
            var item = items[i];
            var store = item.store;
            var sel_items = [];
            for (var j = 0; j < sels.length; j++) {
                sel_items.push(sels[j].data[store.filter.ref_field]);
            }
            store.filter.data.value = sel_items.join(',');
            item.loaded = false;
        }
        item = tab.getActiveTab();
        if (item && item.refresh)
            item.refresh();
    });
        
    return tab;
};

var create_grid = function (object_set) {
    
    // 创建数据存储
    var store = object_set.create_store();
    
    // Grid行扩展
    //var expander = new Ext.grid.RowExpander({
    //    createPanel : create_row_panel.createDelegate(expander, [object_set], true)
    //});

    // 定义Grid的各个列
    var selectionModel = new Ext.grid.CheckboxSelectionModel();
    var columnModel = [
        //expander, 
        new Ext.grid.RowNumberer(), 
        selectionModel
    ];

    for (var i = 0; i < object_set.attrs.length; i++) {
        var item = {
            header      : object_set.attrs[i].title, 
            width       : object_set.attrs[i].width, 
            dataIndex   : object_set.attrs[i].name,
            sortable	: true
//            editor      : object_set.attrs[i].editor
        };
        if (typeof(object_set.attrs[i].renderer) != 'undefined') {
            item.renderer = object_set.attrs[i].renderer;
        }
        if (typeof(object_set.attrs[i].ref_key) != 'undefined') {
            var ref_set = ObjectSet.set_map[object_set.attrs[i].ref_key];
            item.renderer = ref_set.get_disp_field.createDelegate(ref_set);
        }
        columnModel.push(item);
    }

    // 定义Grid的列过滤器
    var filter_items = [];
    for (var i = 0; i < object_set.attrs.length; i++) {
        var item = {
            dataIndex   : object_set.attrs[i].name, 
            type        : object_set.attrs[i].type
        };
        if (item.type == 'int') {
            item.type = 'numeric';
        }
        if (typeof(object_set.attrs[i].ref_key) != 'undefined' 
            && !object_set.attrs[i].delay_ref) {
                var ref_set = ObjectSet.set_map[object_set.attrs[i].ref_key];
                if (typeof(ref_set.refered) != 'undefined') {
                    item.type = 'list';
                    item.store = ref_set.key_map;
                    item.labelField = ref_set.disp_field;
                    item.loadOnShow = false;
                    item.loaded = true;
                    item.phpMode = true;
                }
        }
        filter_items.push(item);
    }
    var filters = new Ext.grid.GridFilters({filters : filter_items});

    // 定义Grid的分页器
    var paging_bar = new Ext.PagingToolbar({
        pageSize    : 25,
        store       : store,
        autoWidth   : true, 
        plugins     : filters, 
        displayInfo : true,
        displayMsg  : object_set.title + ' {0} - {1} of {2}',
        emptyMsg    : '没有' + object_set.title, 
        items       : ['-']
    });
    
    // 定义Grid
    var grid = new Ext.grid.GridPanel({
        region      : 'center',
        layout      : 'fit',
        cm          : new Ext.grid.ColumnModel(columnModel),
        plugins     : [/*expander, */filters], 
        sm          : selectionModel,
        filters     : filters, 
        stripeRows  : true, 
        bbar        : paging_bar, 
        store       : store, 
        autoWidth   : true, 
        loaded      : false, 
        listeners   : {
            celldblclick    : function (g, r, c) {
                var record = store.getAt(r);  // Get the Record
                var fieldName = grid.getColumnModel().getDataIndex(c); // Get field name
                var data = record.get(fieldName);
                clipboardData.setData('text', data.toString());
            }, 
            beforeshow      : function () {
                if (!this.loaded) {
                    this.loaded = true;
                    paging_bar.changePage(0);
                }
                store.force_ref_load = true;
            }, 
            hide            : function () {
                store.force_ref_load = false;
            }
        }, 
        onStop      : function () {
            this.store.removeAll();
            this.loaded = false;
            store.force_ref_load = false;
        }, 
        refresh     : function () {
            this.store.reload();
        }
    });

    return grid;
};

var create_edit_win = function (object_set) {
    // 定义编辑对话框
    var form_items = [];
    for (var i = 0; i < object_set.attrs.length; i++) {
        var item = {
            //id          : object_set.attrs[i].name, 
            name        : object_set.attrs[i].name, 
            fieldLabel  : object_set.attrs[i].title, 
            disabled    : !object_set.attrs[i].initable, 
            allowBlank  : object_set.attrs[i].nullable
        };
        if (typeof(object_set.attrs[i].xtype) != 'undefined') {
            item.xtype = object_set.attrs[i].xtype;
        }
        if (typeof(object_set.attrs[i].ref_key) != 'undefined') {
            item.xtype = combo_xtypes[object_set.attrs[i].ref_key];
        }
        form_items.push(item);
    }
    
    var edit_form = new Ext.form.FormPanel({
        labelWidth      : 75,
        bodyStyle       : 'padding:15px',
        labelPad        : 20,
        layoutConfig    : {
            labelSeparator  : ''
        },
        defaults: {
            width           : 240, // Movie Id 需要的长度
            msgTarget       : 'side'
        },
        defaultType     : 'textfield',
        items           : form_items 
        //listeners       : {
        //    'render'        : function () {
        //        this.form.loadRecord(this.record);
        //    }
        //}
    });

    var edit_win = new Ext.Window({
        layout      : 'fit',
        width       : 410,
        height      : (form_items.length) * 26 + 96,
        closeAction : 'hide',
        plain       : true,
        title       : object_set.title + '编辑',
        modal       : true,
        items       : [edit_form],
        buttons     : [{
            text: 'Save'
        }, {
            text: 'Cancel', 
            handler: function() {
                edit_win.hide();
            }
        }]
    });
        
    return edit_win;
};

// 处理“添加”按钮事件
var add_item = function(object_set, grid, edit_win) {
    
    var record_copy = grid.getSelectionModel().getSelected() || object_set.default_record;
    var record = record_copy.copy();
    
    var store = grid.getStore();
    
    var edit_form = edit_win.items.get(0);
    
    edit_win.buttons[0].handler = function() {
        edit_form.form.updateRecord(record);
        Ext.MessageBox.wait('正在处理中，请稍等……', '提示');
        Ext.Ajax.request({
            url     : object_set.module_url,
            params  : {
                o       : 'add', 
                item    : Ext.encode(record.data)
            }, 
            success : function (response) {
                Ext.MessageBox.hide();
                var result = Ext.decode(response.responseText);
                if (result.success) {
                    if (typeof result.data == 'object') {
                        convert_record(object_set.Record, result.data);
                        Ext.apply(record.data, result.data);
                    }
                    record.id = record.data.id;
                    store.add([record]);
                    edit_win.hide();
                    object_set.refresh_key_map();
                } else {
                    Ext.MessageBox.alert('错误', result.message);
                }
            }, 
            failure : function (response) {
                Ext.MessageBox.alert('异常', 'status :\t' + response.status + '\ndesc :\t' + response.statusText);
            }
        });
    };
    
    edit_win.show();
    
    for (var i = 0; i < object_set.attrs.length; i++) {
        var field = edit_form.form.findField(object_set.attrs[i].name);
        if (field != null) {
            if (object_set.attrs[i].initable) {
                field.enable();
            } else {
                field.disable();
            }
        }
    }
    
    edit_form.form.loadRecord(record);
};

// 处理“编辑”按钮事件
var edit_item = function(object_set, grid, edit_win) {
    
    if (grid.getSelectionModel().getCount() == 0)
        return;
        
    var record_real = grid.getSelectionModel().getSelected();
    var record = record_real.copy();

    var store = grid.getStore();
    
    var edit_form = edit_win.items.get(0);
    
    edit_win.buttons[0].handler = function() {
        Ext.MessageBox.wait('正在处理中，请稍等……', '提示');
        edit_form.form.updateRecord(record);
        var json_item = Ext.encode(record.data);
        Ext.Ajax.request({
            url     : object_set.module_url,
            params  : {
                o       : 'mod', 
                item    : json_item
            }, 
            success : function (response) {
                Ext.MessageBox.hide();
                var result = Ext.decode(response.responseText);
                if (result.success) {
                    if (typeof result.data == 'object') {
                        convert_record(object_set.Record, result.data);
                        Ext.apply(record.data, result.data);
                    }
                    Ext.apply(record_real.data, record.data);
                    record_real.commit();
                    edit_win.hide();
                    object_set.refresh_key_map();
                } else {
                    Ext.MessageBox.alert('错误', result.message);
                }
            }, 
            failure : function (response) {
                Ext.MessageBox.alert('异常', 'status :\t' + response.status + '\ndesc :\t' + response.statusText);
            }
        });
    };
    
    edit_win.show();
    for (var i = 0; i < object_set.attrs.length; i++) {
        var field = edit_form.form.findField(object_set.attrs[i].name);
        if (field != null) {
            if (object_set.attrs[i].editable) {
                field.enable();
            } else {
                field.disable();
            }
        }
    }
    edit_form.form.loadRecord(record);
};

// 处理“删除”按钮事件
var del_item = function(object_set, grid) {
    var store = grid.getStore();
    
    var sels = grid.getSelectionModel().getSelections();
    if (sels.length == 0) {
        return;
    }
    var items = [];
    for (var i = 0; i < sels.length; i++) {
        var item = {};
        if (typeof(object_set.outer_key) == 'string') {
            item[object_set.outer_key] = sels[i].data[object_set.outer_key];
        } else {
            for (var j = 0; j < object_set.outer_key.length; j++)
                item[object_set.outer_key[j]] = sels[i].data[object_set.outer_key[j]];
        }
        items.push(item);
    }
    Ext.MessageBox.wait('正在处理中，请稍等……', '提示');
    Ext.Ajax.request({
        url     : object_set.module_url,
        params  : {
            o       : 'del', 
            ids     : Ext.encode(items)
        }, 
        success : function (response) {
            Ext.MessageBox.hide();
            var result = Ext.decode(response.responseText);
            if (result.success) {
                var errmsg = '';
                for (var i = 0; i < sels.length; i++) {
                    if (result.data[i].success) {
                        store.remove(sels[i]);
                    } else {
                        items[i].message = result.data[i].message;
                        errmsg += '<p>' + Ext.encode(items[i]);
                    }
                }
                if (errmsg != '') {
                    Ext.MessageBox.alert('错误', errmsg);
                }
                object_set.refresh_key_map();
            } else {
                Ext.MessageBox.alert('错误', result.message);
            }
        }, 
        failure : function (response) {
            Ext.MessageBox.alert('异常', 'status :\t' + response.status + '\ndesc :\t' + response.statusText);
        }
    });
};
