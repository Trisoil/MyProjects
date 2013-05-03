package my.myorm.tool.orm;

import javax.servlet.ServletContextListener;
import javax.servlet.ServletContextEvent;

import my.myorm.tool.jdbc.DataSourceFactory;
import my.mysystem.tool.ContextUtil;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-8
 * Time: 19:08:28
 * To change this template use File | Settings | File Templates.
 */
public class MyORMListener  implements ServletContextListener {
    public void contextInitialized(ServletContextEvent servletContextEvent) {
        try{
            ContextUtil.initRootPath(servletContextEvent.getServletContext());
            String rootpath =ContextUtil.rootPath;
             String datasourceFactory = servletContextEvent.getServletContext().getInitParameter("myorm");
             Class clazz=Class.forName(datasourceFactory);
             DataSourceFactory dataSourceFactory=(DataSourceFactory)clazz.newInstance();
             ORMInitializer.init(rootpath+"WEB-INF/classes/table_bean_map.xml",dataSourceFactory);
             System.out.println("****************** init myorm ... ok ******************");
        }catch(Exception ex){
            ex.printStackTrace();
            System.out.println("****************** init myorm error!!!");
        }
    }

    public void contextDestroyed(ServletContextEvent servletContextEvent) {
        //To change body of implemented methods use File | Settings | File Templates.
    }
}
