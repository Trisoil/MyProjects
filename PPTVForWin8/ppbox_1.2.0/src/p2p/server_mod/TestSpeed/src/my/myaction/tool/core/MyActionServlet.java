package my.myaction.tool.core;

import my.myaction.tool.action.ActionInitializer;
import my.myaction.tool.action.ActionExecutor;
import my.mysystem.tool.ContextUtil;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import java.io.IOException;


/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-9
 * Time: 11:07:52
 * To change this template use File | Settings | File Templates.
 */
public class MyActionServlet extends HttpServlet {
     public void init(javax.servlet.ServletConfig servletConfig) throws javax.servlet.ServletException {
         super.init(servletConfig);
         try{
         ContextUtil.initRootPath(servletConfig.getServletContext());
         String action=servletConfig.getServletContext().getInitParameter("myaction");
         ActionInitializer.init(action);
         System.out.println("****************** init myaction ... ok ******************");
         }catch(Exception ex){
             ex.printStackTrace();
             System.out.println("****************** init myaction error!!!");
             System.exit(0);
         }
     }
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        try{
        String uri=request.getRequestURI();
        if(uri.startsWith("/")){
            uri=uri.substring(1);
        }
        if(uri.endsWith("/")){
           uri=uri.substring(uri.length()-1); 
        }
        String[] params=uri.split("/");
        String classname=params[1];
        for(int i=2;i<params.length-1;i++){
            classname+="."+params[i];
        }
        String methodname=params[params.length-1];
        ActionExecutor.execute(classname,methodname);
         
        }catch(Exception ex){
            ex.printStackTrace();
           if("0".equals(request.getHeader("Ajax_Redirect_Type"))){//Ajax请求且自动处理重定向
               response.setHeader("Ajax_Response_Type","forward:");
              response.getWriter().write("对不起，发生了不可预知的异常,请检查您的输入或联系相关人员");
               return;
           }
           throw new ServletException("发生了异常");
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
           this.doPost(request,response);
    }
}
