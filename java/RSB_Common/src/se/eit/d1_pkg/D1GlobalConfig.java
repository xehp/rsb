/*
GlobalConfig.java

Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
2013-02-27
Created by Henrik Bjorkman (www.eit.se/hb)

2014-11-20
Moved from web package to RSB package
*/

package se.eit.d1_pkg;

import java.io.PrintStream;
import java.nio.file.Path;
import java.nio.file.Paths;

public class D1GlobalConfig {
	public static boolean light=true;
	
	
	
	public static final int defaultIpPort=8080;




	public static final boolean DEBUG_READ_LOCKS=true;  // If true code will check that database has been read locked when it needs to be. If this is false the code will run faster but not check for that error.


	
	public int port=defaultIpPort;
	public String luaScriptDir="lua";  // A read only directory from which scripts and other resources can be loaded 
	public String httpRootDir="web"; // A read only directory from which public html, javascript and images can be loaded	
	public String savesRootDir="saved_games"; // A directory where the server may save files.
	
	public boolean startWebBrowser=false;
	
	public final long MinutesBetweenAutoSave=60;

	// Login server is used like this, the main server is started with option:
	// -e : allow external servers, set this switch if this is the main login server. External servers use -g to connect to this server.
	// -n <password> : Prevents users from starting games directly on main server.
	// Example:
	// -e -p 8888 -r /home/henrik/rsb -n lol
	//
	// Other servers are started with this option: 
	// -g <main-server-host> <main-server-port> <sub-server-user-name> <sub-server-password> : game server only (no user data), This server shall connect to the main server, players will be sent over from there.
	// Example:
	// -p 8800 -l ../../lua -d ../../web -s ~/tmp/saved_games -g localhost 8888 a a
	//
	// The sub servers will log in to the main server and register itself in the list of servers. 
	// Clients will log in to the main server and see the list of servers.
	// Later servers can ask main server for client authorization. 
	
	//public boolean gameOnly=false;
	public String loginServerHostname=null;
	public int loginServerPort=0;
	public String loginServerUsername=null;
	public String loginServerUserPw=null;
	
	public String myHostname=null;
	//public int myPort=-1;
	
	public String startGamePw=null;

	public boolean allowExternal=false;
	
	
	public boolean useDeJni=false;
	
	
	public D1GlobalConfig()
	{
	}

	public String toString()
	{
		StringBuffer sb=new StringBuffer();
 	    Path http = Paths.get(httpRootDir);
 	    sb.append("http dir: "+ http.toAbsolutePath()+"\n");
		
 	    Path saves = Paths.get(savesRootDir);
 	    sb.append("save dir: "+ saves.toAbsolutePath()+"\n");
		
  	    sb.append("server ip port: "+ port+"\n");
				
  	    sb.append("start web browser: "+startWebBrowser+"\n");
  	    
		return sb.toString();
	}
	
	public void logConfig(PrintStream out)
	{
		out.println(toString());
	}
	
	// The main server (AKA lobby server) is the one where users login.
	// The other servers are called external servers, the user must login to the main server and then be redirected to here.
	public boolean isMainServer()
	{
		return (loginServerHostname==null);
	}
	
}
