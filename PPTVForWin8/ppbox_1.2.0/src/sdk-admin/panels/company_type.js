//company_type.js

var panel_def_company_type = {
		name            : 'company_type',
		title           : '公司类型',
		menu_path       : 'type_info',
		module_url      : 'module/company_type.php',
		outer_key       : 'id',
		disp_field      : 'name',
		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '名称',
			width           : 80
		}]
};

var company_type_set = register_grid_panel(panel_def_company_type);
