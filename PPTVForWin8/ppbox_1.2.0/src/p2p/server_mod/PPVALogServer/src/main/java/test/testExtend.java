package test;

import java.util.*;

import org.jdom.Element;

import com.PPVALog.bean.VodLogChartShowItem;
import com.PPVALog.bean.base.LogChartShowItemBase;

public class testExtend {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		LogChartShowItemBase []chartShowItemList = new VodLogChartShowItem[10];
		chartShowItemList[0] = new VodLogChartShowItem();
		String []t=VodLogChartShowItem.showTableRowName;
	//	String []t2=chartShowItemList[0].getClass().;
	//	Element temp = new Element("test");
	//	temp.setText("test data");
	//	chartShowItemList[0].createByElement(temp);
	}

}
