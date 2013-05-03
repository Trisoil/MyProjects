//domain_stratyge.js

var panel_def_domain_strategy = {
		name            : 'domain_strategy',
		title           : '��������',
		menu_path       : 'product_info',
		module_url      : 'module/domain_strategy.php',
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
			width           : 50
		}, {
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ʒ',
			width           : 80,
			ref_key         : 'product'
		} , {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 180
		}]
};

var domain_strategy_set = register_grid_panel(panel_def_domain_strategy);
