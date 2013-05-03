package my.myorm.tool.jdbc;

import java.sql.ResultSet;
import java.util.Map;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-2
 * Time: 11:12:53
 * To change this template use File | Settings | File Templates.
 */
public interface RowMapper {
    public Object mapRow(ResultSet rs, Map<String,String> columns) throws Exception;
}
