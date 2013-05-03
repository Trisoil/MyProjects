package tool;

import java.util.*;

import java.text.SimpleDateFormat;

	public class TimeUtil {
	    public static String getCurrentData() {
	    	java.util.Date dateNow=new java.util.Date();
	    	SimpleDateFormat dateFormat=new SimpleDateFormat ("yyyy_MM_dd");
	    	return dateFormat.format(dateNow);
	    }
}
