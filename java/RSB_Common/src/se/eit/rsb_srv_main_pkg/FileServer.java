/*

FileServer.java

Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license




This is a generic class to handle line oriented messages over TCP/IP

How to use

Create an instance of this class, give server port number as argument.
From the server thread call "process" every cycle.
If there is a new client an object of type "WebConnection" is returned
Create a new thread to run that connection via WebConnection.
Calling server thread will need to also call "myWait" somewhere to avoid using 100% of CPU. 




History:
Created by Henrik Björkman 2013-05-16

*/

package se.eit.rsb_srv_main_pkg;


import java.net.*;

import se.eit.d1_pkg.D1Base;
import se.eit.d1_pkg.D1GlobalConfig;
import se.eit.d1_pkg.D1SubRoot;
import se.eit.flameengine_package.FlameRoom;
import se.eit.web_package.*;


// TODO: Perhaps we can get rid of this class altogether

public class FileServer implements WebFileServer
{
    public final static int DEFAULT_PORT = D1GlobalConfig.defaultIpPort; // Or use same as in ConnectionCentral?
    protected int port;  // This is the port number to use when opening the tcp/ip server port.
    protected ServerSocket serverSocket;  // this is our tcp/ip server socket, clients connect to it.
    WebConnection[] listOfConnections=new WebConnection[32];   // hmm, There is also a list of connections in ServerThread. We should remove one of these.
    D1GlobalConfig config;
    D1SubRoot dataBase;
    
    /*
    public static void println(String s)
    {
        System.out.println(s);
    }
    */

    public static void debug(String str)
    {
    	WordWriter.safeDebug("FileServer: "+str);

    }

    public void error(String str)
    {
		WordWriter.safeError("FileServer: "+str);
        System.exit(1);
    }
    
    
    // Exit with an error message, when an exception occurs.
    /*
    public void fail(Exception e, String msg) 
    {
		System.out.flush();		    	
        System.err.println(msg + ": " +  e);
		Thread.dumpStack();
        System.exit(1);
    }
    */
    
    public FileServer(D1GlobalConfig config, D1SubRoot dataBase) 
    {
    	this.config=config;
    	this.dataBase=dataBase;
    }
    
    
    /*
    public synchronized void myWait(int time_ms)
	{
	    try 
	    {
	    	this.wait(time_ms);
	    }  
	    catch (InterruptedException e) {;}
	}
    */
    
    
    
	

	public WebFileData getFileData(String fileName)
	{
		WebFileData wfd=null;
		
		if (dataBase!=null)
		{
			final int len=fileName.length();
			if ((len>=(5+4)) && (fileName.substring(0, 5).equals("/tmp/")))
			{
				String path=fileName.substring(5, len-4);
								
				// "/rsb_3.4.7.png"
				//DbBase o = dataBase.getObjFromIndexPath(path);
				D1Base o = dataBase.findObjectByNameIndexOrId(path);

		    	if ((o!=null) && (o instanceof FlameRoom))
		    	{
					// http://stackoverflow.com/questions/3211156/how-to-convert-image-to-byte-array-in-java
				
		    		// TODO This package should not depend on CityPvp package.
					FlameRoom cpr=(FlameRoom)o;
					wfd = cpr.getFile(config);
		    	}

			}
		}
		
		
		
	    return wfd;		
	}

}





