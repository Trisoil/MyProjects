//schedules.js
var workload_status = [
	[0, '��'],
	[1, '��'],
	[2, 'С']
];

var quality_status = [
	[0, '��'],
	[1, '��'],
	[2, '��']
];

register_enum('workload_type', '������', workload_status);
register_enum('quality_type', '��������', quality_status);

var panel_def_schedules = {
		name            : 'schedules',
		title           : 'ÿ�չ�����ˮ',
		menu_path       : 'schedules_everyday',
		module_url      : 'module/schedules.php',
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
			type            : 'int',
			defaultValue    : 0,
			title           : '����',
			width           : 60,
			ref_key         : 'user',
			initable        : false
		}, {
			name            : 'time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '����',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'content',
			type            : 'string',
			defaultValue    : '',
			title           : '��������',
			width           : 250
		}, {
			name            : 'workload',
			type            : 'int',
			defaultValue    : 1,
			title           : '������',
			width           : 60,
			ref_key         : 'workload_type'
		}, {
			name            : 'quality',
			type            : 'int',
			defaultValue    : 1,
			title           : '�������',
			width           : 60,
			ref_key         : 'quality_type'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '��ע',
			width           : 280
		}]
};

var schedules_set = register_grid_panel(panel_def_schedules);
