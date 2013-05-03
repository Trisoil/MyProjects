//test.js

var result_status = [ 
    [0, '<font color=green>正常</font>'], 
    [1, '<font color=brown>有问题</font>'], 
    [2, '<font color=red>非正常</font>']
];

register_enum('test_result', '测试结果', result_status);

var panel_def_test = {
		name            : 'test',
		title           : '测试',
		menu_path       : 'equipment_man',
		module_url      : 'module/equipment_test.php',
		outer_key       : 'id',
		disp_field      : 'name',
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
			width           : 120,
			ref_key         : 'equipment'
		}, {
			name            : 'protocol_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '协议',
			width           : 60,
			ref_key         : 'protocol'
		}, {
			name            : 'format_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '格式',
			width           : 60,
			ref_key         : 'format'
		}, {
			name            : 'result',
			type            : 'int',
			defaultValue    : 0,
			title           : '结果',
			width           : 60,
			ref_key         : 'test_result'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 500
		}]
};

var test_set = register_grid_panel(panel_def_test);
