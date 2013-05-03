var _commseldivHTML = "<div id=\"commSelDiv\" class=\"comm_sel_div\">" +
                      "   <div id=\"commSelDivTitle\" class=\"comm_sel_div_title\">" +
                      "       <a href=\"javascript:void(0);\" onClick=\"hideCommSelDiv();\">关闭</a>" +
                      "   </div>" +
                      "   <div id=\"commSelDivSearch\" class=\"comm_sel_div_search\"></div>" +
                      "   <div id=\"commSelDivSearchHint\" class=\"comm_sel_div_search_hint\">"+
                      "     提示:可输入首字母拼音查找,例:输入mx查找\"明星\""+
                      "   </div>" +
                      "   <div id=\"commSelDivContentZone\" class=\"comm_sel_div_contentzone\"><div id=\"commSelDivContent\" class=\"comm_sel_div_content\"></div></div>" +
                      "</div>";
var _commseldivsearchHTML = "输入关键字: <input name=\"commsel_key\" type=\"text\" id=\"commsel_key\" " +
                            "size=\"15\" maxlength=\"20\" " +
                            "onkeyup=\"doCommSelSearchRealTime();\"> " +
                            "<input name=\"submit_cs\" type=\"button\" class=\"BUTTON4\" value=\"搜索\" " +
                            "onClick=\"doCommSelSearch();\">";
var _commseldivCoverHTML = "<div id=\"commSelDivCover\" onclick=\"hideCommSelDiv();\"></div>";
var _Const_DivWidth = 0;
var _Const_DivHeight = 0;
var _Const_ObjectType = 0;
var _Const_ObjectID=0;
var _Cur_DataType = "";
var _Cur_E_Target=null;
var _Const_Sepechar="|";
document.write(_commseldivHTML);
document.write(_commseldivCoverHTML);


var commSelDiv,commSelDivTitle,commSelDivSearch,commSelDivContentZone,commSelDivContent,commSelDivCover;
var commSelKey;
function initCommSel(_objecttype,_objectid,_sepechar) {
    _Const_ObjectType = _objecttype;
    if(_objectid!=null) _Const_ObjectID=_objectid;
    if(_sepechar!=null) _Const_Sepechar=_sepechar;
    commSelDiv = document.getElementById("commSelDiv");
    commSelDivTitle = document.getElementById("commSelDivTitle");
    commSelDivSearch = document.getElementById("commSelDivSearch");
    commSelDivContentZone = document.getElementById("commSelDivContentZone");
    commSelDivContent = document.getElementById("commSelDivContent");
    commSelDivCover = document.getElementById("commSelDivCover");
    _Const_DivWidth = _isIE ? commSelDiv.clientWidth : commSelDiv.clientWidth;
    _Const_DivHeight = _isIE ? commSelDiv.clientHeight : commSelDiv.clientHeight;
    commSelDivCover.className = _isIE ? "comm_sel_div_cover" : "comm_sel_div_cover_ff";
    commSelDivCover.style.height=document.body.scrollHeight;
    commSelDivSearch.innerHTML = _commseldivsearchHTML;
    commSelKey = document.getElementById("commsel_key");
}

function _showDataDiv(e,_datatype,pos,idcount){
    _Cur_E_Target=e;
    commSelKey.setAttribute("from","commdata");
    commSelKey.setAttribute("pos",pos==null?"":pos);
    if(idcount!=null&&idcount>0) commSelKey.setAttribute("idcount",idcount);
    else commSelKey.removeAttribute("idcount");
    showCommSelDiv(e, _datatype);
}
function showCommSelDiv(e, _datatype) {
    _Cur_DataType = _datatype;
    var e_divw = 0,e_divh = 0;
    try {
        e_divw = parseInt(e.getAttribute("divw"));
        e_divh = parseInt(e.getAttribute("divh"));
    } catch(e) {
    }
    commSelDivContent.innerHTML="";
    showCommSelDivCover(e);
    var t = e.offsetTop,  h = e.clientHeight, w = e.clientWidth, l = e.offsetLeft, p = e.type;
    while(e = e.offsetParent) {
        t += e.offsetTop;
        l += e.offsetLeft;
    }
    commSelDiv.style.width = e_divw > 0 ? e_divw : _Const_DivWidth;
    commSelDiv.style.height = e_divh > 0 ? e_divh : _Const_DivHeight;
    commSelDiv.style.zIndex = 100;
    commSelDiv.style.visibility = "visible";
    var pos=commSelKey.getAttribute("pos");
    if(pos==null||pos==''){
        commSelDiv.style.left = l;
        commSelDiv.style.top = t + h + 4;
    }else if(pos=="right"){
        commSelDiv.style.left = l+w+4;
        commSelDiv.style.top = t;
    }
    commSelDivContentZone.style.height = (_isIE ? commSelDiv.style.pixelHeight : commSelDiv.clientHeight) - 45;
    loadInitData(_datatype);
}
function resetCommSelDivPos(){
    if(_Cur_E_Target==null) return;
    if(commSelDiv.style.visibility=="hidden") return;
    var e=_Cur_E_Target;
    var t = e.offsetTop,  h = e.clientHeight, w = e.clientWidth, l = e.offsetLeft, p = e.type;
    while(e = e.offsetParent) {
        t += e.offsetTop;
        l += e.offsetLeft;
    }
    var pos=commSelKey.getAttribute("pos");
    if(pos==null){
        commSelDiv.style.left = l;
        commSelDiv.style.top = t + h + 4;
    }else if(pos=="right"){
        commSelDiv.style.left = l+w+4;
        commSelDiv.style.top = t;
    }
}


