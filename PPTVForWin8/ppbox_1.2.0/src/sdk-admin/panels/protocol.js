//protocol.js

var panel_def_protocol = {
		name            : 'protocol',
		title           : 'Э��',
		menu_path       : 'product_info',
		module_url      : 'module/protocol.php',
		outer_key       : 'id',
		disp_field      : 'name',
		deletable       : false,
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
			width           : 60
		}]
};

var protocol_set = register_grid_panel(panel_def_protocol);
