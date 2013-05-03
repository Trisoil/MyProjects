package tool.mail;

import javax.activation.DataHandler;
import javax.mail.Message;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import java.util.Date;
import java.util.Properties;

/**
 * Created by IntelliJ IDEA.
 *
 *
 * @version 1.0
 *          Date: 2005-11-28
 *          Time: 21:58:57
 */
public class MailSender {

    private String mailServerHost = "";
    private String mailServerAccount = "";
    private String mailServerPassword = "";
    private String fromMail = "";
    private String fromUserName = "";
    private String toMail = "";
    private String replyMail = "";
    private String subject = "";
    private String body = "";
    private boolean isHtml = true;

    /**
     * 邮件发送
     */

    public void send() {
        try {
            //建立连接通道,使用smtp认证方式
            Properties props = System.getProperties();
            props.setProperty("file.encoding", "GB2312");
            props.put("mail.smtp.auth", "true");
            Session session = Session.getInstance(props);
            session.setDebug(false);
            Transport transport = session.getTransport("smtp");
            transport.connect(mailServerHost, mailServerAccount, mailServerPassword);

/*
            //debug
            if ( true ){
               Properties prop = session.getProperties();
               prop.list(System.out);
            }
*/

            //构造Mime消息格式
            Message msg = new MimeMessage(session);
            //发件人
            if((fromMail == null) || fromMail.equals("")) {
                msg.setFrom();
            } else {
                msg.setFrom(new InternetAddress(fromMail, fromUserName, "GB2312"));
            }
            //收件人
            InternetAddress[] address = {new InternetAddress(toMail)};
            msg.setRecipients(Message.RecipientType.TO, address);
            //回复人
            if((replyMail != null) && !replyMail.equals("")) {
                InternetAddress[] reply = {new InternetAddress(replyMail)};
                msg.setReplyTo(reply);
            }
            msg.setSentDate(new Date());
            //邮件主题
            msg.setSubject(subject);
            if(body == null) {
                body = "";
            }
            if(isHtml == false) {
                //txt类型
                msg.setText(body);
            } else {
                //HTML格式的邮件体
                body = new String(body.getBytes("GB2312"), "ISO-8859-1");
                msg.setDataHandler(new DataHandler(new ByteArrayDataSource(body, "text/html")));
            }

            //发送
            msg.saveChanges();
            transport.sendMessage(msg, msg.getAllRecipients());
            transport.close();
        } catch(Exception mex) {
            mex.printStackTrace();
        }
    }

    public String getMailServerHost() {
        return mailServerHost;
    }

    public void setMailServerHost(String mailServerHost) {
        this.mailServerHost = mailServerHost;
    }

    public String getMailServerAccount() {
        return mailServerAccount;
    }

    public void setMailServerAccount(String mailServerAccount) {
        this.mailServerAccount = mailServerAccount;
    }

    public String getMailServerPassword() {
        return mailServerPassword;
    }

    public void setMailServerPassword(String mailServerPassword) {
        this.mailServerPassword = mailServerPassword;
    }

    public String getFromMail() {
        return fromMail;
    }

    public void setFromMail(String fromMail) {
        this.fromMail = fromMail;
    }

    public String getFromUserName() {
        return fromUserName;
    }

    public void setFromUserName(String fromUserName) {
        this.fromUserName = fromUserName;
    }

    public String getToMail() {
        return toMail;
    }

    public void setToMail(String toMail) {
        this.toMail = toMail;
    }

    public String getReplyMail() {
        return replyMail;
    }

    public void setReplyMail(String replyMail) {
        this.replyMail = replyMail;
    }

    public String getSubject() {
        return subject;
    }

    public void setSubject(String subject) {
        this.subject = subject;
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    }

    public boolean isHtml() {
        return isHtml;
    }

    public void setHtml(boolean html) {
        this.isHtml = html;
    }


}
