// PlayerConnectionThread.java
//
// Copyright (C) 2012 Henrik Bjorkman www.eit.se
//
// This is the thread that handles the connection from a client.
// One instance of this class per client.
//
//
// Written using this instruction:
// http://courseweb.xu.edu.ph/courses/ics10/tutorials/Java_Unleashed_Second_Edition/ch25.htm#UsingUDPIP
//
// History:
// Created by Henrik Bjorkman 1997-05-03
// Moved to own file. Henrik 1999-09-08
// Changed to use swing. Henrik 2012-10-14 
// Changed to udp. Henrik 2012-10-16
// Adapted for use with RSB. Henrik 2013-05-04


package se.eit.rsb_srv_main_pkg;

import java.io.IOException;
import java.util.Random;

import javax.mail.MessagingException;

import se.eit.rsb_package.*;
import se.eit.rsb_server_pkg.OpServer;
import se.eit.rsb_server_pkg.SendEmail;
import se.eit.rsb_server_pkg.ServerBase;
import se.eit.rsb_server_pkg.ServerTcpConnection;
import se.eit.empire_package.EmpireServer;
import se.eit.flameengine_package.FlameServer;
import se.eit.LoginLobbyPkg.LoginLobbyConnection;
import se.eit.LoginLobbyPkg.LoginLobbyServer;
import se.eit.d1_pkg.*;
//import se.eit.drekkarGamePkg.DrekkarGameServer; // This is not needed in 2d versions such as flame & empire
import se.eit.web_package.*;
//import se.eit.yukigassen_pkg.YukigassenServer; // This is not needed in 2d versions such as flame & empire



//This class is the thread that handles all communication with a client
public class PlayerConnectionThread extends Thread implements WebSocketConnection/*, NotificationReceiver*/
{
	static final long desired_frame_rate = 0; // actually milliseconds per frame, if zero run as fast as possible bit a small sleep/wait only.

	//static final boolean send_everything=false; // true here was used for testing, not used now, this flag can be removed eventually
	
    protected D1GlobalConfig config=null;
	
	protected ServerTcpConnection stc=null;
	UserAccount player=null;
    
	
    static Random generator = new Random();

    
    public MyBlockingQueue<String> msgQueue=new MyBlockingQueue<String>(32);

    private LoginLobbyConnection loginServerConnection;

    //protected boolean support2d=false;
    //protected boolean support3d=false;
    protected String gamesSupportedByClientAndServer=null; // List of games supported by both the current client and the server.
    
    static final protected String gamesSupportedByServer = "empire cityPvp cardGame flame lobby textAdventure TankerStrike yukigassen drekkar";
    
