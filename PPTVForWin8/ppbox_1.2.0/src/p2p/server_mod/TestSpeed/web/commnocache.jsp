<%
    if(session.getAttribute("cache") != null) {
        session.removeAttribute("cache");
        //out.println("permit have cache");
    } else {
        response.setHeader("Pragma", "No-Cache");
        response.setDateHeader("Expires", 0);
        response.setHeader("Cache-Control", "no-Cache");
        //out.println("no cache");
    }
%>