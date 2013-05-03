package test;
import com.speed.comm.Constant;

import tool.DESUtil;

public class testCase {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("yes");
		System.out.println(DESUtil.encodeString("12345", Constant.DES_KEY).toString());
		
	}

}
