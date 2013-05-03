package my.myorm.tool.orm;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-3
 * Time: 10:00:10
 * To change this template use File | Settings | File Templates.
 */
public class TableUtil {
    //是否为分表
    public static boolean isSubmeter(String table) {
        //以下划线和英文字母结尾
        if(table.indexOf("_") == -1) return false;
        String exes = TableBeanMap.getSplitSpec();
        String ex = table.toLowerCase().substring(table.lastIndexOf("_"));
        if(exes.indexOf(ex) > -1) return true;

        return false;
    }

    //获取总表
    public static String getSummary(String table) {
        if(isSubmeter(table)) {
            int index = table.lastIndexOf("_");
            return table.substring(0, index);
        } else {
            return table;
        }
    }

}
