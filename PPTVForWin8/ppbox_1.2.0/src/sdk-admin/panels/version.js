//version.js

var format_hylink = function (v) {
    return '<a href="/packet/sdk/' + v + '" target="_blank">' + v + '</a>';
};

var panel_def_version = {
		name            : 'version',
		title           : '������',
		menu_path       : 'project_man',
		module_url      : 'module/version.php',
		outer_key       : 'id',
		disp_field      : 'name',
		sortInfo        : {field: 'publish_time', direction: 'DESC'},
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '���',
			initable        : false,
			editable        : false,
			width           : 40
		}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ŀ',
			width           : 100,
			ref_key         : 'project'
		}, {
			name            : 'branch_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��֧',
			width           : 40,
			ref_key         : 'branch'
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '��Ա',
			width           : 80,
			ref_key         : 'user',
			initable        : false
		}, {
			name            : 'publish_time',
			type            : 'date',
			defaultValue    : new Date(),
			title           : '����ʱ��',
			width           : 80,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '�ļ���',
			renderer        : format_hylink,
			width           : 320
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '����',
			width           : 500
		}]
};

var version_set = register_grid_panel(panel_def_version);
