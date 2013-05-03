//t_problem.js

var progress_type = [
	['0','<font color=red>分析问题</font>'],
	['1','<font color=blue>解决阶段</font>'],
	['2','<font color=green>已解决</font>']
];

var solution_type = [ 
    ['1','推迟解决'],
    ['2','第三方解决'],
	['3','代码修复'],
	['4','设计优化'],
	['5','不能重现'],
	['6','不解决']
];

register_enum('solution_type', '解决方案', solution_type);
register_enum('progress_type', '进展', progress_type);
var panel_def_problem = {
		name            : 'problem',
		title           : '问题跟踪',
		menu_path       : 'project_man',
		module_url      : 'module/problem.php',
		outer_key       : 'id',
		disp_field      : 'description',
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
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '标题',
			width           : 150
		}, {
			name            : 'owner_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '提交人员',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'begin_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '提交时间',
			width           : 70,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
		//	name            : 'bug_id',
		//	type            : 'int',
		//	defaultValue    : 0,
		//	title           : '内部bug'
		//	width           : 120,
		//	ref_key         : 'bugfree'
	//	}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目',
			width           : 80,
			ref_key         : 'project'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '责任人员',
			width           : 60,
			ref_key         : 'internal_engineer'
		}, {
			name            : 'progress',
			type            : 'int',
			defaultValue    : '',
			title           : '进展',
			width           : 80,
			ref_key			: 'progress_type'
		}, {
	        name            : 'problem_type',
			type            : 'int',
			defaultValue    : 0,
			title           : '问题类型',
			width           : 60,
			ref_key         : 'problem_type'
		},{
			name            : 'specificmatters',
			type            : 'string',
			defaultValue    : '',
			title           : '具体问题',
			width           : 120
		}, {
			name            : 'solution',
			type            : 'int',
			defaultValue    : '',
			title           : '解决方案',
			width           : 80,
			ref_key         : 'solution_type'
		}, {
			name            : 'complete_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '解决时间',
			width           : 80,
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
