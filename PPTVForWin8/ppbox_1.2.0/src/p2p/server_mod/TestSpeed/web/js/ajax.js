var HttpXML = function() {
}

HttpXML.prototype.GetHttpRequest = function() {
    if(window.XMLHttpRequest) {
        return new XMLHttpRequest();
    } else if(window.ActiveXObject) {
        return new ActiveXObject("Microsoft.XMLHTTP");
    }
}

HttpXML.prototype.LoadUrlText = function(urlToCall, asyncFunctionPointer) {
    var bAsync = ( typeof(asyncFunctionPointer) == 'object' ) ;
    var oXmlHttp = this.GetHttpRequest() ;
    oXmlHttp.open("GET", urlToCall, true);
    if(bAsync) {
        oXmlHttp.onreadystatechange = function() {
            if(oXmlHttp.readyState == 4) {
                if(oXmlHttp.status == 200) {
                    asyncFunctionPointer.showResult(oXmlHttp.responseText);
                } else {
                    //alert('XML request error: ' + oXmlHttp.statusText + ' (' + oXmlHttp.status + ')');
                }
            }
        }
    }
    oXmlHttp.send(null);
}

HttpXML.prototype.LoadUrlXml = function(urlToCall, asyncFunctionPointer) {
    var bAsync = ( typeof(asyncFunctionPointer) == 'object' ) ;
    var oXmlHttp = this.GetHttpRequest() ;
    oXmlHttp.open("GET", urlToCall, true);
    if(bAsync) {
        oXmlHttp.onreadystatechange = function() {
            if(oXmlHttp.readyState == 4) {
                if(oXmlHttp.status == 200) {
                    asyncFunctionPointer.showResult(oXmlHttp.responseXML);
                } else {
                    //alert('XML request error: ' + oXmlHttp.statusText + ' (' + oXmlHttp.status + ')');
                }
            }
        }
    }
    oXmlHttp.send(null);
}