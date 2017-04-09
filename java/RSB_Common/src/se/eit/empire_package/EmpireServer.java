/*
EmpireServer.java

Copyright (c) 2015 Henrik Bj��rkman (www.eit.se/hb)

*/

package se.eit.empire_package;



import java.io.IOException;

import se.eit.rsb_package.*;
import se.eit.rsb_server_pkg.OpServer;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;



public class EmpireServer extends OpServer {

	
	//DbSubRoot defaultMibEntry=null;
	
	EmpireTerrain et;
	EmpireUnitTypeList eul;
	//EmpireState playerAvatar;
	
	int debugCount=0;
	
	
	//static final private boolean sendEmpWorld=true;

	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireServer.class.getSimpleName();	
	}	
	
    public void debug(String str)
	{
    	WordWriter.safeDebug(className()+"("+stc.getTcpInfo()+"): "+str);
	}

    public static void error(String str)
	{
    	WordWriter.safeError(className()+": "+str);
	}

	
	public EmpireServer()
	{
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
	
    /*
    public String createAndStore(String worldName, GlobalConfig config, PlayerData player, ServerTcpConnection cc)
    {
    	
    }
    */
    
	// We override this since for empire we don't want any custom script
	@Override
    public void configureGame()
    {
    	int nState = stc.promptInt("world_size", "How many players/states shall there be?", 2, 1024);
    	
   	
    	int minSize=(int)Math.round(Math.sqrt(nState)*6);
    	int maxSize=minSize*4; // nState+32;
    	
    	
    	int size = minSize;
    	
    	if (maxSize>minSize)
    	{    	
    		size = stc.promptInt("world_size", "How large shall the world be? "+minSize+"=min, "+maxSize+"=max", minSize, maxSize);
    	}
    	
    	
    	
    	// size must be dividable by 2.
    	size = ((size+1)/2) * 2;
    	
    	
    	EmpireWorld ew = (EmpireWorld)worldBase;
		
		ew.generateSelf(nState, size);   	
    }
	
    /*
	public static DbBase findRelativeFromIndex(String nameOrIndex, DbBase sid[])
	{
		int index=Integer.parseInt(nameOrIndex);
		for(int i=0;i<sid.length; i++)
		{
			if (sid[i].getIndex()==index)
			{
				return sid[i];
			}
		}
		return null;
	}

	public static DbBase findRelativeFromName(String nameOrIndex, DbBase sid[])
	{
		for(int i=0;i<sid.length; i++)
		{
			if (sid[i].getName().equals(nameOrIndex))
			{
				return sid[i];
			}
		}
		return null;
	}

	
	public static DbBase findRelativeFromNameOrIndex(String nameOrIndex, DbBase sid[])
	{		
		if (WordReader.isInt(nameOrIndex))
		{
			return findRelativeFromIndex(nameOrIndex, sid);
		}
		else
		{
			return findRelativeFromName(nameOrIndex, sid);		
		}
		
	}
    */
    
	public static void append(WordWriter ww, String msg)
	{
		ww.writeName("TextBoxAppend");
		ww.writeString(msg);
		ww.endLine();
	}
/*
	public static void prompt(ServerTcpConnection stc)
	{
		WordWriter ww = new WordWriter(stc.getTcpConnection());
		ww.writeName("empPing"); // is this still needed?
		ww.endLine();
	}
*/

	
	public void postMessage(EmpireState empireState, String str) throws IOException
	{
		worldBase.lockWrite();
		try
		{
			if (empireState!=null)
			{
				//playerAvatar.postMessage("console as message: "+str);
				empireState.postMessageToThis(str);				
			}
			else
			{
				final EmpireWorld ew=(EmpireWorld)worldBase;
				final EmpireStatesList enl=ew.getEmpireStatesList();
				for(D1Base db: enl.listOfStoredObjects)
				{
					EmpireState es = (EmpireState)db;
					es.postMessageToThis(str);
				}
			}
		}
		finally
		{
			worldBase.unlockWrite();
		}
	}

	
	
	
	


    // this interprets a message from a client
	// Returns true if the command was found
	@Override
	public boolean interpretCommand(String cmd, WordReader wr, WordWriter ww) throws IOException
	{
		if (super.interpretCommand(cmd, wr, ww))
		{
			return true;
		}
	
		final char ch=cmd.charAt(0);
		switch(ch)
		{
			case 'g':
				if (cmd.equals("go"))
				{
					if (isAdmin())
					{
						int d=EmpireUnitType.defaultMsPerTick;
						if ((wr.isOpenAndNotEnd()) && (wr.isNextIntOrFloat()))
						{
							d=wr.readInt(); // delay, lower value -> faster game
						}
		
						EmpireWorld ew=(EmpireWorld)this.worldBase;
						ew.setGameSpeed(d);
						postMessage(null,"game on");
						ww.writeLine("game is now on, to pause use 'pause' command.");
					}
					else
					{
						ww.writeLine("Permission denied");
					}						
					return true;
				}	
				break;
			case 'm':
			{
				if (cmd.equals("mute")) // There is also a mute functionality in OpServer/GameUser. Do we need both?
				{
					if (isAdmin())
					{
						String n=wr.readWord(); // Name Or Index
						try
						{
							EmpireWorld ew=(EmpireWorld)this.worldBase;
							ew.lockWrite();

								
								//EmpireStatesList enl=ew.getEmpireNationsList();
								EmpireState en=null;
								
								en=getEmpireStateFromName(worldBase, n);
								
								if (en!=null)
								{
									en.postMessageToThis("you have been muted by op/admin");
									en.mutePlayer();
									ww.writeLine("player "+en.getName()+": was muted");
								}
								else
								{
									ww.writeLine("player "+ n +" was not found");					
								}
						}
						finally
						{
							EmpireWorld ew=(EmpireWorld)this.worldBase;
							ew.unlockWrite();
						}
						
					}
					else
					{
						ww.writeLine("Permission denied");
					}
					return true;
				}
				break;
			}
			case 'p':
			{
				if (cmd.equals("pause"))
				{
					if (wr.isOpenAndNotEnd())
					{
						helpCommand(cmd, wr, ww, null);
					}
					else
					{
						if (isAdmin())
						{
							EmpireWorld ew=(EmpireWorld)this.worldBase;
							ew.setGameSpeed(0);
							postMessage(null, "game paused");
							ww.writeLine("Game is paused, to start again use command 'go'");
						}
						else
						{
							ww.writeLine("Permission denied");
						}
					}
					return true;
				}	
				break;
			}
			case 'u':
				if (cmd.equals("unMute"))
				{
					if (isAdmin())
					{
						String n=wr.readWord(); // Name Or Index
						try
						{
							worldBase.lockWrite();

								
								//EmpireStatesList enl=ew.getEmpireNationsList();
								EmpireState en=null;
								
								en=getEmpireStateFromName(worldBase, n);
								
								if (en!=null)
								{
									if (en.isMutedPlayer())
									{
										en.postMessageToThis("you are no longer muted by op/admin");
										en.unMutePlayer();
										ww.writeLine("player "+en.getName()+": is no longer muted");
									}
									else
									{
										ww.writeLine("player "+ n +" is not muted");										
									}
								}
								else
								{
									ww.writeLine("player "+ n +" was not found");					
								}
						}
						finally
						{
							worldBase.unlockWrite();
						}
						
					}
					else
					{
						ww.writeLine("Permission denied");
					}
					return true;
				}
				break;
			default:
				break;
		}
		return false;
	}
	

	private EmpireState getEmpireStateFromName(WorldBase worldBase, String n) 
	{
		EmpireWorld ew=(EmpireWorld)this.worldBase;
		D1Base es = ew.empireStatesList.findObjectByNameIndexOrId(n);
		if (es instanceof EmpireState)
			return (EmpireState)es;
		
		return null;
	}
	

	public boolean mainHelp(String cmd, WordReader wr, WordWriter ww, String hist) throws IOException
	{
		
		if ((cmd==null) || (cmd.length()==0))
		{
			ww.writeLine("Main commands:");				
			//ww.writeLine("  ac : avatar command");				
			ww.writeLine("  ac : avatar command");				
			ww.writeLine("  dc : default object command");				
			ww.writeLine("  pc : player command");				
			ww.writeLine("  sc : server command");				
			ww.writeLine("");				
			ww.writeLine("For more help about one main command try:");
			ww.writeLine("  "+hist+" <main command>");
			ww.writeLine("Example:");
			ww.writeLine("  "+hist+" ac");
			ww.writeLine("");				
		}
		/*else if (subCmd.equals("ac"))
		{
			ww.indentString="  ";
			ww.incIndentation();
			avatar.interpretCommand("help", wr, ww);
			ww.decIndentation();
		}*/
		else if (cmd.equals("ac"))
		{
			String subCmd = wr.readWord();
			//ww.indentString="  ";
			((EmpireState)playerAvatar).helpCommand(subCmd, wr, ww, hist+" "+cmd);
		}
		else if (cmd.equals("dc"))
		{
			String subCmd = wr.readWord();
			//ww.indentString="  ";
			defaultObj.helpCommand(subCmd, wr, ww, hist+" "+cmd);
		}
		else if (cmd.equals("pc"))
		{
			String subCmd = wr.readWord();
			//ww.indentString="  ";
			userAccount.helpCommand(subCmd, wr, ww, hist+" "+cmd);
		}
		else if (cmd.equals("sc"))
		{
			String subCmd = wr.readWord();
			//ww.indentString="  ";
			helpCommand(subCmd, wr, ww, hist+" "+cmd);
		}
		else if (helpCommand(cmd, wr, ww, hist))
		{
			// ok
		}
		else
		{
			ww.writeLine("Unknown or no help for command '"+cmd+"'");
			ww.writeLine("try:");							
			ww.writeLine("  help");							
		}
		return true;
	}
	
	protected void unknownCommand(String cmd)
	{
		debug("unknown command from client '"+userAccount.getName()+"' "+cmd);
		try {
			writeLine("unknown command " + cmd);
		} catch (IOException e) {
			stc.error("unknown command " + cmd);
		}

	}

	
	
	public boolean cmdFromPlayer(String consoleCommandLine) throws IOException
	{
		WordWriter ww=new WordWriter();
		ww.indentString="  ";

		ww.writeLine("echo '"+consoleCommandLine+"'");	
		boolean r = cmdFromPlayer(ww, consoleCommandLine);
		if (r==false)
		{
			ww.writeString("unknown command from player");
		}
		String m = ww.getString();
		writeLine(m);	
		return r;
	}

	// This is used to interpret console input from player/user.
	public boolean cmdFromPlayer(WordWriter ww, String consoleCommandLine) throws IOException
	{
		debug("cmdFromPlayer "+consoleCommandLine);

		WordReader wr=new WordReader(consoleCommandLine);
		String cmd = wr.readWord();
		return cmdFromPlayer(cmd, wr, ww);
	}
	
	// This is used to interpret console input from player/user.
	public boolean cmdFromPlayer(String cmd, WordReader wr, WordWriter ww) throws IOException
	{
		boolean cmdFound=false;
		
		ww.incIndentation();

		if (cmd.length()==0)
		{
			ww.writeLine("empty command line");
			return true;
		}
			
		// Player typed something on console
		
		worldBase.lockWrite();
		try
		{
			final char ch=cmd.charAt(0);
			switch(ch)
			{
				case 'a':
				{
					if (cmd.equals("ac"))
					{
						String subCmd = wr.readWord();						
						if (!((EmpireState)playerAvatar).interpretCommand(subCmd, wr, ww))
						{
							if (!subCmd.equals("help"))
							{
								ww.writeLine("Sub command '"+subCmd+"' not found, try:");
								ww.writeLine("  help ac");
							}
							else
							{
								ww.writeLine("For more help try:");
								ww.writeLine("  help ac <sub command>");
							}
						}
						cmdFound = true;
					}
					break;
				}
				case 'h':
				{
					if (cmd.equals("help"))
					{
						String subCmd = wr.readWord();
						mainHelp(subCmd, wr, ww, "help");
						cmdFound = true;
					}
					break;
				}
				case 'd':
				{
					if (cmd.equals("dc"))
					{
						String subCmd = wr.readWord();						
						if (!defaultObj.interpretCommand(subCmd, wr, ww))
						{
							if (!subCmd.equals("help"))
							{
								ww.writeLine("Sub command '"+subCmd+"' not found, try:");
								ww.writeLine("  help dc");
							}
							else
							{
								ww.writeLine("For more help try:");
								ww.writeLine("  help dc <sub command>");
							}
						}
						cmdFound = true;
					}
					break;
				}
				case 'p':
				{
					if (cmd.equals("pc"))
					{
						String subCmd = wr.readWord();						
						if (!userAccount.interpretCommand(subCmd, wr, ww))
						{
							if (!subCmd.equals("help"))
							{
								ww.writeLine("Sub command '"+subCmd+"' not found, try:");
								ww.writeLine("  help pc");
							}
							else
							{
								ww.writeLine("For more help try:");
								ww.writeLine("  help pc <sub command>");
							}
						}
						cmdFound = true;
					}
					break;
				}
				case 's':
				{
					if (cmd.equals("sc"))
					{
						String subCmd = wr.readWord();						
						if (!interpretCommand(subCmd, wr, ww))
						{
							if (!subCmd.equals("help"))
							{
								ww.writeLine("Sub command '"+subCmd+"' not found, try:");
								ww.writeLine("  sc help");
							}
							else
							{
								ww.writeLine("For more help try:");
								ww.writeLine("  sc help <sub command>");
							}
						}
						cmdFound = true;
					}
					break;
				}
			
				default:
					break;
			}
			
			
			// For backwards compatibility we try to find the sub command if it was not one of the main commands.
			if (!cmdFound)
			{
				// It can be a command to be handled by super class, avatar object or player object.
				// Here we try each possible interpreter until one is found that handles this command.
				if (super.interpretCommand(cmd, wr, ww))
				{
					cmdFound = true;										
				}
				else if (defaultObj.interpretCommand(cmd, wr, ww))
				{
					cmdFound = true;					
				}
				else if (((EmpireState)playerAvatar).interpretCommand(cmd, wr, ww))
				{
					cmdFound = true;
				}
				else if (userAccount.interpretCommand(cmd, wr, ww))
				{
					cmdFound = true;				
				}				
				else if (interpretCommand(cmd, wr, ww))
				{
					cmdFound = true;				
				}				
				else
				{
					cmdFound = false;
				}
				
			}
			
		}
		finally
		{
			worldBase.unlockWrite();
		}
		
		
		if (!cmdFound)
		{
			ww.writeLine("Command '"+cmd+"' not found, try help.");
			cmdFound = true;
		}	


		ww.decIndentation();

		ww.writeLine("ok");
		
		return cmdFound;
	}

	
	// This handles the commands sent from a client.
	// Returns true if cmd was found.
	public boolean parseCommand(String cmd, WordReader wr, WordWriter ww) throws IOException
	{
		// Get first character in the command (for better performance)
		final char ch=cmd.charAt(0);
		
		switch(ch)
		{
		case 'm':
			if (cmd.equals("mirrorAck")) // TODO: This shall be in class MirrorServer, clients must send a mirrorAck after receiving mirrorUpdated.
			{
				serverSequenceNumberReceived=wr.readInt();
				return true;
			}
			else if (cmd.equals("mouseDown"))
			{
				debug("mouseDown from client '"+userAccount.getName()+"' "+cmd);
				int x=wr.readInt();
				int y=wr.readInt();					
				debug("mouseDown: "+x+" "+y);
				ww.writeWord("upperTextAreaAppend");
				ww.writeString("mouse down at "+x+" "+y);
				return true;
			}
			else if (cmd.equals("mouseUp"))
			{					
				debug("mouseUp from client '"+userAccount.getName()+"' "+cmd);
				int x=wr.readInt();
				int y=wr.readInt();					
				debug("mouseUp: "+x+" "+y);										
				ww.writeWord("upperTextAreaAppend");
				EmpireUnit eu=et.findUnitAt(x,y);
				if (eu!=null)
				{
					ww.writeString(eu.toString());
				}
				else
				{
					ww.writeString("no unit at "+x+" "+y);
				}
				return true;
			}
			break;
		case 'c':
		{
			if (cmd.equals("consoleInput"))
			{
				// We get here when user typed something in the console.
				String consoleCommandLine=wr.readString();
				debug("consoleCommandLine '"+consoleCommandLine+"'");
					
				cmdFromPlayer(consoleCommandLine);
				return true;
			}
			if (cmd.equals("cancelOrder"))
			{					
				((EmpireState)playerAvatar).cancelOrder(wr, ww);
				return true;
			}
			else if (cmd.equals("cancel"))
			{
				// See also "backOrCancel"
				debug("cancel from client '"+userAccount.getName()+"'");
				stc.close();
				return true;
			}
			break;
		}
		case 'g':
		{
			if (cmd.equals("groupUnits"))
			{					
				((EmpireState)playerAvatar).groupUnits(wr, ww);
				return true;
			}
			break;						
		}
		case 'u':
			if (cmd.equals("unitOrder"))
			{					
				((EmpireState)playerAvatar).unitOrder(wr, ww);
				return true;
			}
			break;						
		default:
			break;
		}
		return false;
	}
	
	protected void joinEmpire(EmpireWorld ew)
	{
		try 
		{
			worldBase=ew;
			defaultObj=ew;


			// Find the nation for this player			
					
			// Does this player have a state?
			final EmpireStatesList enl;
			worldBase.lockRead();
			try
			{
				enl=ew.getEmpireStatesList();
				if (enl==null)
				{
					error("no states list");
				}
				playerAvatar=(EmpireState) enl.getNationByOwner(userAccount.getName()); // will give not null if this player owns a state in this game.
			}
			finally
			{
				worldBase.unlockRead();
			}

			
			// If not perhaps player is co ruling a state?
			if (playerAvatar==null)
			{		
				playerAvatar=(EmpireState) enl.getNationByCoRuler(userAccount.getName()); // will give not null if this player is a co ruler a state in this game.
			}
			
			// If not perhaps player can get a state?
			if (playerAvatar==null)
			{		
				// Is a password required for this game?
				final String pw=ew.getGamePassword();
				if (pw.length()>0)
				{
					final String ep=stc.promptString("game_password", "This game requires password");
					if (!pw.equals(ep))
					{
						stc.alertBox("wrong_game_pw", "That was not the correct pw for this game");
						return;
					}			
				}
				
				// Is player banned?
				if (ew.isPlayerBanned(userAccount.getName()))
				{
					stc.alertBox("you_are_banned", "You have been banned from this game.");
					return;					
				}

				if (ew.isPlayerBanned(userAccount.getRegisteredFromAdr()))
				{
					stc.alertBox("ip_is_banned", "Your IP has been banned from this game.");
					return;					
				}
				
				playerAvatar=(EmpireState) enl.takeNation(userAccount.getName());
			}
			
			// 
			if (playerAvatar!=null)
			{
				stc.alertBox("in_charge", "you are in charge of state/nation "+playerAvatar.getName());
			}
			else
			{
				stc.alertBox("in_charge", "all states/nations are taken, you can only spectate");			
			}
		
			beginMirroring();
			
			
			// Tell client SW to draw the empire main window
			stc.writeLine("openGame empire "+ew.getNameOrIndex());

			//counter=ew.isChanged(previousUpdateCounter);

			if (playerAvatar!=null)
			{
				stc.writeLine("joinEmpire "+playerAvatar.getIndex());
			}


			// Before the entire database was sent here using dumpWorld instead of calling findAndSendDbUpdatesToClientAll. Both way work but in long run findAndSendDbUpdatesToClientAll will be able to send only objects the player shall see.
			//dumpWorld(worldBase, stc);

			findAndSendDbUpdatesToClientAll();

			
			stc.writeLine("showWorld");
			stc.writeLine("showWorld");

			
    		// If user log in while game is paused.
			if (ew.gameSpeed==0)
			{
	    		if (isAdmin())
	    		{
    				postMessage(((EmpireState)playerAvatar), "game is paused, you are op/admin, to start the game issue command 'go' (in terminal)");
    			}
	    		else
	    		{
    				postMessage(((EmpireState)playerAvatar), "game is paused (waiting for op/admin to start the game)");
	    			
	    		}
    		}

  			int timeoutCounter=0; // If we get many timeouts in a row we will disconnect
			
			
			// loop waiting for input from client
	  		while(stc.isOpen())
	  		{

	  			try {
	  				final String r = stc.readLine(100);

	  				// This was not timeout so reset that counter
	  				timeoutCounter=0;
				
	  				// Create a word reader to parse the input line
					WordReader wr=new WordReader(r);
					

					// Get the first word in the command line.
					final String cmd=wr.readWord();					

					
					
					// Was there something?
					if (cmd.length()>0)
					{
						// yes

						// Create a word writer for writing the response to the command.
						WordWriter ww = new WordWriter();
						
						// Log first messages to help debugging, can be removed later.
						if ((debugCount<100) && (!cmd.equals("mirrorAck")) && (!cmd.equals("avatarPos")))
						{
							debug("cmd("+debugCount+") "+r);
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
						if (m.length()>0)
						{
							writeLine(m);
						}
					}
					else
					{
						// The input line was empty, this was not expected (timeouts are InterruptedException) but will ignore it for now. Perhaps client sends empty strings?
						//debug("why did we get an empty string from client?");
					}

					
					
					
					
					// If we want to send line by line then we could use this
					/*
					String str=ww.getString();
					WordReader wr3 = new WordReader(str);
					while(wr3.isOpenAndNotEnd())
					{
						String line3=wr3.readLine();
						writeLine(line3);
					}
					*/
					

	  			} catch (InterruptedException e) {
	  				// This was just a timeout. But if we have many in a row we disconnect the client.
	  				if (timeoutCounter>15*60*10)
	  				{
	  					// To many timeouts.
	  					throw new InterruptedException();
	  				}
	  				else
	  				{
		  				timeoutCounter++;	  					
	  				}
	  			}
	
				// Find changes in database. This will send only changed objects to client.
	  			findAndSendDbUpdatesToClient();

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
		finally
		{
			debug("client disconnecting");
			close();
		}
		
		
		
	}
			
    
	@Override
	public void joinWorld()
	{
		joinEmpire((EmpireWorld)worldBase);			
	}

	@Override
	public WorldBase createWorld() {
		return new EmpireWorld();
	}	
    
	/*
	@Override
	public boolean need2dSupport()
	{
		return true;
	}

	@Override
	public boolean need3dSupport()
	{
		return false;
	}*/

}
