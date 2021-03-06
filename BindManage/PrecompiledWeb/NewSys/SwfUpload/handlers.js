var Handlers = function(){
    var pp=this;
	return{
	    swfUploadLoaded : function() {
		    FeaturesDemo.start(this); // This refers to the SWFObject because		
		    // init button here
	    },
	    btnEnable:function(){
	        var btn = Ext.getCmp('customuploadclose');
	        if(btn)
	            btn.enable();
	    },
	    btnDisable:function(){
	        var btn = Ext.getCmp('customuploadclose');
	        if(btn)
	            btn.disable();
	    },
	    fileQueued : function(file) {
		    try {			
			    var su=FeaturesDemo.su;
			    var r=new this.RC({id:file.id,fileName:file.name,fileSize:file.size,state:-1});
			    this.ds.add([r]);			
		    } catch (ex) {
			    alert(ex);
		    }
	    },
	    fileQueueError : function(file, errorCode, message) {
		    try {
			    var errorName = "";
			    switch (errorCode) {
				    case SWFUpload.QUEUE_ERROR.QUEUE_LIMIT_EXCEEDED :
					    errorName = "当前上传的文件个数超过了限制，最多可上传"+FeaturesDemo.SU.settings.file_queue_limit+"个文件！";
					    break;
				    case SWFUpload.QUEUE_ERROR.FILE_EXCEEDS_SIZE_LIMIT :
					    errorName = "当前上传的文件大小超过了限制，最大可上传"+FeaturesDemo.SU.settings.file_size_limit+"的文件";
					    break;
				    case SWFUpload.QUEUE_ERROR.ZERO_BYTE_FILE :
					    errorName = "Zero byte file selected.";
					    break;
				    case SWFUpload.QUEUE_ERROR.INVALID_FILETYPE :
					    errorName = "Invalid filetype selected.";
					    break;
				    default :
					    errorName = "File not found 404.";
					    break;
			    }

//			    var errorString = errorName
//					    + ":File ID: "
//					    + (typeof(file) === "object" && file !== null
//							    ? file.id
//							    : "na") + ":" + message;

			    Ext.MessageBox.alert('提示',errorName);

		    } catch (ex) {
			    this.debug(ex);
		    }
	    },
	    uploadProgress : function(file, bytesLoaded, totalBytes) {

		    try {
		        var btn = Ext.getCmp('customuploadclose');
	            if(btn)
	                btn.disable();
			    var percent = Math.ceil((bytesLoaded / file.size) * 100);		
			    FeaturesDemo.current.set("rate",percent);
			    FeaturesDemo.current.set("state",1);
			    FeaturesDemo.current.set("speed",SWFUpload.speed.formatBPS(file.sizeUploaded/file.timeElapsed));
			    FeaturesDemo.current.commit();		
		    } catch (ex) {
			    alert(ex);
		    }
	    },
	    uploadSuccess : function(file, serverData, receivedResponse) {
		    try 
	        {			
			    if(data)
			    {
	    	         var data=Ext.decode(serverData);		    	 
			         FeaturesDemo.current.set("code",data.code);
			         FeaturesDemo.current.commit();
			    }			 
			    FeaturesDemo.startSelectedQuene();
		    } catch (ex) {
			    alert(ex);
		    }
	    },
        uploadComplete:function(file) {
            var btn = Ext.getCmp('customuploadclose');
	        if(btn)
	            btn.enable();
        }, 
	    uploadError : function(file, errorCode, message) {		
		    try {
		        var btn = Ext.getCmp('customuploadclose');
	            if(btn)
	                btn.enable();
			    var errorName = "";
			    switch (errorCode) {
				    case SWFUpload.UPLOAD_ERROR.HTTP_ERROR :
				        if(message==405)
				            errorName = "远程主机没有开启，请与管理员联系！";
				        else
					        errorName = "服务器发生错误，请与管理员联系！";
					    break;
				    case SWFUpload.UPLOAD_ERROR.MISSING_UPLOAD_URL :
					    errorName = "MISSING UPLOAD URL";
					    break;
				    case SWFUpload.UPLOAD_ERROR.IO_ERROR :
					    errorName = "IO ERROR";
					    break;
				    case SWFUpload.UPLOAD_ERROR.SECURITY_ERROR :
					    errorName = "SECURITY ERROR";
					    break;
				    case SWFUpload.UPLOAD_ERROR.UPLOAD_LIMIT_EXCEEDED :
					    errorName = "UPLOAD LIMIT EXCEEDED";
					    break;
				    case SWFUpload.UPLOAD_ERROR.UPLOAD_FAILED :
					    errorName = "UPLOAD FAILED";
					    break;
				    case SWFUpload.UPLOAD_ERROR.SPECIFIED_FILE_ID_NOT_FOUND :
					    errorName = "SPECIFIED FILE ID NOT FOUND";
					    break;
				    case SWFUpload.UPLOAD_ERROR.FILE_VALIDATION_FAILED :
					    errorName = "FILE VALIDATION FAILED";
					    break;
				    case SWFUpload.UPLOAD_ERROR.FILE_CANCELLED :
					    errorName = "FILE CANCELLED";
					    break;
				    case SWFUpload.UPLOAD_ERROR.UPLOAD_STOPPED :
					    errorName = "FILE STOPPED";
					    FeaturesDemo.current.set("state",2);
					    FeaturesDemo.current.commit();
					    return;					
				    default :
					    errorName = "UNKNOWN";
					    break;
			    }
			    FeaturesDemo.current.set("state",4);
					    FeaturesDemo.current.commit();
                Ext.MessageBox.alert('提示',errorName);

		    } catch (ex) {
			    this.debug(ex);
		    }
	    }
	}
}();
