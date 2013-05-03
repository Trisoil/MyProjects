package com.PPVALog;

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
//import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;

import com.PPVALog.dao.LogProcessChartLiveDao;
import com.PPVALog.dao.LogProcessChartVodDao;
import com.PPVALog.dao.base.LogProcessChartDaoBase;

import tool.XMLUtil;

import my.myproperties.tool.PropUtil;

public class ChartCreate extends HttpServlet {
	LogProcessChartDaoBase []chartprocess={};

	public ChartCreate() {
		super();
	}

	public void destroy() {
		super.destroy();
	}
	
	@Override
	public void init(ServletConfig config) throws ServletException {
		// TODO Auto-generated method stub
		super.init(config);
		chartprocess = new  LogProcessChartDaoBase[2];
		chartprocess[0] = new LogProcessChartVodDao();
		chartprocess[1] = new LogProcessChartLiveDao();
	}

	protected void doGet(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		//
		// System.out.println("chartCreate_log "+"doget");
		String file_id = request.getParameter("file_id");
		String date = request.getParameter("date");
		String index_id = request.getParameter("index_id");
		String statistics_id = request.getParameter("statistics_id");
		// System.out.println("chartCreate_log "+"index_id= " + index_id +
		// " date=" + date);

		if (file_id.length() == 0 || date.length() == 0) {
			servletErrorProcess(response,"无数据，请返回","");
			return; // 错误返回
		}
		
		LogProcessChartDaoBase currentChartProcess = null;
		//System.out.println("length="+chartprocess.length);
		for(int i1=0; i1<chartprocess.length; i1++){
			if(chartprocess[i1].typeMatch(statistics_id)){	
				currentChartProcess=chartprocess[i1];
				break;
			}
		}
		if(currentChartProcess==null){
			servletErrorProcess(response,"statistics类型无法处理","");
			return;  //未找到匹配
		}
		currentChartProcess.setDatePaths(file_id, date);
		try {
			currentChartProcess.processXML();
			response.setContentType("text/html; charset=UTF-8");
			response.sendRedirect(currentChartProcess.getRedirectURL(index_id));
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			servletErrorProcess(response,"解析gz包出错","");
			e.printStackTrace();
		} catch (JDOMException e) {
			// TODO Auto-generated catch block
			servletErrorProcess(response,"xml字段提取出错","");
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			servletErrorProcess(response,"出错",e.getMessage());
			e.printStackTrace();
		}
		return;		
	}
	
	private void servletErrorProcess(HttpServletResponse response,String errorString,String errorMessage){
		response.setContentType("text/html; charset=UTF-8");
		PrintWriter out;
		try {
			out = response.getWriter();
			out.println(errorString);
			out.println("<a href= \"javascript:history.back()\">返回</a>");
			out.println("<br>");
			out.println(errorMessage);
			out.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
