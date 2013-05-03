// user.js

var panel_def_user = {
    name            : 'user', 
    title           : '�û�',
    menu_path       : 'access', 
    module_url      : 'module/user.php', 
    outer_key       : 'id', 
    disp_field      : 'name', 
	sortInfo        : {field: 'name', direction: 'ASC'},
    attrs           : [{
        name            : 'id', 
        type            : 'int', 
        defaultValue    : 0, 
        title           : '���',
        initable        : false,
        editable        : false, 
        width           : 40
    }, {
        name            : 'role', 
        type            : 'int', 
        defaultValue    : 0,
        title           : '��ɫ',
        width           : 100,
        ref_key         : 'role'
    }, {
        name            : 'name', 
        type            : 'string', 
        defaultValue    : 'user',
        title           : '����',
        editable        : false, 
        width           : 100
    }, {
        name            : 'pass', 
        type            : 'string', 
        defaultValue    : '', 
        title           : '����',
        xtype           : 'passwordfield', 
        width           : 210
    }, {
        name            : 't_add', 
        type            : 'date', 
        defaultValue    : '2009-04-01 00:00:00', 
        title           : '����ʱ��',
        width           : 120,
        renderer        : format_date_time, 
        xtype           : 'datefield', 
        initable        : false
    }, {
        name            : 't_online', 
        type            : 'date', 
        defaultValue    : '2009-04-01 00:00:00', 
        title           : '�������ʱ��',
        width           : 120,
        renderer        : format_date_time, 
        xtype           : 'datefield', 
        initable        : false
    }]
};

var user_set = register_grid_panel(panel_def_user);
