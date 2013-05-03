package com.PPVALog.bean.base;

import org.jdom.Element;

/**
 * @author i-strongchi
 * 2011-8-10
 */
public abstract class LogChartShowItemBase {
	private String leftTime = "";  //剩余时间
	private String httpDownloadSpeed = ""; //http下载速度
	private String p2pDownloadSpeed = ""; //p2p下载速度
	private String totalDownloadSpeed = ""; //总下载速度
	private String stateMachine = ""; //状态机
	private String stateMachineMeans = ""; //状态机意思
	private String fullBlockPeerCount = ""; //满资源的节点数
	private String peerCount = ""; //连接节点数
	private String ipPoolPeerCount = ""; //IPPool
	private String totalDownloadBytes = ""; //Http总下载
	private String dataRate ="";//码流率
	private String lostRate ="";//丢包率
	private String reRate ="";//冗余率
	private String totalRequestSubPieceCount ="";//总发送
	private String totalRecievedSubPieceCount ="";//总收到
	private String totalP2PDataBytes ="";//P2P总下载


	public LogChartShowItemBase() {
	}

	public String getLeftTime() {
		return leftTime;
	}

	public void setLeftTime(String leftTime) {
		this.leftTime = leftTime;
	}

	public String getHttpDownloadSpeed() {
		return httpDownloadSpeed;
	}

	public void setHttpDownloadSpeed(String httpDownloadSpeed) {
		this.httpDownloadSpeed = httpDownloadSpeed;
	}

	public String getP2pDownloadSpeed() {
		return p2pDownloadSpeed;
	}

	public void setP2pDownloadSpeed(String p2pDownloadSpeed) {
		this.p2pDownloadSpeed = p2pDownloadSpeed;
	}

	public String getTotalDownloadSpeed() {
		return totalDownloadSpeed;
	}

	public void setTotalDownloadSpeed(String totalDownloadSpeed) {
		this.totalDownloadSpeed = totalDownloadSpeed;
	}

	public String getStateMachine() {
		return stateMachine;
	}

	public void setStateMachine(String stateMachine) {
		this.stateMachine = stateMachine;
	}

	public String getStateMachineMeans() {
		return stateMachineMeans;
	}

	public void setStateMachineMeans(String stateMachineMeans) {
		this.stateMachineMeans = stateMachineMeans;
	}

	public String getFullBlockPeerCount() {
		return fullBlockPeerCount;
	}

	public void setFullBlockPeerCount(String fullBlockPeerCount) {
		this.fullBlockPeerCount = fullBlockPeerCount;
	}

	public String getPeerCount() {
		return peerCount;
	}

	public void setPeerCount(String peerCount) {
		this.peerCount = peerCount;
	}

	public String getIpPoolPeerCount() {
		return ipPoolPeerCount;
	}

	public void setIpPoolPeerCount(String ipPoolPeerCount) {
		this.ipPoolPeerCount = ipPoolPeerCount;
	}

	public String getTotalDownloadBytes() {
		return totalDownloadBytes;
	}

	public void setTotalDownloadBytes(String totalDownloadBytes) {
		this.totalDownloadBytes = totalDownloadBytes;
	}

	public String getDataRate() {
		return dataRate;
	}

	public void setDataRate(String dataRate) {
		this.dataRate = dataRate;
	}

	public String getLostRate() {
		return lostRate;
	}

	public void setLostRate(String lostRate) {
		this.lostRate = lostRate;
	}

	public String getReRate() {
		return reRate;
	}

	public void setReRate(String reRate) {
		this.reRate = reRate;
	}

	public String getTotalRequestSubPieceCount() {
		return totalRequestSubPieceCount;
	}

	public void setTotalRequestSubPieceCount(String totalRequestSubPieceCount) {
		this.totalRequestSubPieceCount = totalRequestSubPieceCount;
	}

	public String getTotalRecievedSubPieceCount() {
		return totalRecievedSubPieceCount;
	}

	public void setTotalRecievedSubPieceCount(String totalRecievedSubPieceCount) {
		this.totalRecievedSubPieceCount = totalRecievedSubPieceCount;
	}

	public String getTotalP2PDataBytes() {
		return totalP2PDataBytes;
	}

	public void setTotalP2PDataBytes(String totalP2PDataBytes) {
		this.totalP2PDataBytes = totalP2PDataBytes;
	}

	public abstract String[] getShowTableRowName();
	public abstract  void createByElement(Element xmlElement);	
	public abstract String getRowData(int i,String out);	
	public abstract String getChartData(int i,String out);	
	public  abstract String getChartToolTipYValue(int i,String out);

}
