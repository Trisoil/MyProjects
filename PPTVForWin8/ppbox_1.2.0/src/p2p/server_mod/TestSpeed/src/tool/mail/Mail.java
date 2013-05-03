package tool.mail;

import my.myspring.tool.SpringBeanProxy;


/**
 * Created by IntelliJ IDEA.
 *
 * @version 1.0
 *          Date: 2005-11-28
 *          Time: 22:14:37
 */
public class Mail {

    public static void sendMail(String toMail, String subject, String body, boolean html) {
        try {
            MailSender mailSender = (MailSender) SpringBeanProxy.getBean("mailSender");
            mailSender.setToMail(toMail);
            mailSender.setSubject(subject);
            mailSender.setBody(body);
            mailSender.setHtml(html);
            mailSender.send();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    public static void sendMail(String fromMail, String fromUserName, String toMail, String replyMail,
                                String subject, String body, boolean html) {
        try {
            MailSender mailSender = (MailSender) SpringBeanProxy.getBean("mailSender");
            mailSender.setFromMail(fromMail);
            mailSender.setFromUserName(fromUserName);
            mailSender.setToMail(toMail);
            mailSender.setReplyMail(replyMail);
            mailSender.setSubject(subject);
            mailSender.setBody(body);
            mailSender.setHtml(html);
            mailSender.send();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

}
