//format.js

var panel_def_format = {
		name            : 'format',
		title           : '¸ñÊ½',
		menu_path       : 'product_info',
		module_url      : 'module/format.php',
		outer_key       : 'id',
		disp_field      : 'name',
		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '±àºÅ',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : 'Ãû³Æ',
			width           : 60
		}]
};

var format_set = register_grid_panel(panel_def_format);
