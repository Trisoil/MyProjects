package my.myaction.tool.core;

import my.myaction.tool.core.MyHttpRequest;
import my.myaction.tool.core.MyHttpResponse;
import my.myaction.tool.core.JSPContext;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.UnsupportedEncodingException;


/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-13
 * Time: 10:08:37
 * To change this template use File | Settings | File Templates.
 */
public class MyActionFilter implements Filter {
    private String requestencode="utf-8";
    private String responseencode="utf-8";
    public void init(FilterConfig filterConfig) throws ServletException {
          String requestencoding=filterConfig.getInitParameter("RequestEncoding");
          String responseencoding=filterConfig.getInitParameter("ResponseEncoding");
          if(requestencoding!=null){
              this.requestencode=requestencoding.trim();
          }
        if(responseencoding!=null){
              this.responseencode=responseencoding.trim();
          }
    }

    public void doFilter(ServletRequest servletRequest, ServletResponse servletResponse, FilterChain filterChain) throws IOException, ServletException {

        //
        HttpServletRequest request=(HttpServletRequest)servletRequest;
        HttpServletResponse response=(HttpServletResponse)servletResponse;


        request=new MyHttpRequest(request);
        response=new MyHttpResponse(request,response);
        JSPContext.put(request,response);

         //设置编码
        request.setCharacterEncoding(requestencode);
        response.setCharacterEncoding(responseencode);

        String Ajax_Response_Forward_Content=request.getParameter("Ajax_Response_Forward");
        if(Ajax_Response_Forward_Content!=null){
            response.getWriter().write(Ajax_Response_Forward_Content);
            return;
        }

          //默认为每个响应添加AjaxFormJS
            String append_myactionjs=request.getParameter("append_myactionjs");
            request.setAttribute("append_myactionjs",append_myactionjs);
            //默认响应类型为TEXT/HTML
            String contenttype=request.getParameter("contenttype");
            if(contenttype==null||contenttype.trim().equals("")) contenttype="text/html";
            response.setContentType(contenttype);


        
        String uri=request.getRequestURI();
        if(uri.endsWith(".jsp")){
             request.getRequestDispatcher(uri).forward(request,response);
             return;
        }
        
        try{
        filterChain.doFilter(request,response);    
       }catch(Exception ex){
           ex.printStackTrace();
           if("0".equals(request.getHeader("Ajax_Redirect_Type"))){//Ajax请求且自动处理重定向
               response.getWriter().write("对不起，发生了不可预知的异常,请检查您的输入或联系相关人员");
               return;
           }
           throw new ServletException("发生了异常");
       }
    }

    public void destroy() {

    }



    public static void main(String[] args) throws UnsupportedEncodingException {
        System.out.println(HttpServletRequest.class.getCanonicalName());
    }
}
