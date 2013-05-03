package test;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class testDate {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		SimpleDateFormat bartDateFormat = new SimpleDateFormat("yyyy_MM_dd");
		Date start_date = null;
		Date nextDate = null;
		Calendar c = Calendar.getInstance();
		
		try {			
			start_date = bartDateFormat.parse("2011_8_10");
			System.out.println(start_date.toString());
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		c.setTime(start_date);
		c.add(Calendar.DAY_OF_YEAR,-1);
		nextDate=c.getTime();
		System.out.println(nextDate.toString());
		System.out.println(bartDateFormat.format(nextDate));
	}

}
