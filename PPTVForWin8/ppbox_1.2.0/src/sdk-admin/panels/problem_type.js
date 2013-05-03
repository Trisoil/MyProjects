//problem_type.js

var panel_def_problem_type = {
		name            : 'problem_type',
		title           : '��������',
		menu_path       : 'type_info',
		module_url      : 'module/problem_type.php',
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
			width           : 100
		}, {
			name            : 'description',
			type            : 'string',
		    defaultValue    : '',
			title           : '����',
			width           : 150	
		}]
};

var problem_type_set = register_grid_panel(panel_def_problem_type);