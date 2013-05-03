package com.PPVALog.dao.base;

import org.jdom.Element;

/**
 * @author i-strongchi
 * 2011-8-17
 */
public abstract class LogProcessChartDaoBase extends LogProcessDaoBase {
	protected Element targetRootElement;
	protected Element orginRootElement;
	
	public String getRedirectURL(String id) {
		return "chartShow.jsp?index_id=" + id + "&date=" + date;
	}
}
