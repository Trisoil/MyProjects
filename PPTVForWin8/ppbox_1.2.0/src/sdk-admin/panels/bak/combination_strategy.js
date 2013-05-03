//combination_strategy.js

var panel_def_combination_strategy = {
		name            : 'combination_strategy',
		title           : '组合策略信息',
		menu_path       : 'info',
		module_url      : 'module/combination_strategy.php',
		outer_key       : ['id','strategy_id'],
		disp_field      : 'name',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40,
			ref_key         : 't_strategy'
		}, {
			name            : 'strategy_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '对应策略编号',
			initable        : false,
			editable        : false,
			width           : 40,
			ref_key         : 't_strategy'
		}]
};

var combination_strategy_set = register_grid_panel(panel_def_combination_strategy);
