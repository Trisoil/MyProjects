//t_platform.js

var panel_def_platform = {
		name            : 'platform',
		title           : '平台',
		menu_path       : 'info',
		module_url      : 'module/platform.php',
		outer_key       : 'id',
		disp_field      : 'name',
		editable        : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			editable        : false,
			width           : 60
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '平台名称',
			width           : 100
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '平台描述',
			width           : 400
		}]
};

var platform_set = register_grid_panel(panel_def_platform);
