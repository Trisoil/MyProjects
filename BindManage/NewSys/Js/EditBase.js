
Ext.ns('PPLive','PPLive.PPInstall');

PPLive.PPInstall.EditBase=function(){   
    var btnid;
    return{
        cookiestate:1,    //是否设置cookie为stateprovider 0为不设置 1为设置
        pageSize:30,
        domainName:"http://192.168.26.114/subext/",
        intRegex:/^[1-9]\d*$/,
        intRegexText:'该输入项必须为正整型',
        zeroRegex:/^[1-9]\d*$|0/,
        zeroRegexText:'该输入项必须为正整型且包括0',
        wordRegex:/^([a-z]|[A-Z]|\s)*$/,
        wordRegexText:'该输入项只能包含单词字母与空格',
        wordnumregex: /^(\w|-|\s)*$/,
        wordnumregexText:'该输入项只能包含字母数字减号空格与下划线',
        setStateProvider:function(){
            if(this.cookiestate==1)
                Ext.state.Manager.setProvider(new Ext.state.CookieProvider());
        },
        callbackError:function(result){
            var btn = Ext.getCmp(btnid);
            if(btn)
                btn.enable();
            var msg = result.get_message();
            var reg = /.*Castle.ActiveRecord.ActiveRecordBase.Delete.*/g;
            if(result._exceptionType=='Castle.ActiveRecord.Framework.ActiveRecordException'&&reg.test(result._stackTrace))
                msg='该项有子项未删除，请删除其他页面中的子项后删除该项！';
            Ext.MessageBox.show({title: '错误',msg: msg,buttons: Ext.MessageBox.OK,icon:Ext.MessageBox.ERROR});
        },
        pageEntry:function(fn){
            if(!fn)return;
            document.onkeydown=function(ev){
                var e = ev ? ev :(window.event ? window.event : null); 
                if(e.keyCode==13)
                {
                    fn();
                }
            } 
        },
        copyBoard:function(copy){
            if(copy)
            {
	            if (window.clipboardData) {
		            window.clipboardData.setData("Text", copy);
	            } else if (window.netscape) {
                    try {
                        netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
                    } catch (e) {
                        alert("你使用的FF浏览器,复制功能被浏览器拒绝！\n请在浏览器地址栏输入'about:config'并回车\n然后将 'signed.applets.codebase_principal_support'设置为'true'");
                    }
                    var clip = Components.classes['@mozilla.org/widget/clipboard;1'].createInstance(Components.interfaces.nsIClipboard);
		            if (!clip) return;
		            var trans = Components.classes['@mozilla.org/widget/transferable;1'].createInstance(Components.interfaces.nsITransferable);
		            if (!trans) return;
		            trans.addDataFlavor('text/unicode');
		            var str = new Object();
		            var len = new Object();
		            var str = Components.classes["@mozilla.org/supports-string;1"].createInstance(Components.interfaces.nsISupportsString);
		            str.data = copy;
		            trans.setTransferData("text/unicode",str,copy.length*2);
		            var clipid = Components.interfaces.nsIClipboard;
		            if (!clip) return false;
		            clip.setData(trans,null,clipid.kGlobalClipboard);
	            }
	            return true;
	        }
	        else
	        {
	            return false;
	        }
        },
        showTip:function(value, meta, rec, rowIdx, colIdx, ds){
            return '<div ext:qtitle="" ext:qtip="' + value + '">'+ value +'</div>';
        },
        showBoolValue:function(value, meta, rec, rowIdx, colIdx, ds){
            if(value)
            {
                return '是';
            }
            return "<font style='color:red' >否</font>";
        },
        getRecord:function(grid,r){
            var record;
            if(typeof r=='number')
                record=grid.store.data.items[r];
            else
            {
                var records=grid.selModel.selections.items;
                if(records.length!=1)
                {
                    Ext.MessageBox.alert('提示','只能选择一条记录进行操作！');
                    return;
                } 
                record=records[0];
            }
            return record;
        },
        getReocrds:function(grid,r){
            var records=[];
            if(typeof r=='number')
                records.push(grid.store.data.items[r]);
            else
            {
                records=grid.selModel.selections.items;
                if(records.length==0)
                {
                    Ext.MessageBox.alert('提示','请至少选择一条记录进行操作！');
                    return;
                }
            }
            return records;
        },
        getKeys:function(grid,r){
            var keys=[];
            if(typeof r=='number')
                keys.push(grid.store.data.items[r].id);
            else
            {
                keys=grid.selModel.selections.keys;
                if(keys.length==0)
                {
                    Ext.MessageBox.alert('提示','请至少选择一条记录进行操作！');
                    return;
                }
            }
            return keys;
        },
        getParameterByName:function(url,name){
            var allpars = url.split("?")[1];
            if(allpars)
            {
                var args = allpars.split("&");
                for(var i=0; i<args.length; i++)
                {
                    var arg = args[i].split("=");
                    if(arg[0]==name)
                        return arg[1];
                }
            }
            return null;
        },
        btnDisable:function(id){
            btnid=id;
            var btn = Ext.getCmp(btnid);
            if(btn)
                btn.disable();
        }
    }
}();

