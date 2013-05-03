package my.myaction.tool.core;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import javax.servlet.http.Part;
import javax.servlet.AsyncContext;
import javax.servlet.DispatcherType;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;

import java.util.Collection;
import java.util.Enumeration;
import java.util.Map;
import java.util.Locale;
import java.util.Date;
import java.security.Principal;
import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.io.BufferedReader;
import java.text.SimpleDateFormat;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-6
 * Time: 13:45:26
 * To change this template use File | Settings | File Templates.
 */
public class MyHttpRequest implements HttpServletRequest {
    protected HttpServletRequest request=null;
    protected  MyHttpRequest(HttpServletRequest request){
             this.request=request;
    }
    public String getAuthType() {
        return request.getAuthType();
    }

    public Cookie[] getCookies() {
        return request.getCookies();
    }

    public long getDateHeader(String s) {
        return request.getDateHeader(s);
    }

    public String getHeader(String s) {
        return request.getHeader(s);
    }

    public Enumeration getHeaders(String s) {
        return request.getHeaders(s);
    }

    public Enumeration getHeaderNames() {
        return request.getHeaderNames();
    }

    public int getIntHeader(String s) {
        return request.getIntHeader(s);
    }

    public String getMethod() {
        return request.getMethod();
    }

    public String getPathInfo() {
        return request.getPathInfo();
    }

    public String getPathTranslated() {
        return request.getPathTranslated();
    }

    public String getContextPath() {
        return request.getContextPath();
    }

    public String getQueryString() {
        return request.getQueryString();
    }

    public String getRemoteUser() {
        return request.getRemoteUser();
    }

    public boolean isUserInRole(String s) {
        return request.isUserInRole(s);
    }

    public Principal getUserPrincipal() {
        return request.getUserPrincipal();
    }

    public String getRequestedSessionId() {
        return request.getRequestedSessionId();
    }

    public String getRequestURI() {
        return request.getRequestURI();
    }

    public StringBuffer getRequestURL() {
        return request.getRequestURL();
    }

    public String getServletPath() {
        return request.getServletPath();
    }

    public HttpSession getSession(boolean b) {
        return request.getSession(b);
    }

    public HttpSession getSession() {
        return request.getSession();
    }

    public boolean isRequestedSessionIdValid() {
        return request.isRequestedSessionIdValid();
    }

    public boolean isRequestedSessionIdFromCookie() {
        return request.isRequestedSessionIdFromCookie();
    }

    public boolean isRequestedSessionIdFromURL() {
        return request.isRequestedSessionIdFromURL();
    }

    public boolean isRequestedSessionIdFromUrl() {
        return request.isRequestedSessionIdFromUrl();
    }

    public Object getAttribute(String s) {
        return request.getAttribute(s);
    }

    public Enumeration getAttributeNames() {
        return request.getAttributeNames();
    }

    public String getCharacterEncoding() {
        return request.getCharacterEncoding();
    }

    public void setCharacterEncoding(String s) throws UnsupportedEncodingException {
        request.setCharacterEncoding(s);
    }

    public int getContentLength() {
        return request.getContentLength();
    }

    public String getContentType() {
        return request.getContentType();
    }

    public ServletInputStream getInputStream() throws IOException {
        return request.getInputStream();
    }

    public String getParameter(String s) {
        return request.getParameter(s);
    }

    public Enumeration getParameterNames() {
        return request.getParameterNames();
    }

    public String[] getParameterValues(String s) {
        return request.getParameterValues(s);
    }

    public Map getParameterMap() {
        return request.getParameterMap();
    }

    public String getProtocol() {
        return request.getProtocol();
    }

    public String getScheme() {
        return request.getScheme();
    }

    public String getServerName() {
        return request.getServerName();
    }

    public int getServerPort() {
        return request.getServerPort();
    }

    public BufferedReader getReader() throws IOException {
        return request.getReader();
    }

    public String getRemoteAddr() {
        return request.getRemoteAddr();
    }

    public String getRemoteHost() {
        return request.getRemoteHost();
    }

    public void setAttribute(String s, Object o) {
        request.setAttribute(s,o);
    }

    public void removeAttribute(String s) {
       request.removeAttribute(s);
    }

    public Locale getLocale() {
       return request.getLocale();
    }

    public Enumeration getLocales() {
        return request.getLocales();
    }

    public boolean isSecure() {
        return request.isSecure();
    }

