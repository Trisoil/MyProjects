package my.mylog4j.tool; /**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-8
 * Time: 17:49:27
 * To change this template use File | Settings | File Templates.
 */

import org.apache.log4j.PropertyConfigurator;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import my.mysystem.tool.ContextUtil;


public class MyLog4jListener implements ServletContextListener{


    public void contextInitialized(ServletContextEvent servletContextEvent) {
        try{
       ContextUtil.initRootPath(servletContextEvent.getServletContext());
            String rootpath =ContextUtil.rootPath;
           String file = servletContextEvent.getServletContext().getInitParameter("log4j");
           if(file!=null&&!file.trim().equals("")){
            PropertyConfigurator.configure ( rootpath+file.trim() ) ;
           }
            System.out.println("****************** init log4j ... ok ******************");
        }catch(Exception ex){
            ex.printStackTrace();
            System.out.println("****************** init log4j error!!!");
        }
    }

    public void contextDestroyed(ServletContextEvent servletContextEvent) {
        //To change body of implemented methods use File | Settings | File Templates.
    }
}
