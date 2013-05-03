//equipment.js

var panel_def_equipment = {
		name            : 'equipment',
		title           : '设备',
		menu_path       : 'equipment_man',
		module_url      : 'module/equipment.php',
		outer_key       : 'id',
		disp_field      : 'name',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '名称',
			width           : 100
		}, {
			name            : 'type_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '类型',
			width           : 70,
			ref_key         : 'equipment_type'
		}, {
			name            : 'company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '厂商',
			width           : 50,
			ref_key         : 'company'
		}, {
			name            : 'chip_company_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '芯片厂商',
			width           : 50,
			ref_key         : 'company'
		}, {
			name            : 'version',
			type            : 'string',
			defaultValue    : '',
			title           : '版本',
			width           : 50
		} ,{
			name            : 'platform_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '平台',
			width           : 100,
			ref_key         : 'platform'
		} ,{
			name            : 'player',
			type            : 'string',
			defaultValue    : '',
			title           : '播放器',
			width           : 100
		}, {
			name            : 'owner',
			type            : 'int',
			defaultValue    : 0,
			title           : '使用人员',
			width           : 80,
			ref_key         : 'user'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 200
		}]
};

var equipment_set = register_grid_panel(panel_def_equipment);
