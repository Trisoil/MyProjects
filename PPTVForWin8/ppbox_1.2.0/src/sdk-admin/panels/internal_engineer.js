//internal_engineer.js

var panel_def_internal_engineer = {
		name            : 'internal_engineer',
		title           : '内部人员',
		menu_path       : 'people',
		module_url      : 'module/internal_engineer.php',
		outer_key       : 'id',
		disp_field      : 'name',
		deletable       : false,
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
			title           : '姓名',
			width           : 60
		}, {
//			name            : 'company_id',
//			type            : 'int',
//			defaultValue    : 0,
//			title           : '公司',
//			ref_key         : 'company',
//			width           : 60
//		}, {
			name            : 'type_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '角色',
			ref_key         : 'staff_type',
			width           : 40
		}, {
			name            : 'telephone',
			type            : 'string',
			defaultValue    : 0,
			title           : '电话',
			width           : 200
		}, {
			name            : 'email',
			type            : 'string',
			defaultValue    : '',
			title           : '邮箱',
			width           : 200
		}]
};

var internal_engineer_set = register_grid_panel(panel_def_internal_engineer);

