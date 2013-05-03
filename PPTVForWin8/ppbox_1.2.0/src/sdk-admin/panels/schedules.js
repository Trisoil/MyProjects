//schedules.js
var workload_status = [
	[0, '大'],
	[1, '中'],
	[2, '小']
];

var quality_status = [
	[0, '优'],
	[1, '良'],
	[2, '差']
];

register_enum('workload_type', '工作量', workload_status);
register_enum('quality_type', '工作质量', quality_status);

var panel_def_schedules = {
		name            : 'schedules',
		title           : '每日工作流水',
		menu_path       : 'schedules_everyday',
		module_url      : 'module/schedules.php',
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
			type            : 'int',
			defaultValue    : 0,
			title           : '姓名',
			width           : 60,
			ref_key         : 'user',
			initable        : false
		}, {
			name            : 'time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '日期',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'content',
			type            : 'string',
			defaultValue    : '',
			title           : '工作内容',
			width           : 250
		}, {
			name            : 'workload',
			type            : 'int',
			defaultValue    : 1,
			title           : '任务量',
			width           : 60,
			ref_key         : 'workload_type'
		}, {
			name            : 'quality',
			type            : 'int',
			defaultValue    : 1,
			title           : '完成质量',
			width           : 60,
			ref_key         : 'quality_type'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '备注',
			width           : 280
		}]
};

var schedules_set = register_grid_panel(panel_def_schedules);
