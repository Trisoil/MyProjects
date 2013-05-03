package tool;

import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.io.xml.DomDriver;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2009-11-24
 * Time: 10:48:55
 * To change this template use File | Settings | File Templates.
 */
public class ObjectSerialer {
     public static  String toXML(Object obj) throws Exception{
        XStream xs = new XStream(new DomDriver());
        String xml=xs.toXML(obj);
        return xml;

    }
    public static  Object toObj(String json)  throws Exception{
        XStream xs = new XStream(new DomDriver());
		return xs.fromXML(json);
    }
}
