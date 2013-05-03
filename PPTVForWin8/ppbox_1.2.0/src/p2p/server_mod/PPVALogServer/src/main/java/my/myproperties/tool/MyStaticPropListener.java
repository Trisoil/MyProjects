package my.myproperties.tool;

import my.myproperties.tool.PropUtil;
import my.mysystem.tool.ContextUtil;

import javax.servlet.*;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-7
 * Time: 18:38:57
 * To change this template use File | Settings | File Templates.
 */

public class MyStaticPropListener implements ServletContextListener {


    public void contextInitialized(ServletContextEvent servletContextEvent) {
        String files = servletContextEvent.getServletContext().getInitParameter("staticprop");
        String proponlad=servletContextEvent.getServletContext().getInitParameter("proponload");
        try{
             if(PropUtil.getPropertyLoadCaller()==null&&proponlad!=null&&!proponlad.trim().equals("")){
                 PropUtil.setPropertyLoadCaller((PropertyLoadCaller)Class.forName(proponlad.trim()).newInstance());
             }
        }catch(Exception ex){
            ex.printStackTrace();
        }
        if (files == null || files.trim().equals("")) return;
        try {
            //获取根路径
            ContextUtil.initRootPath(servletContextEvent.getServletContext());
            String rootpath =ContextUtil.rootPath;
            String[] propFiles = files.trim().split(",");
            for (int i = 0; i < propFiles.length; i++) {
                String path = rootpath + propFiles[i].trim();
                PropUtil.loadProperties(path); 
            }
            System.out.println("****************** init staticprop ... ok ******************");
        } catch (Exception ex) {
            ex.printStackTrace();
            System.out.println("****************** init staticprop error!!!");
        }
    }

    public void contextDestroyed(ServletContextEvent servletContextEvent) {
        //To change body of implemented methods use File | Settings | File Templates.
    }
}
