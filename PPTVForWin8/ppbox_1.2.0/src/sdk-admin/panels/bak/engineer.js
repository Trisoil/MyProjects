//engineer.js

var panel_def_engineer = {
		name            : 'engineer',
		title           : '工程师',
		menu_path       : 'access',
		module_url      : 'module/engineer.php',
		outer_key       : ['id','name','company_id'],
		disp_field      : 'name',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 60
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '工程师姓名',
			width           : 100
		}, {
			name            : 'company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '所属公司编号',
			ref_key         : 't_company',
			width           : 100
		}, {
			name            : 'telephone',
			type            : 'int',
			defaultValue    : 0,
			title           : '电话',
			width           : 120
		}, {
			name            : 'email',
			type            : 'string',
			defaultValue    : '',
			title           : '电子邮箱',
			width           : 120
		}, {
			name            : 'role',
			type            : 'string',
			defaultValue    : '',
			title           : '角色',
			width           : 100
		}]
};

var engineer_set = register_grid_panel(panel_def_engineer);

