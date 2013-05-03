//t_project.js

var panel_def_project = {
		name            : 'project',
		title           : '项目',
		menu_path       : 'access',
		module_url      : 'module/project.php',
		outer_key       : ['id','name','platform_id','strategy_id','product_id'],
		disp_field      : 'name',
		editable        : false,
		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '编号',
			editable        : false,
			width           : 60
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '项目名称',
			width           : 400
		}, {
			name            : 'platform_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关平台编号',
			width           : 60,
			ref_key         : 't_platform'
		}, {
			name            : 'begin_time',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '项目开始时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'strategy_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关策略编号',
			width           : 60,
			ref_key         : 't_strategy'
		}, {
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关产品编号',
			width           : 60,
			ref_key         : 't_product'
		}, {
			name            : 'progress',
			type            : 'string',
			defaultValue    : '',
			title           : '项目进展',
			width           : 400
		}, {
			name            : 'complete_time',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '问题解决时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '项目描述',
			width           : 500
		}]
};

var project_set = register_grid_panel(panel_def_project);
