//strategy.js

var panel_def_strategy = {
		name            : 'strategy',
		title           : '策略',
		menu_path       : 'access',
		module_url      : 'module/strategy.php',
		outer_key       : ['id','platform_id','product_id'],
		disp_field      : 'name',
		editable        : false,
		deletable       : false,
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
			title           : '策略名称',
			width           : 100
		}, {
			name            : 'platform_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关平台编号',
			width           : 60,
			ref_key         : 't_platform'
		}, {
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关产品编号',
			width           : 60,
			ref_key         : 't_product'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '策略描述',
			width           : 400
		}]
};

var strategy_set = register_grid_panel(panel_def_strategy);
