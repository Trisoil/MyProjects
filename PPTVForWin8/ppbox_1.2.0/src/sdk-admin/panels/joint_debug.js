//joint_debug.js

var panel_def_joint_debug = {
    name            : 'joint_debug',
	title           : '��������',
	menu_path       : 'project_man',
	module_url      : 'module/joint_debug.php',
	outer_key       : 'id',
	disp_field      : 'description',
	attrs           : [{
		name            : 'id',
		type			: 'int',
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
		ref_key         : 'problem',
		initable        : false,
		editable        : false
	}, {
		name            : 'engineer_id',
		type            : 'int',
		defaultValue    : 0,
		title           : '�ύ��Ա',
		width           : 60,
		ref_key         : 'engineer'
	}, {
		name            : 'description',
		type            : 'string',
		defaultValue    : '',
		title           : '��������',
		width           : 800
	}]
};
var joint_debug_set = register_grid_panel(panel_def_joint_debug);
