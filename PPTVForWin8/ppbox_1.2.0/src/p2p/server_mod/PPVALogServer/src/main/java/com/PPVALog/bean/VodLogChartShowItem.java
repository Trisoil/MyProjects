package com.PPVALog.bean;

import java.text.DecimalFormat;

import org.jdom.Element;

import tool.IPSeeker;

import com.PPVALog.bean.base.LogChartShowItemBase;

/**
 * @author i-strongchi
 * 2011-8-10
 */
public class VodLogChartShowItem extends LogChartShowItemBase {
	
	public static String []showTableRowName = {"id","总发送","总收到",
		"丢包率","冗余率","http总下载","p2p总下载","剩余时间(s)","http速度",
		"p2p速度","总下载速度","状态机","满资源的节点数","连接节点数","IPPool"};
	public static String []showChartSeriesTitle = 
		{"id","leftTime","httpDownloadSpeed","p2pDownloadSpeed","totalDownloadSpeed","stateMachine","dataRate"};
	public static String []showConstantName={"CND IP"};
	
	public static final String HTTP_STATE = "2300";
	public static final String P2P_STATE = "3200";
	public static final String HTTP_P2P_STATE = "2200";
	public static final String PURE_HTTP = "3200";
	
	@Override
	public String getChartData(int i, String out) {
		// TODO Auto-generated method stub
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
			return getTotalDownloadSpeed();
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
		// TODO Auto-generated method stub
		switch (i){
		case 0:
			return out;
		case 1:
			return getTotalRequestSubPieceCount();
		case 2:
			return getTotalRecievedSubPieceCount();
		case 3:
			return getLostRate();
		case 4:
			return getReRate();
		case 5:
			return getTotalDownloadBytes();
		case 6:
			return getTotalP2PDataBytes();	
		case 7:
			return getLeftTime();
		case 8:
			return getHttpDownloadSpeed();
		case 9:
			return getP2pDownloadSpeed();
		case 10:
			return getTotalDownloadSpeed();
		case 11:
			return getStateMachine() + "  (" + getStateMachineMeans() + ")";
		case 12:
			return getFullBlockPeerCount();
		case 13:
			return getPeerCount();
		case 14:
			return getIpPoolPeerCount();
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
		this.setTotalDownloadSpeed(getTotalDownloadSpeedByElement());
		this.setStateMachine(getStateMachineByElement(xmlElement));
		this.setStateMachineMeans(getStateMachineMeansByElement());
		this.setFullBlockPeerCount(getFullBlockPeerCountByElement(xmlElement));
		this.setPeerCount(getPeerCountByElement(xmlElement));
		this.setIpPoolPeerCount(getIpPoolPeerCountByElement(xmlElement));
		this.setTotalDownloadBytes(getTotalDownloadBytesByElement(xmlElement));
		this.setDataRate(getDataRateByElement(xmlElement));
		this.setLostRate(getLostRateByElement(xmlElement));
		this.setReRate(getReRateByElement(xmlElement));
		this.setTotalRequestSubPieceCount(getTotalRequestSubPieceCountByElement(xmlElement));
		this.setTotalRecievedSubPieceCount(getTotalRecievedSubPieceCountByElement(xmlElement));
		this.setTotalP2PDataBytes(getTotalP2PDataBytesByElement(xmlElement));
		// System.out.println("child");

	}
	
	public static String getConstantData(int i,Element e){
		switch (i){
		case 0:
			return getCNDIPByElement(e);
		default:
			return "";
		}
	}

	private String getLeftTimeByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("leftTime");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp) / 1000);
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

	private String getTotalDownloadSpeedByElement() {
		try {
			int totalDownloadSpeed = Integer.parseInt(getHttpDownloadSpeed())
					+ Integer.parseInt(getP2pDownloadSpeed());
			return Integer.toString(totalDownloadSpeed);
			// System.out.println(dataSeries[i][j-2] + "+" + dataSeries[i][j-1]
			// + "=" + dataSeries[i][j]);
		} catch (Exception e) {
			return "wrong data";
		}
	}

	private String getStateMachineByElement(Element xmlElement) {
		return xmlElement.getChildText("stateMachine");
	}

	private String getStateMachineMeansByElement() {
		String statemachine = getStateMachine();
		if (statemachine.equals(VodLogChartShowItem.HTTP_STATE))
			return "HTTP";
		else if (statemachine.equals(VodLogChartShowItem.P2P_STATE))
			return "P2P";
		else if (statemachine.equals(VodLogChartShowItem.HTTP_P2P_STATE))
			return "Http+P2P";
		else if (statemachine.equals(VodLogChartShowItem.PURE_HTTP))
			return "纯http";
		return "";
	}

	private String getFullBlockPeerCountByElement(Element xmlElement) {
		return xmlElement.getChildText("FullBlockPeerCount");
	}

	private String getPeerCountByElement(Element xmlElement) {
		return xmlElement.getChildText("PeerCount");
	}

	private String getIpPoolPeerCountByElement(Element xmlElement) {
		return xmlElement.getChildText("IpPoolPeerCount");
	}
	
	private String getTotalDownloadBytesByElement(Element xmlElement) {
		return xmlElement.getChildText("totalDownloadBytes");
	}
	
	private String getDataRateByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("dataRate");
		if (temp.equals("wrong data"))
			return temp;
		else
			return Integer.toString(Integer.parseInt(temp) / 1024);
	}
	
	private String getLostRateByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("lostRate");
		DecimalFormat df = new DecimalFormat("#.##");//设置保留小数位数
		if (temp.equals("wrong data"))
			return temp;
		else
			return df.format( Double.parseDouble(temp))+"%";
	}
	
	private String getReRateByElement(Element xmlElement) {
		String temp = xmlElement.getChildText("reRate");
		DecimalFormat df = new DecimalFormat("#.##");//设置保留小数位数
		if (temp.equals("wrong data"))
			return temp;
		else
			return df.format( Double.parseDouble(temp))+"%";
	}
	
	private String getTotalRequestSubPieceCountByElement(Element xmlElement) {
		return xmlElement.getChildText("totalRequestSubPieceCount");
	}
	
	private String getTotalRecievedSubPieceCountByElement(Element xmlElement) {
		return xmlElement.getChildText("totalRecievedSubPieceCount");
	}
	
	private String getTotalP2PDataBytesByElement(Element xmlElement) {
		return xmlElement.getChildText("totalP2PDataBytes");
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

	private static String getCNDIPByElement(Element xmlElement) {
		String inString = xmlElement.getChildText("cdnip");
		int iplength = inString.indexOf('/');
		iplength = inString.indexOf('/',iplength+1);
		//ipString=http://xxx.xxx.xxx.xxx
		String ipString;
		//numString=xxx.xxx.xxx.xxx
		String numString;
		if(inString.indexOf('/',iplength+1)==-1){
			ipString = inString.substring(0,inString.length());
			numString = inString.substring(iplength+1,inString.length());
		}else{
			ipString = inString.substring(0, inString.indexOf('/',iplength+1));
			numString = inString.substring(iplength+1, inString.indexOf('/',iplength+1));
		}
		//testIp=xxx
		int dotIndex = numString.indexOf('.');
		String testIp = numString.substring(0,dotIndex);
		
		try{
			int t= Integer.parseInt(testIp);
		}catch (Exception e){
			return ipString;
		}
		String userLocation = "(" + IPSeeker.getInstance().getArea( numString ) + "," + IPSeeker.getInstance().getCountry( numString ) + ")";
		return ipString + " " + userLocation;
	}

}
