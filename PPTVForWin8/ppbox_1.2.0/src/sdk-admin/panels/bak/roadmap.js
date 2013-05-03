//roadmap.js

var panel_def_roadmap = {
		name            : 'roadmap',
		title           : '路标',
	    menu_path       : 'info',
		module_url      : 'module/roadmap.php',
		outer_key       : 'project_id',
		disp_field      : 'name',
		attrs           : [{
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目编号',
			width           : 60,
			ref_key         : 't_project'
		}, {
			name            : 'stage',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目所处阶段',
			width           : 60
		}, {
			name            : 'plan_timeline',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '阶段计划完成时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'actual_timeline',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '阶段实际完成时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '路标描述',
			width           : 400
		}]
};

var roadmap_set = register_grid_panel(panel_def_roadmap);  
