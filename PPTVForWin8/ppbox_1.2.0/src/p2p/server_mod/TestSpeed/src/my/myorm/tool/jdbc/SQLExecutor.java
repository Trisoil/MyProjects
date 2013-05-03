package my.myorm.tool.jdbc;


import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import javax.sql.DataSource;
import java.sql.*;
import java.util.List;
import java.util.LinkedList;
import java.util.Properties;
import java.text.SimpleDateFormat;

import my.myproperties.tool.PropUtil;
import my.myspring.tool.SpringBeanProxy;

public class SQLExecutor extends ResultSetHandler {

    private static Log logger = LogFactory.getLog("SQL_LOG");
    private static Log loggerDebug = LogFactory.getLog("SQL_DEBUG");

    private int maxrows = 100000;
    private int query_time_out_sec = 15;
    private Connection conn = null;
    private List params = new LinkedList();
    private PreparedStatement prepStatement = null;
    private boolean isCreateConnOnNew = false;
    private boolean isAllreadyInserted = false;
    private boolean isPageQueryed = false;
    private DataSource dataSource=null;

    private String dsName="dataSource1"; 


    private void createConn(boolean autoCommit) throws Exception {
        //DataSource datasource = dataSourceFactory.getDataSource();
        DataSource datasource = (DataSource) SpringBeanProxy.getBean(this.dsName);
        if(datasource == null) {
            throw new Exception("datasource==null");
        }
        conn = datasource.getConnection();
        conn.setAutoCommit(autoCommit);
    }

    protected SQLExecutor(boolean isCreateConnOnNew) throws Exception {
        if(isCreateConnOnNew) {
            createConn(false);
        }
        this.isCreateConnOnNew = isCreateConnOnNew;
    }

    protected SQLExecutor() throws Exception {
        this(false);
    }

    protected SQLExecutor(String dsname) throws Exception {
        if(dsname!=null&&!"".equals(dsname)) this.dsName=dsname;
    }

    public void addParam(Object param) {
        params.add(param);
    }

    public long execute(String sql) throws Exception {
        return execute(sql, false);
    }

    public long execute(String sql, boolean isAutoIncr) throws Exception {
        try {
            if(!isCreateConnOnNew) {
                createConn(true);
            }
            createPrepStatement(sql, true);
            long result = prepStatement.executeUpdate();
            if(isInsert(sql) && isAutoIncr) {//返回自增一ID
                isAllreadyInserted = true;
                sql = "select @@identity as id";
                result = queryForBean(sql, DefaultBean.class, true, "").getLong("id");
                isAllreadyInserted = false;
            }
            if(isCreateConnOnNew) {
                commit();
            }
            return result;
        } catch(Exception ex) {
            if(isCreateConnOnNew) {
                rollBack();
            }
            throw ex;
        } finally {
            if(!isCreateConnOnNew) {
                close();
            }
        }
    }

    public ResultList query(String sql, boolean isAll, Pages pages) throws Exception {
        return query(sql, isAll, pages, true);
    }

    public RecordMap queryForMap(String sql, boolean isAll) throws Exception {
        return queryForMap(sql, isAll, true);
    }

    public <T> T queryForBean(String sql, Class<T> t) throws Exception {
        return queryForBean(sql, t, "");
    }

    public <T> T queryForBean(String sql, Class<T> t, String tableAlia) throws Exception {
        return queryForBean(sql, t, true, tableAlia);
    }

