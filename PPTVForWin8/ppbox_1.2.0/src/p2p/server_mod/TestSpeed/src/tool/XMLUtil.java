package tool;

import org.jdom.input.SAXBuilder;
import org.jdom.Document;
import org.jdom.Element;

import java.io.InputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.List;

/**
 * Created by IntelliJ IDEA.
 *
 * @author Xu Jianbo
 * @version 1.0
 *          Date: 2008-3-31
 *          Time: 14:53:38
 */
public class XMLUtil {

    public static void main(String[] args) {

        try {
            String path = "http://local.pplive.com:8080/getUrlList.jsp?key=20110414141801963480";
            //String path="http://211.151.32.238:8080/xihttp_pptv_xml_movie.jsp?kw=%E6%97%A0%E6%9E%81";
            SAXBuilder sb = new SAXBuilder();
            URL url = new URL(path);
            InputStream is = url.openStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(is));
            Document document = sb.build(reader);
            Element rootElement = document.getRootElement();
            List<Element> list=rootElement.getChildren();
            if(list==null) return;
            for(Element e:list){
                System.out.println(e.getChildText("id"));
                System.out.println(e.getChildText("test_time"));
                System.out.println(e.getChildText("url"));
            }

        } catch(Exception e) {
            e.printStackTrace();
        }
    }


}
