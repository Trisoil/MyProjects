//test.js

var result_status = [ 
    [0, '<font color=green>����</font>'], 
    [1, '<font color=brown>������</font>'], 
    [2, '<font color=red>������</font>']
];

register_enum('test_result', '���Խ��', result_status);

var panel_def_test = {
		name            : 'test',
		title           : '����',
		menu_path       : 'equipment_man',
		module_url      : 'module/equipment_test.php',
		outer_key       : 'id',
		disp_field      : 'name',
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'equipment_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '�豸',
			width           : 120,
			ref_key         : 'equipment'
		}, {
			name            : 'protocol_id',
			type            : 'int',
			defaultValue    : 0,
			title           : 'Э��',
			width           : 60,
			ref_key         : 'protocol'
		}, {
			name            : 'format_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��ʽ',
			width           : 60,
			ref_key         : 'format'
		}, {
			name            : 'result',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			width           : 60,
			ref_key         : 'test_result'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 500
		}]
};

var test_set = register_grid_panel(panel_def_test);
