package com.PPVALog.dao.base;

import java.io.File;
import java.io.IOException;

import org.jdom.JDOMException;

import my.myproperties.tool.PropUtil;

/**
 * @author i-strongchi
 * 2011-8-17
 */
public abstract class LogProcessDaoBase {
	protected String processType = "";
	protected String file_id = "";
	protected String date = "";
	protected String path = "";
	protected String orginXMLPath = "";
	protected String targetXMLPath = "";
	protected String cmd="";

//	public String getProcessType() {
//		return processType;
//	}
	
	public boolean typeMatch(String type){
		return type.equals(processType);
	}

	public void processXML() throws JDOMException, Exception {
		File targetfile = new File(targetXMLPath);
		if(!targetfile.exists()){
			Process p;
			p = Runtime.getRuntime().exec(cmd);
			p.waitFor();
			analyzeXML();
			File orginXMLFile = new File(orginXMLPath);
			orginXMLFile.delete();
		}
	}
	
	public void analyzeXML() throws JDOMException, Exception{
		
	}

	public void setDatePaths(String file_id, String date) {
		this.file_id = file_id;
		this.date = date;
		String dirpath = PropUtil.getInstance("constant").getProperty(
				"PPVALogDir");
		this.path = PropUtil.getInstance("constant").getProperty("PPVALogPath")
				+ "/" + date + "/" + file_id + ".gz";
		this.orginXMLPath = dirpath + processType + "/" + date + "/" + file_id
				+ ".xml";
		this.targetXMLPath = dirpath + processType + "/" + date + "/" + file_id
				+ "_target.xml";		
		//this.cmd = dirpath + "statistics_reader.exe " + path + " " + orginXMLPath;
		this.cmd = dirpath + "statistic_reader-ce-3100-gcc41-mt -i " + path + " -o " + orginXMLPath;
		makeSaveDirectory();
	}

	private void makeSaveDirectory() {
		File pathFile = new File(targetXMLPath);
		if (!pathFile.getParentFile().exists())
			pathFile.getParentFile().mkdirs();// 建目录
	}
}
