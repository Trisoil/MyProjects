//pplication.js

var confirm_t = [
	[0, '<font color=brown>������</font>'],
	[1, '<font color=green>����׼</font>'],
	[2, '<font color=red>δͨ��</font>']
];

register_enum('confirm_type','����״̬',confirm_t);

var panel_def_application = {
		name            : 'application',
		title           : '����',
	    menu_path       : 'equipment_man',
		module_url      : 'module/application.php',
		outer_key       : 'id',
		disp_field      : 'status',
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
			width           : 60,
			ref_key         : 'equipment'
		}, {
			name            : 'applicant',
			type            : 'int',
			defaultValue    : 0,
			title           : '������',
			width           : 60,
			ref_key         : 'engineer'
		}, {
			name            : 'application_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '����ʱ��',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
//			initable        : false
		}, {
			name            : 'confirm_status',
			type            : 'int',
			defaultValue    : 0,
			title           : '����״̬',
			width           : 60,
			ref_key         : 'confirm_type'
		}, {	
			name            : 'confirm_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '����ʱ��',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield'
	//		initable        : false
		}, {
			name            : 'status',
			type            : 'string',
			defaultValue    : '',
			title           : '��ע',
			width           : 200
		}]
};

var application_set = register_grid_panel(panel_def_application);  
