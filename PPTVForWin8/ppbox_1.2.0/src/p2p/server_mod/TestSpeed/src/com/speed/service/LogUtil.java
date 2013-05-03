package com.speed.service;

import my.myorm.tool.jdbc.SQLExecutor;
import my.myorm.tool.jdbc.SQLManager;

import javax.servlet.http.HttpServletRequest;

import com.speed.bean.SysLog;
import com.speed.comm.Util;
import com.speed.dao.DAOManager;

import java.util.Date;

/**
 * Created by IntelliJ IDEA.
 *
 * @author Xu Jianbo
 * @version 1.0
 *          Date: 2010-4-17
 *          Time: 15:38:08
 */
public class LogUtil {

    public static void writeLog(String username, String operate, String content,
                                HttpServletRequest request, SQLExecutor dbcon) {
        try {
            if(dbcon == null) dbcon = SQLManager.getSQLExecutor();
            SysLog log = new SysLog();
            log.setUserName(username);
            log.setOperate(operate);
            log.setContent(content);
            log.setIP(request.getRemoteAddr());
            log.setCreateTime(new Date());
            DAOManager.sysLogDao.save(log);
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

}