/*------------------ People Select -----------------------*/

function _showPeopleDiv(e,_itemid,_quickadd){
    _Cur_E_Target=e;
    commSelKey.setAttribute("from","people");
    commSelKey.setAttribute("itemid",_itemid);
    if(_quickadd!=null) commSelKey.setAttribute("quickadd",_quickadd);
    else commSelKey.removeAttribute("quickadd");
    showPeopleSelDiv(e,_itemid);
}
function showPeopleSelDiv(e, _itemid) {
    var e_divw = 0,e_divh = 0;
    try {
        e_divw = parseInt(e.getAttribute("divw"));
        e_divh = parseInt(e.getAttribute("divh"));
    } catch(e) {
    }
    commSelDivContent.innerHTML="";
    showCommSelDivCover(e);
    var t = e.offsetTop,  h = e.clientHeight, w = e.clientWidth, l = e.offsetLeft, p = e.type;
    while(e = e.offsetParent) {
        t += e.offsetTop;
        l += e.offsetLeft;
    }
    commSelDiv.style.width = e_divw > 0 ? e_divw : _Const_DivWidth;
    commSelDiv.style.height = e_divh > 0 ? e_divh : _Const_DivHeight;
    commSelDiv.style.zIndex = 100;
    commSelDiv.style.visibility = "visible";
    commSelDiv.style.left = l+w+4;
    commSelDiv.style.top = t;
    commSelDivContentZone.style.height = (_isIE ? commSelDiv.style.pixelHeight : commSelDiv.clientHeight) - 45;
    loadInitPeopleData(_itemid);
}
function resetPeopleSelDivPos(){
    if(_Cur_E_Target==null) return;
    if(commSelDiv.style.visibility=="hidden") return;
    var e=_Cur_E_Target;
    var t = e.offsetTop,  h = e.clientHeight, w = e.clientWidth, l = e.offsetLeft, p = e.type;
    while(e = e.offsetParent) {
        t += e.offsetTop;
        l += e.offsetLeft;
    }
    commSelDiv.style.left = l+w+4;
    commSelDiv.style.top = t;
}

/*------------------ People Select End -----------------------*/


function _clearPeopleIds(itemid){
    var obj=document.getElementById("people_ids_"+itemid);
    if(obj!=null) obj.value='';
    var objname=document.getElementById("people_names_"+itemid);
    if(objname!=null) objname.innerHTML='';
}


function showCommSelDivCover(e) {
    document.body.style.width = "100%";
    document.body.style.height = "100%";
    //document.body.style.overflow = "hidden";
    commSelDivCover.style.visibility = "visible";
    if(commSelKey != null) commSelKey.value = "";
}

function hideCommSelDiv() {
    _Cur_DataType = "";
    document.body.style.overflow = "auto";
    commSelDivCover.style.visibility = "hidden";
    commSelDiv.style.visibility = "hidden";
}

function clearCommSelDatas(_datatype) {
    var _objids = document.getElementById("comm_ids_" + _datatype);
    var _objtexts = document.getElementById("comm_div_" + _datatype);
    if(_objids != null && _objtexts != null) {
        _objids.value = "";
        _objtexts.innerHTML = "";
    }
}

