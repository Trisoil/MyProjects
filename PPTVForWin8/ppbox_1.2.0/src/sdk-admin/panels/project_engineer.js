//project_engineer.js

var panel_def_project_engineer = {
		name            : 'project_engineer',
		title           : '��Ŀ��Ա',
		menu_path       : 'project_man',
		module_url      : 'module/project_engineer.php',
		outer_key       : ['project_id','engineer_id'],
		disp_field      : 'role',
		attrs           : [{
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ŀ',
			width           : 130,
			ref_key         : 'project'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ա',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'role',
			type            : 'string',
		    defaultValue    : '',
			title           : '��ɹ���',
			width           : 100
		}]
};

var project_engineer_set = register_grid_panel(panel_def_project_engineer);

