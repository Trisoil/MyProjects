/* 
*扩展Ext的数据代理
*依赖asp.net ajax库,使用前页面需注册ScriptManager对象

*/

Ext.data.WebServiceProxy = function(config){
    Ext.data.WebServiceProxy.superclass.constructor.call(this);
    Ext.apply(this,config);
    
    
};


Ext.extend(Ext.data.WebServiceProxy,Ext.data.DataProxy,{

    //配置项，WebService的相对路径
    servicePath : "",
    
    //配置项，需要调用的WebService的方法名
    methodName  : "",
    
    //配置项，对象参数 {nms:"",id:"",.....}
    methodParams : {},
    
    load:function(params, reader, callback, scope, arg){
        if(this.fireEvent("beforeload", this, params) !== false){
            var  o = {
                params : Ext.apply(this.methodParams, params || {}),
                request: {
                    callback : callback,
                    scope : scope,
                    arg : arg
                },
                reader: reader,
                callback : callback,
                scope: this
            };
            Sys.Net.WebServiceProxy.invoke(this.servicePath, this.methodName, false, 
                                            this.methodParams,
                                            this.loadResponse.createDelegate(this),
                                            this.loadFailed.createDelegate(this),  
                                            o, 300000);
        }
        else
            callback.call(scope||this, null, arg, false);
    },
    
    loadResponse:function(response,context){
        var result = context.reader.readRecords(response);
        this.fireEvent("load", this, context, context.request.arg);
        context.callback.call(context.request.scope, result, context.request.arg, true);
    },
    
    loadFailed:function(error,context){
        var stackTrace = error.get_stackTrace();
        var message = error.get_message();
        var statusCode = error.get_statusCode();
        var exceptionType = error.get_exceptionType();
        var timedout = error.get_timedOut();
   
        if(exceptionType.toLowerCase() !== "system.net.sockets.socketexception" || exceptionType.toLowerCase() == "system.runtime.remoting.remotingexception")
        {
        }
        else
        {
            //alert("Stack Trace: " +  stackTrace + "\n" +
            //"Service Error: " + message + "\n" +
            //"Status Code: " + statusCode + "\n" +
            //"Exception Type: " + exceptionType + "\n" +
            //"Timedout: " + timedout);
            //改成Ext的提示框并且10秒钟后关闭
            Ext.MessageBox.alert("提示",message);
            window.setTimeout(function(){
                Ext.MessageBox.hide();
            },10000);
        }
        this.fireEvent("loadexception", this, context, error, null);
        context.callback.call(context.request.scope,null,context.request.arg,false);            
    }
});
