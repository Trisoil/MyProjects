//equipment_accessory.js

var panel_def_equipment_accessory = {
		name            : 'equipment_accessory',
		title           : '附件',
		menu_path       : 'equipment_man',
		module_url      : 'module/equipment_accessory.php',
		outer_key       : 'id',
		disp_field      : 'description',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'equipment_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '设备',
			width           : 180,
			ref_key         : 'equipment'
		},{
			name            : 'accessory_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '附件类型',
			width           : 60,
			ref_key         : 'accessory_type'
		},{
            name            : 'description',
	        type            : 'string',
	        defaultValue    : '', 
	        title           : '描述',
	        width           : 400
		}]
};

var equipment_accessory_set = register_grid_panel(panel_def_equipment_accessory);