    private ResultList query(String sql, boolean isAll, Pages pages, boolean clearParams) throws Exception {
        if(pages == null || !sql.trim().toLowerCase().startsWith("select ") || sql.toLowerCase().indexOf(" from ") <= 0 || (pages.getRequest() == null && pages.getCurPage() == -1)) {
            try {
                if(!isCreateConnOnNew && !isAllreadyInserted && !isPageQueryed) {
                    createConn(true);
                }
                createPrepStatement(sql, clearParams);
                ResultSet rs = prepStatement.executeQuery();
                //long begin=System.currentTimeMillis();
                ResultList list = handleResult(rs, isAll);
                //System.out.println(list.size()+":"+(System.currentTimeMillis()-begin)+"ms");
                try {
                    rs.close();
                } catch(Exception ex) {
                    ex.printStackTrace();
                }
                return list;
            } catch(Exception e) {
                throw e;
            }
            finally {
                if(!isCreateConnOnNew && !isAllreadyInserted && !isPageQueryed) {
                    close();
                }
            }

        } else {
            try {
                if(!isCreateConnOnNew) {
                    createConn(true);
                }
                sql = sql.trim();
                String newsql = "select count(" + pages.countField + ") as totalcount " +
                        (sql.toLowerCase().indexOf(" order by ") > 0 ?
                                sql.substring(sql.toLowerCase().indexOf(" from "), sql.toLowerCase().indexOf(" order by ")) :
                                sql.substring(sql.toLowerCase().indexOf(" from "))
                        );
                if(sql.toLowerCase().indexOf("group by") >= 0) {
                    newsql = "select count(" + pages.countField + ") as totalcount from (" +
                            (sql.toLowerCase().indexOf(" order by ") > 0 ?
                                    sql.substring(0, sql.toLowerCase().indexOf(" order by ")) :
                                    sql) + " ) as a";
                }
                isPageQueryed = true;
                int allCount = queryForBean(newsql, DefaultBean.class, false, "").getInt("totalcount");
                int[] aintPage = getPageRecordsNumber(allCount, pages.getCurPage() != -1 ? String.valueOf(pages.getCurPage()) : pages.getRequest().getParameter("ipage"), null, pages.getPageSize());
                sql = (sql.toLowerCase().indexOf(" limit ") > 0 ?
                        sql.substring(0, sql.toLowerCase().indexOf(" limit ")) : sql)
                        + " limit " + String.valueOf(aintPage[1]) + "," + String.valueOf(aintPage[2]);
                ResultList result = query(sql, isAll, null, true);
                result.setAllCount(allCount);
                isPageQueryed = false;
                return result;
            } catch(Exception ex) {
                throw ex;
            } finally {
                if(!isCreateConnOnNew) {
                    close();
                }
            }
        }
    }

    private RecordMap queryForMap(String sql, boolean isAll, boolean clearParams) throws Exception {
        ResultList list = query(sql, isAll, null, clearParams);
        if(list.size() == 0) return null;
        return list.get(0);
    }

    private <T> T queryForBean(String sql, Class<T> t, boolean clearParams, String tableAlia) throws Exception {
        RecordMap first = queryForMap(sql, false, clearParams);
        if(first == null) return null;
        return first.get(t, tableAlia);
    }

    protected void close() throws Exception {
        if(conn != null) {
            conn.close();
        }

    }

    protected void commit() throws Exception {
        if(conn != null)
            conn.commit();
    }

    protected void rollBack() throws Exception {
        if(conn != null)
            conn.rollback();
    }


