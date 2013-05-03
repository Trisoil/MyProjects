package tool;

import org.jdom.input.SAXBuilder;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.URL;
import java.util.List;

/**
 * Created by IntelliJ IDEA.
 * 
 * @author Xu Jianbo
 * @version 1.0 Date: 2008-3-31 Time: 14:53:38
 */
public class XMLUtil {

	public static void main(String[] args) {

		try {
			String path = "http://local.pplive.com:8080/getUrlList.jsp?key=20110414141801963480";
			// String
			// path="http://211.151.32.238:8080/xihttp_pptv_xml_movie.jsp?kw=%E6%97%A0%E6%9E%81";
			SAXBuilder sb = new SAXBuilder();
			URL url = new URL(path);
			InputStream is = url.openStream();
			BufferedReader reader = new BufferedReader(
					new InputStreamReader(is));
			Document document = sb.build(reader);
			Element rootElement = document.getRootElement();
			List<Element> list = rootElement.getChildren();
			if (list == null)
				return;
			for (Element e : list) {
				System.out.println(e.getChildText("id"));
				System.out.println(e.getChildText("test_time"));
				System.out.println(e.getChildText("url"));
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static Document getDocument() throws JDOMException, Exception {
		return new Document();
	}

	public static Document getDocument(String path) throws JDOMException,
			Exception {
		InputStreamReader reader = null;
		FileInputStream  fis = null;
		try
			{
			SAXBuilder sb = new SAXBuilder();
			/*URL url = new URL(path);
			InputStream is = url.openStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));*/
			fis = new FileInputStream(path);
			reader = new InputStreamReader(fis, "UTF-8");
			sb.setValidation(false);
	        return sb.build(reader); 
		}
		finally{
			if (null != reader)
            {
                reader.close();
            }
            if (null != fis)
            {
                fis.close();
            } 
		}
	}

	public static Element getRootElement(Document xml) throws JDOMException,
			Exception {
		Element rootElement = xml.getRootElement();
		return rootElement;
	}

	public static List<Element> getChildrenElement(Element parentElement)
			throws JDOMException, Exception {
		List<Element> list = parentElement.getChildren();
		return list;
	}
	
	public static void createRootElement(Document xml, String rootString) throws JDOMException, Exception {
		Element xmlRoot = new Element(rootString);
		xml.setRootElement(xmlRoot); 
	}
	
	public static void addChildElement(Element parent,Element child) throws JDOMException, Exception {
		parent.addContent(child);
	}
	
	public static void saveXML(Document xml) throws JDOMException, Exception {
		OutputStreamWriter writer = null;
		FileOutputStream fos = null;

		// 设置xml信息的格式
		XMLOutputter out = new XMLOutputter();
		Format format = Format.getCompactFormat();
	//	format.setLineSeparator(CommonCharacter.LINE_FEED);
		format.setIndent("\t");
		format.setEncoding("UTF-8");
		out.setFormat(format);

		File fout=new File("D:\\test.xml");
		if (!fout.exists()) {
			fout.createNewFile();
		} 
		//生成的xml文件名称
		fos = new FileOutputStream(fout);
		writer = new OutputStreamWriter(fos, "UTF-8");

		//生成指定名称的xml文件
		out.output(xml, writer);
		writer.close();
		fos.close(); 
	}
	
	public static void saveXML(Document xml,String path) throws JDOMException, Exception {
		OutputStreamWriter writer = null;
		FileOutputStream fos = null;

		// 设置xml信息的格式
		XMLOutputter out = new XMLOutputter();
		Format format = Format.getCompactFormat();
	//	format.setLineSeparator(CommonCharacter.LINE_FEED);
		format.setIndent("\t");
		format.setEncoding("UTF-8");
		out.setFormat(format);

		File fout=new File(path);
		if (!fout.exists()) {
			fout.createNewFile();
		} 
		//生成的xml文件名称
		fos = new FileOutputStream(fout);
		writer = new OutputStreamWriter(fos, "UTF-8");

		//生成指定名称的xml文件
		out.output(xml, writer);
		writer.close();
		fos.close(); 
	}
}
