//project_engineer.js

var panel_def_project_engineer = {
		name            : 'project_engineer',
		title           : '项目参与人员',
		menu_path       : 'access',
		module_url      : 'module/project_engineer.php',
		outer_key       : ['project_id','engineer_id'],
		disp_field      : 'name',
		attrs           : [{
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目编号',
			width           : 60,
			ref_key         : 't_project'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '工程师编号',
			width           : 60,
			ref_key         : 't_engineer'
		}, {
			name            : 'role',
			type            : 'string',
		    defaultValue    : '',
			title           : '人员担任角色',
			width           : 100
		}]
};

var project_engineer_set = register_grid_panel(panel_def_project_engineer);

