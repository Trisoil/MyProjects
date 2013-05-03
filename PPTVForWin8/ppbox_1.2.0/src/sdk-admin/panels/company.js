//company.js

var panel_def_company = {
	name            : 'company',
	title           : '公司',
	menu_path       : 'people',
	module_url      : 'module/company.php',
	outer_key       : 'id',
	disp_field      : 'name',
	sortInfo        : {field: 'name', direction: 'ASC'},
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
		width           : 60
	}, {
		name            : 'type_id',
		type            : 'int',
		defaultValue    : 0, 
		title           : '类型',
		width           : 70,
		ref_key         : 'company_type'
	}, {
		name            : 'address',
		type            : 'string',
		defaultValue    : '',
		title           : '地址',
		width           : 200
	}]
};
var company_set = register_grid_panel(panel_def_company);
