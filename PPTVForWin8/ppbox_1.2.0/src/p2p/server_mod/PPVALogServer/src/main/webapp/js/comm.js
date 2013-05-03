var _sUserAgent = navigator.userAgent;
var _isOpera = _sUserAgent.indexOf('Opera') > -1;
var _isIE = _sUserAgent.indexOf('compatible') > -1
        && _sUserAgent.indexOf('MSIE') > -1
        && !_isOpera;

//窗口弹出函数
function newwindow(winUrl, winName, strResize, strscrollbars, winW, winH, winL, winT)
{
    var thisPopupWin;
    var iLeft,iTop;
    if(winL == null) {
        iLeft = (window.screen.width - winW) / 2;
    }
    else {
        iLeft = winL;
    }
    if(winT == null) {
        iTop = (window.screen.height - winH) / 2;
    }
    else {
        iTop = winT;
    }

    thisWin = window.open(winUrl, winName, "width=" + winW + ",height=" + winH + ",top=" + parseInt(iTop) + ",left=" + parseInt(iLeft) + ",toolbar=no,menubar=no,scrollbars=" + strscrollbars + ",resizable=" + strResize + ",location=no,status=no");
    thisWin.focus();
    return thisWin;
}

function openwindow(url, winName, height, width)
{
    xposition = 0;
    yposition = 0;
    if((parseInt(navigator.appVersion) >= 4 ))
    {
        xposition = (screen.width - width) / 2;
        yposition = (screen.height - height) / 2;
    }
    theproperty = "width=" + width + ","
            + "height=" + height + ","
            + "location=0,"
            + "menubar=0,"
            + "resizable=1,"
            + "scrollbars=0,"
            + "status=0,"
            + "titlebar=0,"
            + "toolbar=0,"
            + "hotkeys=0,"
            + "screenx=" + xposition + "," //仅适用于Netscape
            + "screeny=" + yposition + "," //仅适用于Netscape
            + "left=" + xposition + "," //IE
            + "top=" + yposition; //IE
    var _win=window.open(url, winName, theproperty);
    _win.focus();
    //return _win;
}

function showModalWin(url,params,width,height){
    var _xposition = 0;
    var _yposition = 0;
    if((parseInt(navigator.appVersion) >= 4 )){
        _xposition = (screen.width - width) / 2;
        _yposition = (screen.height - height) / 2;
    }
    var rArr = showModalDialog(url, params,
            'edge:raised;scroll:1;help:0;status:0;resizeable:0;'+
            'dialogLeft:'+_xposition+';dialogTop:'+_yposition+';'+
            'dialogWidth:'+width+'px;dialogHeight:'+height+'px');
    return rArr;
}

//删除确认函数
function delconfirm(strTemp)
{
    if(strTemp == null) strTemp = "确定要删除吗？";
    if(confirm(strTemp))return true;
    return false;
}

//打印函数
function doprint()
{
    this.focus();
    window.print();
}

function setStatus(inf) {
    window.status = inf;
}

//设置对象的className
function setClass(obj, classname) {
    if(obj != null) {
        var flen = obj.length;
        if(isNaN(flen)) {
            obj.className = classname;
        }
        else if(obj.length > 0) {
            for(i = 0; i < obj.length; i++) {
                obj[i].className = classname
            }
        }
    }
}

function title_click(url) {
    window.location.href = url;
}

//判断字符串是否为空
function isblank(str) {
    return strtrim(str) == '';
}

function strtrim(str) {
    return str.replace(/(^\s*)|(\s*$)/g, "");
}

//去除左右空格
function str_trim(sStr) {
    charCode = new Array();
    //alert(sStr.length);
    for(i = 0; i < sStr.length; i++) {
        charCode[i] = sStr.charCodeAt(i);
    }
    for(k = sStr.length - 1; k >= 0; k--) {
        if(charCode[k] != 32) break;
    }
    sRightStr = sStr.substring(0, k + 1);
    for(j = 0; j < sRightStr.length; j++) {
        if(charCode[j] != 32) break;
    }
    sLeftStr = sRightStr.substring(j);
    //alert(sLeftStr.length);
    return sLeftStr;
}


function MM_findObj(n, d) { //v4.01
    var p,i,x;
    if(!d) d = document;
    if((p = n.indexOf("?")) > 0 && parent.frames.length) {
        d = parent.frames[n.substring(p + 1)].document;
        n = n.substring(0, p);
    }
    if(!(x = d[n]) && d.all) x = d.all[n];
    for(i = 0; !x && i < d.forms.length; i++) x = d.forms[i][n];
    for(i = 0; !x && d.layers && i < d.layers.length; i++) x = MM_findObj(n, d.layers[i].document);
    if(!x && d.getElementById) x = d.getElementById(n);
    return x;
}

