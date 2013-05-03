// target.js

var panel_def_target = {
    name            : 'target', 
    title           : '����',
    menu_path       : 'access', 
    module_url      : 'module/target.php', 
    outer_key       : 'id', 
    disp_field      : 'name', 
//    initable        : false, 
//    editable        : false, 
//    deletable       : false, 
    attrs           : [{
        name            : 'id', 
        type            : 'int', 
        defaultValue    : 0, 
        title           : '���',
        initable        : false,
        editable        : false, 
        width           : 40
    }, {
        name            : 'name', 
        type            : 'string', 
        defaultValue    : '',
        title           : '����',
        width           : 120
    }, {
        name            : 'desc', 
        type            : 'string', 
        defaultValue    : '', 
        title           : '����',
        width           : 200
    }]
};

var target_set = register_grid_panel(panel_def_target);

