var ns4 = document.layers;
var ns6 = document.getElementById && !document.all;
var ie4 = document.all;
offsetX = 0;
offsetY = 20;
var toolTipSTYLE = "";
var bCurDisplay=false;
var bOverDiv=false;
var bPermitHide=false;
function initToolTips()
{
    if(ns4 || ns6 || ie4)
    {
        bCurDisplay=false;
        bOverDiv=false;
        if(ns4) toolTipSTYLE = document.toolTipLayer;
        else if(ns6) toolTipSTYLE = document.getElementById("toolTipLayer").style;
        else if(ie4) toolTipSTYLE = document.all.toolTipLayer.style;
        if(ns4) document.captureEvents(Event.MOUSEMOVE);
        else
        {
            toolTipSTYLE.visibility = "visible";
            toolTipSTYLE.display = "none";
        }
        document.onmousemove = moveToMouseLoc;
    }
}
function toolTip(msg, fg, bg, offsetx, offsety, w, bfix)
{
    if(toolTip.arguments.length < 1) // hide
    {
        //bPermitHide=true;
        //if(bCurDisplay||bOverDiv) return;
        //bCurDisplay=false;
        if(ns4) toolTipSTYLE.visibility = "hidden";
        else toolTipSTYLE.display = "none";
    }
    else // show
    {
        //alert(bCurDisplay+"|"+bCurHide+"|tooltipshow");
        //bPermitHide=false;
        //if(bCurDisplay||bOverDiv) return;
        //bCurDisplay=true;
        if(offsetx) offsetX = offsetx; else offsetX=10;
        if(offsety) offsetY = offsety; else offsetY=15;
        if(!fg) fg = "#000000";
        if(!bg) bg = "#FFFFDD";
        var wstr=w?" width:"+w+"px; ":" ";
        var content =
                '<table border="0" style="'+wstr+'" cellspacing="0" cellpadding="1" bgcolor="' + fg + '"><td>' +
                '<table border="0" cellspacing="0" cellpadding="2" bgcolor="' + bg +
                '"><td align="left"><font face="sans-serif" color="' + fg +
                '" style="font-size:11px">' + msg +
                '&nbsp\;</font></td></table></td></table>';
        if(ns4)
        {
            toolTipSTYLE.document.write(content);
            toolTipSTYLE.document.close();
            toolTipSTYLE.visibility = "visible";
        }
        if(ns6)
        {
            document.getElementById("toolTipLayer").innerHTML = content;
            toolTipSTYLE.display = 'block'
        }
        if(ie4)
        {
            document.getElementById("toolTipLayer").innerHTML = content;
            toolTipSTYLE.display = 'block'
        }
    }
}
function moveToMouseLoc(e)
{
    //if(bCurDisplay || bOverDiv) return;
    if(ns4 || ns6)
    {
        x = e.pageX;
        y = e.pageY;
    }
    else
    {
        x = event.x + document.body.scrollLeft;
        y = event.y + document.body.scrollTop;
    }
    toolTipSTYLE.left = x + offsetX;
    toolTipSTYLE.top = y + offsetY;
    return true;
}

function swapDiv(overDiv){
    if(!overDiv&&bPermitHide) {
        if(ns4) toolTipSTYLE.visibility = "hidden";
        else toolTipSTYLE.display = "none";
        bCurDisplay=false;
        bPermitHide=false;
    }
    bOverDiv=overDiv;
    //bCurHide=b;
    //alert("div|"+b);
}
