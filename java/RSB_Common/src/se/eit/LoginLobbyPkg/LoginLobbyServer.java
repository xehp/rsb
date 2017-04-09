/*
LoginServer.java


Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
License: www.eit.se/rsb/license


*/

// This is not needed in flame version

package se.eit.LoginLobbyPkg;


import java.io.IOException;

import se.eit.rsb_package.*;
import se.eit.rsb_server_pkg.OpServer;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;



public class LoginLobbyServer extends OpServer 
{
	final int timeOutTime_ms = 10000; // 
	final int timeOutTimeMaxCount = 5*60*1000/timeOutTime_ms; // Tells how many timeOutTime_ms we wait until client alive ping
	int currentbuilding = 0;

	int debugCount=0;
	
	int team=LoginLobbyWorld.UnknownType;

	RsbString myEntry=null;
	
	String serverIp=null;
	int serverPort=-1;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return LoginLobbyServer.class.getSimpleName();	
	}	
	
    public void debug(String str)
	{
    	WordWriter.safeDebug(className()+"("+stc.getTcpInfo()+"): "+str);
	}

    public static void error(String str)
	{
    	WordWriter.safeError(className()+": "+str);
	}

    
    
	public LoginLobbyServer() {
		super();
	}


	
	/*
	@Override
    public String createAndStore(String worldName)
    {
   		createAndStoreNewGame(worldName);
		return worldName;
    }
    */

	@Override
	public void joinWorld()
	{

		try
		{
			LoginLobbyWorld w=(LoginLobbyWorld)worldBase;
			final int n = w.getNActivePlayers();

			if (n<20)
			{
				joinLobby();
			}
			else
			{
				stc.alertBox("serverFull", "server is full");
			}
		}
		catch (ClassCastException e)
		{
			error(""+worldBase.getName()+" is not a LoginWorld game: "+e);
		}
	}
	

	
	
	public boolean handleConsoleInput(String consoleCommandLine, WordWriter ww) throws IOException
	{
		boolean cmdFound=false;
		
		// Player typed something on console
		worldBase.lockWrite();
		try
		{
			WordReader wr=new WordReader(consoleCommandLine);
			String cmd = wr.readWord();
			
			
			if (cmd.equals(""))
			{
				writeLine("");
				return true;
			}
			
			if (super.interpretCommand(cmd, wr, ww))
			{
				return true;
			}

			final char ch=cmd.charAt(0);

			switch(ch)
			{			
				case 'h':
					if (cmd.equals("help"))
					{		
						writeLine("Commands");											
						writeLine("say, list, help");											
						cmdFound=true;					
					}
					break;
				case 'l':
					if (cmd.equals("list"))
					{			
						LoginLobbyWorld w = (LoginLobbyWorld)worldBase;
						
						for(D1Base p: w.getListOfActivePlayers().listOfStoredObjects)
						{
							//writeLine(String.format("%2s %3d %4s  %s  %s",a1+a2, es2.getIndex(), "~"+es2.getId(), es2.getName(), es2.stateMotto));
							writeLine(p.getName());											
						}
						cmdFound=true;
					}
					break;
				default:
					break;
			}
	
		}
		finally
		{
			worldBase.unlockWrite();
		}
		
		if (cmdFound==false)
		{
			stc.writeLine("consoleMessage \"unknown command '"+consoleCommandLine+"'\"");
			cmdFound=true;
		}
		
		return cmdFound;
	}		
	
	
	static public String getHostFromUrl(String tmp)
	{
		String h="";
		int i=1;
		while(i<tmp.length())
		{
			final char ch=tmp.charAt(i++);
			if (ch==':')
			{
				break;
			}							
			h+=ch;							
		}
		return h;
	}
	
	
	// This handles the commands sent from a client.
	// Returns true if command was found.
	// Returns false if command not found 	
	boolean parseCommand(String cmd, WordReader wr, WordWriter ww) throws IOException
	{
		final char ch=cmd.charAt(0);
		
		switch(ch)
		{
			case 'b':
			{
				if (cmd.equals("backOrCancel"))
				{
					//stc.close();
					return true;
				}
				break;
			}
			case 'c':
			{
				if (cmd.equals("consoleInput"))
				{
					String consoleCommandLine=wr.readString();
					debug("consoleCommandLine '"+consoleCommandLine+"'");
	
					handleConsoleInput(consoleCommandLine, ww);
					return true;
				}
				else if (cmd.equals("cancel"))
				{
					debug("reply from client '"+cmd+"'");
					stc.close(); // TODO: cancel is not same as disconnect order, we should keep connection and instead return to menu.
					//throw new InterruptedException("cancel from client");							
					return true;
				}
				break;									
			}
			case 'i':
				if (cmd.equals("isClientCodeOk")) // || (cmd.equals("isClientOk"))) 
				{
					final int playerIndex = wr.readInt();
					final int playerCode = wr.readInt();
					final int questionIndex = wr.readInt(); // Client may have more than one question out, this reference number shall be sent in the reply so that client will know which question it is the answer to. 
					
					debug("Question from sub server, playerIndex="+playerIndex+", playerCode="+playerCode+", questionIndex="+questionIndex);
					
					LoginLobbyWorld w=(LoginLobbyWorld)worldBase;

					GameUser playerData=w.isClientCodeOk(playerIndex, playerCode);
					
					// 0 = no
					// 1 = yes
					//String a = (isOk==false) ? "0" : "1";
					
					// If the message format is changed here remember to also change in LoginLobbyConnection where the message is received.
				
					if ((playerData!=null) && (playerData.player!=null))
					{
						//final String a=playerData.toString();
						final String b=playerData.player.toString();
						debug("code ok, name="+playerData.getName());
						ww.writeWord("isClientCodeOkReply");  
						ww.writeInt(questionIndex);
						ww.writeString("ok"+" "+b);
					}
					else
					{
						debug("code not ok, "+playerCode);
						ww.writeWord("isClientCodeOkReply");
						ww.writeInt(questionIndex);
						ww.writeString("nok");
					}
					return true;
				}							
				break;									
			case 'm':
				if (cmd.equals("mirrorAck")) // TODO: This shall be in class MirrorServer
				{
					serverSequenceNumberReceived=wr.readInt();
					return true;
				}
				break;
				
				
			case 't':						
				if (cmd.equals("textMsg"))
				{
					debug("reply from client '"+cmd+"'");
					final String txtMsg=wr.readString();
					String line=WordReader.removeQuotes(txtMsg);
					WordReader wr2=new WordReader(line);
					String cmd2=wr2.readWord();
					if (interpretCommand(cmd2, wr2, ww)==false)
					{
						unknownCommand(cmd2, ww);
					}
					return true;
				}
				break;
				
			default:
				break;
		}				
		return false;
	}
	

	protected void joinLobby()
	{
		LoginLobbyWorld w = (LoginLobbyWorld)worldBase;
		

		team=chooseType();

		
		defaultObj=worldBase;			
		
		try 
		{
			//beginMirroring();


			// Find the avatar for this player			

			gameUser = w.playerConnectedFindOrCreateGameUser(userAccount, team); // Remember to call playerDisconnected when leaving

			if (gameUser==null)
			{
				debug("already connected");
				stc.alertBox("alreadyPlaying", "User '"+userAccount.getName()+"' is already connected");
				return;
			}
			
			
			playerAvatar = (D1ThreadSafe) w.getDbIdObj(gameUser.avatarId);
			
			if (playerAvatar==null)
			{
				debug("already playing");
				stc.alertBox("alreadyPlaying", "Avatar '"+gameUser.getName()+"' is already connected");
				return;
			}
			
			LoginLobbyAvatar avatar =(LoginLobbyAvatar)playerAvatar;


			
			// Tell all others that a new player (or a server) connected.
			//w.lockWrite();
			try
			{
				switch(team)
				{
					case LoginLobbyWorld.PlayerType:
					{
						
						// Before server sends a reconnectServer it shall send this message to give login codes for the new server.
						// Give client a temporary pin code, to be used to authenticate with sub server later.
						stc.writeLine("setClientIdxAndCode "+gameUser.getIndex()+" "+gameUser.tmpPinCode);

						int n=16;
						while(--n>=0)
						{
							
							
							LoginLobbyAvatar selectedServer=chooseServer();
							
							
							//myEntry = (RsbString)w.activePlayers.findOrCreateChildObject(player.getName(), "RsbString");
	
							if (selectedServer!=null)
							{
								// Remember to send setClientIdxAndCode before sending this command.
								debug("Sending client to '"+selectedServer.getIp()+"'");
								stc.writeLine("reconnectServer \""+selectedServer.getIp()+"\"");
								stc.close();
							}
							
							
						}
						break;
					}
					case LoginLobbyWorld.ServerType:
					{
						
						//serverIp=stc.promptString("serverIp", "server ip");
						serverPort=stc.promptInt("serverPort", "server port",-1 , 0xFFFF);

						if (serverIp==null)
						{

							String tmp=stc.getTcpInfo();
							serverIp = getHostFromUrl(tmp);
							avatar.setIp("/"+serverIp+":"+serverPort);
						}
						else
						{
							String tmp="/"+serverIp+":"+serverPort;
							avatar.setIp(tmp);							
						}

						stc.alertBox("lobbyJoinedSuccessfully", worldBase.getName());
						
						break;
					}					
					default:
					{
						error("unknown playerOrServer");
						break;
					}
				}
			}
			finally
			{
				//w.unlockWrite();
			}
			
			
			
			
			
			//stc.writeLine("openLoginWorld "+w.getNameOrIndex());




			//findAndSendDbUpdatesToClientAll();

			
			//stc.writeLine("avatarId " + playerAvatar.getId()); 
			
			
			

  			int timeoutCounter=0; // If we get many timeouts in a row we will disconnect
			
			
			// loop waiting for input from client
	  		while(stc.isOpen())
	  		{

	  			try {
	  				final String r = stc.readLine(timeOutTime_ms);

	  				// This was not timeout so reset that counter
	  				timeoutCounter=0;
				
					WordReader wr=new WordReader(r);
					WordWriter ww=new WordWriter();
					
					final String cmd=wr.readWord();					
	
					if (cmd.length()>0)
					{
						// Log first messages to help debugging, can be removed later.
						if ((debugCount<100) && (!cmd.equals("mirrorAck")) && (!cmd.equals("avatarPos")))
						{
							debug("cmd("+debugCount+") "+cmd);
							debugCount++;
						}

						if (parseCommand(cmd, wr, ww)==false)
						{
							// Command was not found by parseCommand, perhaps it is backOrCancel?
							if (cmd.equals("backOrCancel"))
							{
								// Yes
								//stc.close();
								return;
							}
							else
							{
								// No, command was not found.
								unknownCommand(cmd, ww);
							}
						}

						// When the command line has been handled this will send the response message back to client.
						String m=ww.getString();
						if ((m!=null) && (m.length()>0))
						{
							debug("Sending to client: '"+m+"'");
							stc.writeLine(m);
						}
						
						
					}
					else
					{
						// TODO: why did we get an empty string?
					}

					String m=ww.getString();
					stc.writeLine(m);
					
	  			} catch (InterruptedException e) {
	  				// This was just a timeout. 
	  				
	  				if (timeoutCounter==timeOutTimeMaxCount)
	  				{
	  	  				// Send mirror updated to client, the client shall answer. This way we know if client is still alive.
	   					sendMirrorUpdated();
 	  				}
	  				
	  				// If we have some more timeout in a row we disconnect the client.
	  				if (timeoutCounter>(timeOutTimeMaxCount+10))
	  				{
	  					stc.close();
	  					//throw new InterruptedException();
	  				}
	  				else
	  				{
		  				timeoutCounter++;	  					
	  				}
	  			}
	
				// Find changes in database.
				//findAndSendDbUpdatesToClient();

  			}
				
		} catch (IOException e) {
			// Probably just a disconnect
			//e.printStackTrace();
			debug("run: IOException "+e);
		} 
		finally
		{
			
			// Super class will close the connection and unregister notifications.
			close();
			
			
			try 
			{
				w.lockWrite();
				
				if (gameUser!=null)
				{
					debug("gameUser left, name='"+gameUser.getName()+"'");
					w.playerDisconnected(gameUser);
					gameUser=null;
				}
			}
			finally
			{
				w.unlockWrite();
			}

		}
		
	}
	
	@Override
	public WorldBase createWorld()
	{
		return new LoginLobbyWorld();
	}
	
	
	protected int chooseType()
	{
		// buttonNames below must match PlayerType, ServerType and UnknownType

		Object[] buttonNames = {"server",
                "player",
                "Cancel"};

		final int tr = stc.promptButtons("selectType", "Are you a player or a server?", buttonNames);
		
		return tr;
	}

	
	protected LoginLobbyAvatar chooseServer()
	{
		LoginLobbyWorld w = (LoginLobbyWorld)worldBase;

		// buttonNames below must match PlayerType, ServerType and UnknownType

		D1Base[] list = w.getExternalServers().getListOfSubObjectsThreadSafe();
		
		int n = list.length;
		
		if (n==0)
		{
			stc.alertBox("noExternalServers", "no external servers found");
		}
		else
		{
			Object[] buttonNames = new Object[n];
			for(int i=0;i<n;i++)
			{
				buttonNames[i]=list[i].getName();
			}
			
			final int tr = stc.promptButtons("selectExternalServer", "Choose a server", buttonNames);
			
			//return (String)buttonNames[tr];
			if (tr>=0)
			{
				return (LoginLobbyAvatar)list[tr];
			}
		}
		return null;
	}
	
	/*
	@Override
	public boolean need2dSupport()
	{
		/ *if (this.config.loginServerHostname!=null)
		{
			// Returning in this case for both 2d and 3d support will make this option unavailable on external servers
			return true;
		}* /
		return false;
	}

	@Override
	public boolean need3dSupport()
	{
		/ *if (this.config.loginServerHostname!=null)
		{
			// Returning in this case for both 2d and 3d support will make this option unavailable on external servers
			return true;
		}* /
		// no the above does not work, config is null at this point

		return false;
	}
	*/

	@Override
	public boolean onlyOnMainServer()
	{
		return true;
	}
	
	public static void createAndStoreLobby(D1SubRoot dbRoot)
	{
		final String typeName="lobby";
		final String name="mainLobby";
		D1SubRoot r = (D1SubRoot) dbRoot.findOrCreateChildObject(WorldBase.nameOfWorldsDb, WorldBase.typeOfWorldsDb);
		D1SubRoot g = (D1SubRoot)r.findOrCreateChildObject(typeName, D1SubRoot.className());
		g.lockWrite();
		try
		{
			if (g.findGameObjNotRecursive(name)!=null)
			{
				System.out.println("Lobby already exist, name='"+name+"'");
			}	    
			else
			{
				System.out.println("Creating the Lobby, name='"+name+"'");
				//ServerBase s=PlayerConnectionThread.serverFactory(typeName);
				//WorldBase worldBase = s.createWorld();
				WorldBase worldBase = new LoginLobbyWorld();
				worldBase.linkSelf(g);
				worldBase.regName(name);
				worldBase.generateSelf();
				worldBase.setCreatedBy("_main");
			}
		}
		finally
		{
			g.unlockWrite();
		}
	}

}