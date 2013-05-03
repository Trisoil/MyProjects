package com.PPVALog.script;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Calendar;
import java.util.Date;
import java.util.Properties;

public class CleanLog {
	
	static String configName = "cleanConfig.properties";
	
	public static void main(String[] args) {
		InputStream inputStream = CleanLog.class.getClassLoader().getResourceAsStream(configName);
		Properties p = new Properties();
		try {
			p.load(inputStream);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		
		String logDirPath = null;
		Integer month = null;
		
		String monthString = p.getProperty("month");
		logDirPath = p.getProperty("logDirPath");
		if(monthString != null && logDirPath != null){
			month = Integer.valueOf(monthString);
		}else{
			System.out.println("wrong config");
			return;
		}
				
		File logDir = new File(logDirPath);
		if(!logDir.exists()){
			System.out.println("目录不存在,退出");
			return;
		}
		File []logList = logDir.listFiles();
		Calendar calendar = Calendar.getInstance();
		calendar.add(Calendar.MONTH, month);
		Date date = calendar.getTime();
		
		for(int i=0; i<logList.length; i++){
			File dir = logList[i];
			String dirName = dir.getName().replace('_', '/');
			try{
				Date dirDate = new Date(dirName);
				if(date.after(dirDate)){
					delete(dir);				
				}
			}catch(Exception e){
				//非PPVALog创建的目录
				//e.printStackTrace();
			}
		}
	}
	
	private static void delete(File file){
		if(file.isDirectory()){			
			File []filelist = file.listFiles();
			for(int i=0; i<filelist.length; i++){
				File f = filelist[i];
				delete(f);
			}
		}
		file.delete();	
	}

}
