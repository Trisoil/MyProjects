package com.speed.comm;


import tool.SuperString;

import com.speed.comm.Util;

import javax.servlet.ServletContext;

public class Constant {

    public static String ROOTPATH = "";
    public static String CLASSPATH = "";
    public static ServletContext context = null;

    public static String DES_KEY = "";

    public static String RESSUFFIX = "";

    public static String CHARSET = "utf-8";

    public static String[] OBJECTTYPE = {"电影", "电视剧", "综艺节目", "动漫卡通", "人物", "图片", "图集", "集", "电视台", "体育", "资讯", "新闻", "游戏"};
    public static String[] OBJECTTABLE = {"movie", "tv", "show", "cartoon", "people", "picture", "setpic", "set", "tvs", "sport", "info", "news", "game"};
    public static String[] CONTENTFIELD = {"MovieID", "TvID", "ShowID", "CartoonID", "PeopleID", "PictureID", "SetPicID", "SetID", "tvsid", "sportid", "infoid", "newsid", "gameid"};
    public static String[] CONTENTTABLE = {"bk_movie", "bk_tv", "bk_show", "bk_cartoon", "bk_people", "bk_picture", "bk_setpic", "bk_set", "bk_tvs", "bk_sport", "bk_info", "bk_news", "bk_game"};

    public static void init() {
        RESSUFFIX = SuperString.notNull(Util.getProperties("RES_SUFFIX"));
        DES_KEY = SuperString.notNull(Util.getProperties("CONST_DES_KEY"));
    }
}