function MM_validateForm() { //v4.0
    var i,p,q,nm,test,num,min,max,errors = '',args = MM_validateForm.arguments;
    var firstobj;
    for(i = 0; i < (args.length - 2); i += 3) {
        var errorspre = errors;
        test = args[i + 2];
        val = MM_findObj(args[i]);
        if(val) {
            nm = val.title;
            if((val = strtrim(val.value)) != "") {
                if(test.indexOf('isEmail') != -1) {
                    var checkstr = /^(\w|\.|-)+\@(\w|\.|-)+\.+(\w)+$/i;
                    if(!checkstr.test(val)) errors += '- ' + nm + ' 必须是一个正确的Email地址.\n';
                    //p = val.indexOf('@');
                    //if(p < 1 || p == (val.length - 1))
                } else if(test.indexOf('isDateTime') != -1) {
                    var checkstr = /^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})$/;
                    if(!checkstr.test(val)) errors += '- ' + nm + ' 必须是一个正确时间格式，如[2006-02-14 20:11:05].\n';
                } else if(test.indexOf('isDate') != -1) {
                    var checkstr = /^(\d{4})-(\d{2})-(\d{2})$/;
                    if(!checkstr.test(val)) errors += '- ' + nm + ' 必须是一个正确日期格式，如[2006-02-14].\n';
                }
                else if(test != 'R') {
                    num = parseFloat(val);
                    if(isNaN(val)) errors += '- ' + nm + ' 必须是数字.\n';
                    if(test.indexOf('inRange') != -1) {
                        p = test.indexOf(':');
                        min = test.substring(8, p);
                        max = test.substring(p + 1);
                        if(num < min || max < num) errors += '- ' + nm + ' 必须在 ' + min + ' - ' + max + ' 之间.\n';
                    }
                }
            }
            else if(test.charAt(0) == 'R') errors += '- ' + nm + ' 不能为空.\n';
        }
        if(errorspre != errors && firstobj == null) firstobj = MM_findObj(args[i]);
    }
    if(errors) {
        alert('发生下面错误:\n' + errors);
        if(firstobj != null) {
            try {
                if(!firstobj.disabled && !firstobj.readonly && firstobj.type != "hidden")firstobj.focus();
            } catch(e) {
            }
        }
    }
    document.MM_returnValue = (errors == '');
}

function isUserName(arg) {
    var checkstr = /^([A-Za-z0-9]{3,16})$/;
    return checkstr.test(arg);
}

function isMobile(arg) {
    var checkstr = /^([1][3][0-9]{9})$/;
    return checkstr.test(arg);
}

function isDate(arg) {
    var checkstr = /^(\d{4})-(\d{2})-(\d{2})$/;
    return checkstr.test(arg);
}
function isDateTime(arg) {
    var checkstr = /^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})$/;
    return checkstr.test(arg);
}
function isEmail(arg) {
    var checkstr = /^(\w|\.|-)+\@(\w|\.|-)+\.+(\w)+$/i;
    return checkstr.test(arg);
}

function getlength(arg) {
    return arg.replace(/[^\x00-\xff]/g, "aa").length;
}

