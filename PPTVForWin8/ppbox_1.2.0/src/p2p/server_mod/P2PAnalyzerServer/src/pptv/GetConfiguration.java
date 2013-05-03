package pptv;

import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.*;

/**
 * Servlet implementation class GetConfiguration
 */
@WebServlet("/GetConfiguration.jsp")
public class GetConfiguration extends HttpServlet {
	private static final long serialVersionUID = 1L;
       
	private String configFileRealPath = "";
	private byte configContent[];
	
    /**
     * @see HttpServlet#HttpServlet()
     */
    public GetConfiguration() {
        super();
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		// 在这里请求了配置
		try {
			// 检测版本情况
			String peerVersion = request.getParameter("peer_version");
			String peerId = request.getParameter("peer_id");
			
			if( peerVersion == null || peerId == null ) {
				throw new Exception("format error");
			}
			
			System.out.print( peerVersion );
			if( this.configFileRealPath.length() == 0 ) {
				ServletContext docApp = this.getServletContext();
				this.configFileRealPath = docApp.getRealPath("/peerConfig.xml");
				
				// 读取出文件进入内存
				FileInputStream fi = new FileInputStream(this.configFileRealPath);;
				try {
					byte content[] = new byte[10240];
					int readed = fi.read(content);
					this.configContent = new byte[readed];
					System.arraycopy( content , 0, this.configContent, 0, readed);
				} catch( IOException ioe ) {
					throw ioe;
				} finally {
					fi.close();
				}
			}
			
			response.getOutputStream().write(this.configContent);
			response.setStatus( 200 , this.configContent.toString());
		} catch( Exception e ) {
			response.setStatus(404, e.getMessage());
		}
	}

}
