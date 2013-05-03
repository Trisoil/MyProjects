//product.js

var panel_def_product = {
		name            : 'product',
		title           : '产品',
		menu_path       : 'info',
		module_url      : 'module/product.php',
		outer_key       : 'id',
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
			title           : '产品名称',
			width           : 100
		}]
};

var product_set = register_grid_panel(panel_def_product);
