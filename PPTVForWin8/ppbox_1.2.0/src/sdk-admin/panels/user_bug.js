//t_user_bug.js


var panel_def_user_bug= {
		name            : 'user_bug',
		title           : '�û�����',
		menu_path       : 'project_man',
		module_url      : 'module/user_bug.php',
		outer_key       : 'id',
		disp_field      : 'name',
//		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'track_id',
			type            : 'int',
		    defaultValue    : 0,
		    title           : '�ڲ������',
			width           : 70,
			ref_key         : 'problem'
		}, {
			name			: 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '�ύ��Ա',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '��ϵ��',
			width           : 60
		}, {
			name            : 'telephone',
			type            : 'string',
			defaultValue    : '',
			title           : '�绰',
			width           : 90
		}, {
			name            : 'address',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 50
		}, {
//			name            : 'buy_date',
//			type            : 'string',
//			defaultValue    : '',
//			title           : '��������',
//			width           : 60
//		}, {
//			name            : 'version',
//			type            : 'string',
//			defaultValue    : '',
//			title           : '����汾',
//			width           : 60
//		}, {
			name            : 'serial_number',
			type            : 'string',
			defaultValue    : '',
			title           : 'MAC��ַ',
			width           : 105
		}, {
			name            : 'net_service',
			type            : 'string',
			defaultValue    : '',
			title           : '���绷��',
			width           : 60
		}, {
			name            : 'H3_ways',
			type            : 'string',
			defaultValue    : '',
			title           : '���뷽ʽ',
			width           : 60
		}, {
			name            : 'scope',
			type            : 'string',
			defaultValue    : '',
			title           : '��������',
			width           : 300
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '���Ϸ�Χ',
			width           : 200
		}, {
			name            : 'frequency',
			type            : 'string',
			defaultValue    : '',
			title           : '����Ƶ��',
			width           : 200
		}]
};

var user_bug_set = register_grid_panel(panel_def_user_bug);

