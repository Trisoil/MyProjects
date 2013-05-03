//company.js

var panel_def_company = {
    name            : 'company',
	title           : '公司',
	menu_path       : 'access',
	module_url      : 'module/company.php',
	outer_key       : 'id',
	disp_field      : 'name',
	editable        : false,
	attrs           : [{
		name            : 'id',
		type			: 'int',
		defaultValue    : 0,
		title           : '编号',
		initable        : false,
		editable        : false,
		width           : 40
	}, {
		name            : 'name',
		type            : 'string',
		defaultValue    : '',
		title           : '公司名称',
		width           : 100
	}, {
		name            : 'type',
		type            : 'string',
		defaultValue    : '', 
		title           : '类型',
		width           : 100
	}, {
		name            : 'address',
		type            : 'string',
		defaultValue    : '',
		title           : '地址',
		width           : 400
	}]
};
var company_set = register_grid_panel(panel_def_company);
