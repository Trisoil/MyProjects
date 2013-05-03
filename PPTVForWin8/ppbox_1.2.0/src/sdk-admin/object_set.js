
var store_listeners = {
    loadexception : function () {
        if (typeof this.reader.jsonData != 'undefined' && 
            typeof this.reader.jsonData.message != 'undefined')
            Ext.MessageBox.alert('错误', this.reader.jsonData.message);
        else
            Ext.MessageBox.alert('错误', '数据加载错误！');
    }
};

var ObjectRecord = Ext.extend(Ext.data.Record, {

    old_get	: Ext.data.Record.prototype.get, 

    get		: function (field) {
        if (typeof(field) == 'string') {
            var v = this.old_get.apply(this, [field]);
            if (typeof(this.ref_keys[field]) != 'undefined') {
                v = ObjectSet.get_set(this.ref_keys[field]).get_disp_field(v);
            }
            return v;
        } else if (typeof(field) == 'array') {
            var value = {};
            for (var j = 0; j < field.length; j++)
                value[field[j]] = this.get(field[j]);
            return Ext.encode(value);
        } else if (typeof(field) == 'object') {
            var value = '';
            for (var j = 0; j < field.items.length; j++) {
                if (j > 0)
                    value += field.delim;
                value += this.get(field.items[j]);
            }
            return value;
        }
    }
});

ObjectRecord.create = function(o) {
    var f = Ext.extend(ObjectRecord, {});
    var p = f.prototype;
    p.fields = new Ext.util.MixedCollection(false, function(field){
        return field.name;
    });
    for(var i = 0, len = o.length; i < len; i++){
        p.fields.add(new Ext.data.Field(o[i]));
    }
    f.getField = function(name){
        return p.fields.get(name);
    };
    return f;
};

var ObjectSet = function (options) {
    Ext.apply(this, options);

    // 定义记录构建器（creator）
    var record_items = [];
    this.ref_keys = {};
    this.ref_bys = {};
    for (var i = 0; i < this.attrs.length; i++) {
        if (typeof(this.attrs[i].ref_key) != 'undefined') {
            this.ref_keys[this.attrs[i].name] = this.attrs[i].ref_key;
            var ref_set = ObjectSet.get_set(this.attrs[i].ref_key);
            if (typeof ref_set == 'undefined')
                Ext.MessageBox.alert('异常', this.attrs[i].ref_key);
            if (typeof(ref_set.ref_bys[this.name]) == 'undefined')
                ref_set.ref_bys[this.name] = [];
            ref_set.ref_bys[this.name] = ref_set.ref_bys[this.name].concat(this.attrs[i]);
            ref_set.refered = true;
        }
        var item = {
            name            : this.attrs[i].name, 
            type            : this.attrs[i].type, 
            defaultValue    : this.attrs[i].defaultValue
        };
        if (item.type == 'date') {
            item.dateFormat = 'Y-m-d H:i:s';
        }
        record_items.push(item);
    }
    this.Record = ObjectRecord.create(record_items);
    this.Record.prototype.ref_keys = this.ref_keys;
    
    var s = new Ext.data.JsonStore({
        fields 		: this.Record
    });
    s.loadData([{}]); 
    this.default_record = s.getAt(0);
    
    // 如果记录有唯一关键字，定义查找表
    this.key_map = this.create_store();
    if (typeof(this.outer_key) == 'string') {
        this.refered = false;
    }
};

// ObjectSet实例索引表
ObjectSet.set_map = {};

ObjectSet.register_enum = function (name, title, value_name_list) {
    var key_map = new Ext.data.Store({
        reader  : new Ext.data.ArrayReader({id: 0}, ['id', 'name'])
    });
    key_map.loadData(value_name_list);
    return ObjectSet.register_key_map(name, title, key_map, 'id', 'name');
};

ObjectSet.register_key_map = function (name, title, key_map, outer_key, disp_field) {
    var set = {
        name        : name, 
        title       : title, 
        key_map     : key_map, 
        outer_key   : outer_key, 
        disp_field  : disp_field, 
        ref_keys    : {}, 
        ref_bys     : {}
    };
    set.get_disp_field = ObjectSet.prototype.get_disp_field;
    ObjectSet.set_map[name] = set;
    return set;
};

ObjectSet.register_set = function (set) {
    // 默认配置值
    for (var i = 0; i < set.attrs.length; i++) {
        // 数据集没有指定可添加，默认为true
        if (typeof(set.initable) == 'undefined')
            set.initable = true;
        // 数据集没有指定可编辑，默认为true
        if (typeof(set.editable) == 'undefined')
            set.editable = true;
        // 数据集没有指定可删除，默认为true
        if (typeof(set.deletable) == 'undefined')
            set.deletable = true;
        // 属性没有指定可初始化，默认为true
        if (typeof(set.attrs[i].initable) == 'undefined')
            set.attrs[i].initable = true;
        // 属性没有指定可编辑，默认为与“可初始化”相同
        if (typeof(set.attrs[i].editable) == 'undefined')
            set.attrs[i].editable = true;
        if (typeof(set.attrs[i].nullable) == 'undefined')
            set.attrs[i].nullable = false;
    }
    ObjectSet.set_map[set.name] = set;
    return set;
}

ObjectSet.get_set = function (name) {
    var set = ObjectSet.set_map[name];
    if (!set.ref_keys) {
        set = new ObjectSet(set);
        ObjectSet.set_map[name] = set;
        if (set.refered && !set.delay_ref)
            set.key_map.load();
    }
    return set;
};

