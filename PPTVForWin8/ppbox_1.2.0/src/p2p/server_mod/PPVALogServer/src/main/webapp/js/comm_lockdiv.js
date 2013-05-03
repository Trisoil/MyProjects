var const_alert_message="";
var _commdivCoverHTML = "<div id=\"commDivCover\" onClick=\"clickDivCover();\"></div>";
var _commdivBox="<div id=\"commDivBox\" class=\"comm_div_box\"></div>";
document.write(_commdivCoverHTML);
document.write(_commdivBox);

var commDivCover,commDivBox;

function initDivCover(alertmsg){
    if(alertmsg||''!='') const_alert_message=alertmsg;
    commDivCover = document.getElementById("commDivCover");
    commDivBox=document.getElementById("commDivBox");
    commDivCover.className = _isIE ? "comm_div_cover" : "comm_div_cover_ff";
    commDivCover.style.height=document.body.scrollHeight;
    //commDivCover.innerHTML=_commdivBox;
}

function showCommDivCover() {
	alert("showCommDivCover");
    commDivBox.style.visibility="visible";
    commDivBox.style.top=(document.body.clientHeight-226)/2;
    commDivBox.style.left=(document.body.clientWidth-400)/2;
    //var boxW=document.body.clientWidth-(100*2);
    //if(boxW>800) boxW=800;
    //commDivBox.style.width=boxW;
    //commDivBox.style.height=document.body.clientHeight-(100*2);
    commDivBox.style.width=400;
    commDivBox.style.height=226;
    
    document.body.style.width = "100%";
    document.body.style.height = "100%";
    document.body.style.overflow = "hidden";
    commDivCover.style.visibility = "visible";
    //commDivCover.style.filter="alpha(opacity=0)";

    //Effect.Appear("commDivCover",{duration:0.3,from:0.0,to:0.8});
}

function hideCommDivCover() {
    document.body.style.overflow = "auto";
    commDivBox.style.visibility='hidden';
    //commDivCover.style.filter="alpha(opacity=0)";
    commDivCover.style.visibility = "hidden";
}

function clickDivCover(){
    //if(const_alert_message||''!='') alert(const_alert_message);
    //hideCommDivCover();
}
function closeDivCover(){
	hideCommDivCover();
}