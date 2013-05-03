//t_problem.js

var panel_def_problem = {
		name            : 'problem',
		title           : '问题',
		menu_path       : 'info',
		module_url      : 'module/problem.php',
		outer_key       : 'id',
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
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 400
		}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关项目编号',
			width           : 60,
			ref_key         : 't_project'
		}, {
			name            : 'begin_time',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '问题提交时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'owner_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '提交问题工程师编号',
			width           : 60,
			ref_key         : 't_engineer'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '解决问题工程师编号',
			width           : 60,
			ref_key         : 't_engineer'
		}, {
			name            : 'progress',
			type            : 'string',
			defaultValue    : '',
			title           : '进展',
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
			name            : 'instruction',
			type            : 'string',
			defaultValue    : '',
			title           : '备注',
			width           : 500
		}]
};

var problem_set = register_grid_panel(panel_def_problem);