    private void createPrepStatement(String sql, boolean clearParams) throws Exception {
        if(sql == null) sql = "";
        String wh = "======{$#?#$}======";
        String sql_text = replace(sql, "\\?", "{sql:wh}");
        sql_text = replace(sql_text, "?", wh);
        prepStatement = conn.prepareStatement(sql);
        for(int i = 0; i < params.size(); i++) {
            Object param = params.get(i);
            if(param == null) {
                prepStatement.setObject(i + 1, null);
                continue;
            }
            if(param instanceof Integer) {
                prepStatement.setInt(i + 1, ((Integer) param).intValue());
                sql_text = replaceOnce(sql_text, wh, String.valueOf(((Integer) param).intValue()));
            } else if(param instanceof String) {
                prepStatement.setString(i + 1, (String) param);
                sql_text = replaceOnce(sql_text, wh, "'" + dealSQLStringChar((String) param) + "'");
            } else if(param instanceof Double) {
                prepStatement.setDouble(i + 1, ((Double) param).doubleValue());
                sql_text = replaceOnce(sql_text, wh, String.valueOf(((Double) param).doubleValue()));
            } else if(param instanceof Float) {
                prepStatement.setFloat(i + 1, ((Float) param).floatValue());
                sql_text = replaceOnce(sql_text, wh, String.valueOf(((Float) param).floatValue()));
            } else if(param instanceof Long) {
                prepStatement.setFloat(i + 1, ((Long) param).longValue());
                sql_text = replaceOnce(sql_text, wh, String.valueOf(((Long) param).longValue()));
            } else if(param instanceof Boolean) {
                prepStatement.setBoolean(i + 1, ((Boolean) param).booleanValue());
                sql_text = replaceOnce(sql_text, wh, "'" + ((Boolean) param).booleanValue() + "'");
            } else if(param instanceof java.util.Date) {
                if(param == null) {
                    prepStatement.setObject(i + 1, null);
                    sql_text = replaceOnce(sql_text, wh, "NULL");
                } else {
                    SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    prepStatement.setString(i + 1, formatter.format((java.util.Date) param));
                    sql_text = replaceOnce(sql_text, wh, "'" + formatter.format((java.util.Date) param) + "'");
                }
            } else {
                params.clear();
                throw new Exception("unknown datatype");
            }
        }
        prepStatement.setMaxRows(maxrows);
        prepStatement.setQueryTimeout(query_time_out_sec);
        if(clearParams) {
            params.clear();
        }
        sql_text = replace(sql_text, "{sql:wh}", "\\?");
        Properties prop = PropUtil.getInstance("constant");
        String sql_out_permit = prop == null ? "" : prop.getProperty("LOG_SQL_OUT_PERMIT");
        boolean bOutSQL = sql_out_permit != null && "1".equals(sql_out_permit);
        if(logger != null && bOutSQL) logger.debug(sql_text);
        if(loggerDebug != null && !sql.trim().toLowerCase().startsWith("select")) loggerDebug.debug(sql_text);
    }

    private boolean isInsert(String sql) {
        if(sql.trim().substring(0, 6).equalsIgnoreCase("insert")) return true;
        return false;
    }

    private int[] getPageRecordsNumber(int recordsCount, String iPage, String stripage, int perPageCount) {
        int[] rint = {0, 0, 0};
        int ipage = 1;
        try {
            if(stripage == null || stripage.length() == 0) stripage = iPage;
            ipage = Integer.parseInt(stripage);
        }
        catch(Exception e) {
            ipage = 1;
        }

        if(recordsCount <= 0) {
            return rint;
        }
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        if(perPageCount < recordsCount) rint[0] = 1;
        rint[1] = (ipage - 1) * perPageCount;
        rint[2] = perPageCount;
        if(rint[1] < 0) rint[1] = 0;
        if(rint[1] > recordsCount) rint[1] = recordsCount;
        return rint;
    }

    private boolean isCreateConnOnNew() {
        return isCreateConnOnNew;
    }

    public static void setDataSourceFactory(DataSourceFactory dataSourceFactory) {
        //SQLExecutor.dataSourceFactory = dataSourceFactory;
    }

    public static String dealSQLStringChar(String s) {
        s = replace(s, "'", "\\'");
        return s;
    }

    public static String replace(String text, String repl, String with) {
        return replace(text, repl, with, -1);
    }

    public static String replaceOnce(String text, String repl, String with) {
        return replace(text, repl, with, 1);
    }

    public static String replace(String text, String repl, String with, int max) {
        if(text == null || repl == null || with == null || repl.length() == 0 ||
                max == 0) {
            return text;
        }
        StringBuffer buf = new StringBuffer(text.length());
        int start = 0, end = 0;
        while((end = text.indexOf(repl, start)) != -1) {
            buf.append(text.substring(start, end)).append(with);
            start = end + repl.length();

            if(--max == 0) {
                break;
            }
        }
        buf.append(text.substring(start));
        return buf.toString();
    }
}