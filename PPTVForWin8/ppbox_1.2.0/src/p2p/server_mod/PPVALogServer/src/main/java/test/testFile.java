package test;

import java.io.File;
import java.io.IOException;

public class testFile {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String path= "D:/temp/a.txt";
		//String path= "D:/temp";
		File f=new File(path);
		f.mkdirs();		
	}

}
