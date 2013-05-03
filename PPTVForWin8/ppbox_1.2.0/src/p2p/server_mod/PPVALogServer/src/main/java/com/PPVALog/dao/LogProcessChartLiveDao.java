package com.PPVALog.dao;

import java.util.List;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;

import tool.XMLUtil;

import com.PPVALog.dao.base.LogProcessChartDaoBase;

/**
 * @author i-strongchi
 * 2011-8-17
 */
public class LogProcessChartLiveDao extends LogProcessChartDaoBase {

	public LogProcessChartLiveDao(){
		processType = "live-bufferring-basic";
	}

	@Override
	public void analyzeXML() throws JDOMException, Exception {
		Document orginxml = XMLUtil.getDocument(orginXMLPath);
		Document targetxml = XMLUtil.getDocument();
		// init targetxml
		XMLUtil.createRootElement(targetxml, "root");
		targetRootElement = XMLUtil.getRootElement(targetxml);
		orginRootElement = XMLUtil.getRootElement(orginxml);
		List<Element> orginStatisticDataList = XMLUtil.getChildrenElement(orginRootElement);
		//设置固定值
		setType();
		//
		for (Element orginStatisticData : orginStatisticDataList) {
			Element targetStatisticDataElement = new Element("StatisticData");
			targetStatisticDataElement.setAttribute("ID",orginStatisticData.getAttributeValue("ID"));
			XMLUtil.addChildElement(targetRootElement,targetStatisticDataElement);
			//LiveDownloaderDirverStatisticInfo可能有多个，只处理一个的情况
			//http速度//p2p速度//UdpServer速度//剩余时间//状态机//码流率
			Element orginLiveDownloaderDirverStatisticInfo=isLiveDownloadDriverStatisticsesValidate(orginStatisticData,targetStatisticDataElement);
			if(orginLiveDownloaderDirverStatisticInfo!=null){
				setHttpDownloadSpeed(orginLiveDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setP2pDownloadSpeed(orginLiveDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setUdpServerSpeed(orginLiveDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setRestPlayTime(orginLiveDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setStateMachine(orginLiveDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setDataRate(orginLiveDownloaderDirverStatisticInfo,targetStatisticDataElement);
			}				
		}
		XMLUtil.saveXML(targetxml, targetXMLPath);
	}
	
	private void setType() throws JDOMException, Exception{
		Element tempElement = new Element ("type");
		tempElement.setText("live");
		XMLUtil.addChildElement(targetRootElement, tempElement);
	}
	
	private Element isLiveDownloadDriverStatisticsesValidate(Element orginStatisticData,Element targetStatisticData) throws JDOMException, Exception{
		List<Element> orginLiveDownloaderDirverStatisticInfoList = orginStatisticData
				.getChild("LiveDownloadDriverStatisticses").getChildren(
						"LiveDownloaderDirverStatisticInfo");
		if (orginLiveDownloaderDirverStatisticInfoList.size() == 1) {
			return orginLiveDownloaderDirverStatisticInfoList.get(0);
		}else{
			Element tempElement = new Element("httpDownloadSpeed");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData,tempElement);
			tempElement = new Element("p2pDownloadSpeed");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData,tempElement);
			tempElement = new Element("udpServerSpeed");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData,tempElement);
			tempElement = new Element("restPlayTime");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData,tempElement);
			tempElement = new Element("stateMachine");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData,tempElement);
			tempElement = new Element("dataRate");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData,tempElement);
			return null;
		}
	}
	
	private void setHttpDownloadSpeed(Element orginLiveDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("httpDownloadSpeed");
		tempElement.setText(orginLiveDownloaderDirverStatisticInfo
				.getChild("LiveHttpSpeedInfo").getChildText(
						"NowDownloadSpeed"));
		XMLUtil.addChildElement(targetStatisticData,tempElement);
	}
	
	private void setP2pDownloadSpeed(Element orginLiveDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("p2pDownloadSpeed");
		tempElement.setText(orginLiveDownloaderDirverStatisticInfo
				.getChild("LiveP2PSpeedInfo").getChildText(
						"NowDownloadSpeed"));
		XMLUtil.addChildElement(targetStatisticData,
				tempElement);
	}
	
	private void setUdpServerSpeed(Element orginLiveDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("udpServerSpeed");
		tempElement.setText(orginLiveDownloaderDirverStatisticInfo
				.getChild("LiveUdpServerSpeedInfo").getChildText(
						"NowDownloadSpeed"));
		XMLUtil.addChildElement(targetStatisticData,
				tempElement);
	}
	
	private void setRestPlayTime(Element orginLiveDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("restPlayTime");
		tempElement.setText(orginLiveDownloaderDirverStatisticInfo
				.getChildText("RestPlayTime"));
		XMLUtil.addChildElement(targetStatisticData,
				tempElement);
	}
	
	private void setStateMachine(Element orginLiveDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("stateMachine");
		tempElement.setText(
				orginLiveDownloaderDirverStatisticInfo.getChildText("http_state") +
				orginLiveDownloaderDirverStatisticInfo.getChildText("p2p_state"));
		XMLUtil.addChildElement(targetStatisticData,
				tempElement);
	}
	
	private void setDataRate(Element orginLiveDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("dataRate");
		tempElement.setText(orginLiveDownloaderDirverStatisticInfo
				.getChildText("DataRate"));
		XMLUtil.addChildElement(targetStatisticData,
				tempElement);	
	}
	
}
