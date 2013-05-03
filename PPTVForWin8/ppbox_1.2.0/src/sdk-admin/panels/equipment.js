//equipment.js

var panel_def_equipment = {
		name            : 'equipment',
		title           : '�豸',
		menu_path       : 'equipment_man',
		module_url      : 'module/equipment.php',
		outer_key       : 'id',
		disp_field      : 'name',
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
			width           : 100
		}, {
			name            : 'type_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '����',
			width           : 70,
			ref_key         : 'equipment_type'
		}, {
			name            : 'company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '����',
			width           : 50,
			ref_key         : 'company'
		}, {
			name            : 'chip_company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : 'оƬ����',
			width           : 50,
			ref_key         : 'company'
		}, {
			name            : 'version',
			type            : 'string',
			defaultValue    : '',
			title           : '�汾',
			width           : 50
		} ,{
			name            : 'platform_id',
			type            : 'int',
			defaultValue    : 0,
			title           : 'ƽ̨',
			width           : 100,
			ref_key         : 'platform'
		} ,{
			name            : 'player',
			type            : 'string',
			defaultValue    : '',
			title           : '������',
			width           : 100
		}, {
			name            : 'owner',
			type            : 'int',
			defaultValue    : 0,
			title           : 'ʹ����Ա',
			width           : 80,
			ref_key         : 'user'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 200
		}]
};

var equipment_set = register_grid_panel(panel_def_equipment);
