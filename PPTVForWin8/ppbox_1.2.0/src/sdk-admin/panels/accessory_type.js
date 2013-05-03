//accessory.js

var panel_def_accessory = {
		name            : 'accessory_type',
		title           : '附件类型',
		menu_path       : 'type_info',
		module_url      : 'module/accessory_type.php',
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
			width           : 60
		}]
};

var accessory_set = register_grid_panel(panel_def_accessory);
