package my.myaction.tool.core;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServletRequest;
import java.io.IOException;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-6
 * Time: 14:53:15
 * To change this template use File | Settings | File Templates.
 */
public class MyRequestDispatcher implements RequestDispatcher {
    private RequestDispatcher dispatcher=null;
    private String requestURI="";
    protected  MyRequestDispatcher(RequestDispatcher dispatcher,String requestURI){
             this.dispatcher=dispatcher;
        this.requestURI=requestURI;
    }
    public void forward(ServletRequest servletRequest, ServletResponse servletResponse) throws ServletException, IOException {
        HttpServletRequest request=(HttpServletRequest)servletRequest;
        HttpServletResponse response=(HttpServletResponse)servletResponse;
        String append_myactionjs=(String)request.getAttribute("append_myactionjs");
        if(append_myactionjs==null) append_myactionjs="1";
        else append_myactionjs=append_myactionjs.trim();
        if(requestURI.endsWith("jsp")&&append_myactionjs.equals("1")){
            if(response.isCommitted()){
                throw new IllegalStateException("Cannot forward after response has been committed");
            }
              //清空缓冲区
              response.resetBuffer();
              include(request,response);
              request.getRequestDispatcher("/MyActionComm.jsp").include(request,response);
        }else{
             dispatcher.forward(servletRequest,servletResponse);
       }

    }

    public void include(ServletRequest servletRequest, ServletResponse servletResponse) throws ServletException, IOException {
          dispatcher.include(servletRequest,servletResponse);
    }
}
