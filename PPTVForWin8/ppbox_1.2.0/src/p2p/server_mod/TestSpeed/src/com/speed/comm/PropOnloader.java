package com.speed.comm;

import my.myproperties.tool.PropertyLoadCaller;

import com.speed.comm.Constant;

/**
 * Created by IntelliJ IDEA.
 *
 * @author Xu Jianbo
 * @version 1.0
 *          Date: 2010-4-16
 *          Time: 16:33:22
 */
public class PropOnloader implements PropertyLoadCaller {

    public void onLoad(String filename) throws java.lang.Exception {
        if(filename != null && filename.indexOf("constant") >= 0) {
            Constant.init();
        }
    }
}
