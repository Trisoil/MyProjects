//version.js

var panel_def_version = {
		name            : 'version',
		title           : '版本',
		menu_path       : 'access',
		module_url      : 'module/version.php',
		outer_key       : ['id','name','project_id','engineer_id'],
		disp_field      : 'name',
		editable        : false,
		deletable       : false,
		attrs           : [{
			name            : 'id',
			type            : 'int',
			defaultValue    : 0,
			title           : '版本编号',
			editable        : false,
			width           : 60
		}, {
			name            : 'name',
			type            : 'string',
			defaultValue    : '',
			title           : '版本名称',
			width           : 200
		}, {
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关项目编号',
			width           : 60,
			ref_key         : 't_project'
		}, {
			name            : 'pubulish_time',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '版本发布时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'engineer_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '相关人员编号',
			width           : 60,
			ref_key         : 't_engineer'
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '版本描述',
			width           : 500
		}]
};

var version_set = register_grid_panel(panel_def_version);
