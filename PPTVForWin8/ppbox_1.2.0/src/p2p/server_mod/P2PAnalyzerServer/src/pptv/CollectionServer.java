package pptv;

import java.io.File;

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
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

//import org.apache.commons.fileupload.*;
/*
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.*;
import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;
*/

//import org.apache.commons.fileupload.FileItem;
//import org.apache.commons.fileupload.disk.DiskFileItemFactory;
//import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 * Servlet implementation class CollectionServer
 */
@WebServlet("/ReportStatistics.jsp")
public class CollectionServer extends HttpServlet {
	private static final long serialVersionUID = 1L;
	
	private static final String logPath = new String("/logenv/LogCollectionPath/");
	
	private Connection mysqlConnection;
	private String mysqlUrl = new String("jdbc:mysql://192.168.21.210:3306/CollectionLog");
	private static String lastUsedDbTable = new String();
       
	private Timer timer = null;
	
    /**
     * @see HttpServlet#HttpServlet()
     */
    public CollectionServer() {
        super();
        // TODO Auto-generated constructor stub
    	File filePath = new File(this.logPath);
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
    	File filePath = new File(this.logPath + "/" + dateNowStr);
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
				this.mysqlConnection = DriverManager.getConnection(this.mysqlUrl,"root","123456");
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
						") ENGINE=MyISAM DEFAULT CHARSET=utf8;" ;
				System.out.println(sql);
				describeDB.execute(sql);
				this.lastUsedDbTable = currentData;
			}
			
			// 创建
			Statement stmt = this.mysqlConnection.createStatement();
			String insertSql = "INSERT INTO " + currentData + 
			" (address,peer_version,peer_guid,time,log_file_md5,resource_id,resource_name,statistics_id) VALUES(" +
			"'" + address + "'" + "," + "'" + peerVersion + "'" + "," + "'" + peerGuid + "'" + "," + "NOW()" + "," + "'" + logFileMd5 + "'" + "," + "'" + resourceID + "'" + "," +
			"'" + resourceName + "'" + "," + "'" + statisticsID + "'" + ")";
			System.out.println(insertSql);
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
			/*
			DiskFileItemFactory factory = new DiskFileItemFactory();
			ServletFileUpload file = new ServletFileUpload(factory);
			file.setSizeMax(-1);
			file.setFileSizeMax(-1);
			File realFile = null;
			try {
			     // 上传文件，并解析出所有的表单字段，包括普通字段和文件字段
			     List list = file.parseRequest(request);
			     FileItem fileitem=null;
			     for (Iterator it = list.iterator(); it.hasNext();) {
			    	 fileitem = (FileItem) it.next();
			    	 // 解析表单字的普通字段，即传过来的svrname和dbname，formname，path，unid,divid参数
			    	 if (fileitem.isFormField()) {
			    		 
			    	 }
			     }
			 } catch (Exception e) {
			     e.printStackTrace();
			     throw e;
			 }
			 */
			Random random = new Random();
			String fileNameMd5 = new String();
			
			for( int i = 0; i < 32; i++ ) {
				int n = Math.abs(random.nextInt()) % 16;
				fileNameMd5 += Integer.toHexString(n).toUpperCase();
			}
			System.out.println(fileNameMd5);
			
			String currentData = this.getCurrentData();

			System.out.println(request.getRemoteAddr());
			this.logToDB( request.getRemoteAddr() , peerVersion, peerId, fileNameMd5, 
					resourceId, resourceName, statisticsId , currentData);
			
			PrintWriter pw = new PrintWriter( new BufferedWriter(new FileWriter( 
					this.getCurrentLogPath(currentData) + "/" + fileNameMd5 ))); 
			ServletInputStream in = request.getInputStream(); 
			int i = in.read();
			while (i != -1) {
				pw.print((char) i);
				i = in.read();
			}
			pw.close();
			
/*
			String path = this.getServletContext().getRealPath("/upload");
			DiskFileItemFactory factory = new DiskFileItemFactory();
			ServletFileUpload up = new ServletFileUpload(factory);
			List<FileItem> ls = up.parseRequest(request);
			for (FileItem fileItem : ls) {
			}
*/
		} catch( Exception e ) {
			e.printStackTrace();
			response.setStatus(404, e.getMessage());
		}

		response.setStatus(200);
	}

}
