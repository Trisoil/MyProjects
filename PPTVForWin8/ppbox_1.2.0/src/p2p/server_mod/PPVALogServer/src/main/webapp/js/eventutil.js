// windows的event对象和DOM的event对象一些属性不相同，需要把
//  他们转为相同的格式

// section 1 : 非常重要，判断浏览器类型，版本，及运行平台，以达到跨平台目的
var sUserAgent = navigator.userAgent;
var fAppVersion = parseFloat(navigator.appVersion);

var isOpera = sUserAgent.indexOf('Opera') > -1;
var isKHTML = sUserAgent.indexOf('KHTML') > -1
                || sUserAgent.indexOf('Konqueror') > -1
                || sUserAgent.indexOf('AppleWebKit') > -1;

if (isKHTML) {
        isSafari = sUserAgent.indexOf('AppleWebKit') > -1;
        isKonq = sUserAgent.indexOf('Konqueror') > -1;
}

var isIE = sUserAgent.indexOf('compatible') > -1
            && sUserAgent.indexOf('MSIE') > -1
            && !isOpera;
var isMinIE4 = isMinIE5 = isMinIE5_5 = isMinIE6 = false;
if (isIE) {

        var reIE = new RegExp('MSIE (\\d+\\.\\d+);');
        reIE.test(sUserAgent);
        var fIEVersion = parseFloat(RegExp['$1']);
        isMinIE4 = fIEVersion >= 4;
        isMinIE5 = fIEVersion >= 5;
        isMinIE5_5 = fIEVersion >= 5.5;
        isMinIE6 = fIEVersion >= 6.0;
}


var isMoz = sUserAgent.indexOf('Gecko') > -1
                && !isKHTML;
var isMinMoz1 = sMinMoz1_4 = isMinMoz1_5 = false;
if (isMoz) {
        var reMoz = new RegExp('rv:(\\d+\\.\\d+(?:\\.\\d+)?)');
        reMoz.test(sUserAgent);
        isMinMoz1 = compareVersions(RegExp['$1'], '1.0') >= 0;
        isMinMoz1_4 = compareVersions(RegExp['$1'], '1.4') >= 0;
        isMinMoz1_5 = compareVersions(RegExp['$1'], '1.5') >= 0;
}

if (isOpera) {
        var fOperaVersion;
        if(navigator.appName == 'Opera') {
        fOperaVersion = fAppVersion;
        } else {
        var reOperaVersion = new RegExp('Opera (\\d+\\.\\d+)');
        reOperaVersion.test(sUserAgent);
        fOperaVersion = parseFloat(RegExp['$1']);
        }
        isMinOpera4 = fOperaVersion >= 4;
        isMinOpera5 = fOperaVersion >= 5;
        isMinOpera6 = fOperaVersion >= 6;
        isMinOpera7 = fOperaVersion >= 7;
        isMinOpera7_5 = fOperaVersion >= 7.5;
}

var isNS4 = !isIE && !isOpera && !isMoz && !isKHTML
                && (sUserAgent.indexOf('Mozilla') == 0)
                && (navigator.appName == 'Netscape')
                && (fAppVersion >= 4.0 && fAppVersion < 5.0);
var isMinNS4 = isMinNS4_5 = isMinNS4_7 = isMinNS4_8 = false;
if (isNS4) {
        isMinNS4 = true;
        isMinNS4_5 = fAppVersion >= 4.5;
        isMinNS4_7 = fAppVersion >= 4.7;
        isMinNS4_8 = fAppVersion >= 4.8;
}

var isWin = (navigator.platform == 'Win32') || (navigator.platform == 'Windows');
var isMac = (navigator.platform == 'Mac68K') || (navigator.platform == 'MacPPC')
                || (navigator.platform == 'Macintosh');
