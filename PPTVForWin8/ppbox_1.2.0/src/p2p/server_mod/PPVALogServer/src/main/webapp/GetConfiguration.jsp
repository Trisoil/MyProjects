<%@ page import="com.PPVALog.Config" %>

<%
	try {
		String peerVersion = request.getParameter("peer_version");
		String peerId = request.getParameter("peer_id");
	
		if (peerVersion == null || peerId == null) {
			throw new Exception("format error");
		}

		ServletContext docApp = this.getServletContext();
		String configFileRealPath = docApp.getRealPath("/WEB-INF/peerConfig.xml");
		
		out.print( Config.getConfig().getConfigValue( configFileRealPath ) );
	} catch( Exception e ) {
		e.printStackTrace();
		response.setStatus(404, e.getMessage());
	}

%>
