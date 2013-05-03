package test;

import java.io.IOException;

public class testProcess {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		//String cmd="d:\\notepad.exe "+ "d:\\test.xml";
		String cmd="D:\\logenv\\LogCollectionPath\\statistics_reader.exe "
				+ "D:\\logenv\\LogCollectionPath\\2011_08_03\\FC43269A047A44BD25C8223F29DA3296.gz "
				+ "D:\\logenv\\LogCollectionPath\\2011_08_03\\FC43269A047A44BD25C8223F29DA3296.xml";
		Process p;
		try {
			p = Runtime.getRuntime().exec(cmd);
			p.waitFor();
			
		}catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		System.out.println("main thread is over");

	}

}