ObjectSet.on_main_set_load = function (store, records, option) {
    var records1 = [];
    for (var i = 0; i < records.length; i++) {
        if (this.set_def.sub_filter(records[i])) {
            records1.push(records[i].copy());
        }
    }
    this.removeAll();
    this.add(records1);
    this.fireEvent('load', this, records1, option);
}

ObjectSet.create_sub_store = function () {
    var store = this.main_set.create_store();
    //this.main_set.listen_by(ObjectSet.on_main_set_load, store);
    store.set_def = this;
    store.on('load', function () {
        this.filterBy(this.set_def.sub_filter);
    });
    return store;
}

ObjectSet.register_sub_set = function (main_set, options) {
    if (typeof main_set == "string")
        main_set = ObjectSet.get_set(main_set);
    var sub_set = {};
    Ext.apply(sub_set, options);
    Ext.applyIf(sub_set, main_set);
    sub_set.main_set = main_set;
    sub_set.ref_bys = {};
    sub_set.create_store = ObjectSet.create_sub_store;
    sub_set.key_map = sub_set.create_store();
    ObjectSet.set_map[options.name] = sub_set;
    return sub_set;
};

ObjectSet.init = function () {
    for (var key in ObjectSet.set_map) {
        var set = ObjectSet.get_set(key);
    }
    for (var key in ObjectSet.set_map) {
        var set = ObjectSet.get_set(key);
        if (typeof set.Record != 'undefined' && set.refered && !set.delay_ref)
            set.key_map.load();
        // 定义选择框
        if (typeof set.refered != 'undefined')
            register_combo_xtype(set);
    }
}

ObjectSet.term = function () {
    for (var key in ObjectSet.set_map) {
        if (typeof ObjectSet.set_map[key].Record != 'undefined'
            && ObjectSet.set_map[key].refered)
                ObjectSet.set_map[key].key_map.removeAll();
    }
}

ObjectSet.prototype = {

    // 定义数据存储
    create_store    : function () {
        var store = new Ext.data.JsonStore({
            fields          : this.Record, 
            successProperty : 'success', 
            totalProperty   : 'count', 
            root            : 'data', 
            id              : this.outer_key, 
            url             : this.module_url,
            remoteSort      : true, 
            sortInfo        : this.sortInfo,
            baseParams      : {
                o   : 'get'
            }, 
            listeners       : store_listeners, 
            set_def         : this, 
            force_ref_load  : false,           
            on_ref_load     : function (ref_set) {
                if (this.force_ref_load)
                    this.fireEvent('datachanged', this);
            }
        });
        for (var i = 0; i < this.attrs.length; i++) {
            if (typeof(this.attrs[i].ref_key) != 'undefined') {
                var ref_set = ObjectSet.get_set(this.attrs[i].ref_key);
                ref_set.key_map.on('load', store.on_ref_load.createDelegate(store, [ref_set]));
                if (ref_set.delay_ref) {
                    store.on('load', this.load_ref_key.createDelegate(ref_set, [store, this.attrs[i].name]));
                }
            }
        }
        return store;
    }, 
    
    listen_by       : function (listener, scope) {
        this.refered = true;
        this.key_map.on('load', listener, scope);
    }, 
    
    // 处理关键字引用
    load_ref_key    : function (store, field) {
        var ids = [];
        for (var j = 0; j < store.getCount(); j++) {
            ids.push(store.getAt(j).data[field]);
        }
        var filters = [{
            field   : this.outer_key, 
            data    : {
                type    : 'list', 
                value   : ids
            }
        }];
        this.key_map.load({
            add         : false, 
            params      : {
                filter      : Ext.encode(filters)
            }
        });
    }, 
    
    refresh_key_map : function () {
        if (!this.delay_ref) {
            this.key_map.reload();
        }
    }, 
    
    get_disp_field  : function (id) {
        var record = this.key_map.getById(id);
        if (typeof(record) == 'undefined')
            return this.title + '(' + id + ')';
        return record.get(this.disp_field);
    }
    
};

var combo_xtypes = [];

var get_value = function (set_name, id) {
    return ObjectSet.set_map[set_name].get_disp_field(id);
}

var register_combo_xtype = function (object_set) {
    if (object_set.delay_ref) {
        combo_xtypes[object_set.name] = 'textfield';
    } else {
        var ComboBox = Ext.extend(Ext.form.ComboBox, {
            constructor : function(config) {
                config.forceSelection = true;
                config.editable = false;
                config.mode = 'local';
                config.store = object_set.key_map;
                config.title = '选择' + object_set.title;
                config.valueField = object_set.outer_key;
                config.displayField = object_set.disp_field1 || object_set.disp_field;
                //config.tpl = '<tpl for="."><div class="x-combo-list-item">{[get_value("' + object_set.name + '", values["' + object_set.outer_key + '"])]}</div></tpl>';
                config.triggerAction = 'all';
                ComboBox.superclass.constructor.apply(this, arguments);
            }
        });
        Ext.reg(object_set.name + 'combobox', ComboBox);
        combo_xtypes[object_set.name] = object_set.name + 'combobox';
    }
};

var register_enum = function (name, title, value_name_list) {
    var set = ObjectSet.register_enum(name, title, value_name_list);
    register_combo_xtype(set);
}
