/*
 * Ext JS Library 2.2
 * Copyright(c) 2006-2008, Ext JS, LLC.
 * licensing@extjs.com
 * 
 * http://extjs.com/license
 */

Ext.grid.RowExpander = function(config){
    Ext.apply(this, config);

    this.addEvents({
        beforeexpand: true,
        expand: true,
        beforecollapse: true,
        collapse: true
    });

    Ext.grid.RowExpander.superclass.constructor.call(this);

    this.state = {};
    this.bodyPanels = {};
};

Ext.extend(Ext.grid.RowExpander, Ext.util.Observable, {
    header: "",
    width: 20,
    sortable: false,
    fixed:true,
    menuDisabled:true,
    dataIndex: '',
    id: 'expander',
    lazyRender: true,
    enableCaching: true,

    init: function(grid){
        this.grid = grid;
        this.bodyPanels = {};
        
        var view = grid.getView();
        view.getRowClass = this.getRowClass.createDelegate(this);

        view.enableRowBody = true;

        grid.on('render', function(){
            view.mainBody.on('mousedown', this.onMouseDown, this);
        }, this);
    },

    getRowClass: function(record, rowIndex, p, ds){
        p.cols = p.cols-1;
        var panel = this.bodyPanels[record.id];
        if(!panel && !this.lazyRender){
            panel = this.getBodyPanel(record, rowIndex);
        }
        if(panel){
            p.body = panel;
        }
        return this.state[record.id] ? 'x-grid3-row-expanded': 'x-grid3-row-collapsed';
    },

    getBodyPanel: function(record, index){
        if(!this.enableCaching){
            return this.createPanel(record, index);
        }
        var panel = this.bodyPanels[record.id];
        if(!panel){
            panel = this.createPanel(record, index);
            this.bodyPanels[record.id] = panel;
        }
        return panel;
    },

    onMouseDown: function(e, t){
        if(t.className == 'x-grid3-row-expander'){
            e.stopEvent();
            var row = e.getTarget('.x-grid3-row');
            this.toggleRow(row);
        }
    },

    renderer: function(v, p, record){
        p.cellAttr = 'rowspan="2"';
        return '<div class="x-grid3-row-expander"> </div>';
    },

    beforeExpand: function(record, body, rowIndex){
        if(this.fireEvent('beforeexpand', this, record, body, rowIndex) !== false){
            if(this.lazyRender && body.innerHTML == ""){
                var id = Ext.id(null, 'dss-exp-');
                body.innerHTML = "<div id='" + id + "'></div>";
                var panel = this.getBodyPanel(record, body, rowIndex);
                panel.applyToMarkup(id);
            }
            return true;
        }else{
            return false;
        }
    },

    toggleRow: function(row){
        if(typeof row == 'number'){
            row = this.grid.view.getRow(row);
        }
        this[Ext.fly(row).hasClass('x-grid3-row-collapsed') ? 'expandRow': 'collapseRow'](row);
    },

    expandRow: function(row){
        if(typeof row == 'number'){
            row = this.grid.view.getRow(row);
        }
        var record = this.grid.store.getAt(row.rowIndex);
        var body = Ext.DomQuery.selectNode('tr:nth(2) div.x-grid3-row-body', row);
        if(this.beforeExpand(record, body, row.rowIndex)){
            this.state[record.id] = true;
            Ext.fly(row).replaceClass('x-grid3-row-collapsed', 'x-grid3-row-expanded');
            this.fireEvent('expand', this, record, body, row.rowIndex);
        }
    },

    collapseRow: function(row){
        if(typeof row == 'number'){
            row = this.grid.view.getRow(row);
        }
        var record = this.grid.store.getAt(row.rowIndex);
        var body = Ext.fly(row).child('tr:nth(1) div.x-grid3-row-body', true);
        if(this.fireEvent('beforcollapse', this, record, body, row.rowIndex) !== false){
            this.state[record.id] = false;
            Ext.fly(row).replaceClass('x-grid3-row-expanded', 'x-grid3-row-collapsed');
            this.fireEvent('collapse', this, record, body, row.rowIndex);
        }
    }
});
