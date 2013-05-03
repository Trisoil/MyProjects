//staff_type.js

var panel_def_staff_type = {
		name            : 'staff_type',
		title           : '��Ա����',
		menu_path       : 'type_info',
		module_url      : 'module/staff_type.php',
		outer_key       : 'id',
		disp_field      : 'name',
		deletable       : false,
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
			width           : 60
		}]
};

var staff_type_set = register_grid_panel(panel_def_staff_type);
