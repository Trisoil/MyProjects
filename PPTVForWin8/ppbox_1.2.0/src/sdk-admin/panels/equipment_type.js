//equipment_type.js

var panel_def_equipment_type = {
		name            : 'equipment_type',
		title           : '�豸����',
		menu_path       : 'type_info',
		module_url      : 'module/equipment_type.php',
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

var equipment_type_set = register_grid_panel(panel_def_equipment_type);
