var cell_color_over = "#ccffcc";
var cell_color_click = "#ffcc99";
var cell_color_bgcolor = "#ffffff";
var cell_color_click_rgb = "rgb(255, 204, 153)";
var b_ExecClick = true;
var b_haveOut=true;

function cell_click(obj, checked) {
    if(!b_ExecClick) return;
    if(obj.className||''!='') {
        if(obj.className.indexOf("_click")>0) {
            obj.className=obj.className.replace('_click','');
        }else{
            if(obj.className.indexOf('_over')>0) obj.className=obj.className.replace('_over','_click');
            else obj.className=obj.className+'_click';
        }
    }else{
        if(checked == null) checked = obj.style.backgroundColor + '' != (_isIE ? cell_color_click : cell_color_click_rgb + '');
        obj.style.backgroundColor = checked ? cell_color_click : cell_color_bgcolor
        if(document.getElementById("check_" + obj.id) != null)
            document.getElementById("check_" + obj.id).checked = checked;
    }
    b_haveOut=false;
}

function cell_over(obj) {
    if(!b_haveOut) return;
    if(obj.className||''!='') {
        if(obj.className.indexOf("_over")<0&&obj.className.indexOf("_click")<0){
            obj.className=obj.className+'_over';
        }
    } else {
        if(obj.style.backgroundColor + '' != (_isIE ? cell_color_click : cell_color_click_rgb + '')) {
            if((obj.getAttribute("tagName") + '').indexOf("#") == 0) cell_color_bgcolor = obj.getAttribute("tagName") + '';
            obj.style.backgroundColor = cell_color_over;
        }
    }
}
function cell_out(obj) {
    if(obj.className||''!='') {
        if(obj.className.indexOf("_over")>0) obj.className=obj.className.replace('_over','');
    } else {
        if(obj.style.backgroundColor + '' != (_isIE ? cell_color_click : cell_color_click_rgb + '')) {
            obj.style.backgroundColor = cell_color_bgcolor;
        }
    }
    b_haveOut=true;
}

function cell_status_swap(bStatus) {
    b_ExecClick = bStatus;
}
