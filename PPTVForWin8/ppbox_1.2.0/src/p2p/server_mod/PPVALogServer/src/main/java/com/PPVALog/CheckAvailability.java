package com.PPVALog;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.io.FileSystemUtils;

import my.myproperties.tool.PropUtil;

public class CheckAvailability extends HttpServlet{

	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		if(!isFileAuthority()){
			resp.setStatus(500);
			servletOutProcess(resp,"error: file authority is wrong");
			return;
		}
		if(!isConnectionValid()){
			resp.setStatus(500);
			servletOutProcess(resp,"error: mysql connect error");
			return;
		}
		if(!isDiskFree()){
			resp.setStatus(500);
			servletOutProcess(resp,"error: not enough space ");
			return;
		}	
		resp.setStatus(200);
		servletOutProcess(resp,"succuss 200");
		return;
	}
	
	private boolean isFileAuthority(){		
		String path = PropUtil.getInstance("constant").getProperty("PPVALogPath");
		File tempFile = new File(path);
		if(tempFile.exists()){
			try{
				tempFile= new File(path+"/a.temp");
				tempFile.delete();
				return true;
			}catch (Exception e){
				return false;	
			}
		}else{
			return false;
		}
	} 
	
	private boolean isConnectionValid(){
		try {
			Connection mysqlConnection;
		    mysqlConnection = DriverManager.getConnection(
		         PropUtil.getInstance("constant").getProperty("PPVALogMySqlUrl"),
		         PropUtil.getInstance("constant").getProperty("PPVALogMySqlUserName"),
		         PropUtil.getInstance("constant").getProperty("PPVALogMySqlPassword"));
		    if(!mysqlConnection.isClosed()){
		    	mysqlConnection.close();
		    	return true;
		    }else{
		    	mysqlConnection.close();
		    	return false;
		    }		    
		} catch(Exception e) {
			return false;
		}
	}
	
	private boolean isDiskFree(){
		try {
			String path = PropUtil.getInstance("constant").getProperty("PPVALogPath");
		    long freeSpace = FileSystemUtils.freeSpaceKb(path)/1024/1024;
		    //System.out.println("freeSpace="+freeSpace);
		    if(freeSpace < 20){  return false;}
		    else return true;
		}catch (Exception e){
			return false;
		}
	}
	
	private void servletOutProcess(HttpServletResponse response,String errorString){
		response.setContentType("text/html; charset=UTF-8");
		PrintWriter out;
		try {
			out = response.getWriter();
			out.println(errorString);
			out.flush();
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	

}
