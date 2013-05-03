//project_engineer.js

var panel_def_project_engineer = {
		name            : 'project_engineer',
		title           : '项目人员',
		menu_path       : 'project_man',
		module_url      : 'module/project_engineer.php',
		outer_key       : ['project_id','engineer_id'],
		disp_field      : 'role',
		attrs           : [{
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目',
			width           : 130,
			ref_key         : 'project'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '人员',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'role',
			type            : 'string',
		    defaultValue    : '',
			title           : '完成工作',
			width           : 100
		}]
};

var project_engineer_set = register_grid_panel(panel_def_project_engineer);

