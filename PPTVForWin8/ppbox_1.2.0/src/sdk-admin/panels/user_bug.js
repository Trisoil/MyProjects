//t_user_bug.js


var panel_def_user_bug= {
		name            : 'user_bug',
		title           : '用户故障',
		menu_path       : 'project_man',
		module_url      : 'module/user_bug.php',
		outer_key       : 'id',
		disp_field      : 'name',
//		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'track_id',
			type            : 'int',
		    defaultValue    : 0,
		    title           : '内部处理号',
			width           : 70,
			ref_key         : 'problem'
		}, {
			name			: 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '提交人员',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '联系人',
			width           : 60
		}, {
			name            : 'telephone',
			type            : 'string',
			defaultValue    : '',
			title           : '电话',
			width           : 90
		}, {
			name            : 'address',
			type            : 'string',
			defaultValue    : '',
			title           : '地区',
			width           : 50
		}, {
//			name            : 'buy_date',
//			type            : 'string',
//			defaultValue    : '',
//			title           : '购机日期',
//			width           : 60
//		}, {
//			name            : 'version',
//			type            : 'string',
//			defaultValue    : '',
//			title           : '软件版本',
//			width           : 60
//		}, {
			name            : 'serial_number',
			type            : 'string',
			defaultValue    : '',
			title           : 'MAC地址',
			width           : 105
		}, {
			name            : 'net_service',
			type            : 'string',
			defaultValue    : '',
			title           : '网络环境',
			width           : 60
		}, {
			name            : 'H3_ways',
			type            : 'string',
			defaultValue    : '',
			title           : '接入方式',
			width           : 60
		}, {
			name            : 'scope',
			type            : 'string',
			defaultValue    : '',
			title           : '故障现象',
			width           : 300
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '故障范围',
			width           : 200
		}, {
			name            : 'frequency',
			type            : 'string',
			defaultValue    : '',
			title           : '故障频率',
			width           : 200
		}]
};

var user_bug_set = register_grid_panel(panel_def_user_bug);

