//login.js

var disable_components = function () {
    login_form.form.findField('name').disable();
    login_form.form.findField('pass').disable('');
    login_form.form.findField('pass_new').disable();
    login_form.form.findField('pass_confirm').disable();
    login_form.buttons[0].disable();
    login_form.buttons[1].disable();
    login_form.buttons[2].disable();
};

var enable_login_components = function () {
    login_form.form.findField('name').enable();
    login_form.form.findField('pass').enable('');
    login_form.buttons[0].enable();
};

var enable_logout_components = function () {
    login_form.form.findField('pass').enable('');
    login_form.form.findField('pass_new').enable();
    login_form.form.findField('pass_confirm').enable();
    login_form.buttons[1].enable();
    login_form.buttons[2].enable();
};

var submit_failure = function (action) {
    if (typeof action.result != 'undefined')
        Ext.MessageBox.alert('����', action.result.message);
    else
        Ext.MessageBox.alert('�쳣', 'status :\t' + action.response.status + '\ndesc :\t' + action.response.statusText);
};

var login = function () {
    disable_components();
    
    login_form.form.submit({
        url         : 'module/session.php', 
        params      : {
            o           : 'login', 
            name        : login_form.form.findField('name').getValue(), 
            pass        : login_form.form.findField('pass').getValue()
        }, 
        success    : function (form, action) {
            for (var i = 0; i < init_calls.length; i++) {
                 init_calls[i][0].apply(init_calls[i][1]);
            };
            enable_logout_components();
            Ext.getCmp('main-panel').layout.setActiveItem('op-panel');
            Ext.getCmp('content-panel').layout.setActiveItem('blank-panel');
            //Ext.getCmp('tree-panel').selectPath('/root/info/movie');
        }, 
        failure     : function (form, action) {
            enable_login_components();
            submit_failure(action);
        }
    });
    login_form.form.findField('pass').setValue('')
};

var logout = function () {
    disable_components();
    
    login_form.form.submit({
        url         : 'module/session.php', 
        params      : {o : 'logout'}, 
        success     : function (form, action) {
            for (var i = 0; i < term_calls.length; i++) {
                 term_calls[i][0].apply(term_calls[i][1]);
            };
            enable_login_components();
        }, 
        failure     : function (form, action) {
            submit_failure(action);
            enable_login_components();
        }
    });
    login_form.form.findField('name').setValue('')
    login_form.form.findField('pass').setValue('')
};

var modify_pass = function () {
    if (login_form.form.findField('pass_new').getValue() != login_form.form.findField('pass_new').getValue()) {
        Ext.MessageBox.alert('����', '��������ȷ�����벻һ�£�');
        return;
    }
    disable_components();
    
    login_form.form.submit({
        url         : 'module/session.php', 
        params      : {
            o           : 'modify_pass', 
            pass        : login_form.form.findField('pass').getValue(), 
            pass_new    : login_form.form.findField('pass_new').getValue()
        }, 
        success     : function (form, action) {
            Ext.MessageBox.alert('��Ϣ', '�޸�����ɹ�');
            enable_logout_components();
        }, 
        failure     : function (form, action) {
            submit_failure(action);
            enable_logout_components();
        }
    });
    login_form.form.findField('pass').setValue('')
    login_form.form.findField('pass_new').setValue('')
    login_form.form.findField('pass_confirm').setValue('')
};

var message_panel = new Ext.Panel({
    html        : 'Welcome, guy!', 
    border      : false,
    align       : 'north',
    height      : 260
});
    
var login_form = new Ext.form.FormPanel({
    id          : 'login-form',
    labelWidth  : 75,
    bodyStyle   : {
        padding     : '15px'
    }, 
    labelPad    : 20,
    layoutConfig: {
        labelSeparator: ''
    },
    border      : false,
    width       : 350,
    height      : 160,
    autoHeight  : true,
    items       : [{
        xtype           : 'fieldset',
        title           : '��¼',
        width           : 300,
        collapsible     : false,
        autoHeight      : true,
        defaults        : {width: 120},
        defaultType     : 'textfield',
        items           : [{
            fieldLabel      : '�û���',
            name            : 'name',
            allowBlank      : false, 
            enableKeyEvents : true, 
            listeners       : {
                keypress        : function (t, e) {
                    if (e.getKey() == 13) {
                        login();
                    }
                }
            }
        }, {
            fieldLabel      : '����',
            name            : 'pass',
            inputType       : 'password', 
            enableKeyEvents : true, 
            listeners       : {
                keypress        : function (t, e) {
                    if (e.getKey() == 13) {
                        login();
                    }
                }
            }
        }]
    }, {
        xtype           : 'fieldset',
        title           : '�޸�����',
        width           : 300,
        collapsible     : true,
        collapsed       : true,
        autoHeight      : true,
        defaults        : {width: 120},
        defaultType     : 'textfield',
        items           : [{
            fieldLabel      : '������',
            name            : 'pass_new',
            inputType       : 'password', 
            disabled        : true, 
            enableKeyEvents : true, 
            listeners       : {
                keypress        : function (t, e) {
                    if (e.getKey() == 13) {
                        modify_pass();
                    }
                }
            }
        }, {
            fieldLabel      : 'ȷ��������',
            name            : 'pass_confirm',
            inputType       : 'password', 
            disabled        : true, 
            enableKeyEvents : true, 
            validator       : function (v) {
                return v == login_form.form.findField('pass_new').getValue() || '��������ȷ�����벻һ�£�';
            }, 
            listeners       : {
                keypress        : function (t, e) {
                    if (e.getKey() == 13) {
                        modify_pass();
                    }
                }
            }
        }]
    }], 
    buttons     : [{
        text        : '��¼', 
        handler     : login
    }, {
        text        : 'ע��', 
        disabled    : true, 
        handler     : logout
    }, {
        text        : '�޸�����', 
        disabled    : true, 
        handler     : modify_pass
    }]
});

var loginPanel = {
    id      : 'login-panel',
    layout  : 'center', 
    items   : [login_form]
    //items   : [{
    //    id     : 'login'
    //    layout: 'column',
    //    items: [message_panel, login_form]
    //}]
};
