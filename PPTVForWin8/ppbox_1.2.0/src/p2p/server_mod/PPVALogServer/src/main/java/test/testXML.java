package test;

import org.jdom.Document;
import org.jdom.Element;

import tool.XMLUtil;

public class testXML {
	public static void main(String[] args) {
		try {
			Document xml= XMLUtil.getDocument();
			XMLUtil.createRootElement(xml, "root");
			Element rootE=XMLUtil.getRootElement(xml);
			Element child1 = new Element("child1");
			child1.setAttribute("sender", "10086");
			Element child2 = new Element("child2");
			child2.setAttribute("sender2", "10086"); 
			XMLUtil.addChildElement(rootE, child1);
			XMLUtil.addChildElement(rootE, child2);
					
			XMLUtil.saveXML(xml);
		}catch (Exception e)
		{
			e.printStackTrace();			
		}
	}
}
