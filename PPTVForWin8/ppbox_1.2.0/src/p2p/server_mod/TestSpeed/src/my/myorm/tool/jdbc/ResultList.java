package my.myorm.tool.jdbc;


import java.util.List;
import java.util.ArrayList;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-4
 * Time: 15:55:58
 * To change this template use File | Settings | File Templates.
 */
public class ResultList{
    private List<String> listColumns=new ArrayList<String>();
    private List data=new ArrayList();
    private int allCount=0;

    public int getAllCount() {
        return allCount;
    }

    public void setAllCount(int allCount) {
        this.allCount = allCount;
    }

    public int size(){
        return data.size();
    }
    public RecordMap get(int i){
        return (RecordMap)data.get(i);
    }
    public void add(RecordMap u){
        data.add(u);
    }

    public List<String> getListColumns() {
        return listColumns;
    }

    public void setListColumns(List<String> listColumns) {
        this.listColumns = listColumns;
    }
}
