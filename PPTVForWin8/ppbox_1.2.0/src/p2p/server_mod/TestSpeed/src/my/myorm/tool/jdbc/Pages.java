package my.myorm.tool.jdbc;

import javax.servlet.http.HttpServletRequest;

/**
 * Created by IntelliJ IDEA.
 *
 * @author jianbo.xu
 * @version 1.0
 *          Date: 2006-3-29
 *          Time: 9:33:37
 */
public class Pages {

    public int pageSize;
    public int allCount=0;
    public String countField;
    public HttpServletRequest request;

    public int curPage;

    public Pages() {
        this.pageSize = 20;
        this.countField = "*";
        this.request = null;
        this.curPage = -1;
    }

    public Pages(int pagesize, String countfield) {
        this.pageSize = pagesize;
        this.countField = countfield == null || countfield.length() == 0 ? "*" : countfield;
        this.curPage = -1;
    }

    public Pages(int pagesize, String countfield, HttpServletRequest request) {
        this.pageSize = pagesize;
        this.countField = countfield == null || countfield.length() == 0 ? "*" : countfield;
        this.request = request;
        this.curPage = -1;
    }

    public int getPageSize() {
        return pageSize;
    }

    public void setPageSize(int pageSize) {
        this.pageSize = pageSize;
    }

    public String getCountField() {
        return countField;
    }

    public void setCountField(String countField) {
        this.countField = countField;
    }

    public HttpServletRequest getRequest() {
        return request;
    }

    public void setRequest(HttpServletRequest request) {
        this.request = request;
    }

    public int getCurPage() {
        return curPage;
    }

    public void setCurPage(int curPage) {
        this.curPage = curPage;
    }
}
