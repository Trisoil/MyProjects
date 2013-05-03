// operation.js

var panel_def_operation = {
    name            : 'operation', 
    title           : '²Ù×÷',
    menu_path       : 'access', 
    module_url      : 'module/operation.php', 
    outer_key       : 'id', 
    disp_field      : 'name', 
    initable        : false, 
    editable        : false, 
    deletable       : false, 
    attrs           : [{
        name            : 'id', 
        type            : 'int', 
        defaultValue    : 0, 
        title           : '±àºÅ',
        initable        : false,
        editable        : false, 
        width           : 60
    }, {
        name            : 'op', 
        type            : 'int', 
        defaultValue    : '0',
        title           : '²Ù×÷',
        width           : 60
    }, {
        name            : 'name', 
        type            : 'string', 
        defaultValue    : '0',
        title           : '²Ù×÷',
        width           : 120
    }, {
        name            : 'desc', 
        type            : 'string', 
        defaultValue    : '', 
        title           : 'ÃèÊö',
        width           : 200
    }]
};

var operation_set = register_grid_panel(panel_def_operation);

