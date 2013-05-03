var divTag=null;
var divTagText=null;
var divTagContent=null;
var objTagIDS=null;
function taginit(){
    divTag=document.getElementById("divTag");
    divTagText=document.getElementById("divTagText");
    divTagContent=document.getElementById("divTagContent");
    objTagIDS=document.getElementById("tagids");
}

function showDivTag(e){
    var t = e.offsetTop,  h = e.clientHeight, w=e.clientWidth, l = e.offsetLeft, p = e.type;
    while (e = e.offsetParent){t += e.offsetTop; l += e.offsetLeft;}
    divTag.style.visibility="visible";
    divTag.style.left=l;
    divTag.style.top=t+h+2;

    loadInitTag();
}

function loadInitTag() {
    var url = "XML_TagPicker.jsp";
    var pars = "t=" + getRandomNum();
    var myAjax = new Ajax.Request(url, {method: 'get', parameters: pars, onComplete: loadInitTagCompleted});
}

function loadInitTagCompleted(res) {
    var resText = res.responseText;
    divTagContent.innerHTML="<UL>"+resText+"</UL>";
}

function hideDivTag(){
    divTag.style.visibility="hidden";
}

function clearDivTag(){
    objTagIDS.value="";
    divTagText.innerHTML="";
}

function _clickTag(_tid,_tname){
    if(existInArrayStr(objTagIDS.value,_tid)) return;
    objTagIDS.value=addToArrayStr(objTagIDS.value,_tid);
    divTagText.innerHTML=addToArrayStr(divTagText.innerHTML,_tname);
}

function doTagKeySearchRealTime(){
    var objkey=document.getElementById("tag_key");
    if(objkey.value=='') return;
    var url = "XML_TagPicker.jsp";
    var pars = "key="+objkey.value+"&rt=1&t=" + getRandomNum();
    var myAjax = new Ajax.Request(url, {method: 'post', parameters: pars, onComplete: loadInitTagCompleted});
}

function doTagKeySearch(){
    var objkey=document.getElementById("tag_key");
    //if(isblank(objkey.value)){alert("请输入标签关键字");return;}
    var url = "XML_TagPicker.jsp";
    var pars = "key="+objkey.value+"&t=" + getRandomNum();
    var myAjax = new Ajax.Request(url, {method: 'post', parameters: pars, onComplete: loadInitTagCompleted});
}
