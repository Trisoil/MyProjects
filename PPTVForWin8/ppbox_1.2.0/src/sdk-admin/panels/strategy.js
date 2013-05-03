//strategy.js

var panel_def_strategy = {
		name            : 'strategy',
		title           : '����',
		menu_path       : 'product_info',
		module_url      : 'module/strategy.php',
		outer_key       : 'id',
		disp_field      : 'name',
//		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ʒ',
			width           : 80,
			ref_key         : 'product'
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 100
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 200
		}]
};

var strategy_set = register_grid_panel(panel_def_strategy);
