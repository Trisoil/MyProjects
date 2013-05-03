package my.myspring.tool; /**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-7
 * Time: 18:27:06
 * To change this template use File | Settings | File Templates.
 */

import org.springframework.context.support.ClassPathXmlApplicationContext;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import my.mysystem.tool.ContextUtil;


public class MySpringListener implements ServletContextListener {
    // -------------------------------------------------------
    // ServletContextListener implementation
    // -------------------------------------------------------
    public void contextInitialized(ServletContextEvent sce) {
        try {
            ContextUtil.initRootPath(sce.getServletContext());
            String rootpath =ContextUtil.rootPath;
            String springFiles = sce.getServletContext().getInitParameter("spring");
            if (!(springFiles == null || springFiles.trim().equals(""))) {
                String[] files = springFiles.trim().split(",");
                ClassPathXmlApplicationContext springContext = new ClassPathXmlApplicationContext(files);
                SpringBeanProxy.setApplicationContext(springContext);
                System.out.println("****************** init spring ... ok ******************");
            }
        } catch (Exception ex) {
            ex.printStackTrace();
            System.out.println("****************** init Spring error!!!");
        }
    }

    public void contextDestroyed(ServletContextEvent sce) {
        /* This method is invoked when the Servlet Context 
           (the Web application) is undeployed or 
           Application Server shuts down.
        */
    }
}
