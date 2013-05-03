//strategy.js

var panel_def_strategy = {
		name            : 'strategy',
		title           : '策略',
		menu_path       : 'product_info',
		module_url      : 'module/strategy.php',
		outer_key       : 'id',
		disp_field      : 'name',
//		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '产品',
			width           : 80,
			ref_key         : 'product'
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '名称',
			width           : 100
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 200
		}]
};

var strategy_set = register_grid_panel(panel_def_strategy);
