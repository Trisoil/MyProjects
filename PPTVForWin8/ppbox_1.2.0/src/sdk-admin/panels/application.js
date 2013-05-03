//pplication.js

var confirm_t = [
	[0, '<font color=brown>审批中</font>'],
	[1, '<font color=green>已批准</font>'],
	[2, '<font color=red>未通过</font>']
];

register_enum('confirm_type','审批状态',confirm_t);

var panel_def_application = {
		name            : 'application',
		title           : '申请',
	    menu_path       : 'equipment_man',
		module_url      : 'module/application.php',
		outer_key       : 'id',
		disp_field      : 'status',
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
			width           : 60,
			ref_key         : 'equipment'
		}, {
			name            : 'applicant',
			type            : 'int',
			defaultValue    : 0,
			title           : '申请人',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'application_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '申请时间',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
//			initable        : false
		}, {
			name            : 'confirm_status',
			type            : 'int',
			defaultValue    : 0,
			title           : '审批状态',
			width           : 60,
			ref_key         : 'confirm_type'
		}, {	
			name            : 'confirm_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '批复时间',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
	//		initable        : false
		}, {
			name            : 'status',
			type            : 'string',
			defaultValue    : '',
			title           : '备注',
			width           : 200
		}]
};

var application_set = register_grid_panel(panel_def_application);  
