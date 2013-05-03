//project_company.js

var panel_def_project_company = {
		name            : 'project_company',
		title           : '��Ŀ���빫˾',
		menu_path       : 'project_man',
		module_url      : 'module/project_company.php',
		outer_key       : 'id',
		disp_field      : 'project_id',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ŀ',
			width           : 130,
			ref_key         : 'project'
		}, {
			name            : 'company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��˾',
			width           : 60,
			ref_key         : 'company'
		}]
};

var project_company_set = register_grid_panel(panel_def_project_company);

