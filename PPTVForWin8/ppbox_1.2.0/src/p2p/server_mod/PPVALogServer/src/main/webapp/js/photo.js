//图片预览
function preViewPic(picDivName, imgName, fn, iBorder, iBorderColor, iWidth, iHeight, iMaxW) {
    var picdiv = document.getElementById(picDivName);
    if(picdiv != null && fn != null && fn != '') {
        if(imgName == null) imgName = "picname";
        if(iBorder == null) iBorder = 0;
        if(iBorderColor == null) iBorderColor = "#CCCCCC";
        var strw,strh;
        if(iWidth == null || iWidth == 0) strw = "";
        else strw = " width=" + iWidth + " ";
        if(iHeight == null || iHeight <= 0) strh = "";
        else strh = " height=" + iHeight + " ";
        picdiv.innerHTML = "<img id='" + imgName + "_id' name='" + imgName + "' alt='预览状态...' " + strw + strh + " tagName=" + iMaxW + " border=" + iBorder +
                           " src='" + fn + "' style='border-color:" + iBorderColor + "' " +
                           "onmousewheel=\"width+=(window.event.wheelDelta==120)?((width>20)?-20:-0):((width<" + (iMaxW ? iMaxW : 500) + ")?+20:+0);return false;\" " +
                           ">" +
                           "<div style='position:absolute; width:0px; height:0px; overflow:hidden'><img id='" + imgName + "_realview' src='" + fn + "' " +
                           "onload='onloadPic(this);' tagName='" + imgName + "' ph='" + iHeight + "' iWidth='" + iWidth + "' "+(iHeight!=null?" iHeight='"+iHeight+"' ":"")+ "></div>";
        document.focus();
    }
}

//图片预览
function preViewPic2(picDivName, imgName, fn, iBorder, iBorderColor, iWidth, iHeight, iMaxW) {
    var picdiv = document.getElementById(picDivName);
    if(picdiv != null && fn != null && fn != '') {
        if(imgName == null) imgName = "picname";
        if(iBorder == null) iBorder = 0;
        if(iBorderColor == null) iBorderColor = "#CCCCCC";
        var strw,strh;
        if(iWidth == null || iWidth == 0) strw = "";
        else strw = " width=" + iWidth + " ";
        if(iHeight == null || iHeight <= 0) strh = "";
        else strh = " height=" + iHeight + " ";
        /*
                picdiv.innerHTML = "<img id='"+imgName+"_id' name='" + imgName + "' alt='预览状态...' " + strw + strh + " tagName="+iMaxW+" border=" + iBorder +
                                   " src='file:///" + fn + "' style='border-color:" + iBorderColor + "' " +
                                   "onmousewheel=\"width+=(window.event.wheelDelta==120)?((width>20)?-20:-0):((width<"+(iMaxW?iMaxW:500)+")?+20:+0);return false;\" "+
                                   ">";
        */
/*
        var eimg=document.createElement("IMG");
        eimg.id=imgName+"_id";
        eimg.src="file:///"+fn.replace(/(\\)/g,"/");
        picdiv.appendChild(eimg);
*/
        picdiv.innerHTML = "<img id='" + imgName + "_id' name='" + imgName + "' alt='"+(isIEBrowser()?"预览状态...":"FireFox浏览器是不能预览本地图片的...没办法...这个是无法解决的...")+ "' " + strw + strh + " tagName=" + iMaxW + " border=" + iBorder +
                           " src='file:///" + fn + "' style='color:#996600; border-color:" + iBorderColor + "' " +
                           "onmousewheel=\"width+=(window.event.wheelDelta==120)?((width>20)?-20:-0):((width<" + (iMaxW ? iMaxW : 500) + ")?+20:+0);return false;\" " +
                           ">" +
                           "<div style='position:absolute; width:0px; height:0px; overflow:hidden'><img id='" + imgName + "_realview' src='file:///" + fn + "' " +
                           "onload='onloadPic(this);' tagName='" + imgName + "' ph='" + iHeight + "' iWidth='" + iWidth + "' "+(iHeight!=null?" iHeight='"+iHeight+"' ":"")+"></div>";
        document.focus();
    }
}

function onloadPic(obj) {
    try {
        var photoinfo = document.getElementById("photoview_info");
        photoinfo.innerHTML = "图片实际尺寸：<span class=font12>" + obj.width + "×" + obj.height + "</span>&nbsp;&nbsp;" +
                              "<a href=\"javascript:showPicRealSize('" + obj.id + "');\">显示实际尺寸</a> " +
                              "<a href=\"javascript:showPicSmallSize('" + obj.id + "');\">显示缩略图</a>";
        if(!isIEBrowser()) photoinfo.style.marginBottom = "10px";
        var _imgs = document.getElementById(obj.getAttribute("tagName") + "_id");
        if(obj.width > 0 && obj.width < _imgs.width) {
            _imgs.width = obj.width;
        }
        return;
        if(obj.ph == "-1" && _imgs.height > _imgs.width) {
            _w = _imgs.width;
            _imgs.width = Math.round(_imgs.width * (_imgs.width / _imgs.height));
            _imgs.height = _w;
        }
    } catch(e) {
    }
}
function showPicRealSize(objname) {
    try {
        var obj = document.getElementById(objname);
        var _imgs = document.getElementById(obj.getAttribute("tagName") + "_id");
        if(!isNaN(parseInt(obj.getAttribute("iHeight")))&&parseInt(obj.getAttribute("iHeight"))!=-1) {
            _imgs.setAttribute("iHeight",_imgs.height+'');
            _imgs.height= obj.height;
        }
        _imgs.width = obj.width;
    } catch(e) {
    }
}
function showPicSmallSize(objname) {
    try {
        var obj = document.getElementById(objname);
        var _imgs = document.getElementById(obj.getAttribute("tagName") + "_id");
        var iWidth = parseInt(obj.getAttribute("iWidth"));
        var iHeight=parseInt(obj.getAttribute("iHeight"));
        if(_imgs.width > iWidth) {
            _imgs.width = iWidth;
            if(!isNaN()) _imgs.height=iHeight;
            else if(_imgs.getAttribute("iHeight")) _imgs.height=parseInt(_imgs.getAttribute("iHeight"));
        }
    } catch(e) {
    }
}

function isAllowedPicFile(filename, allowedExt) {
    if(filename == '') return false;
    var arrAllowedExt = new Array();
    var fileext = filename.substring(filename.lastIndexOf(".") + 1, filename.length);
    fileext = fileext.toLowerCase();
    if(allowedExt != "") {
        arrAllowedExt = allowedExt.split(",");
        for(i = 0; i < arrAllowedExt.length; i++) {
            if(fileext == arrAllowedExt[i].toLowerCase()) {
                return true;
            }
        }
        return false;
    }
    return true;
}

function isIEBrowser() {
    var sUserAgent = navigator.userAgent;
    var isOpera = sUserAgent.indexOf('Opera') > -1;
    return sUserAgent.indexOf('compatible') > -1
            && sUserAgent.indexOf('MSIE') > -1
            && !isOpera;
}