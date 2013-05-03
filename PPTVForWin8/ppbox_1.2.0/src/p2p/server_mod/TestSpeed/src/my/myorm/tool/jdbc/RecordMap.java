package my.myorm.tool.jdbc;

import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.LinkedList;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-3
 * Time: 10:39:26
 * To change this template use File | Settings | File Templates.
 */
public class RecordMap {
    private Map<Class, List<Object[]>> data = new HashMap<Class, List<Object[]>>(8, 0.75F);

    public void put(Class key, Object value) throws Exception {
        put(key, value, "");
    }

    public void put(Class key, Object value, String tableAlia) throws Exception {
        if(!data.containsKey(key)) {
            data.put(key, new LinkedList<Object[]>());
        }
        String valuekey = tableAlia == null ? "" : tableAlia.trim().toLowerCase();
        data.get(key).add(new Object[]{valuekey, value});
    }

    public <T> T get(Class<T> key) throws Exception {
        return get(key, "");
    }

    public <T> T get(Class<T> key, String tableAlia) throws Exception {
        List<Object[]> value = data.get(key);
        if(value == null || value.size() == 0) return null;
        if(value.size() == 1) return (T) value.get(0)[1];
        if(tableAlia == null || tableAlia.trim().equals("")) return null;
        for(int i = 0; i < value.size(); i++) {
            Object[] objs = value.get(i);
            if((objs[0] + "").equals(tableAlia.trim().toLowerCase())) {
                return (T) objs[1];
            }
        }
        return null;
    }
}