function loadInitData(_datatype) {
    var loadInitDataAjax = new LoadInitDataAjax(_datatype);
    loadInitDataAjax.execute();
}
var LoadInitDataAjax = Class.create();
LoadInitDataAjax.prototype = {
    initialize: function(_datatype, _params, _method) {
        this.datatype = _datatype;
        this.params = _params;
        this.method = _method == null ? "get" : _method;
    },
    execute: function() {
        var _objids = document.getElementById("comm_ids_" + this.datatype);
        var _objidsstr=_objids!=null?_objids.value:"";
        if(this.datatype=='tag'){
            var _objidssys=document.getElementById("comm_ids_"+this.datatype+"_sys");
            if(_objidssys.value!='') _objidsstr=_objidssys.value+","+_objidsstr;
        }
        var url = "XML_CommDataPicker.jsp";
        if(this.datatype=='tag') url="XML_TagPicker.jsp";
        var pars = "objecttype=" + _Const_ObjectType + "&datatype=" + this.datatype +
                   (this.params != null ? "&" + this.params : "") +
                   "&exist_dataids=" + _objidsstr +
                   "&t=" + getRandomNum();
                   //体育公用电影的数据
        if((this.datatype=="language"||this.datatype=="color")&&_Const_ObjectType==9){
            pars = "objecttype=0"  + "&datatype=" + this.datatype +
                   (this.params != null ? "&" + this.params : "") +
                   "&exist_dataids=" + _objidsstr +
                   "&t=" + getRandomNum();
        }
        //资讯的语言公用电影数据
        if((this.datatype=="language")&&_Const_ObjectType==10){
            pars = "objecttype=0"  + "&datatype=" + this.datatype +
                   (this.params != null ? "&" + this.params : "") +
                   "&exist_dataids=" + _objidsstr +
                   "&t=" + getRandomNum();
        }
        //
        var _ajax = new Ajax.Request(url, {method: this.method, parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var resText = res.responseText;
        commSelDivContent.innerHTML = resText;
    }
};


/*------------------ People Select -----------------------*/

function loadInitPeopleData(_itemid) {
    var loadInitPeopleDataAjax = new LoadInitPeopleDataAjax(_itemid);
    loadInitPeopleDataAjax.execute();
}
var LoadInitPeopleDataAjax = Class.create();
LoadInitPeopleDataAjax.prototype = {
    initialize: function(_itemid, _params, _method) {
        this.itemid = _itemid;
        this.params = _params;
        this.method = _method == null ? "get" : _method;
    },
    execute: function() {
        this.quickadd=commSelKey.getAttribute("quickadd");
        var _objids = document.getElementById("people_ids_" + this.itemid);
        var url = "XML_PeoplePicker.jsp";
        var pars = "objecttype=" + _Const_ObjectType + "&objectid=" + _Const_ObjectID +
                   "&itemid="+this.itemid+
                   (this.quickadd!=null&&this.quickadd?"&quickadd=1":"")+
                   (this.params != null ? "&" + this.params : "") +
                   (_objids != null ? "&exist_dataids=" + _objids.value : "") +
                   "&t=" + getRandomNum();
        //alert(pars);
        var _ajax = new Ajax.Request(url, {method: this.method, parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var resText = res.responseText;
        commSelDivContent.innerHTML = resText;
    }
};
/*------------------ People Select End -----------------------*/



function clickCommData(_did, _dtext, _datatype, _sepechar) {
    var _objids = document.getElementById("comm_ids_" + _datatype);
    var _objtexts = document.getElementById("comm_div_" + _datatype);
    if(_objids != null && _objtexts != null) {
        var _idcount=commSelKey.getAttribute("idcount");
        var _arrids=_objids.value.split(",");
        if(_idcount!=null&&_arrids!=null&&_objids.value!=''&&_arrids.length>=_idcount){
            alert("最多只能选择"+_idcount+"个,已达到最大限制.");
            hideCommSelDiv();
            return;
        }
        if(existInArrayStr(_objids.value, _did)) return;
        _objids.value = addToArrayStr(_objids.value, _did);
        _objtexts.innerHTML = addToArrayStr(_objtexts.innerHTML, _dtext, _sepechar!=null?_sepechar:_Const_Sepechar);
        _arrids=_objids.value.split(",");
        var _objxmlspan = document.getElementById("comm_sel_div_xml_span_" + _did);
        if(_objxmlspan != null) {
            _objxmlspan.innerHTML = _isIE ? _objxmlspan.innerText : _objxmlspan.textContent;
            resetCommSelDivPos();
            if(_idcount!=null&&_arrids!=null&&_objids.value!=''&&_arrids.length>=_idcount){
                hideCommSelDiv();
            }
        }
    }
}

/*-------------search------------------*/

function doCommSelSearchRealTime() {
    doCommSelSearch(true);
}

function doCommSelSearch(bRealTime) {
    var _from=commSelKey.getAttribute("from");
    if(commSelKey.value == '') {
        if(_from == "people") {
            loadInitPeopleData(commSelKey.getAttribute("itemid"));
        } else if(_from=="company"){
            loadInitObjectData(commSelKey.getAttribute("itemid"));
        } else{
            loadInitData(_Cur_DataType);
        }
        return;
    }
    var params = "key=" + commSelKey.value;
    if(bRealTime) params+="&rt=1";
    if(_from=="commdata"){
        var loadInitDataAjax = new LoadInitDataAjax(_Cur_DataType, params, "post");
        loadInitDataAjax.execute();
    }else if(_from=="people"){
        var loadInitPeopleDataAjax = new LoadInitPeopleDataAjax(commSelKey.getAttribute("itemid"),params,"post");
        loadInitPeopleDataAjax.execute();
    }else if(_from=="company"){
        var loadInitObjectDataAjax = new LoadInitObjectDataAjax(commSelKey.getAttribute("itemid"),params,"post");
        loadInitObjectDataAjax.execute();
    }
}




/*
* ********************************Object Select***********************************
* *****************************
* *****************************Company
* *****************************Website
* *****************************
* ********************************************************************************
* */

function _showObjectDiv(e,_itemid,_from,_quickadd){
    _Cur_E_Target=e;
    commSelKey.setAttribute("from",_from);
    commSelKey.setAttribute("itemid",_itemid);
    if(_quickadd!=null) commSelKey.setAttribute("quickadd",_quickadd);
    else commSelKey.removeAttribute("quickadd");
    showObjectSelDiv(e,_itemid,_from);
}
function showObjectSelDiv(e, _itemid,_from) {
    var e_divw = 0,e_divh = 0;
    try {
        e_divw = parseInt(e.getAttribute("divw"));
        e_divh = parseInt(e.getAttribute("divh"));
    } catch(e) {
    }
    commSelDivContent.innerHTML="";
    showCommSelDivCover(e);
    var t = e.offsetTop,  h = e.clientHeight, w = e.clientWidth, l = e.offsetLeft, p = e.type;
    while(e = e.offsetParent) {
        t += e.offsetTop;
        l += e.offsetLeft;
    }
    commSelDiv.style.width = e_divw > 0 ? e_divw : _Const_DivWidth;
    commSelDiv.style.height = e_divh > 0 ? e_divh : _Const_DivHeight;
    commSelDiv.style.zIndex = 100;
    commSelDiv.style.visibility = "visible";
    commSelDiv.style.left = l+w+4;
    commSelDiv.style.top = t;
    commSelDivContentZone.style.height = (_isIE ? commSelDiv.style.pixelHeight : commSelDiv.clientHeight) - 45;
    loadInitObjectData(_itemid);
}
function resetObjectSelDivPos(){
    if(_Cur_E_Target==null) return;
    if(commSelDiv.style.visibility=="hidden") return;
    var e=_Cur_E_Target;
    var t = e.offsetTop,  h = e.clientHeight, w = e.clientWidth, l = e.offsetLeft, p = e.type;
    while(e = e.offsetParent) {
        t += e.offsetTop;
        l += e.offsetLeft;
    }
    commSelDiv.style.left = l+w+4;
    commSelDiv.style.top = t;
}

/*------------------ Object Select End -----------------------*/


/*------------------ Object Select -----------------------*/

function loadInitObjectData(_itemid) {
    var loadInitObjectDataAjax = new LoadInitObjectDataAjax(_itemid);
    loadInitObjectDataAjax.execute();
}
var LoadInitObjectDataAjax = Class.create();
LoadInitObjectDataAjax.prototype = {
    initialize: function(_itemid, _params, _method) {
        this.itemid = _itemid;
        this.params = _params;
        this.method = _method == null ? "get" : _method;
    },
    execute: function() {
        this.quickadd=commSelKey.getAttribute("quickadd");
        this.from = commSelKey.getAttribute("from");
        var _objids = document.getElementById(this.from+ "_ids_" + this.itemid);
        var url;
        url="XML_CompanyPicker.jsp";
        var pars = "itemid="+this.itemid+
                   (this.quickadd!=null&&this.quickadd?"&quickadd=1":"")+
                   (this.params != null ? "&" + this.params : "") +
                   (_objids != null ? "&exist_ids=" + _objids.value : "") +
                   "&t=" + getRandomNum();
        new Ajax.Request(url, {method: this.method, parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var resText = res.responseText;
        commSelDivContent.innerHTML = resText;
    }
};
/*------------------ People Select End -----------------------*/

