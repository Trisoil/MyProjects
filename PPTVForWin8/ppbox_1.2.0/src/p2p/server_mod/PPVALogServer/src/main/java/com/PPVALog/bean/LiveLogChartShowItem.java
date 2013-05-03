package com.PPVALog.bean;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.List;

import org.jdom.Element;

import com.PPVALog.bean.base.LogChartShowItemBase;

/**
 * @author i-strongchi
 * 2011-8-10
 */
public class LiveLogChartShowItem extends LogChartShowItemBase {
	
	public static String []showTableRowName = {"id","剩余时间(s)","http速度",
		"p2p速度","UdpServer速度","状态机","码流率"};
	public static String []showChartSeriesTitle = 
		{"id","leftTime","httpDownloadSpeed","p2pDownloadSpeed","udpDownloadSpeed","stateMachine","dataRate"};
	public static String []showConstantName={};
	private String udpDownloadSpeed = ""; //upd速度
	
	public static final String HTTP_STATE = "23";
	public static final String P2P_STATE = "32";
	public static final String HTTP_P2P_STATE = "22";

	@Override
	public String getChartData(int i, String out) {
		switch(i){
		case 0:
			return out;
		case 1:
			return getLeftTime();
		case 2:
			return getHttpDownloadSpeed();
		case 3:
			return getP2pDownloadSpeed();
		case 4:
			return getUdpDownloadSpeed();
		case 5:
			return getStateMachine();
		case 6:
			return getDataRate(); 
		default:
			return "wrong data";
		}
	}
	@Override
	public String getRowData(int i,String out) {
		switch (i){
		case 0:
			return out;
		case 1:
			return getLeftTime();
		case 2:
			return getHttpDownloadSpeed();
		case 3:
			return getP2pDownloadSpeed();
		case 4:
			return getUdpDownloadSpeed();
		case 5:
			return getStateMachine();
		case 6:
			return getDataRate();	
		default:
			return "";			
		}
	}
	@Override
	public String getChartToolTipYValue(int i,String out){
		switch(i){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			return "";
		case 5:
			return getStateMachine() + "  (" + getStateMachineMeans() + ")";
		case 6:
			return ""; 
		default:
			return "wrong data";
		}
	}

	@Override
	public void createByElement(Element xmlElement) {

		this.setLeftTime(getLeftTimeByElement(xmlElement));
		this.setHttpDownloadSpeed(getHttpDownloadSpeedByElemet(xmlElement));
		this.setP2pDownloadSpeed(getP2PDownloadSpeedByElement(xmlElement));
		this.setUdpDownloadSpeed(getUdpDownloadSpeedByElement(xmlElement));
		this.setStateMachine(getStateMachineByElement(xmlElement));
		this.setStateMachineMeans(getStateMachineMeansByElement());
		this.setDataRate(getDataRateByElement(xmlElement));
		//System.out.println("live "+ getLeftTime());

	}
	
	
	
	public static String getConstantData(int i,Element e){
		switch (i){
		default:
			return "";
		}
	}

	private String getLeftTimeByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("restPlayTime");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp));
	}

	private String getHttpDownloadSpeedByElemet(Element xmlElement) {
		String temp = xmlElement.getChildText("httpDownloadSpeed");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp) / 1024);
	}

	private String getP2PDownloadSpeedByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("p2pDownloadSpeed");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp) / 1024);
	}

	private String getUdpDownloadSpeedByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("udpServerSpeed");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp) / 1024);
	}

	private String getStateMachineByElement(Element xmlElement) {
		return xmlElement.getChildText("stateMachine");
	}

	private String getStateMachineMeansByElement() {
		String statemachine = getStateMachine();
		if (statemachine.equals(LiveLogChartShowItem.HTTP_STATE))
			return "HTTP";
		else if (statemachine.equals(LiveLogChartShowItem.P2P_STATE))
			return "P2P";
		else if (statemachine.equals(LiveLogChartShowItem.HTTP_P2P_STATE))
			return "Http+P2P";
		return "";
	}
	
	private String getDataRateByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("dataRate");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp) / 1024);
	}
	
	
	public String[] getShowTableRowName() {
		return showTableRowName;
	}
	public static String[] getShowChartSeriesTitle() {
		return showChartSeriesTitle;
	}
	public static String[] getShowConstantName() {
		return showConstantName;
	}

	public String getUdpDownloadSpeed() {
		return udpDownloadSpeed;
	}

	public void setUdpDownloadSpeed(String udpDownloadSpeed) {
		this.udpDownloadSpeed = udpDownloadSpeed;
	}

}