    public RequestDispatcher getRequestDispatcher(String s) {
        return new MyRequestDispatcher(request.getRequestDispatcher(s),s);
    }

    public String getRealPath(String s) {
        return request.getRealPath(s);
    }

    public int getRemotePort() {
        return request.getRemotePort();
    }

    public String getLocalName() {
        return request.getLocalName();
    }

    public String getLocalAddr() {
        return request.getLocalAddr();
    }

    public int getLocalPort() {
        return request.getLocalPort();
    }



    public String getParameterString(String s){
         String value=getParameter(s);
         if(value==null) return "";
         return value.trim();
    }
    public int getParameterInt(String s){
       String value=getParameterString(s);
       int rt=0;
       try{
           rt=new Integer(value);
       }catch(Exception ex){
           rt=0;
       }
       return rt;
    }
    public short getParameterShort(String s){
       String value=getParameterString(s);
       short rt=0;
       try{
           rt=new Short(value);
       }catch(Exception ex){
           rt=0;
       }
       return rt;
    }
    public long getParameterLong(String s){
       String value=getParameterString(s);
       long rt=0;
       try{
           rt=new Long(value);
       }catch(Exception ex){
           rt=0;
       }
       return rt;
    }
    public float getParameterFloat(String s){
       String value=getParameterString(s);
       float rt=0;
       try{
           rt=new Float(value);
       }catch(Exception ex){
           rt=0;
       }
       return rt;
    }
    public double getParameterDouble(String s){
       String value=getParameterString(s);
       double rt=0;
       try{
           rt=new Double(value);
       }catch(Exception ex){
           rt=0;
       }
       return rt;
    }
    public byte getParameterByte(String s){
       String value=getParameterString(s);
       byte rt=0;
       try{
           rt=new Byte(value);
       }catch(Exception ex){
           rt=0;
       }
       return rt;
    }
    public boolean getParameterBoolean(String s){
       String value=getParameterString(s);
       boolean rt=false;
       try{
           rt=new Boolean(value);
       }catch(Exception ex){
           rt=false;
       }
       return rt;
    }
    public Date getParameterDate(String s){
        SimpleDateFormat formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
       String value=getParameterString(s);
       if(value.equals("")) return null;
       if(value.indexOf(" ")<=-1){//只有日期或时间
            if(value.indexOf("-")>-1){//日期
                  value+=" 00:00:00";
            }else  if(value.indexOf(":")>-1){//时间
                  value="1970-01-01 "+value;
            }else{
                return null;
            }
       }
       Date rt=null;
       try{
           rt=formatter.parse(value);
       }catch(Exception ex){
           rt=null;
       }
       return rt;
    }





    //添加两个字符串处理函数
    public  String notNull(String strTemp) {
        if(strTemp == null) {
            return new String("");
        } else {
            return strTemp;
        }
    }

    public  String notNullTrim(String strTemp) {
        if(strTemp == null) {
            return new String("");
        }else if(strTemp.trim().equalsIgnoreCase("null")){
            return new String("");
        }
        else {
            return strTemp.trim();
        }
    }
	public AsyncContext getAsyncContext() {
		// TODO Auto-generated method stub
		return null;
	}
	public DispatcherType getDispatcherType() {
		// TODO Auto-generated method stub
		return null;
	}
	public ServletContext getServletContext() {
		// TODO Auto-generated method stub
		return null;
	}
	public boolean isAsyncStarted() {
		// TODO Auto-generated method stub
		return false;
	}
	public boolean isAsyncSupported() {
		// TODO Auto-generated method stub
		return false;
	}
	public AsyncContext startAsync() {
		// TODO Auto-generated method stub
		return null;
	}
	public AsyncContext startAsync(ServletRequest arg0, ServletResponse arg1) {
		// TODO Auto-generated method stub
		return null;
	}
	public boolean authenticate(HttpServletResponse arg0) throws IOException,
			ServletException {
		// TODO Auto-generated method stub
		return false;
	}
	public Part getPart(String arg0) throws IOException, IllegalStateException,
			ServletException {
		// TODO Auto-generated method stub
		return null;
	}
	public Collection<Part> getParts() throws IOException,
			IllegalStateException, ServletException {
		// TODO Auto-generated method stub
		return null;
	}
	public void login(String arg0, String arg1) throws ServletException {
		// TODO Auto-generated method stub
		
	}
	public void logout() throws ServletException {
		// TODO Auto-generated method stub
		
	}
}