var isUnix = (navigator.platform == 'X11') && !isWin && !isMac;
var isWin95 = isWin98 = isWinNT4 = isWin2K = isWinME = isWinXP = false;
if (isWin) {
        isWin95 = sUserAgent.indexOf("Win95") > -1
                || sUserAgent.indexOf("Windows 95") > -1;
        isWin98 = sUserAgent.indexOf("Win98") > -1
                || sUserAgent.indexOf("Windows 98") > -1;
        isWinME = sUserAgent.indexOf("Win 9x 4.90") > -1
                || sUserAgent.indexOf("Windows ME") > -1;
        isWin2K = sUserAgent.indexOf("Windows NT 5.0") > -1
                || sUserAgent.indexOf("Windows 2000") > -1;
        isWinXP = sUserAgent.indexOf("Windows NT 5.1") > -1
                || sUserAgent.indexOf("Windows XP") > -1;
        isWinNT4 = sUserAgent.indexOf("WinNT") > -1
                || sUserAgent.indexOf("Windows NT") > -1
                || sUserAgent.indexOf("WinNT4.0") > -1
                || sUserAgent.indexOf("Windows NT 4.0") > -1
                && (!isWinME && !isWin2K && !isWinXP);
}
var isMac68K = isMacPPC = false;
if (isMac) {
isMac68K = sUserAgent.indexOf("Mac_68000") > -1
            || sUserAgent.indexOf("68K") > -1;
            isMacPPC = sUserAgent.indexOf("Mac_PowerPC") > -1
            || sUserAgent.indexOf("PPC") > -1;
}
var isSunOS = isMinSunOS4 = isMinSunOS5 = isMinSunOS5_5 = false;
if (isUnix) {
        isSunOS = sUserAgent.indexOf("SunOS") > -1;
        if (isSunOS) {
        var reSunOS = new RegExp("SunOS (\\d+\\.\\d+(?:\\.\\d+)?)");
        reSunOS.test(sUserAgent);
        isMinSunOS4 = compareVersions(RegExp["$1"], "4.0") >= 0;
        isMinSunOS5 = compareVersions(RegExp["$1"], "5.0") >= 0;
        isMinSunOS5_5 = compareVersions(RegExp["$1"], "5.5") >= 0;
}
}


//用于比较版本的函数
function compareVersions(sVersion1, sVersion2) {
    var aVersion1 = sVersion1.split('.');
    var aVersion2 = sVersion2.split('.');
    if (aVersion1.length > aVersion2.length) {
    for (var i=0; i < aVersion1.length - aVersion2.length; i++) {
    aVersion2.push('0');
    }
    } else if (aVersion1.length < aVersion2.length) {
    for (var i=0; i < aVersion2.length - aVersion1.length; i++) {
    aVersion1.push('0');
    }
    }
    for (var i=0; i < aVersion1.length; i++) {
        if (aVersion1[i] < aVersion2[i]) {
        return -1;
        } else if (aVersion1[i] > aVersion2[i]) {
        return 1;
        }
        }
        return 0;


}

// end section 1

var EventUtil = new Object;
EventUtil.formatEvent = function (oEvent) {
    if(isIE && isWin) {
        oEvent.charCode = (oEvent.type == "keypress") ? oEvent.keyCode : 0;
        oEvent.eventPhase = 2;
        oEvent.isChar = (oEvent.charCode > 0);
        oEvent.pageX = oEvent.clientX + document.body.scrollLeft;
        oEvent.pageY = oEvent.clientY + document.body.scrollTop;
        oEvent.preventDefault = function () {
            this.returnValue = false;
        };
        if(oEvent.type == "mouseout") {
            oEvent.relatedTarget = oEvent.toElement;
        } else if(oEvent.type == "mouseover") {
            oEvent.relatedTarget = oEvent.fromElement;
        }
        oEvent.stopPropagation = function () {
            this.cancelBubble = true;
        };
        oEvent.target = oEvent.srcElement;
        oEvent.time = (new Date).getTime();
    }
    return oEvent;
}
//得到event
EventUtil.getEvent = function() {
    if(window.event) {
        return this.formatEvent(window.event);
    } else {
        return EventUtil.getEvent.caller.arguments[0];
    }
}
