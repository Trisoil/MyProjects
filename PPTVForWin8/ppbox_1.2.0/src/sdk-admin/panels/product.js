//product.js

var panel_def_product = {
		name            : 'product',
		title           : '²úÆ·',
		menu_path       : 'product_info',
		module_url      : 'module/product.php',
		outer_key       : 'id',
		disp_field      : 'name',
		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '±àºÅ',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : 'Ãû³Æ',
			width           : 80
		}]
};

var product_set = register_grid_panel(panel_def_product);
