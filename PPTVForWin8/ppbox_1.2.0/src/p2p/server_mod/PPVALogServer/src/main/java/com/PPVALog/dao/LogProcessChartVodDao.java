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
public class LogProcessChartVodDao extends LogProcessChartDaoBase {
	
	public LogProcessChartVodDao(){
		processType = "vod-bufferring-basic";
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
		setCDNIP(orginStatisticDataList);
		//
		for (Element orginStatisticData : orginStatisticDataList) {
			Element targetStatisticDataElement = new Element("StatisticData");
			targetStatisticDataElement.setAttribute("ID",orginStatisticData.getAttributeValue("ID"));
			XMLUtil.addChildElement(targetRootElement,targetStatisticDataElement);
			//VodDownloadDriverStatisticse可能有多个，只处理一个的情况
			//http速度//http总下载//剩余时间//状态机//码流率
			Element orginDownloaderDirverStatisticInfo=isVodDownloadDriverStatisticsesValidate(orginStatisticData,targetStatisticDataElement);
			if(orginDownloaderDirverStatisticInfo!=null){
				setHttpDownloadSpeed(orginDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setTotalDownloadBytes(orginDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setLeftTime(orginDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setDataRate(orginDownloaderDirverStatisticInfo,targetStatisticDataElement);
				setStateMachine(orginDownloaderDirverStatisticInfo,targetStatisticDataElement);
			}
			//P2PDownloaderStatisticse可能有多个，只处理一个的情况
			//p2p速度//三个连接数：满资源的节点数、连接节点数、IPPool
			//丢包率//冗余率//总发送//总收到//p2p总下载
			Element orginP2PDownloadStatisticInfo = isVodP2PDownloaderStatisticsesValidate(orginStatisticData,targetStatisticDataElement);
			if(orginP2PDownloadStatisticInfo!=null){
				setP2pDownloadSpeed(orginP2PDownloadStatisticInfo,targetStatisticDataElement);
				setCounts(orginP2PDownloadStatisticInfo,targetStatisticDataElement);
				setLostRate(orginP2PDownloadStatisticInfo,targetStatisticDataElement);
				setReRate(orginP2PDownloadStatisticInfo,targetStatisticDataElement);
				setTotalRequestSubPieceCount(orginP2PDownloadStatisticInfo,targetStatisticDataElement);
				setTotalRecievedSubPieceCount(orginP2PDownloadStatisticInfo,targetStatisticDataElement);
				setUotalP2PDataBytes(orginP2PDownloadStatisticInfo,targetStatisticDataElement);			
			}			
		}
		XMLUtil.saveXML(targetxml, targetXMLPath);
	}
	
	private void setType() throws JDOMException, Exception{
		Element tempElement = new Element ("type");
		tempElement.setText("vod");
		XMLUtil.addChildElement(targetRootElement, tempElement);
	}
	
	private void setCDNIP(List<Element> orginStatisticDataList) throws JDOMException, Exception{
		String tempText="";
		if(orginStatisticDataList.size() > 0){
			List<Element> orginVodDownloaderDirverStatisticInfoList = orginStatisticDataList.get(0)
					.getChild("VodDownloadDriverStatisticses").getChildren(
							"DownloaderDirverStatisticInfo");
			if(orginVodDownloaderDirverStatisticInfoList.size() > 0){
				List<Element> orginHttpDownloadStatisticInfoList = orginVodDownloaderDirverStatisticInfoList.get(0).getChild("HttpDownloaders")
						.getChildren("HttpDownloadStatisticInfo");
				for (Element orginHttpDownloadStatisticInfo : orginHttpDownloadStatisticInfoList) {
					if (orginHttpDownloadStatisticInfo.getChild("IsPause")
							.getText().equals("0")) {
						tempText+=orginHttpDownloadStatisticInfo.getChildText("OriginalUrl");						
					}
				}
			}
		}
		tempText=tempText.equals("")?"wrong data":tempText;
		Element tempElement = new Element("cdnip");
		tempElement.setText(tempText);
		XMLUtil.addChildElement(targetRootElement,tempElement);
	}
	
	private Element isVodDownloadDriverStatisticsesValidate(Element orginStatisticData,Element targetStatisticData) throws JDOMException, Exception{
		List<Element> orginVodDownloaderDirverStatisticInfoList = orginStatisticData
				.getChild("VodDownloadDriverStatisticses").getChildren(
						"DownloaderDirverStatisticInfo");
		if (orginVodDownloaderDirverStatisticInfoList.size() == 1) {
			return orginVodDownloaderDirverStatisticInfoList.get(0);
		}else{
			Element tempElement = new Element("httpDownloadSpeed");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("totalDownloadBytes");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("leftTime");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("stateMachine");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("dataRate");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			return null;
		}
	}
	
	private void setHttpDownloadSpeed(Element orginDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		List<Element> orginHttpDownloadStatisticInfoList = orginDownloaderDirverStatisticInfo.getChild("HttpDownloaders")
				.getChildren("HttpDownloadStatisticInfo");
		for (Element orginHttpDownloadStatisticInfo : orginHttpDownloadStatisticInfoList) {
			// System.out.println("chartCreate_log "+"ispause= " +
			// orginHttpDownloadStatisticInfo.getChild("IsPause").getText());
			if (orginHttpDownloadStatisticInfo.getChild("IsPause")
					.getText().equals("0")) {
				// System.out.println("HTTP速度："+orginHttpDownloadStatisticInfo.getChild("SpeedInfo").getChildText("NowDownloadSpeed"));
				Element tempElement = new Element("httpDownloadSpeed");
				tempElement.setText(orginHttpDownloadStatisticInfo
						.getChild("SpeedInfo").getChildText(
								"NowDownloadSpeed"));
				XMLUtil.addChildElement(targetStatisticData,
						tempElement);
			}
		}
	}
	
	private void setTotalDownloadBytes(Element orginDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		List<Element> orginHttpDownloadStatisticInfoList = orginDownloaderDirverStatisticInfo.getChild("HttpDownloaders")
				.getChildren("HttpDownloadStatisticInfo");
		for (Element orginHttpDownloadStatisticInfo : orginHttpDownloadStatisticInfoList) {
			// System.out.println("chartCreate_log "+"ispause= " +
			// orginHttpDownloadStatisticInfo.getChild("IsPause").getText());
			if (orginHttpDownloadStatisticInfo.getChild("IsPause")
					.getText().equals("0")) {
				//http总下载
				Element tempElement = new Element("totalDownloadBytes");
				tempElement.setText(orginHttpDownloadStatisticInfo
						.getChild("SpeedInfo").getChildText(
								"TotalDownloadBytes"));
				XMLUtil.addChildElement(targetStatisticData,
						tempElement);
			}
		}
	}
	
	private void setLeftTime(Element orginDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("leftTime");
		tempElement.setText(orginDownloaderDirverStatisticInfo
				.getChildText("t"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
	
	private void setDataRate(Element orginDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("dataRate");
		tempElement.setText(orginDownloaderDirverStatisticInfo
				.getChildText("DataRate"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
	
	private void setStateMachine(Element orginDownloaderDirverStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("stateMachine");
		tempElement.setText(orginDownloaderDirverStatisticInfo
				.getChildText("http_state")
				+ orginDownloaderDirverStatisticInfo
						.getChildText("p2p_state")
				+ orginDownloaderDirverStatisticInfo
						.getChildText("timer_using_state")
				+ orginDownloaderDirverStatisticInfo
						.getChildText("timer_state"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
	
	private Element isVodP2PDownloaderStatisticsesValidate(Element orginStatisticData,Element targetStatisticData) throws JDOMException, Exception{
		List<Element> orginP2PDownloadStatisticInfoList = orginStatisticData
				.getChild("P2PDownloaderStatisticses").getChildren(
						"P2PDownloadStatisticInfo");
		if (orginP2PDownloadStatisticInfoList.size() == 1) {
			return orginP2PDownloadStatisticInfoList.get(0);
		}else{
			Element tempElement = new Element("p2pDownloadSpeed");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("FullBlockPeerCount");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("PeerCount");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("IpPoolPeerCount");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("lostRate");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("reRate");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("totalRequestSubPieceCount");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("totalRecievedSubPieceCount");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);
			tempElement = new Element("totalP2PDataBytes");
			tempElement.setText("wrong data");
			XMLUtil.addChildElement(targetStatisticData, tempElement);			
			return null;
		}
	}
	
	private void setP2pDownloadSpeed(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("p2pDownloadSpeed");
		tempElement.setText(orginP2PDownloadStatisticInfo.getChild(
				"SpeedInfo").getChildText("NowDownloadSpeed"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}

	private void setCounts(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("FullBlockPeerCount");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("FullBlockPeerCount"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
		tempElement = new Element("PeerCount");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("PeerCount"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
		tempElement = new Element("IpPoolPeerCount");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("IpPoolPeerCount"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}

	private void setLostRate(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("lostRate");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("LostRate"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
	
	private void setReRate(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("reRate");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("ReRate"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
	
	private void setTotalRequestSubPieceCount(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("totalRequestSubPieceCount");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("TotalRequestSubPieceCount"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}

	private void setTotalRecievedSubPieceCount(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("totalRecievedSubPieceCount");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("TotalRecievedSubPieceCount"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
	
	private void setUotalP2PDataBytes(Element orginP2PDownloadStatisticInfo,Element targetStatisticData) throws JDOMException, Exception{
		Element tempElement = new Element("totalP2PDataBytes");
		tempElement.setText(orginP2PDownloadStatisticInfo
				.getChildText("TotalP2PDataBytes"));
		XMLUtil.addChildElement(targetStatisticData, tempElement);
	}
}
