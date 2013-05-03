var curGroupObj=null;
var curSubObj=null;
var curListValue=new Array();

function init_commsel(dataname,groupid,subid,bremainfirst,prefix,thirdlistid,thirdid){
    dataname=dataname||'';
    prefix=prefix!=null?prefix:'';
    curListValue["GID_"+dataname]=groupid;
    curListValue["SUBID_"+dataname]=subid;
    curListValue["SUBTEXT_"+dataname]='';
    curListValue["THIRDID_"+dataname]=thirdid;
    var objgroupid=prefix+dataname+"gid";
    var objsubid=prefix+dataname+"id";
    if(dataname=='city'){
        objgroupid=prefix+dataname+"id";
        objsubid=prefix+'districtid';
    }else if(dataname=='diseasesys'){
        objgroupid=prefix+dataname+"id";
        objsubid=prefix+'diseaseid';
    }else if(dataname=='area'){
        objgroupid=prefix+"provinceid";
        objsubid=prefix+"areaid";
    }
    var objgroup=document.getElementById(objgroupid);
    if(objgroup!=null){
        objgroup.setAttribute("dataname",dataname);
        objgroup.setAttribute("thirdlistid",thirdlistid);
    }
    reloadSubList(objgroup,objsubid,bremainfirst,false);
}

function reloadSubList(obj,subListID,bRemainFirst,bFocus){
    //clear sublist
    curGroupObj=obj;
    var subList=document.getElementById(subListID);
    if(subList==null) return;
    for(var i=subList.options.length-1;i>=(bRemainFirst?1:0);--i) subList.remove(i);
    curSubObj=subList;
    subList.setAttribute("dataname",obj.getAttribute("dataname"));
    var thirdList=document.getElementById(obj.getAttribute("thirdlistid")||'');
    if(thirdList!=null) for(i=thirdList.options.length-1;i>=(bRemainFirst?1:0);--i) thirdList.remove(i);
    if(obj.value.length>=0){
        var loadSubListAjax = new LoadSubListAjax(obj.value,obj.getAttribute("thirdlistid"));
        loadSubListAjax.execute();
    }
}

var LoadSubListAjax = Class.create();
LoadSubListAjax.prototype = {
    initialize: function(_gid, _thirdlistid, _method) {
        this.gid=_gid||'';
        this.thirdlistid=_thirdlistid;
        this.method = _method == null ? "get" : _method;
    },
    execute: function() {
        if(curGroupObj==null|| curSubObj==null) return;
        this.dataname=curGroupObj.getAttribute("dataname")||'';
        var url = "XML_List_Catalog.jsp";
        if(this.dataname=='position') url="XML_List_Position.jsp";
        if(this.dataname=='city') url="XML_List_District.jsp";
        if(this.dataname=='area') url="XML_List_Area.jsp";
        if(this.dataname=='diseasesys') url="XML_List_DiseaseBySystem.jsp";
        url=""+url;
        var pars = "groupid=" + this.gid+
                   "&t=" + getRandomNum();
        //alert(url+"?"+pars);
        var _ajax = new Ajax.Request(url, {method: this.method, parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var options = res.responseXML.getElementsByTagName("option");
        for(var i=0;i<options.length;i++){
            var o=options[i];
            var _optionValue=o.getAttribute("value");
            var _optionText=o.getAttribute("text");
            var oOption = document.createElement('OPTION');
            oOption.value = _optionValue;
            oOption.text = _optionText;
            if(curListValue['SUBID_'+this.dataname]==_optionValue
                    || (curListValue['SUBTEXT_'+this.dataname]||'').indexOf(_optionText)>=0){
                oOption.selected=true;
                oOption.className="input_list_01sel";
            }
            curSubObj.options.add(oOption);
        }
        curSubObj.disabled=options.length<=0;
        /*第三个列表*/
        if(this.thirdlistid!=null){
            curSubObj.setAttribute("dataname",this.dataname);
            var loadThirdListAjax = new LoadThirdListAjax(this.dataname,curSubObj.value,this.thirdlistid);
            loadThirdListAjax.execute();
        }
    }
};


function reloadThirdList(obj,subListID,bRemainFirst,bFocus){
    //clear sublist
    var subList=document.getElementById(subListID);
    if(subList==null) return;
    for(var i=subList.options.length-1;i>=(bRemainFirst?1:0);--i) subList.remove(i);
    if(obj.value.length>=0){
        var loadThirdListAjax = new LoadThirdListAjax(obj.getAttribute("dataname")||'',
                obj.value,subListID);
        loadThirdListAjax.execute();
    }
}

/*------------load disease by postion-----------*/
var LoadThirdListAjax = Class.create();
LoadThirdListAjax.prototype = {
    initialize: function(_dataname, _gid,_thirdlistid,_method) {
        this.dataname=_dataname;
        this.gid=_gid||'';
        this.thirdlistid=_thirdlistid;
        this.method = _method == null ? "get" : _method;
    },
    execute: function() {
        var url = "XML_List_DiseaseByPosition.jsp";
        var pars = "groupid=" + this.gid+
                   "&t=" + getRandomNum();
        var _ajax = new Ajax.Request(url, {method: this.method, parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var objthirdlist=document.getElementById(this.thirdlistid);
        if(objthirdlist==null) return;
        var curthirdid=curListValue['THIRDID_'+this.dataname]|'';
        var options = res.responseXML.getElementsByTagName("option");
        for(var i=0;i<options.length;i++){
            var o=options[i];
            var _optionValue=o.getAttribute("value");
            var _optionText=o.getAttribute("text");
            var oOption = document.createElement('OPTION');
            oOption.value = _optionValue;
            oOption.text = _optionText;
            if(curthirdid==_optionValue){
                oOption.selected=true;
                oOption.className="input_list_01sel";
            }
            objthirdlist.options.add(oOption);
        }
        objthirdlist.disabled=options.length<=0;
    }
};