	public static void static_error(String str)
	{
		System.out.flush();
		System.err.println(className()+": static_error: "+str);
		Thread.dumpStack();
		System.exit(1);
	}

	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return PlayerConnectionThread.class.getSimpleName();	
	}

	public void error(String s)
	{
		stc.error(className()+": "+s);
	}

	  
    public void debug(String str)
	{
    	WordWriter.safeDebug(className()+"("+stc.getTcpInfo()+"): "+str);
	}

	public static void println(String s)
	{
	    System.out.println("PlayerConnectionThread: "+s);
	}

    
	// Remember to start the thread after creating the instance.
  	public PlayerConnectionThread(D1GlobalConfig config, WebConnection tc, D1SubRoot db, LoginLobbyConnection loginServerConnection)   
  	{
  		this.config = config;
  		this.stc = new ServerTcpConnection(db, tc);
  		this.loginServerConnection = loginServerConnection;
	    debug("PlayerConnectionThread: "+tc.getTcpInfo());	    
  	}
  
  	/*
  static int my_abs(int a)
  {
	  if (a<0)
	  {
		  return -a;
	  }
	  	  
	  return a;
  }
  */

  
  
	//To get the first word in a string.
	public static String getFirstWord(String str) 
	{
	    int i=0;
	    while (i<str.length() && !Character.isSpaceChar(str.charAt(i))) {i++;}
	    return(str.substring(0,i));
	}
	  
	  
	//To get a string without the first n words in string str.
	public static String skipWords(String str, int n)
	{
	    int i=0;
	    
	    while (i<str.length() && Character.isSpaceChar(str.charAt(i))) {i++;}
	    
	    while (n>0)
	    {
	      while (i<str.length() && !Character.isSpaceChar(str.charAt(i))) {i++;}
	      while (i<str.length() && Character.isSpaceChar(str.charAt(i))) {i++;}
	      n--;
	    }
	    
	    return(str.substring(i));
	}

  

    

	
	public boolean isOpen()
	{
		return (stc!=null) && (stc.isOpen());
	}
	
 
 	

	
	
	public UserAccount findPlayer(String name)
	{
    	if (name==null)
    	{
            error("name was null");
    		return null;
    	}
    	
    	D1SubRoot playersDatabase = stc.findOrCreatePlayersDb();
    	
		D1SubRoot ro = playersDatabase.findDb(name);
		

		if (ro==null)
		{
		   	debug("player "+ name+" was not found");
			return null;
		}
			
		try
		{
			return (UserAccount)ro;				
		}
		catch (ClassCastException e)
		{
			error("\"" + name+"\" was not a player "+e);
		}
		return null;
	}
	

	
    // This code is only needed on main login server
	// TODO we need a way to remove this functionality completely on other than main login server 
    public UserAccount playerLogin()
    {
  		String name=null;
  		int n=0;
    	debug("playerLogin started from "+stc.getTcpInfo());

        while(++n<16) 
		{
        	name = stc.promptString("enter_player_name", "enter your name");
        	
        	if (name==null)
        	{
        		break;
        	}

        	UserAccount player=findPlayer(name);
        	  			
    		if (player!=null)
    		{
            	//debug("playerLogin asking for pw for player '"+name+"' from "+stc.getTcpInfo());       	
				String pw = stc.promptString("enter_player_pw","give password");
		    		    			
				if (pw==null)
				{
					break;
				}
				else if (player.isPasswordOk(pw))
    			{
                	System.out.println("Player '"+name+"' logged in from '"+stc.getTcpInfo()+"'");    	
    				stc.alertBox("login_ok", "login ok");
    				return player;
    			}
    			else
    			{
                	debug("playerLogin wrong pw for player '"+name+"' from "+stc.getTcpInfo());      	
    				stc.alertBox("wrong_password", "wrong pw");
    				return null;
    			}
    		}
    		else
    		{
            	debug("playerLogin not found '"+name+"' from "+stc.getTcpInfo());       	
    			stc.alertBox("player_not_found", "player " + name + " not found");
				return null;
    		}
    		
		}
    	System.out.println("Login failed, "+n+" tries"+", from "+stc.getTcpInfo());    	

    	return null;
    }
    
    // Register a new player
    // This code is only needed on main login server
    public UserAccount regNewPlayer()
    {
  		String name=null;
  		String info=stc.getTcpInfo();
    	debug("reg new player "+info);
  		String password=null;

    	debug("regNewPlayer started from "+stc.getTcpInfo());
  		
        for(;;) 
		{        	
        	// Ask player to give a desired user name
        	name = stc.promptString("enter_player_name", "give a new name");
        	if (name==null)
        	{
        		break;
        	}
        	else if (OpServer.isStringOkAsPlayerName(name))
    		{
	    		UserAccount player = findPlayer(name) ;
	    		
	    		if (player==null)
	    		{
	    			// PlayerData not found, we can continue registering the new player profile
	    			

	    			while((stc!=null) && (stc.isOpen()) && (password==null))
	    	        {    			
	    				password = stc.promptString("enter_new_password", "give new password");
		    			   					    			
		    			if (OpServer.isStringOkAsPw(password))
		    			{
			    			/*String pw2 = promptBox("confirm_new_password", "confirm new password");
			    			
			    			if ((isEmailAddressOk(pw2, 3)) && (pw2.equals(pw)))*/
			    			{
			    				// password is ok
			    				
			    				for(;;)
			    				{
					            	String emailAddress = stc.promptString("enter_email", "give an email address to be used if you forget your password");
					            	if (emailAddress==null)
					            	{
					            		return null;
					            	}
					            	else if (OpServer.isEmailAddressOk(emailAddress))
					    			{
						    	    	debug("regNewPlayer accepted for '"+name+"', from "+stc.getTcpInfo());	    			

						    	    	D1SubRoot playersDb = stc.findOrCreatePlayersDb();
				    					playersDb.lockWrite();
				    					try
				    					{
				    						player = new UserAccount(playersDb, name, info, "null", config);
				    					}
				    					finally
				    					{
					    					playersDb.unlockWrite();				    						
				    					}
					            		
					    				if (player.setPassword(password)!=true)
					    				{
						    				stc.alertBox("password_failed_to_set", "The password given was rejected.");
					    				}
					    				
					    				
					    				player.emailAddress=emailAddress;
				    					//playersDb.setGlobalConfig(config);

					    				stc.alertBox("player_reg_accepted", "player reg accepted");

					    				int r=0;
					    				try {
					    					r=SendEmail.sendEmailTo(player.emailAddress, "mpe", "Hello " + player.getName()+ "\nWelcome to MultiPlayerEmpire\nwww.eit.se/hb/mpe\nActivation code: "+player.emailVerificationCode +"\n\n");
					    				}
						    		    catch (MessagingException mex) {
						    		         // Prints all nested (chained) exceptions as well
						    		        mex.printStackTrace();
						    		        r|=2;
						    		    }
						    			
						    			if (r!=0)
						    			{
						    				stc.alertBox("email_failed", "sending email did not seem to work"); 
						    			}
					    				
					    				return player;
					    			}
					            	else
					            	{
					            		stc.alertBox("email_not_ok", "the entered email address was not ok");
					            	}
			    				}
			    			}
			    			/*else
			    			{
			    				stc.alertBox("passwords_did_not_match", "passwords did not match");
			    			}*/
		    			}
		    			else
		    			{
		    				stc.alertBox("password_to_long_or_short", "to long, to short or contains characters not allowed");
		    			}
	    	        }
	    		}
	    		else
	    		{
	    			stc.alertBox("player_already_exist", "player "+name+" already exist");
	    		}
    		
    		}
    		else
    		{
				stc.alertBox("name_to_long_or_short", "to long, to short or containing characters not allowed");
    		}
		}
        return null;
    }

    
    // This code is only needed on main login server
    public void recoverPw()
    {
  		String name=null;
    	debug("recover password from "+stc.getTcpInfo());
    	
        for(;;) 
		{
        	name = stc.promptString("enter_player_name", "give a new name");
        	if (name==null)
        	{
        		break;
        	}
        	else if (OpServer.isStringOkAsPlayerName(name))
    		{
    		
	
	    		UserAccount player = findPlayer(name) ;
	    		
	    		if (player!=null)
	    		{
	    			stc.alertBox("player_found", "username " + player.getName() + " found, will send email (it can take a few minutes, if still not found check spam filters)");
	    			
	    			int r=0;
	    			try {
	    				String tmpPw=player.createTemporaryPassword();
	    				r=SendEmail.sendEmailTo(player.emailAddress, "your temporary password", tmpPw);
	    			}
	    		    catch (javax.mail.internet.AddressException ie) {
	    		         // Prints all nested (chained) exceptions as well
	    		        ie.printStackTrace();
	    		        r|=2;
	    		    }
	    			
	    			if (r!=0)
	    			{
	    				stc.alertBox("email_failed", "sending email did not seem to work"); 
	    			}
	    			
	        		break;
	    		}
	    		else
	    		{
	    			stc.alertBox("user_not_found", "the given username don't seem to exist");
	        		break;
	    		}
    		}
    		else
    		{
				stc.alertBox("name_to_long_or_short", "to short or to long");
    		}
		}		
    	
    	
    }

    /*
    public void set(String prefName, String prefValue)
    {
    	player.addPlayerPrefThreadSafe(prefName, prefValue);	
    }
    */
    
    
    // // These keyboardMappings will hopefully match the enum in main_sdl2.cpp
    public void keyboardMappings() throws IOException
    {
    	for(;;)
    	{    		
	    	int[] keys={
	    			UserAccount.moveForward,
	    			UserAccount.moveBackwards,
	    			UserAccount.moveLeft,
	    			UserAccount.moveRight,
	    			UserAccount.jumpUp,
	    			UserAccount.actionKey,
	    			UserAccount.enterText,
	    			UserAccount.showScore,
	    			UserAccount.backOrCancel,
	    			//PlayerData.swapTeam,
	    			UserAccount.clearText,
	    			UserAccount.invScreen
	    			};	    	
			

	    	
    		D1List<String> list=new D1List<String>();

    		for(int i=0; i<keys.length; i++)
    		{
    			list.add(player.optionString(keys[i]));
    		}
    		list.add("set all default");
    		list.add("back");
			
					
	    	int r = stc.promptButtons("keyboardMappings", "Choose game action to assign a key to", list);
	    	
	    	if (r<0)
	    	{
	    		debug("less than zero from promptButtons");
    	    	sendPlayerPreferences();
    			return;
	    	}
	    	else if (r<keys.length)
	    	{
	    		int internalActionCode=keys[r];
				String prefName = UserAccount.getNameOfUserActionCode(internalActionCode);
    			int prefValue=stc.promptKey("selectAButton", "enter key to use for "+prefName);
    			player.addPlayerPrefThreadSafe(internalActionCode, prefValue);

    			debug("playerPreference "+prefName + " "+prefValue);

    			// Send the new key pref to client right away
    			WordWriter ww=new WordWriter();
    			ww.writeWord("playerPreference");
    			ww.writeWord(prefName);
    			ww.writeString(""+prefValue);
    			String str = ww.getString();
				try {
					stc.writeLine(str);
				} catch (IOException e1) {
					e1.printStackTrace();
				} 			
	    	}
	    	else
	    	{
	    		r-=keys.length;
		    	switch(r)
		    	{
		    		case 0:
		    		{
		    			debug("playerPreferenceDefault");
		    			player.clearStoredThreadSafe();
		    			
		    			player.setDefault();

						try {
							stc.writeLine("playerPreferenceDefault");
						} catch (IOException e) {
							e.printStackTrace();
						}
		    			break;
		    		}
		    		default:
		    	    	sendPlayerPreferences();
		    			return;
		    	}
	    	}
    	}
    }

    public void addQuickKeys() throws IOException
    {    	
		int key=stc.promptKey("selectAQuickKey", "press a key you wish to use for quick command");
	
		debug("addQuickKeys "+key+" '"+UserAccount.getNameOfInternalKeyCode(key)+"'");
		if (key==0)
		{
			stc.alertBox("not_supported_key", "That key is not yet supported");
		}
		else
		{		
			String cmd=stc.promptString("selectAQuickCmd", "enter the command to perform when '"+UserAccount.getNameOfInternalKeyCode(key)+"' is pressed");

			player.setQuickKeyCmd(key, cmd);
		}
    }

    public void removeQuickKey(int key) throws IOException
    {    	
		int r=stc.promptNoOrYes("Do you want to remove quick command on '"+UserAccount.getNameOfInternalKeyCode(key)+"'");
	
		if (r==0)
		{
			// Do nothing
		}
		else
		{		
			player.removeQuickKeyCmd(key);
		}
    }
    
    public void quickKeys() throws IOException
    {
    	for(;;)
    	{
			D1List<String> list=new D1List<String>();
			D1List<Integer> keyCode=new D1List<Integer>();	    	
			
	    	for (Integer key: player.quickKeyCmdHashMap.keySet())
	    	{
	    		String value=player.quickKeyCmdHashMap.get(key);	    		
				String m=String.format("%-7s %-7s %s",UserAccount.getNameOfInternalKeyCode(key), key, value);
				list.add(m);
				keyCode.add(key);
	    	}
	    	final int n=player.quickKeyCmdHashMap.size();
	    	
			list.add("add quick key command");
			list.add("back");
	    	
	    	int r = stc.promptButtons("keyboardMappings", "Quick command keys", list);
	    	
	    	if (r<0)
	    	{
	    		debug("less than zero from promptButtons");
		    	sendPlayerPreferences();
				return;
	    	}
	    	else if (r<n)
	    	{
	    		removeQuickKey(keyCode.get(r));
	    	}
	    	else if (r==n)
	    	{
	    		addQuickKeys();
	    	}
	    	else if (r==(n+1))
	    	{
	    		break;
	    	}
    	}
    }
    
    public void accountSettingsMenu() throws IOException
    {
    	if (!config.isMainServer())
    	{
    		stc.alertBox("no_account_settings", "This is an external server, account settings is only on the main server");
    	}
    	else
    	{
	        // This code is only needed on main login server
	    	for(;;)
	    	{			
				
				Object[] options = {"Change password",
	                    "Change email",
	                    "Email activation",
	                    "keyboard mappings",
	                    "quick keys",
	                    "test reconnect",  // This was added to test reconnectServer, remove this later.
	                    "Back"};
				
	        	int r = stc.promptButtons("change_pw", "Choose setting to change", options);
	        	switch(r)
	        	{
	        		case 0:
	        		{
	    				String pw = stc.promptString("enter_new_password", "give new password");
	    		    	
	    				if (OpServer.isStringOkAsPw(pw))
	    				{
	    	    			String pw2 = stc.promptString("confirm_new_password", "confirm new password");
	    	    			
	    	    			if ((OpServer.isStringOkAsPw(pw2)) && (pw2.equals(pw)))
	    	    			{    	    				
	    	    				player.setPassword(pw);
	    	    				//player.setGlobalConfig(config);
	    	    				player.saveRecursive();
	    	    				
	    		    			stc.alertBox("password_changed", "password changed");
	    	    				return;
	    	    			}
	    	    			else
	    	    			{
	    	    				stc.alertBox("passwords_did_not_match", "passwords did not match");
	    	    			}
	    				}
	    				else
	    				{
	    					stc.alertBox("password_to_long_or_short", "to long or short");
	    				}
	        			break;
	        		}
	        		case 1: 
	        		{
	    				String ea = stc.promptString("enter_new_email", "give new email address");
	    		    	
	    				if (OpServer.isEmailAddressOk(ea))
	    				{
	    					player.emailAddress=ea;
	    					//player.setGlobalConfig(config);
	    					player.saveRecursive();
	    					
	    					int r2=0;
	    					try{
	    						r2=SendEmail.sendEmailTo(player.emailAddress, "mpe", "Hello " + player.getName()+ "\nWelcome to MultiPlayerEmpire\nwww.eit.se/hb/mpe\nActivation code: "+player.emailVerificationCode +"\n\n");
	    					}
	    	    		    catch (javax.mail.internet.AddressException ie) {
		   	    		         // Prints all nested (chained) exceptions as well
		   	    		        ie.printStackTrace();
		   	    		        r2|=2;
	    	    		    }
	    	    			if (r2!=0)
	    	    			{
	    	    				stc.alertBox("email_failed", "the given email address did not seem to work"); 
	    	    			}
	        				return;
	    				}
	    				else
	    				{
	    					stc.alertBox("email_to_long_or_short", "to long or short, did not change it");
	    				}
	        			
	        			break;
	        		}
	        		case 2: 
	        		{
	    				if (player.emailVerificationCode==0)
	    				{
	    					
	    					stc.alertBox("already_activated", "your email adress is already verified");
	    				}
	    				else
	    				{
	    				
	    					int i = stc.promptInt("enter_activation_code", "enter activation code", 1, 999999);
	    			    	
	    					if (i<0)
	    					{
	    						stc.alertBox("number_not_in_range", "number not in range");
	    						return;
	    					}
	    					else if (i==player.emailVerificationCode)
	    					{
	    						stc.alertBox("email_verified", "email verified");
	    						player.emailVerificationCode=0;
	    						//player.setGlobalConfig(config);
	    						player.saveRecursive();
	    						return;
	    					}
	    				}
	        			
	        			break;        		
	        		}
	        		case 3: 
	        		{
	        			keyboardMappings();
	        			break;
	        		}
	        		case 4: 
	        		{
	        			quickKeys();
	        			break;
	        		}
	        		
	        		case 5: // This was added to test reconnectServer, remove this later.
	        		{
	        			stc.cc.writeLine("reconnectServer localhost 8080");
	        			break;
	        		}
	        		
	        		default: return;
	        	}
				
				
	    	}
    	}
    }
    
    // Creates a server object from name
    // All game servers needs to be added here
    // When adding a game here add it also in the help text in gamesSupportedByServer.
	static public ServerBase serverFactory(String t)
	{
		final char ch=t.charAt(0);
		
		switch(ch)
		{
			/*case 'D':
			case 'd':
			{
				if ((t.equalsIgnoreCase("drekkar")) || (t.equalsIgnoreCase("DrekkarServer")))
				{
					// This is not needed in 2d versions such as flame & empire
					return new DrekkarGameServer();
				}
				break;
			}*/
			case 'E':
			case 'e':
			{
				if ((t.equalsIgnoreCase("empire")) || (t.equalsIgnoreCase("EmpireServer")))
				{
					return new EmpireServer();
				}
				break;
			}
			case 'F':
			case 'f':
			{
				if ((t.equalsIgnoreCase("flame")) || (t.equalsIgnoreCase("FlameServer")))
				{
					return new FlameServer();
				}
				break;
			}
			case 'L':
			case 'l':
			{
				if ((t.equalsIgnoreCase("lobby")) || (t.equalsIgnoreCase("LoginLobbyServer")))
				{
					return new LoginLobbyServer();
				}
				break;
			}
			/*case 'Y':
			case 'y':
			{
				if ((t.equalsIgnoreCase("yukigassen")) || (t.equalsIgnoreCase("YukigassenServer")))
				{
					// This is not needed in 2d versions such as flame & empire
					return new YukigassenServer();
				}
				break;
			}*/
			default:
				break;
		}
		
		static_error("Did not find a server for '"+t+"'");
		
		return null;
	}
    
    public String selectTypeOfGame()
    {
		D1List<String> tmpNamesList=new D1List<String>();

		
		WordReader sr=new WordReader(gamesSupportedByClientAndServer);
		while (sr.isOpenAndNotEnd())
		{
   			String tmpName=sr.readWord();
   			tmpNamesList.add(tmpName);
		}	

		
		if (tmpNamesList.size()>1)
		{
			// Client program support more than one game so we need to ask which one to start.
			D1List<String> typeNamesList=new D1List<String>();
			for(int i=0; i<tmpNamesList.getCapacity(); i++)
			{
				String name=tmpNamesList.get(i);
				if (name!=null)
				{
					typeNamesList.add(name);
				}
			}
		
			
			int typeOfGame = stc.promptButtons("list_enter_game_type_name", "select type of game", typeNamesList);    		
	
			if (typeOfGame>=0)
			{
		    	String gameTypeName = typeNamesList.get(typeOfGame);
		    	//stc.setGameTypeName(gameTypeName);
				return gameTypeName;
			}
			else
			{
				//stc.setGameTypeName(null);
			}
			
		}
		else if (tmpNamesList.size()==1)
		{
			// Client program support only one game so no need to ask which game player wants.
			
	    	String gameTypeName = tmpNamesList.get(0); // Assuming it in in position 0, should work for now.
	    	//stc.setGameTypeName(gameTypeName);
			return gameTypeName;
		}
		else
		{
			// Client program does not support any game that server also supports.

			stc.alertBox("noCompatibleGame", "Did not find any game that is compatible with your client program");
		}
		
		return null;
    }


		

    
    // Start a new game, ask user for which type of game to start
    // Returns the name of the started game
    public String startNewGame(UserAccount userAccount)
    {
    	//String typeOfGame=null;
    	debug("startNewGame");
		
        String worldName=null;

        if (config.startGamePw!=null)
    	{
    		String tmpPw=stc.promptString("start_game_pw", "A password is required to start a new game");
    		if (!config.startGamePw.equals(tmpPw))
    		{
    			return null;
    		}
    	}


    	stc.gameTypeName = selectTypeOfGame();
    	//int typeOfGame = getNrFromName(stc.gameTypeName);
    	
		if (stc.gameTypeName==null)
		{
			debug("user pressed cancel or disconnected.");
			worldName=null;
		}
		else 
		{
			ServerBase s=serverFactory(stc.gameTypeName);
			
			if (s==null)
			{
				debug("could not start server for '"+stc.gameTypeName+"'");
				worldName=null;
			}
			else
			{
				WorldBase wb = s.createAndStore(config, userAccount, stc);
				if (wb==null)
				{
					debug("game was not created '"+stc.gameTypeName+"'");
					worldName=null;					
				}
				else
				{
					worldName =wb.getName();
				}
			}
			
		}
		
    	return worldName;
    }
    
    
    
    
    
    public void listGames() throws IOException
    { 	
    	stc.gameTypeName = selectTypeOfGame();
    	
    	D1SubRoot gamesDatabase = stc.findOrCreateGameDb();
    	
    	gamesDatabase.lockRead();
	    try
	    {
			if (gamesDatabase.listOfStoredObjects!=null)
			{	    	
				for (D1Storable bo : gamesDatabase.listOfStoredObjects)
				{
			    	debug(bo.getName());		    	
			    	
			    	if (bo instanceof WorldBase)
			    	{
			    		WorldBase wb = (WorldBase)bo;
				    	stc.writeLine("consoleMessage \""+bo.getName()+" ("+wb.notificationDataList.size()+")\"");  //
			    	}
			    	else
			    	{
			    		stc.writeLine("consoleMessage \""+bo.getName()+"\"");  //
			    	}
			    	
		    	}
			}
	    }
	    finally
	    {
	    	gamesDatabase.unlockRead();
	    }
    }
			

    public void promptListGames() throws IOException
    { 	
    	stc.gameTypeName = selectTypeOfGame();
    	
    	if (stc.gameTypeName==null)
    	{
    		return;
    	}
    	
    	D1SubRoot gamesDatabase = stc.findOrCreateGameDb();
    	
    	
  	
    	
    	gamesDatabase.lockRead();
	    try
	    {
	    	
	    	if (gamesDatabase.listOfStoredObjects!=null)
			{	    	
		    	//int n = gamesDatabase.getNSubObjects();

		    	D1List<String> possibilities = new D1List<String>();
		    	//Object possibilities[] = new Object[n];
		    	//int i=0;
		    	
				for (D1Storable bo : gamesDatabase.listOfStoredObjects)
				{
					String str=bo.getName();

					// Add a number indicating the number of players online
					if (bo instanceof WorldBase)
			    	{
			    		WorldBase wb = (WorldBase)bo;
						str+=" ("+wb.notificationDataList.size()+")";
			    	}
					possibilities.add(str);					
		    	}
				
				stc.promptButtons("promptListGames", "existing games (number of connected players)", possibilities);
			}
	    }
	    finally
	    {
	    	gamesDatabase.unlockRead();
	    }
    }

    
    
    // We get here when client wants to play in one world
    protected void playWorld(String worldName, UserAccount userAccount)
    {
        debug("worldName \"" + worldName+"\" playerName \"" + userAccount.getName() +"\"");
        
        if (worldName==null)
        {
        	return;
        }
        
        D1SubRoot wdb = stc.findOrCreateGameDb();
        WorldBase w = (WorldBase)wdb.findDbSubRootNotRecursiveReadLock(worldName);

  		if (w!=null)
        {
    		stc.alertBox("joining_world", "joining world "+ w.getNameAndPath("/"));
    		
    		//String serverType=getServerTypeFromWorldType(w);
    		String serverType=w.serverForThisWorld();
    		
    		if (serverType!=null)
    		{
				ServerBase s=serverFactory(serverType);
				if (s==null)
				{
					debug("could not start server for '"+serverType+"'");
					worldName=null;
				}
				else
				{
					s.joinWorld(w, config, userAccount, stc);
				}
    		}
    		else
    		{
            	stc.alertBox("game_not_found", "that game is not supported by server"+ worldName);
    		}
    		
    		
        }
        else
        {
        	stc.alertBox("world_not_found", "world not found "+ worldName);
        }
    }   

    // Returns the name of the world that the user wants, null if no world selected.
    protected String selectAndPlayWorld(UserAccount userAccount)
    {

    	stc.gameTypeName = selectTypeOfGame();

    	if (stc.gameTypeName==null)
    	{
    		return null;
    	}
    	
    	String worldName=stc.selectWorld();
    	
    	
    	if (worldName!=null)
        {
    		playWorld(worldName, userAccount);
        }
    	
    	return worldName;
    }
    
    // This code is only needed on main login server
	public void playerLoginOrRecoverPw() throws IOException
	{
        while(player==null) 
		{		 
		
			// These are the expected replies on our prompting.
			Object[] options = {"Login",
		            "Reg new account",
		            "Recover pw",
		            "Cancel"};
			
			// Send request to client to prompt user for what he or she want to do, login or register...
			int r = stc.promptButtons("login_or_reg", "Welcome to drekkar", options);
			
			// We have a reply. Call next method to proceed accordingly.
			switch(r)
			{
				case 0: player=playerLogin(); break;
				case 1: player=regNewPlayer(); break;
				case 2: recoverPw(); break;
				default: throw new IOException("cancel");
			}
		}
	}
    
	public void sendPlayerPreferences() throws IOException
	{
		/*
		if ((player!=null) && (player.listOfStoredObjects!=null) && (support3d))
	    {
        	for (DbBase b: player.listOfStoredObjects)
        	{
        		if (b instanceof RsbString)
        		{
        			RsbString rs=(RsbString)b;
        			WordWriter ww=new WordWriter();
        			ww.writeWord(cmd);
        			ww.writeWord(rs.getName());
        			ww.writeString(rs.getValue());
        			String str = ww.getString();
	        		stc.writeLine(str); 			
        		}
        	}
	    }
	    */
		
		WordWriter ww=new WordWriter();
		player.sendPlayerPreferences(ww);
		String str = ww.getString();
		
		// We should be able to send str directly in one (stc.writeLine(str)) but our web client will not see the individual lines, so we send them line by line to stc.
		//stc.writeLine(str);
		WordReader wr=new WordReader(str);
		while (wr.isOpenAndNotEnd())
		{
			String line=wr.readLine();
			stc.writeLine(line);
		}
		

	}

	public String startAndPlayWorld(UserAccount userAccount)
	{
		String worldName=startNewGame(userAccount);
		if (worldName!=null)
		{
			playWorld(worldName, userAccount);
		}
		return worldName;
	}
	
	public void askUserWhatToDo(UserAccount userAccount) throws IOException
    {
		int n=16;
        while ((stc!=null) && (stc.isOpen()) && (--n>=0))
		{
        	// These are the alternatives to be given to the user at client end.
			Object[] options = {"Continue game",		                    
                    "Start new game",
                    "List existing games",
                    "Account settings",
                    "Cancel"};
			
        	int r = stc.promptButtons("join_or_create", "Do you want to join an existing game or start a new?", options);
        	switch(r)
        	{
        		case 0: selectAndPlayWorld(userAccount); break;
        		case 1: startAndPlayWorld(userAccount); break;
        		case 2: promptListGames(); break;
        		case 3: accountSettingsMenu(); break;
        		default: n=0; break;
        	}
   		}
    }
	
	public void askUserWhatToDoSubServer(UserAccount userAccount) throws IOException
    {
		int n=16;
        while ((stc!=null) && (stc.isOpen()) && (--n>=0))
		{
        	// These are the alternatives to be given to the user at client end.
			Object[] options = {"Continue game",		                    
                    "Start new game",
                    "Cancel"};
			
        	int r = stc.promptButtons("join_or_create", "Do you want to join an existing game or start a new?", options);
        	switch(r)
        	{
        		case 0: selectAndPlayWorld(userAccount); break;
        		case 1: startAndPlayWorld(userAccount); break;
        		default: n=0; break;
        	}
   		}
    }


	// Ask client if it support 2D or 3D games
	// TODO: Make separate questions instead that are answered yes or no.
	/*
	public void checkClient3dSupport() throws IOException
	{
        if ((stc!=null) && (stc.isOpen()))
		{
        	// These are the alternatives to be given to the user at client end.
			Object[] options = {"2d",		                    
                    "3d",
                    "2d and 3d",
                    "Cancel"};
			
        	int r = stc.promptButtons("2d_or_3d_support", "Are you running a 2d or 3d capable client?", options);
        	switch(r)
        	{
        		case 0: support2d=true; break;
        		case 1: support3d=true; break;
        		case 2: support2d=true; support3d=true; break;
        		default: break;
        	}

   		}
	}
	*/
	
	
	public void checkClientGameSupport() throws IOException
	{
        if ((stc!=null) && (stc.isOpen()))
		{
        	// For the alternatives to be given to the user at client end see method PlayerConnectionThread.serverFactory for available games.
        	String tmpList = stc.promptString("game_support", "Which games does this client support?");
        
		    if (tmpList==null)
		    {
		    	debug("The client does not support any game");
		    }
		    else
		    {
		    	//debug("Client game support: '"+tmpList+"'");
		
		    	// Tell client if there was a compatibility problem
				WordReader sr=new WordReader(tmpList);
				WordWriter sw=new WordWriter();
				while (sr.isOpenAndNotEnd())
				{
					String tmpName=sr.readWord();
					if ((config.allowExternal!=true) && (tmpName.equals("lobby") || tmpName.equals(LoginLobbyServer.className())))
					{
						// If its not a main server (AKA lobby server) then server does not need or support lobby.
						//debug("this is not a lobby server");
					}
					else if (serverFactory(tmpName)==null)
					{
						debug("Client want a game that we do not have: '"+tmpName+"'");
						stc.alertBox("unknownGame", "Server can not run '"+tmpName+"'. It can run: '"+gamesSupportedByServer+"'");
					}
					else
					{
						sw.writeWord(tmpName);
					}
				}
				gamesSupportedByClientAndServer = sw.getString();
				debug("gamesSupportedByClientAndServer: '"+gamesSupportedByClientAndServer+"'");
		    }
   		}
	}
	
	
	public UserAccount checkClientCredentials() throws IOException
	{
		UserAccount userAccount = null;
        while(userAccount==null) 
		{		 	
			// These are the expected replies on our prompting. 

        	// The client is supposed to have gotten index and code from the login server.
        	// So first we ask the client for that.
        	int playerIndex = stc.promptInt("enter_player_index", "enter player index", Integer.MIN_VALUE, Integer.MAX_VALUE);
        	int playerCode = stc.promptInt("enter_player_code", "enter player code", Integer.MIN_VALUE, Integer.MAX_VALUE);
        	
        	
        	//NotificationData notificationData = new NotificationData(this, playerIndex);
        	
        	// Then we ask server if it was OK.
        	// Now we need to connect login server to ask if playerIndex and playerCode was OK. That is done via the LoginServerConnection.
        	userAccount = loginServerConnection.isClientCodeOk(/*notificationData,*/ playerIndex, playerCode);

        	if (userAccount!=null)
        	{
        		stc.alertBox("player_code_accepted", "login ok");
        		return userAccount;
        	}
		}
        return null;
	}

	
    
    //Provide the service.
  	public void run() 
  	{    
  		System.out.println("A client connected from \""+stc.getTcpInfo()+"\"");

  		// This must match what is done by ClientThread  		
  		
        try {

        	// The first is to wait for client to say what protocol version it wish to use
			String protocol = stc.readLine(60*1000); // 60*1000 = 1 min timeout
 		
			//debug("testing !\"#��%&/()=!");
			
			// The names here are a bit misleading. Its more like simple or advanced protocol selected here.
	  		if (protocol.equals("rsb_game") || protocol.equals("rsb_web_game"))
	  		{
	  			debug("client accepted for protocol '" + protocol+"'");

				stc.dontUseRef=protocol.equals("rsb_web_game");

	  			
				//stc.writeLine("xehpuk.com/mpe/"+ Version.ver);
		
				// Verify that the client is of same version.
				// This is done by sending a string to client with the servers version.
				// The client is expected to reply with its version.
		  		//String serverNameAndVersion=WordReader.replaceCharacters(Version.getProgramNameAndVersion()+"/server", ' ', '_');
		  		//String clientNameAndVersion=WordReader.replaceCharacters(Version.getProgramNameAndVersion()+"/client", ' ', '_');
				String serverNameAndVersion=Version.serverVersion();
				String clientNameAndVersion=Version.clientVersion();
				String clientVersion=stc.askAndWait(serverNameAndVersion,clientNameAndVersion, "cancel");
				if (clientVersion!=null)
				{
				  //debug("clientVersion ok, "+clientVersion);
				}
				else
				{
				  debug("clientVersion not ok, expected "+clientNameAndVersion);
				  throw new IOException("cancel");
				}
				
				// Ask client if it support 2D or 3D games
				//checkClient3dSupport();
				checkClientGameSupport();

				if (gamesSupportedByClientAndServer!=null)
				{
				
					// Now its time to as client to login or register a new user account.
					if (config.isMainServer())
					{
						// Normal direct login
						playerLoginOrRecoverPw();
	
						// Here the user have logged in to an account. 
						
						// send users preferred keyboard mappings
			        	sendPlayerPreferences();

			        	// Now ask client/user what to do next.
			        	askUserWhatToDo(player);
					}
					else
					{
						// This is not a the main login server, client was sent here.
						// Indirect login.
						// Contact main login server to ask if client is OK.
						UserAccount userAccount = checkClientCredentials();
						askUserWhatToDoSubServer(userAccount);
					}	        			        
				}
				else
				{
					debug("closing client connection since it did not support any games");
				}
	  		}
	  		else
	  		{
	  			debug("client not accepted, unknown protocol "+protocol);
	  		}

		} catch (InterruptedException e) {
			// This was probably just timeout.
			//e.printStackTrace();
			debug("run: InterruptedException "+e);
		} catch (IOException e) {
			// Probably just a disconnect
			//e.printStackTrace();
			debug("run: IOException "+e);
		}
        
        close();
        debug("run end");
	}

  	
  	public void close()
  	{
  		if (stc != null)
  		{
  			if (stc.isOpen())
  			{
  				try {
					stc.writeLine("close");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
  			}
  			stc.close();
  		} 
  	}
  	
  	
	public void finalize()
	{
	    debug("finalize");    
	    
	    close();
	}

	
	public void say(String msg)
	{
		this.msgQueue.put(msg);
	}


	@Override
	public WebFileData takeSocketData(String string) {
		// TODO Auto-generated method stub
		return null;
	}

/*
	@Override
	public void notify(int subscribersRef, int sendersRef) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void unlinkNotify(int subscribersRef) {
		// TODO Auto-generated method stub
		
	}*/

}
