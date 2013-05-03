//combination_strategy.js

var panel_def_combination_strategy = {
		name            : 'combination_strategy',
		title           : '组合策略',
		menu_path       : 'product_info',
		module_url      : 'module/combination_strategy.php',
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
			name            : 'comb_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '组合策略',
			width           : 60,
			ref_key         : 'strategy'
		}, {
			name            : 'strategy_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '简单策略',
			width           : 80,
			ref_key         : 'strategy'
		}]
};

var combination_strategy_set = register_grid_panel(panel_def_combination_strategy);
