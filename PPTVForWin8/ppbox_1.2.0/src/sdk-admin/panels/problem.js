//t_problem.js

var progress_type = [
	['0','<font color=red>��������</font>'],
	['1','<font color=blue>����׶�</font>'],
	['2','<font color=green>�ѽ��</font>']
];

var solution_type = [ 
    ['1','�Ƴٽ��'],
    ['2','���������'],
	['3','�����޸�'],
	['4','����Ż�'],
	['5','��������'],
	['6','�����']
];

register_enum('solution_type', '�������', solution_type);
register_enum('progress_type', '��չ', progress_type);
var panel_def_problem = {
		name            : 'problem',
		title           : '�������',
		menu_path       : 'project_man',
		module_url      : 'module/problem.php',
		outer_key       : 'id',
		disp_field      : 'description',
		sortInfo        : {field: 'begin_time', direction: 'DESC'},
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 150
		}, {
			name            : 'owner_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '�ύ��Ա',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'begin_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '�ύʱ��',
			width           : 70,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
		//	name            : 'bug_id',
		//	type            : 'int',
		//	defaultValue    : 0,
		//	title           : '�ڲ�bug'
		//	width           : 120,
		//	ref_key         : 'bugfree'
	//	}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ŀ',
			width           : 80,
			ref_key         : 'project'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '������Ա',
			width           : 60,
			ref_key         : 'internal_engineer'
		}, {
			name            : 'progress',
			type            : 'int',
			defaultValue    : '',
			title           : '��չ',
			width           : 80,
			ref_key			: 'progress_type'
		}, {
	        name            : 'problem_type',
			type            : 'int',
			defaultValue    : 0,
			title           : '��������',
			width           : 60,
			ref_key         : 'problem_type'
		},{
			name            : 'specificmatters',
			type            : 'string',
			defaultValue    : '',
			title           : '��������',
			width           : 120
		}, {
			name            : 'solution',
			type            : 'int',
			defaultValue    : '',
			title           : '�������',
			width           : 80,
			ref_key         : 'solution_type'
		}, {
			name            : 'complete_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '���ʱ��',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'instruction',
			type            : 'string',
			defaultValue    : '',
			title           : '��ע',
			width           : 500
		}]
};

var problem_set = register_grid_panel(panel_def_problem);
