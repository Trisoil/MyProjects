//t_platform.js

var float_t = [
	[0, '硬浮点'],
	[1, '软浮点']
];
register_enum('float_t', '浮点类型', float_t);

var byte_ord = [
	[1, 'big-endian'],
	[0, 'little-endian']
];
register_enum('byte_ord','字节序',byte_ord);

var panel_def_platform = {
		name            : 'platform',
		title           : '平台',
		menu_path       : 'product_info',
		module_url      : 'module/platform.php',
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
			title           : '名称',
			width           : 100
		}, {
			name            : 'float_type',
			type            : 'int',
			defaultValue    : 0,
			title           : '浮点类型',
			width           : 80,
			ref_key         : 'float_t'
		}, {
			name            : 'byte_order',
			type            : 'int',
			defaultValue    : 0,
			title           : '字节序',
			width           : 80,
			ref_key         : 'byte_ord'
		}, {
			name            : 'tool_chain',
			type            : 'string',
			defaultValue    : '',
			title           : '工具链',
			width           : 50
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 400
		}]
};

var platform_set = register_grid_panel(panel_def_platform);

