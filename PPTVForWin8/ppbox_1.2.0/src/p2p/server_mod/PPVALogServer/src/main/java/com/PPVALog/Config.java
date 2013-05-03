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

public class Config {
	public static String configContent;
	public static Config instance;
	
	public static Config getConfig() {
		if(null == instance) {
			instance = new Config();
		}
		
		return instance;
	}
	public String getConfigValue( String xmlFilePath ) {
		try {
			// 检测版本情况
			if( configContent == null ) {
				// 读取出文件进入内存
				FileInputStream fi = new FileInputStream(xmlFilePath);
				try {
					byte content[] = new byte[10240];
					int readed = fi.read(content);
					byte configTemp[] = new byte[readed];
					System.arraycopy( content , 0, configTemp, 0, readed);
					configContent = new String(configTemp);
				} catch( IOException ioe ) {
					throw ioe;
				} finally {
					fi.close();
				}
			}
			
			return configContent;
		} catch( Exception e ) {
			return null;
		}
	}
}
