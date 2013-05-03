package com.PPVALog;

import java.io.File;
import java.net.*;

import java.io.DataOutputStream;

import java.util.Random;
import java.math.*;
import java.util.*;
import java.text.SimpleDateFormat;
import java.sql.*;

import java.io.*;

import java.io.IOException;
import java.util.List;
import java.util.Iterator;

import java.util.Timer;
import java.util.TimerTask;
import java.nio.CharBuffer;

import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
//import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;
import org.apache.log4j.spi.LoggerFactory;

import tool.ServletUtils;

import my.myproperties.tool.PropUtil;

/**
 * Servlet implementation class CollectionServer
 */
//@WebServlet("/ReportStatistics.jsp")
public class ReportStatistics extends HttpServlet {
	private static final Logger logger = Logger.getLogger(ReportStatistics.class);
	private static final long serialVersionUID = 1L;
	
	private Connection mysqlConnection;
	private static String lastUsedDbTable = new String();
       
	private Timer timer = null;
	
    /**
     * @see HttpServlet#HttpServlet()
     */
    public ReportStatistics() {
        super();
        // TODO Auto-generated constructor stub
    	File filePath = new File(PropUtil.getInstance("constant").getProperty("PPVALogPath"));
    	filePath.mkdirs();
    }

    public void destroy() {   
        super.destroy();
        if( timer!=null ) {
//        	timer = new Timer(true);
//        	timer.schedule(task1, delay * 60 * 1000, delay * 60 * 1000);
        }   
    }
    
    private String getCurrentData() {
    	java.util.Date dateNow=new java.util.Date();
    	SimpleDateFormat dateFormat=new SimpleDateFormat ("yyyy_MM_dd");
    	return dateFormat.format(dateNow);
    }
    
    private String getCurrentLogPath(String dateNowStr) {
    	// 在这里创建路径
    	File filePath = new File(PropUtil.getInstance("constant").getProperty("PPVALogPath") + "/" + dateNowStr);
    	filePath.mkdir();
    	return filePath.getAbsolutePath();
    }
    
	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		response.setStatus(404);
	}

	private boolean logToDB( String address , String peerVersion , 
			String peerGuid , String logFileMd5 , String resourceID , 
			String resourceName , String statisticsID , String currentData) throws Exception {
		try {
			if( this.mysqlConnection == null ) {
				// 还没有连接,或者已经出错
				;
				this.mysqlConnection = DriverManager.getConnection(
						PropUtil.getInstance("constant").getProperty("PPVALogMySqlUrl"),
						PropUtil.getInstance("constant").getProperty("PPVALogMySqlUserName"),
						PropUtil.getInstance("constant").getProperty("PPVALogMySqlPassword"));
			}
			
			// 查看表是否存在
			if( !lastUsedDbTable.equals( currentData ) ){
				Statement describeDB = this.mysqlConnection.createStatement();
				String sql = "CREATE TABLE IF NOT EXISTS " + currentData + "(" +
						"id int(11) NOT NULL AUTO_INCREMENT," +
						"address tinytext NOT NULL," +
						"peer_version char(32) NOT NULL," +
						"peer_guid char(32) NOT NULL," +
						"time datetime NOT NULL," +
						"log_file_md5 char(32) NOT NULL," +
						"resource_id char(32) NOT NULL," +
						"resource_name text NOT NULL," +
						"statistics_id text NOT NULL," +
						"PRIMARY KEY (`id`)" +
						") ENGINE=InnoDB DEFAULT CHARSET=utf8;" ;
//				System.out.println(sql);
				logger.info(sql);
				describeDB.execute(sql);
				this.lastUsedDbTable = currentData;
			}
			
			// 创建
			Statement stmt = this.mysqlConnection.createStatement();
			String insertSql = "INSERT INTO " + currentData + 
			" (address,peer_version,peer_guid,time,log_file_md5,resource_id,resource_name,statistics_id) VALUES(" +
			"'" + address + "'" + "," + "'" + peerVersion + "'" + "," + "'" + peerGuid + "'" + "," + "NOW()" + "," + "'" + logFileMd5 + "'" + "," + "'" + resourceID + "'" + "," +
			"'" + resourceName + "'" + "," + "'" + statisticsID + "'" + ")";
//			System.out.println(insertSql);
			logger.info(insertSql);
			stmt.execute(insertSql);
		} catch(Exception e) {
			e.printStackTrace();
			// 将 connection 设置为 null
			this.mysqlConnection = null;
			throw e;
		}
		
		return true;
	}
	
	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		// 在这里处理请求
		//
		try {
			String peerVersion = request.getParameter("peer_version");
			String peerId = request.getParameter("peer_id");
			String resourceId = request.getParameter("rid");
			String statisticsId = request.getParameter("statistics_id");
			String resourceName = request.getParameter("resourceName");

			Random random = new Random();
			String fileNameMd5 = new String();
			
			for( int i = 0; i < 32; i++ ) {
				int n = Math.abs(random.nextInt()) % 16;
				fileNameMd5 += Integer.toHexString(n).toUpperCase();
			}

			String currentData = this.getCurrentData();

			String fileName = this.getCurrentLogPath(currentData) + "/" + fileNameMd5 + ".gz";
			File f = new File(fileName);
			if (f.exists()) {
//				System.out.println("file exist: " + fileName );
				logger.error("file exist: " + fileName);
				return;
			}
			if (!f.createNewFile()) {
//				System.out.println("createNewFile failed: " + fileName );
				logger.error("createNewFile failed: " + fileName);
				return;
			}

			this.logToDB( ServletUtils.getServerIpByRequest(request) , peerVersion, peerId, fileNameMd5, 
					resourceId, resourceName, statisticsId , currentData);
			
			DataOutputStream output = new DataOutputStream(new FileOutputStream(fileName));
			ServletInputStream in = request.getInputStream();
			int arg0 = 1;
			 
			while(arg0 > 0) {
				byte tmp[] = new byte[1024];
				arg0 = in.read(tmp);
				if( arg0 > 0 )
					output.write(tmp, 0, arg0);
			}
			
			output.close();
		} catch( Exception e ) {
			e.printStackTrace();
			response.setStatus(404, e.getMessage());
		}

		response.setStatus(200);
	}

}
