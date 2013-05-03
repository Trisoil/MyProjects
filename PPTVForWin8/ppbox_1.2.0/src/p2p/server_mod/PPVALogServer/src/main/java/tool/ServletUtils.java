package tool;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;


public class ServletUtils {

	public static String getServerIpByRequest(HttpServletRequest request) {
		String fromHost = request.getHeader("X-Real-IP");
		if(SuperString.isEmpty(fromHost)){
		      fromHost=request.getRemoteHost();
		}
		return fromHost;
	}

	public static void sendXMLResponse(HttpServletResponse response, int status, String xml){
		response.setContentType("text/xml; charset=UTF-8");
		response.setHeader("Cache-Control","no-cache");	
		response.setStatus(status);
		PrintWriter out;
		try {
			out = response.getWriter();
			out.println(xml);
			out.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}				
	}
}
