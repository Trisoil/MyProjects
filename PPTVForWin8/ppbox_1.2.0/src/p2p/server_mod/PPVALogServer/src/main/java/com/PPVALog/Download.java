package com.PPVALog;

import java.io.*;
import java.net.*;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
//import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import my.myproperties.tool.PropUtil;

public class Download extends HttpServlet {
    
	public Download() {
        super();
    }
    
	public void destroy() {   
        super.destroy();
    }

	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// 
		String file_id = request.getParameter("file_id");
		String date = request.getParameter("date");

		if( file_id.length() == 0 || date.length() == 0 ) {
			return;
		}
		
    	String path = PropUtil.getInstance("constant").getProperty("PPVALogPath") + "/" + date + "/" + file_id + ".gz";
    	File f = new File(path);
    	
    	FileInputStream fi = new FileInputStream(path);
    	
    	response.setContentType("application/x-download");
    	String filedisplay = file_id + ".gz";
    	String filenamedisplay = URLEncoder.encode(filedisplay,"UTF-8");
    	response.addHeader("Content-Disposition","attachment;filename=" + filenamedisplay);
    	response.setContentLength((int)f.length());
    	
    	int pieceLength = 1024;
    	int readed = 0;
    	byte content[] = new byte[pieceLength];
    	java.io.OutputStream os = response.getOutputStream();
    	
    	try {
	    	while( (readed = fi.read(content)) > 0 ) {
	    		os.write(content , 0 , readed);
	    	}
    	} finally {
        	fi.close();
        	os.flush();
        	os.close();
    	}
    	
	}
}
