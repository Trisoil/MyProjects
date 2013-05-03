package my.myproperties.tool;

import my.mysystem.tool.ContextUtil;

import javax.servlet.ServletContextListener;
import javax.servlet.ServletContextEvent;


import java.io.File;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-7
 * Time: 18:39:42
 * To change this template use File | Settings | File Templates.
 */
public class MyDynamicPropListener implements ServletContextListener {

    private static boolean isAlive=true;

    private static Object lock = new Object();

    public void contextInitialized(ServletContextEvent servletContextEvent) {
        String files = servletContextEvent.getServletContext().getInitParameter("dynamicprop");
        String proponlad = servletContextEvent.getServletContext().getInitParameter("proponload");
        try {
            if (PropUtil.getPropertyLoadCaller() == null && proponlad != null && !proponlad.trim().equals("")) {
                PropUtil.setPropertyLoadCaller((PropertyLoadCaller) Class.forName(proponlad.trim()).newInstance());
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        if (files == null || files.trim().equals("")) return;
        try {
            //获取根路径
            ContextUtil.initRootPath(servletContextEvent.getServletContext());
            String rootpath = ContextUtil.rootPath;
            String[] propFiles = files.trim().split(",");
            for (int i = 0; i < propFiles.length; i++) {
                String[] config = propFiles[i].trim().split(":");
                long inteval = new Long(config[1]);
                if (inteval == 0) inteval = 10 * 60 * 1000;
                PropUtil.loadProperties(rootpath + config[0]);
                new DynamicPropReloader(rootpath + config[0], inteval).start();
            }
            System.out.println("****************** init dynamicprop ... ok ******************");
        } catch (Exception ex) {
            ex.printStackTrace();
            System.out.println("****************** init dynamicprop error!!!");
        }
    }

    public void contextDestroyed(ServletContextEvent servletContextEvent) {
        synchronized (lock) {
            try {
                isAlive=false;
                lock.notify();
            } catch (Exception ex) {
            }
        }

        //System.exit(0);
    }

    private class DynamicPropReloader extends Thread {

        Log logger= LogFactory.getLog("DynamicPropertiesLoader");
        private String filePath = null;
        private long inteval = 10 * 60 * 1000;
        private long lastModifyTime = 0;

        public DynamicPropReloader(String filePath, long inteval) {
            this.filePath = filePath;
            this.inteval = inteval;
        }

        public void run() {
            while (isAlive) {
                try {
                    File file = new File(filePath);
                    long begin = System.currentTimeMillis();
                    if (file.lastModified() <= lastModifyTime) {
                    } else {
                        PropUtil.loadProperties(filePath);
                        lastModifyTime = file.lastModified();

                        logger.info("");
                        logger.info("#######" +
                                file.getName() + " Reload OK,use " + (System.currentTimeMillis() - begin) + "ms "+
                        "#######");
                        logger.info("");
                    }
                    synchronized (lock) {
                        lock.wait(inteval);
                    }
                    if(!isAlive){
                        System.out.println("**************** properties reload task exit.");
                        break;
                    }


                } catch (Exception ex) {
                    ex.printStackTrace();
                    System.out.println("**************** "+ filePath + ":error!");
                }
            }
        }

      
    }
}
