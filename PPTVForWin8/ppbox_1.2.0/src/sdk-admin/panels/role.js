// role.js

var panel_def_role = {
    name            : 'role', 
    title           : '��ɫ',
    menu_path       : 'access', 
    module_url      : 'module/role.php', 
    outer_key       : 'id', 
    disp_field      : 'name', 
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
        defaultValue    : 'role',
        title           : '����',
        width           : 100
    }, {
        name            : 'desc', 
        type            : 'string', 
        defaultValue    : '', 
        title           : '����',
        width           : 200
    }]
};

var role_set = register_grid_panel(panel_def_role);