function vme(arg) {
    var checkstr = /^([^<>*"'\$\\\/])+$/;
    alert(checkstr.test(arg));
}

function existInArrayStr(_arrstr, _v) {
    if(_arrstr==null||_arrstr=='') return false;
    if(_v == '') return false;
    var _arr = _arrstr.split(",");
    if(_arr == null || _arr.length == 0) return false;
    for(var i = 0; i < _arr.length; i++) if(_arr[i] == _v) return true;
    return false;
}

function addToArrayStr(_arrstr, _v,_sepechar) {
    if(_arrstr==null||_arrstr=='') return _v;
    return _arrstr + (_sepechar==null?",":_sepechar) + _v;
}

function removeFromArrayStr(_arrstr,_v,_sepechar){
    if(_arrstr==null||_arrstr=='' || _v==null || _v=="") return _arrstr;
    var _arr=_arrstr.split(_sepechar==null?",":_sepechar);
    if(_arr==null||_arr.length==0) return _arrstr;
    var _rstr="";
    for(var i=0;i<_arr.length;i++){if(_arr[i]!=_v) _rstr+=(_rstr==""?"":",")+_arr[i];}
    return _rstr;
}

function recoverHTMLTags(_str) {
    if(_str == null || _str.length == 0) return "";
    return _str.replace(/(&lt;)/g, "<")
            .replace(/(&gt;)/g, ">")
            .replace(/(&amp;)/g, "&")
            .replace(/(&nbsp;)/g, " ")
            .replace(/(&quot;)/g, "\"")
            .replace(/(<br>)/g, "\r\n");
}

function getRandomNum() {
    return new Date().getTime() + Math.random() * 1000000000 + "" + Math.random() * 1000000000;
}

function isNumberKey(keycode) {
    return (keycode < 48 || keycode > 57) && keycode != 8 && keycode != 37 && keycode != 39;
}

function isNumberKeyForPurchase(keycode) {
    return (keycode >= 48 && keycode <= 57) || (isIE && keycode >= 96 && keycode <= 105);
}
function isNumberKeyControl(keycode) {
    //IE:190 110.  8BackSpace 9Tab 37<- 39-> 46Del
    //IE:46. 8BackSpace 0Tab <-->Del
    return isIE ? keycode == 190 || keycode == 110 ||
                  keycode == 8 || keycode == 9 || keycode == 37 || keycode == 39 || keycode == 46 :
           keycode == 0 || keycode == 8 || keycode == 46;
}

function RoundNum(number, fractionDigits) {
    var d = 1;
    for(var i = 0; i < fractionDigits; i++) d *= 10;
    return Math.round(number * d) / d;
}
function isDotKey(keycode) {
    return isIE ? keycode == 190 || keycode == 110 : keycode == 46;
}
function isZeroKey(keycode) {
    return keycode == 48 && (isIE && keycode == 96);
}

function FormatNumber(srcStr, nAfterDot) {
    var resultStr,nTen,strLen,dotPos,nAfter;
    srcStr = "" + srcStr + "";
    strLen = srcStr.length;
    dotPos = srcStr.indexOf(".", 0);
    if(dotPos == -1) {
        resultStr = srcStr + ".";
        for(var i = 0; i < nAfterDot; i++) {
            resultStr = resultStr + "0";
        }
        return resultStr;
    }
    else {
        if((strLen - dotPos - 1) >= nAfterDot) {
            nAfter = dotPos + nAfterDot + 1;
            nTen = 1;
            for(var j = 0; j < nAfterDot; j++) {
                nTen = nTen * 10;
            }
            resultStr = Math.round(parseFloat(srcStr) * nTen) / nTen;
            return resultStr;
        }
        else {
            resultStr = srcStr;
            for(i = 0; i < (nAfterDot - strLen + dotPos + 1); i++) {
                resultStr = resultStr + "0";
            }
            return resultStr;
        }
    }
}


function UrlEncode(str) {
    var ret = "";
    var strSpecial = "!\"#$%&'()*+,/:;<=>?[]^`{|}~%";
    for(var i = 0; i < str.length; i++) {
        var chr = str.charAt(i);
        var c = str2asc(chr);
        tt += chr + ":" + c + "n";
        if(parseInt("0x" + c) > 0x7f) {
            ret += "%" + c.slice(0, 2) + "%" + c.slice(-2);
        } else {
            if(chr == " ")
                ret += "+";
            else if(strSpecial.indexOf(chr) != -1)
                ret += "%" + c.toString(16);
            else
                ret += chr;
        }
    }
    return ret;
}

function inputWordLimit() {
    var e = EventUtil.getEvent();
    var obj = e.target;
    if(obj == null) return false;
    var keycode = isIE ? e.keyCode : e.which;
    if(!isNumberKeyControl(keycode)&&obj.value.length >= parseInt(obj.getAttribute("maxnum")))return false;
}
function refreshInputWordLen() {
    var e = EventUtil.getEvent();
    var obj = e.target;
    if(obj == null) return false;
    var objhint = document.getElementById("ilhint_" + obj.id);
    if(objhint != null) objhint.innerHTML = "当前输入" + obj.value.length + "字,";
}


function carryAmount(fAmount) {
    return Math.round(fAmount * 2) / 2;
}

var StringBuilder = function() {
    this._buffer = [];
    this._arg1 = (arguments.length > 0) ? String(arguments[0]) : "";
    this._arg2 = (arguments.length > 1) ? String(arguments[1]) : "";
}
StringBuilder.prototype.append = function(str) {
    //    this._buffer.push(String(str));                                //这个速度没有下面这个快
    this._buffer[this._buffer.length] = String(str);
//    this._buffer[this._buffer.length] = str;            //去掉强制转换将更快，但是下面的表格输出就要修改了
}
StringBuilder.prototype.toString = function() {
    return (this._arg2 ? this._arg1 : "") + this._buffer.join(this._arg2 + this._arg1) + this._arg2;
}
StringBuilder.prototype.clear = function() {
    this._buffer = [];
}
StringBuilder.prototype.add = StringBuilder.prototype.append;

