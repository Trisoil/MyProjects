function PopupWindow(options)
{
    if(typeof(PopupWindow._initialized) == "undefined")
    {
        PopupWindow.currentLayer = 1;
        PopupWindow.prototype._init = function(options)
        {
            for(var property in options)
            {
                this[property] = options[property];
            }
            if(!this.windowId)
                this.windowId = "Iori";
            if(!this.container)
                this.container = document.body;
            if(!this.mode)
                this.mode = 0;
            if(this.mode == 2 && (typeof(this.paramString) == "undefined"))
            {
                this.paramString = "";
            }
            if(!this.area)
                this.area = 0;
            if(typeof(this.windowCSS) == "undefined")
                this.windowCSS = (this.area == 0 ? "w_278 p_alert" : "sys_tip w340");
            if(typeof(this.barCSS) == "undefined")
                this.barCSS = (this.area == 0 ? "alpha30black3" : "pop_tr");
            if(typeof(this.showCarpet) == "undefined")
                this.showCarpet = true;
            if(!this.showPosition)
                this.showPosition = 0;
            if(!this.width)
                this.width = 0;
            if(!this.height)
                this.height = 0;
            if(typeof(this.top) == "undefined")
                this.top = -1;
            if(typeof(this.left) == "undefined")
                this.left = -1;
            if(typeof(this.mobile) == "undefined")
                this.mobile = true;
            if(this.mode == 3)
            {
                this.showPosition = 0;
                if(typeof(this.zIndex) == "undefined")
                    this.zIndex = 10;
                if(typeof(this.boxType) == "undefined")
                    this.boxType = 1;
                if(typeof(this.okText) == "undefined")
                    this.okText = "确定";
                if((this.boxType == 2) && (typeof(this.cancelText) == "undefined"))
                    this.cancelText = "取消";
            }
            if(!this.windowColor)
                this.windowColor = "";
            if(typeof(this.autogeny) == "undefined")
            {
                this.autogeny = true;
            }
            if(typeof(this.suicide) == "undefined")
            {
                this.suicide = true;
            }
            this.timeoutId = null;
            this.alive = false;
        };
        PopupWindow.prototype._render = function()
        {
            if(this.showCarpet)
            {
                if(typeof(PopupWindow["carpet" + PopupWindow.currentLayer.toString()]) == "undefined")
                {
                    this.carpet = new Carpet({carpetId:this.windowId + "-Carpet",zIndex:(this.zIndex - 1) || 0,autogeny:this.autogeny,suicide:false});
                    PopupWindow["carpet" + PopupWindow.currentLayer.toString()] = this.carpet;
                }
                else
                {
                    this.carpet = PopupWindow["carpet" + PopupWindow.currentLayer.toString()];
                    this.carpet.setZindex(this.zIndex - 1);
                }
            }
            this._createWindow();
            if(this.mode > 0)
            {
                this._createBar();
                this._createContent();
            }
            this.alive = true;
        };
        PopupWindow.prototype._createWindow = function()
        {
            if(this.mode == 0)
            {
                if(!this.mainDiv)
                {
                    throw new Error("Mode0 must have a \"mainDiv\" argument !");
                }
                else
                {
                    if(this.mainDiv.parentNode)
                        this.mainDiv.parentNode.removeChild(this.mainDiv);
                    this.window = this.mainDiv;
                }
            }
            else
            {
                this.window = document.createElement("DIV");
            }
            this.window.id = this.windowId;
            if(typeof(this.zIndex) != "undefined")
            {
                this.window.style.position = "absolute";
                this.window.style.zIndex = this.zIndex;
            }
            if(this.width > 0)
                this.window.style.width = this.width + "px";
            if(this.height > 0)
                this.window.style.height = this.height + "px";
            if(this.area == 0)
            {
                this.window.style.background = (this.windowColor.length > 0 ? this.windowColor : getManageBackgroundColor());
            }
            else
            {
                if(this.windowColor.length > 0)
                    this.window.style.background = this.windowColor;
            }
            this.window.className = this.windowCSS;
            if(this.area == 0)
                this.window.className += " popupwindow"; else
                this.window.className += " popupwindow01";
            this.window.style.display = "none";
            this.container.appendChild(this.window);
            this.window.hide = function()
            {
                thisObject.hide();
            };
        };
        PopupWindow.prototype._createBar = function()
        {
            this.bar = document.createElement("DIV");
            this.bar.id = this.windowId + "-Bar";
            this.bar.className = this.barCSS;
            if(this.mobile)
            {
                this.bar.style.cursor = "move";
                this._onDraggingHandler = this._onDragging.bindAsEventListener(this);
                this._onDragEndHandler = this._onDragEnd.bindAsEventListener(this);
                Event.observe(this.bar, "mousedown", this._onDragStart.bindAsEventListener(this));
            }
            this.closeIMG = document.createElement("IMG");
            this.closeIMG.src = "images/close.gif";
            this.closeIMG.alt = "关闭";
            this.closeIMG.title = "关闭";
            Event.observe(this.closeIMG, "click", this.hide.bindAsEventListener(this));
            var a = document.createElement("A");
            a.appendChild(this.closeIMG);
            this.bar.appendChild(a);
            this.window.appendChild(this.bar);
        };
        PopupWindow.prototype._onDragStart = function(evt)
        {
            this.lastMouseX = evt.clientX;
            this.lastMouseY = evt.clientY;
            Event.observe(document.body, "mousemove", this._onDraggingHandler);
            Event.observe(document.body, "mouseup", this._onDragEndHandler);
            return false;
        };
        PopupWindow.prototype._onDragging = function(evt)
        {
            this.window.style.left = (parseInt(this.window.style.left) + evt.clientX - this.lastMouseX) + "px";
            this.window.style.top = (parseInt(this.window.style.top) + evt.clientY - this.lastMouseY) + "px";
            this.lastMouseX = evt.clientX;
            this.lastMouseY = evt.clientY;
            return false;
        };
        PopupWindow.prototype._onDragEnd = function()
        {
            Event.stopObserving(document.body, "mousemove", this._onDraggingHandler);
            Event.stopObserving(document.body, "mouseup", this._onDragEndHandler);
        };
        PopupWindow.prototype._createContent = function()
        {
            switch(this.mode)
                    {case 1:if(!this.paramDiv)
            {
                throw new Error("Mode1 must have a \"paramDiv\" argument !");
            }
            else
            {
                if(this.paramDiv.parentNode)
                    this.paramDiv.parentNode.removeChild(this.paramDiv);
                this.content = this.paramDiv;
            }
                break;case 2:if(typeof(this.paramString) == "undefined")
            {
                throw new Error("Mode2 must have a \"paramString\" argument !");
            }
            else
            {
                this.content = document.createElement("DIV");
                this.content.id = this.windowId + "-Content";
                this.content.className="popup_content";
                this.content.innerHTML = this.paramString;
            }
                break;case 3:if(typeof(this.tipString) == "undefined")
            {
                throw new Error("Mode3 must have a \"tipString\" argument !");
            }
            else
            {
                this.content = document.createElement("DIV");
                this.content.id = this.windowId + "-Content";
                var p = document.createElement("P");
                p.id = this.windowId + "-Tip";
                this.tip = p;
                p.className = "bold";
                p.innerHTML = this.tipString;
                this.content.appendChild(p);
                if(typeof(this.descriptionString) != "undefined")
                {
                    p = document.createElement("DIV");
                    p.id = this.windowId + "-Description";
                    this.description = p;
                    p.innerHTML = this.descriptionString;
                    this.content.appendChild(p);
                }
                if(this.area == 0)
                {
                    if(this.boxType > 0)
                    {
                        p = document.createElement("P");
                        var a = document.createElement("A");
                        a.className = "mbtn fl mr10";
                        var span = document.createElement("SPAN");
                        span.className = "before";
                        span.innerText = "#";
                        a.appendChild(span);
                        span = document.createElement("SPAN");
                        span.id = this.windowId + "-OK";
                        span.className = "center";
                        span.innerHTML = this.okText;
                        this.okButton = span;
                        a.appendChild(span);
                        Event.observe(span, "click", this.hide.bindAsEventListener(this));
                        span = document.createElement("SPAN");
                        span.className = "after";
                        span.innerText = "#";
                        a.appendChild(span);
                        p.appendChild(a);
                        if(this.boxType == 2)
                        {
                            a = document.createElement("A");
                            a.className = "mbtn fl";
                            span = document.createElement("SPAN");
                            span.className = "before";
                            span.innerText = "#";
                            a.appendChild(span);
                            span = document.createElement("SPAN");
                            span.id = this.WindowId + "-Cancel";
                            span.className = "center";
                            span.innerHTML = this.cancelText;
                            this.cancelButton = span;
                            a.appendChild(span);
                            Event.observe(span, "click", this.hide.bindAsEventListener(this));
                            span = document.createElement("SPAN");
                            span.className = "after";
                            span.innerText = "#";
                            a.appendChild(span);
                            p.appendChild(a);
                        }
                        this.content.appendChild(p);
                    }
                    var clear = document.createElement("DIV");
                    clear.className = "clear";
                    this.content.appendChild(clear);
                }
                else
                {
                    if(this.boxType > 0)
                    {
                        var d = document.createElement("DIV");
                        d.className = "mt20";
                        this.okButton = document.createElement("INPUT");
                        this.okButton.id = this.windowId + "-OK";
                        this.okButton.type = "button";
                        this.okButton.value = this.okText;
                        this.okButton.className = "btn03";
                        d.appendChild(this.okButton);
                        if(this.okLink)
                            this.okButton.onclick = function(okLink)
                            {
                                window.location.href = okLink;
                            }.bind(this, this.okLink); else
                            Event.observe(this.okButton, "click", this.hide.bindAsEventListener(this));
                        if(this.boxType == 2)
                        {
                            this.cancelButton = document.createElement("INPUT");
                            this.cancelButton.id = this.windowId + "-Cancel";
                            this.cancelButton.type = "button";
                            this.cancelButton.value = this.cancelText;
                            this.cancelButton.className = "btn02";
                            this.cancelButton.onmouseover = function()
                            {
                                this.className = "btn03";
                            };
                            this.cancelButton.onmouseout = function()
                            {
                                this.className = "btn02";
                            };
                            if(this.cancelLink)
                                this.cancelButton.onclick = function(cancelLink)
                                {
                                    window.location.href = cancelLink;
                                }.bind(this, this.cancelLink); else
                                Event.observe(this.cancelButton, "click", this.hide.bindAsEventListener(this));
                            d.appendChild(this.cancelButton);
                        }
                        this.content.appendChild(d);
                    }
                }
            }
                break;}
            this.window.appendChild(this.content);
            //alert(this.window.innerHTML);
        };
        PopupWindow.prototype.show = function()
        {
            if(this.showCarpet)
            {
                this.carpet.show();
                PopupWindow.currentLayer++;
            }
            this.window.style.display = "block";
            switch(this.showPosition)
                    {case 0:Event.observe(window, "resize", this._onWindowResizeHandler);this.setCenter();break;case 1:var evt = this.evtCaller ? this.evtCaller : GetEvent();this.window.style.left = evt.clientX + "px";this.window.style.top = evt.clientY + "px";break;case 2:if(this.left > -1)
                this.window.style.left = this.left + "px";if(this.top > -1)
                this.window.style.top = this.top + "px";break;}
            Event.observe(document.body, "keydown", this._onBodyKeyDownHandler);
            this.window.focus();
        };
        PopupWindow.prototype.hide = function()
        {
            if(this.beforeCloseCallback) {
                this.beforeCloseCallback();
            }
            Event.stopObserving(document.body, "keydown", this._onBodyKeyDownHandler);
            if(this.showCarpet)
            {
                this.carpet.hide();
                PopupWindow.currentLayer--;
            }
            if(this.showPosition == 0)
                Event.stopObserving(window, "resize", this._onWindowResizeHandler);
            this.window.style.display = "none";
            if(this.suicide)
            {
                this.container.removeChild(this.window);
                this.alive = false;
            }
            if(typeof(this.iframe) != "undefined")
            {
                this.iframe.style.display = "none";
            }
        };
        PopupWindow.prototype.setCenter = function()
        {
            var windowWidth;
            var xOffset;
            var windowHeight;
            var yOffset;
            if(window.innerHeight)
            {
                windowWidth = window.innerWidth;
                xOffset = window.pageXOffset;
                windowHeight = window.innerHeight;
                yOffset = window.pageYOffset;
            }
            else
            {
                windowWidth = document.documentElement.offsetWidth;
                xOffset = document.documentElement.scrollLeft;
                windowHeight = document.documentElement.offsetHeight;
                yOffset = document.documentElement.scrollTop;
            }
            this.window.style.left = (xOffset + (windowWidth - this.window.offsetWidth) / 2) + "px";
            this.window.style.top = (yOffset + (windowHeight - this.window.offsetHeight) / 2) + "px";
        };
        PopupWindow.prototype._onKeyDown = function(evt)
        {
            if(evt.keyCode == 27)
                this.hide();
        };
        PopupWindow.prototype.appendHandler = function(button, handler)
        {
            if(this.mode == 3 && this.boxType > 0)
            {
                if(button == "ok")
                    Event.observe(this.okButton, "click", handler);
                if(button == "cancel" && this.boxType == 2)
                    Event.observe(this.cancelButton, "click", handler);
            }
        };
        PopupWindow.prototype.removeHandler = function(button, handler)
        {
            if(this.mode == 3 && this.boxType > 0)
            {
                if(button == "ok")
                    Event.stopObserving(this.okButton, "click", handler);
                if(button == "cancel" && this.boxType == 2)
                    Event.stopObserving(this.cancelButton, "click", handler);
            }
        };
        PopupWindow.prototype.setTip = function(message)
        {
            if(this.mode == 3)
            {
                this.tip.innerHTML = message;
            }
        };
        PopupWindow.prototype.setDescription = function(message)
        {
            if(this.mode == 3)
            {
                this.description.innerHTML = message;
            }
        };
        PopupWindow.prototype.setContent = function(message)
        {
            if(this.mode > 0)
            {
                this.content.innerHTML = message;
            }
        };
        PopupWindow.prototype.setWindow = function(message)
        {
            this.window.innerHTML = message;
        };
        PopupWindow.prototype.setTimeoutHide = function(timeout)
        {
            clearTimeout(this.timeoutId);
            this.timeoutId = setTimeout(this.hide.bind(this), timeout);
        };
        PopupWindow.prototype.appendCloseIMGHandler = function(handler)
        {
            Event.observe(this.closeIMG, "click", handler);
        };
        PopupWindow._initialized = true;
    }
    this._onWindowResizeHandler = this.setCenter.bindAsEventListener(this);
    this._onBodyKeyDownHandler = this._onKeyDown.bindAsEventListener(this);
    this._init(options);
    this._render();
    if(this.autogeny)
        this.show();
    var thisObject = this;
    if(this.autogeny)
        this.window.focus();
}
function Carpet(options)
{
    if(typeof(Carpet._initialized) == "undefined")
    {
        Carpet.prototype._init = function(options)
        {
            for(var property in options)
            {
                this[property] = options[property];
            }
            if(typeof(this.autogeny) == "undefined")
            {
                this.autogeny = true;
            }
            if(typeof(this.suicide) == "undefined")
            {
                this.suicide = true;
            }
        };
        Carpet.prototype._render = function()
        {
            this.carpet = document.createElement("DIV");
            this.carpet.id = this.carpetId;
            var styles = this.carpet.style;
            styles.zIndex = this.zIndex;
            styles.position = "absolute";
            styles.margin = "0px";
            styles.padding = "0px";
            styles.top = (this.top || 0) + "px";
            styles.left = (this.left || 0) + "px";
            styles.width = (this.width || ((document.body.clientWidth && (document.body.clientWidth > screen.width)) ? document.body.clientWidth : screen.width)) + "px";
            styles.height = (this.height || ((document.body.clientHeight && (document.body.clientHeight > screen.height)) ? document.body.clientHeight : screen.height)) + "px";
            styles.background = "#FFFFFF";
            styles.filter = "alpha(opacity=80)";
            styles.opacity = 0.8;
            styles.zoom = 1;
            styles.display = "none";
            document.body.appendChild(this.carpet);
        };
        Carpet.prototype.getCarpet = function()
        {
            return this.carpet;
        };
        Carpet.prototype.show = function()
        {
            var selects = document.getElementsByTagName("SELECT");
            var isPop;
            for(var i = 0; i < selects.length; i++)
            {
                isPop = selects[i].ispop || selects[i].getAttribute("ispop");
                if(!isPop)
                {
                    selects[i].style.visibility = "hidden";
                }
            }
            var flashObjects = $$("embed");
            flashObjects.each(function(flashObject)
            {
                flashObject.hide();
            });
            this.carpet.style.display = "block";
        };
        Carpet.prototype.hide = function()
        {
            var selects = document.getElementsByTagName("SELECT");
            var isPop;
            for(var i = 0; i < selects.length; i++)
            {
                isPop = selects[i].ispop || selects[i].getAttribute("ispop");
                if(!isPop)
                {
                    selects[i].style.visibility = "visible";
                }
            }
            var flashObjects = $$("embed");
            flashObjects.each(function(flashObject)
            {
                flashObject.show();
            });
            this.carpet.style.display = "none";
            if(this.suicide)
                document.body.removeChild(this.carpet);
        };
        Carpet.prototype.setZindex = function(zIndex)
        {
            this.carpet.style.zIndex = zIndex;
        };
        Carpet._initialized = true;
    }
    this._init(options);
    this._render();
}
var errorMessageBoxDisplayArea = null;
function setErrorMessage(msg)
{
    if(errorMessageBoxDisplayArea == null)
        createBoxOnLoad();
    errorMessageBoxDisplayArea.setWindow("<a href=\"#\" title=\"关闭\"><img src=\"/images/close_g.gif\" alt=\"关闭\" onclick=\"javascript:errorMessageBoxDisplayArea.hide();\" /></a><span class=\"bold\">" + msg + "</span>");
    errorMessageBoxDisplayArea.show();
    errorMessageBoxDisplayArea.setTimeoutHide(5000);
}
function createBoxOnLoad()
{
    var errorMsg = document.createElement("DIV");
    errorMsg.style.width = 390 + "px";
    errorMsg.innerHTML = "<a href=\"#\" title=\"关闭\"><img src=\"/images/close_g.gif\" alt=\"关闭\" onclick=\"javascript:errorMessageBoxDisplayArea.hide();\" /></a><span class=\"bold\"></span>";
    errorMessageBoxDisplayArea = new PopupWindow({windowId:"errorWindow",container:document.getElementById("errorContainer"),mainDiv:errorMsg,showCarpet:false,windowColor:"",windowCSS:"greenbox ml40",autogeny:false,suicide:false,area:1,showPosition:3});
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