//roadmap.js

var panel_def_roadmap = {
		name            : 'roadmap',
		title           : '日程',
	    menu_path       : 'project_man',
		module_url      : 'module/roadmap.php',
		outer_key       : ['project_id', 'stage'],
		disp_field      : 'plan_timeline',
		attrs           : [{
			name            : 'project_id',
			type            : 'int',
			defaultValue    : 0,
			title           : '项目',
			width           : 60,
//			initable        : false,
//			editable        : false,
			ref_key         : 'project'
		}, {
			name            : 'stage',
			type            : 'int',
			defaultValue    : 0,
			title           : '阶段',
			width           : 60
		}, {
			name            : 'content',
			type            : 'string',
	        defaultValue    : '',
			title           : '工作内容',
			width           : 200
		}, {
			name            : 'plan_timeline',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '计划完成时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'actual_timeline',
			type            : 'date',
			defaultValue    : '000-00-00 00:00:00',
			title           : '实际完成时间',
			width           : 120,
			renderer        : format_date,
			xtype           : 'datefield',
			initable        : false
		}, {
			name            : 'description',
			type            : 'string',
			defaultValue    : '',
			title           : '描述',
			width           : 400
		}]
};

var roadmap_set = register_grid_panel(panel_def_roadmap);  
