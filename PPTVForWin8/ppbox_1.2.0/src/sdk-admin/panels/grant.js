// grant.js

var panel_def_grant = {
    name            : 'grant', 
    title           : '授权',
    menu_path       : 'access', 
    module_url      : 'module/grant.php', 
    outer_key       : ['role', 'target', 'operation'],
    disp_field      : 'name', 
//    editable        : false, 
    attrs           : [{
        name            : 'role', 
        type            : 'int', 
        defaultValue    : 0, 
        title           : '角色',
        width           : 100,
        ref_key         : 'role'
    }, {
        name            : 'target', 
        type            : 'int', 
        defaultValue    : 0,
        title           : '权限',
        width           : 120,
        ref_key         : 'target'
    }, {
        name            : 'operation', 
        type            : 'int', 
        defaultValue    : 0,
        title           : '操作',
        width           : 120,
        ref_key         : 'operation'
    }]
};

var grant_set = register_grid_panel(panel_def_grant);

