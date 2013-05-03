// role.js

var panel_def_role = {
    name            : 'role', 
    title           : '½ÇÉ«',
    menu_path       : 'access', 
    module_url      : 'module/role.php', 
    outer_key       : 'id', 
    disp_field      : 'name', 
    attrs           : [{
        name            : 'id', 
        type            : 'int', 
        defaultValue    : 0, 
        title           : '±àºÅ',
        initable        : false,
        editable        : false, 
        width           : 40
    }, {
        name            : 'name', 
        type            : 'string', 
        defaultValue    : 'role',
        title           : 'Ãû³Æ',
        width           : 100
    }, {
        name            : 'desc', 
        type            : 'string', 
        defaultValue    : '', 
        title           : 'ÃèÊö',
        width           : 200
    }]
};

var role_set = register_grid_panel(panel_def_role);

