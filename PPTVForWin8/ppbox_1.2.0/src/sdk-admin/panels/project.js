//t_project.js

var project_progress_status = [
	[0,'<font color=green>������</font>'],
	[1,'<font color=red>�ѽ���</font>']
];

var priority_status = [
	[0,'��'],
	[1,'��'],
	[2,'��']
];

var result_status = [
	[0,'<font color=green>����</font>'],
	[1,'<font color=red>ȡ��</font>'],
	[2,'<font color=brown>�Ƴ�</font>']
];

register_enum('project_progress_type', '��չ', project_progress_status);
register_enum('result_type', '���', result_status);
register_enum('priority_type', '���ȼ�', priority_status);

var panel_def_project = {
		name            : 'project',
		title           : '��Ŀ',
		menu_path       : 'project_man',
		module_url      : 'module/project.php',
		outer_key       : 'id',
		disp_field      : 'name',
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
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 110
		}, {
			name            : 'priority',
			type            : 'int',
			defaultValue    : 1,
			title           : '���ȼ�',
			width           : 50,
			ref_key         : 'priority_type'
		} ,{
			name            : 'product_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ʒ',
			width           : 80,
			ref_key         : 'product'
		}, {
			name            : 'strategy_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '����',
			width           : 80,
			ref_key         : 'strategy'
		}, {
			name            : 'domain_strategy_id',
			type            : 'int',
			defaultValue    : 2,
			title           : '��������',
			width           : 80,
			ref_key         : 'domain_strategy'
		}, {
			name            : 'platform_id',
			type            : 'int',
			defaultValue    : 0,
			title           : 'ƽ̨',
			width           : 90,
			ref_key         : 'platform'
		}, {
			name            : 'begin_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '��ʼʱ��',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
//			initable        : false
		}, {
			name            : 'progress',
			type            : 'int',
			defaultValue    : 0,
			title           : '��չ',
			width           : 60,
			ref_key         : 'project_progress_type'
		}, {
			name            : 'result',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			width           : 60,
			ref_key         : 'result_type'
		}, {
			name            : 'complete_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '����ʱ��',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 250
		}]
};

var project_set = register_grid_panel(panel_def_project);
