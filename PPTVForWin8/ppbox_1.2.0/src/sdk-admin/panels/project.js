//t_project.js

var project_progress_status = [
	[0,'<font color=green>进行中</font>'],
	[1,'<font color=red>已结束</font>']
];

var priority_status = [
	[0,'高'],
	[1,'中'],
	[2,'低']
];

var result_status = [
	[0,'<font color=green>正常</font>'],
	[1,'<font color=red>取消</font>'],
	[2,'<font color=brown>推迟</font>']
];

register_enum('project_progress_type', '进展', project_progress_status);
register_enum('result_type', '结果', result_status);
register_enum('priority_type', '优先级', priority_status);

var panel_def_project = {
		name            : 'project',
		title           : '项目',
		menu_path       : 'project_man',
		module_url      : 'module/project.php',
		outer_key       : 'id',
		disp_field      : 'name',
                sortInfo        : {field: 'begin_time', direction: 'DESC'}, 
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
			width           : 110
		}, {
			name            : 'priority',
			type            : 'int',
			defaultValue    : 1,
			title           : '优先级',
			width           : 50,
			ref_key         : 'priority_type'
		} ,{
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '产品',
			width           : 80,
			ref_key         : 'product'
		}, {
			name            : 'strategy_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '策略',
			width           : 80,
			ref_key         : 'strategy'
		}, {
			name            : 'domain_strategy_id',
			type            : 'int',
			defaultValue    : 2,
			title           : '域名策略',
			width           : 80,
			ref_key         : 'domain_strategy'
		}, {
			name            : 'platform_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '平台',
			width           : 90,
			ref_key         : 'platform'
		}, {
			name            : 'begin_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '开始时间',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
//			initable        : false
		}, {
			name            : 'progress',
			type            : 'int',
			defaultValue    : 0,
			title           : '进展',
			width           : 60,
			ref_key         : 'project_progress_type'
		}, {
			name            : 'result',
			type            : 'int',
			defaultValue    : 0,
			title           : '结果',
			width           : 60,
			ref_key         : 'result_type'
		}, {
			name            : 'complete_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '更新时间',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 250
		}]
};

var project_set = register_grid_panel(panel_def_project);
