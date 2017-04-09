//SendEmail.java
//http://www.vipan.com/htdocs/javamail.html
//For this to compile in eclipse the path to javamail-1.4.5 needs to be added to the project.
//javamail-1.4.5 can be downloaded from http://www.oracle.com/technetwork/java/javamail/index-138643.html
//Unzip and put it in eg ~/eclipse
//Project -> Properties -> Libraries -> Add external jar
//Give ~/eclipse/javamail-1.4.5/mail.jar
//
// TODO 
// Use the source code from
// https://java.net/projects/javamail/pages/Home
//

package se.eit.rsb_server_pkg;


import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;

//Send a simple, single part, text/plain e-mail
public class SendEmail {

  // returns 0 if OK
  public static int sendEmailTo(String to, String subject, String message) throws AddressException {
 	int r=0;
 	
 	System.out.println("sendEmailTo " + to + " "+ subject);

    // Create properties, get Session
    Properties props = new Properties();

 	
    // SUBSTITUTE YOUR EMAIL ADDRESSES HERE!!!
    final String from = "noreply@eit.se";

    // If using static Transport.send(), need to specify which host to send it to
    // SUBSTITUTE YOUR ISP'S MAIL SERVER HERE!!!
    //final String host = "smtp.riksnet.se"; // smtp.riksnet.se will only work from within riksnet 
    final String host = "piweb"; // piweb will only accept email to itself.
    //final String host = "localhost"; // localhost is default for this, it does not work on all hosts, not all host have sendmail or postfix running.
    //final String host = "smtp.tele2.se"; // This will require a username and password, see below
    props.put("mail.smtp.host", host);
     
    // To see what is going on behind the scene
    props.put("mail.debug", "true");
     

    if (to.equals("null@eit.se"))
    {
     	System.out.println("not sending to " + to);
    	return r;
    }
    	
/*
// http://stackoverflow.com/questions/16108351/sending-mail-using-java-mail-error
props.put("mail.smtp.host", "smtp.gmail.com");
props.put("mail.smtp.socketFactory.port", "465");
props.put("mail.smtp.socketFactory.class", "javax.net.ssl.SSLSocketFactory");
props.put("mail.smtp.auth", "true");
props.put("mail.smtp.port", "465");
*/
    
    Session session = Session.getInstance(props);

    try {
         // Instantiate a message
         Message msg = new MimeMessage(session);

         //Set message attributes
         msg.setFrom(new InternetAddress(from));	         
         InternetAddress[] address = {new InternetAddress(to)};
         msg.setRecipients(Message.RecipientType.TO, address);
         msg.setSubject(subject);
         msg.setSentDate(new Date());

         // Set message content
         msg.setText(message);

         //Send the message
         //String user="xehpuk";
         //String pw="******";
         //Transport.send(msg, user, pw);
         Transport.send(msg);
    }
    catch (MessagingException mex) {
         // Prints all nested (chained) exceptions as well
         mex.printStackTrace();
         r |= 1;
    }
    return r;
  }
}//End of class