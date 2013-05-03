//project_company.js

var panel_def_project_company = {
		name            : 'project_company',
		title           : '项目参与公司',
		menu_path       : 'project_man',
		module_url      : 'module/project_company.php',
		outer_key       : 'id',
		disp_field      : 'project_id',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目',
			width           : 130,
			ref_key         : 'project'
		}, {
			name            : 'company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '公司',
			width           : 60,
			ref_key         : 'company'
		}]
};

var project_company_set = register_grid_panel(panel_def_project_company);

