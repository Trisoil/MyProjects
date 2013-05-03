//t_platform.js

var float_t = [
	[0, 'Ӳ����'],
	[1, '����']
];
register_enum('float_t', '��������', float_t);

var byte_ord = [
	[1, 'big-endian'],
	[0, 'little-endian']
];
register_enum('byte_ord','�ֽ���',byte_ord);

var panel_def_platform = {
		name            : 'platform',
		title           : 'ƽ̨',
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
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 100
		}, {
			name            : 'float_type',
			type            : 'int',
			defaultValue    : 0,
			title           : '��������',
			width           : 80,
			ref_key         : 'float_t'
		}, {
			name            : 'byte_order',
			type            : 'int',
			defaultValue    : 0,
			title           : '�ֽ���',
			width           : 80,
			ref_key         : 'byte_ord'
		}, {
			name            : 'tool_chain',
			type            : 'string',
			defaultValue    : '',
			title           : '������',
			width           : 50
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 400
		}]
};

var platform_set = register_grid_panel(panel_def_platform);

