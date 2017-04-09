/*
FlameServer.java

Copyright (c) 2015 Henrik Björkman (www.eit.se/hb)

February 28 2016: Renamed to Flame - Alexander Björkman

*/

package se.eit.flameengine_package;

import java.io.IOException;
import java.util.Random;

import se.eit.rsb_package.*;
import se.eit.rsb_server_pkg.OpServer;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;





public class FlameServer extends OpServer 
{

	int currentbuilding = 0;
	FlameAvatar avatar=null;
	FlameWorld w=null;
	
	int debugCount=0;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return FlameServer.class.getSimpleName();	
	}	
	
    public void debug(String str)
	{
    	WordWriter.safeDebug(className()+"("+stc.getTcpInfo()+"): "+str);
	}

    public static void error(String str)
	{
    	WordWriter.safeError(className()+": "+str);
	}

    
    
	public FlameServer() {
		super();
	}


	FlameRoom getCurrentRoom()
	{
		if (playerAvatar != null)
		{
			D1Base p = playerAvatar.getParent();
			if (p instanceof FlameRoom)
		    return (FlameRoom)(p);
		}
		return null;
	}

	@Override
	public void joinWorld()
	{
		w=(FlameWorld)worldBase;
		joinHmeg();
	}
	


	// This will as player which team he or she wants to join.
	public int chooseTeam()
	{
	    Random random = new Random();

		Object[] buttonNames = {"random",
                "blue (1)",
                "red  (2)",
                "Cancel"};

		final int tr = stc.promptButtons("selectTeam", "Which team do you wish to join", buttonNames);
		
		switch(tr)
		{
			case 0: break;
			case 1: return 1;
			case 2: return 2;
			default: break;
		}
		return random.nextInt(2)+1;
	}

	public boolean cmdFromPlayer(String consoleCommandLine) throws IOException
	{
		WordWriter ww=new WordWriter();
		ww.writeLine("echo '"+consoleCommandLine+"'");	
		boolean r = cmdFromPlayer(ww, consoleCommandLine);
		if (r==false)
		{
			ww.writeString("unknown command");
		}
		String m = ww.getString();
		writeLine(m);	
		return r;
	}

	@Override
	protected boolean helpCommand(String cmd, WordReader wr, WordWriter ww, String hist) throws IOException
	{
	
		if ((cmd==null) || (cmd.length()==0))
		{
			ww.writeLine("Main commands:");				
			ww.writeLine("  ac : avatar command");				
			ww.writeLine("  pc : player command");				
			ww.writeLine("  sc : server command");				
			ww.writeLine("");				
			if (hist!=null)
			{
				ww.writeLine("For more help about one main command try:");
				ww.writeLine("  help <command>");
				ww.writeLine("Example:");
				ww.writeLine("  help sc");
				ww.writeLine("");
			}
			return true;
		}
		else if (cmd.equals("ac"))
		{
			String subCmd = wr.readWord();						
			avatar.helpCommand(subCmd, wr, ww, hist+" "+cmd);
			return true;
		}
		else if (cmd.equals("pc"))
		{
			String subCmd = wr.readWord();						
			userAccount.helpCommand(subCmd, wr, ww, hist+" "+cmd);
			return true;
		}
		else if (cmd.equals("sc"))
		{
			String subCmd = wr.readWord();						
			super.helpCommand(subCmd, wr, ww, hist+" "+cmd);
			return true;
		}
		else if (avatar.helpCommand(cmd, wr, ww, hist))
		{
			// ok
		}
		else if (super.helpCommand(cmd, wr, ww, hist))
		{
			// ok
		}
		else
		{
			ww.writeLine("Unknown or no help for command '"+cmd+"'");
			ww.writeLine("try:");							
			ww.writeLine("  help");							
			return false;
		}

		return true;		
	}
	
	// This is used to interpret console input from player/user.
	public boolean cmdFromPlayer(WordWriter ww, String consoleCommandLine) throws IOException
	{
		boolean cmdFound=false;
		WordReader wr=new WordReader(consoleCommandLine);
		String cmd = wr.readWord();
	
		ww.indentString="  ";
		ww.incIndentation();
		
		debug("cmdFromPlayer "+consoleCommandLine);
		

		if (cmd.length()==0)
		{
			ww.writeLine("empty command line");
			return true;
		}
			
		// Player typed something on console
		
		w.lockWrite();
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
						if (!avatar.interpretCommand(subCmd, wr, ww))
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
						
						helpCommand(subCmd, wr, ww, cmd);
						
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
								ww.writeLine("  help "+cmd+" <sub command>");
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
						if (!super.interpretCommand(subCmd, wr, ww))
						{
							if (!subCmd.equals("help"))
							{
								ww.writeLine("Sub command '"+subCmd+"' not found, try:");
								ww.writeLine("  help sc");
							}
							else
							{
								ww.writeLine("For more help try:");
								ww.writeLine("  help "+cmd+" <sub command>");
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
				else if (avatar.interpretCommand(cmd, wr, ww))
				{
					cmdFound = true;
				}
				/*else if (ci.interpretCommand(avatar, cmd, wr, ww))
				{
					cmdFound = true;				
				}*/				
				else if (userAccount.interpretCommand(cmd, wr, ww))
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
			w.unlockWrite();
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
		final char ch=cmd.charAt(0);

		
		switch(ch)
		{
		case 'a':
			if (cmd.equals("avatarPos"))
			{
				// Client will send this regularly to update its position.
				//int clientSquenceNumberReceived;
				try
				{
					w.lockWrite();
					
					avatar.readWantedPao(wr);									
					
					//clientSquenceNumberReceived = wr.readInt();
					
	 	        	avatar.setUpdateCounter();
				}
				finally
				{
					w.unlockWrite();
				}
				
				// Server must send avatarAck after it receives an avatarPos, or maybe a mirrorUpdated will do?
				//stc.writeLine("avatarAck "+clientSquenceNumberReceived); 

				return true;
			}
			break;
			
		case 'c':
			if (cmd.equals("consoleInput"))
			{
				// We get here when user typed something in the console.
				String consoleCommandLine=wr.readString();
				debug("consoleCommandLine '"+consoleCommandLine+"'");
					
				cmdFromPlayer(consoleCommandLine);
				return true;
			}
			if (cmd.equals("cancel"))
			{
				debug("reply from client '"+userAccount.getName()+"' "+cmd);
				stc.close(); // TODO: should keep connection and instead return to menu.
				//throw new InterruptedException("cancel from client");							
				return true;
			}
			break;

		case 'k':
	
			if (cmd.equals("keyup"))
			{
				try
				{
					w.lockWrite();
					String m=wr.readString();
	     	        WordReader mr=new WordReader(m);
	     	        int k=mr.readInt();
	     	        switch(k)
					{
					/*case 'A': case 'D':
						avatar.wantedRotationSpeed=0.0f;
						break;
	     	        case 'W': case 'S':
	     	        	((CityPvpAvatar)avatar).wantedForwardMotion = 0;
						break;       	*/
					default: 
						break;
					}	
				}
				finally
				{
					w.unlockWrite();
				}
				return true;
			}
			else if (cmd.equals("keypress"))
			{
				return true;
			}
			else if (cmd.equals("keydown"))
			{
				try
				{
					w.lockWrite();
					
					String m=wr.readString();
	     	        WordReader mr=new WordReader(m);
	     	        int k=mr.readInt();
	     			FlameRoom cr=getCurrentRoom();
	     			FlameEntity a = avatar;
	     			if (cr==null)
	     			{
	     				return true;
	     			}
	
	     			
	     			// if in control room then 
	     			if (cr.getParent() instanceof FlameRoom)
	     			{
	     				FlameRoom cpr=(FlameRoom)cr.getParent();
	     				if (cr.isControlPanel(avatar.position.x,avatar.position.y))
	     				{
	     					a=cr;
	     					cr=cpr;
	     				}
	     			}
	  
	     			// Make sure case is capital letter!
	     	        switch(k)
					{
		     	        case ' ':
		     	        	
		     	        	// Shoot... only if we are a tank!
		     	        	avatar.shoot();
		     	        	break;
						/*case 'A':
		     	        	//a.move(-1,0);
		     	        	//a.move(new HmegPao (-0.33f, 0, 0));
	
							avatar.wantedRotationSpeed=-6.0f;
							break;
		     	        case 'D':
		     	        	//a.move(new HmegPao (0.33f, 0, 0));
		     	        	avatar.wantedRotationSpeed=6.0f;
							break;
		     	        case 'W':
		     	        	//a.move(new HmegPao (0, -1.33f, 0));
		     	        	((CityPvpAvatar)a).wantedForwardMotion = 0.4f;
							break;
		     	        case 'S':
		     	        	//a.move(new HmegPao (0, 0.33f, 0));
		     	        	//a.velocity = new HmegPao (0, 0.33f, 0);
		     	        	((CityPvpAvatar)a).wantedForwardMotion = -0.2f;
							break;*/
		     	       case 'G':
		     	    	    // This is the cheat button, when user press it give more resources
		     	    	    debug("giveItem");
		     	    	    avatar.giveItem(0, 1);
		     	        	avatar.giveItem(1, 2);
		     	        	avatar.giveItem(2, 3);
		     	        	avatar.giveItem(3, 4);
		     	        	avatar.giveItem(7, 5);
		     	        	avatar.giveItem(8, 6);
		     	        	avatar.giveItem(9, 7);
		     	        	avatar.giveItem(10, 8);
		     	        	avatar.giveItem(11, 9);
		     	        	avatar.fill_mineral+=1;
		     	        	avatar.fill_stone+=1;
		     	        	avatar.fill_wood+=1;
		     	        	avatar.postMessageToThis("resources given");
		     	        	avatar.setUpdateCounter();
							break;	
		     	      case 'q':
		     	        	try
		     	        	{
		     	        		w.lockWrite();
		     	        		avatar.relinkSelf(a);
			     	        	a = avatar;
			     	        	avatar.move(new FlamePao(-1, 0, 0));
		     	        	}
		     	        	finally
		     	        	{
		     	        		w.unlockWrite();
		     	        	}		     	        	
							break;
		     	      default: 
		     	    	  break;
					}
	
				}
				finally
				{
					w.unlockWrite();
				}
				return true;
				
			}
			break;
		
		case 'i':
			if (cmd.equals("inventoryClick"))
			{
				final String s=wr.readString();
				final int i=Integer.parseInt(s);
				
		        	FlameEntity obj = (FlameEntity) avatar.getObjFromIndex(i);
		        	if (obj !=null)
		        	{
	 	        	currentbuilding = obj.entitySprite;
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
			else if (cmd.equals("mapClick"))
			{
				
		        	// Detta var ett klick på kartan
				final float worldX=wr.readFloat();
				final float worldY=wr.readFloat();
				
	 	        debug("mapClick:"+worldX+" "+worldY);
	
				try
				{
					w.lockWrite();
	 	        	if (currentbuilding == 0)
	 	        	{
	 	        		// Klienten vill ta bort något
	 	        		int t = getCurrentRoom().getTile(worldX, worldY);
	 	        		if (t != -1)
	 	        		{
	 	        			//getCurrentRoom().changeTile(worldX, worldY, 0);	
	 	        			avatar.fill_mineral +=  FlameTile.loot_mineral(t);
	 	        			avatar.fill_stone +=  FlameTile.loot_stone(t);
	 	        			avatar.fill_wood +=  FlameTile.loot_wood(t);
	 	        		}
	 	        	}
	     	        else
	 	        	{
	     	        	// Klienten vill lägga till något
	 	        		if (
	 	        				FlameTile.cost_mineral(currentbuilding) <= avatar.fill_mineral 			 &&
	 	        				FlameTile.cost_wood(currentbuilding) <= avatar.fill_wood 					 && 
	 	        				FlameTile.cost_stone(currentbuilding) <= avatar.fill_stone					 )
	 	        				  
	 	        		{
	 	        			//getCurrentRoom().changeTile(worldX, worldY, currentbuilding);
	 	        			avatar.fill_mineral -= FlameTile.cost_mineral(currentbuilding);
	 	        			avatar.fill_wood    -= FlameTile.cost_wood(currentbuilding);
	 	        			avatar.fill_stone   -= FlameTile.cost_stone(currentbuilding) ;
		     	        	avatar.setUpdateCounter();
	 	        		}
	 	        		else
	 	        		{
	 	        			debug("Client does not have enough resources");
		     	        	avatar.postMessageToThis("not enough resources");
	 	        		}
	 	        	}
		        		
				}
				finally
				{
					w.unlockWrite();
				}
		        	
		        	//getCurrentRoom().changeTile(worldX, worldY, currentbuilding);
				
				return true;
				
			}
			break;
		case 't':						
			if (cmd.equals("textMsg"))
			{
				debug("reply from client '"+userAccount.getName()+"' "+cmd);
				final String txtMsg=wr.readString();
				String line=WordReader.removeQuotes(txtMsg);
				WordReader wr2=new WordReader(line);
				WordWriter ww2=new WordWriter();
				String cmd2=wr2.readWord();
				if (interpretCommand(cmd2, wr2, ww2)==false)
				{
					unknownCommand(cmd2, ww2);
				}
				String m=ww2.getString();
				writeLine(m);
				return true;
			}
			break;
			
	
		default:
			break;
				
		}
		return false;
	}
	
	protected void joinHmeg()
	{
		//int notifyIdx=-1;
		FlameUser avatarPlayerReferences=null;
	
		int team = chooseTeam();
		
		worldBase=w;
		defaultObj=w;			
		
		try 
		{
			beginMirroring();
				
			
			// Find the avatar for this player			

			avatarPlayerReferences = w.playerConnected(userAccount, team); // Remember to call playerDisconnected when leaving
			
			if (avatarPlayerReferences==null)
			{
				stc.alertBox("alreadyPlaying", "PlayerData '"+userAccount.getName()+"' is already connected");
				return;
			}
			
			
			playerAvatar = (D1ThreadSafe)w.getDbIdObj(avatarPlayerReferences.avatarId); // TODO avatarPlayerReferences.getAvatar();
			// For debugging, an avatar must have been found or created.
			if (playerAvatar==null)
			{
				debug("no avatar found or could not be created");
				return;
			}
		
			stc.writeLine("openGame hmeg "+w.getNameOrIndex());

			
			avatar=(FlameAvatar)playerAvatar;
			avatar.stc = stc;
			Random rand = new Random();
			if (avatar.getParent() instanceof FlameRoom)
			{
				FlameRoom spawnRoom = (FlameRoom)avatar.getParent();
				int nex = rand.nextInt(spawnRoom.xSectors / 4);
				int ney = rand.nextInt(spawnRoom.ySectors / 4);
				
				while (spawnRoom.getTile(nex * 4 - 1.5f, ney * 4 - 1.5f ) != 0)
				{
					nex = rand.nextInt(spawnRoom.xSectors / 4);
					ney= rand.nextInt(spawnRoom.ySectors) / 4;
				}
				
				avatar.position.x = nex * 4 - 1.5f ;
				avatar.position.y = ney * 4 - 1.5f ;
				System.out.println("Position: "+nex + " ,"+ney+ " "+((FlameRoom)avatar.getParent()).xSectors + " "+ ((FlameRoom)avatar.getParent()).ySectors);
			}
			
			// Tell other players that this player joined
			w.postMessageThreadSafe(avatar.getName()+" joined team "+avatarPlayerReferences.teamIndex);


			// For debugging, this can be removed later.
			if (avatarPlayerReferences.getId()<0)
			{
				error("avatarPlayerReferences.getId()<0");
				return;				
			}
			if (avatarPlayerReferences.avatarId!=playerAvatar.getId())
			{
				error("avatarPlayerReferences.avatarId!=playerAvatar.getId()");
				return;
			}
			if (avatarPlayerReferences.getId()!=avatar.getAvatarPlayerReferences().getId())
			{
				error("avatarPlayerReferences.getId()!=avatar.avatarPlayerReferencesId");
				return;
			}
			if (avatarPlayerReferences.teamIndex!=team)
			{
				error("avatarPlayerReferences.teamIndex==team");
				return;				
			}
			
			

			
			
			// If we send only nearest make sure we have sent avatar before telling client its id.
			updateObjToClientRecursive(playerAvatar);
			updateObjToClientRecursive(avatarPlayerReferences);

			// Tell client which object that is its avatar
			//stc.writeLine("avatarId " + playerAvatar.getId()); 

			// Tell client which object that is its AvatarPlayerReferences
			stc.writeLine("setAprId " + avatarPlayerReferences.getId()); 

			
			// Send all existing objects to client
			findAndSendDbUpdatesToClientAll();

			stc.writeLine("showWorld ");
			
			stc.writeLine("consoleMessage \"Hello, from the super awsome cool server!\n\"");

			
  			int timeoutCounter=0; // If we get many timeouts in a row we will disconnect
			
			
			// loop waiting for input from client
	  		while(stc.isOpen())
	  		{
	  			long prevUpdate_ms=System.currentTimeMillis();
	  			
	  			try {
	  				final String r = stc.readLine(50);

	  				// This was not timeout so reset that counter
	  				timeoutCounter=0;
				
					WordReader wr=new WordReader(r);
					
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
							stc.writeLine(m);
						}
					}
					else
					{
						// The input line was empty, this was not expected (timeouts are InterruptedException) but will ignore it for now. Perhaps client sends empty strings?
						//debug("why did we get an empty string from client?");
					}
					
					
					

	  			} catch (InterruptedException e) {
	  				// This was just a timeout. But if we have many in a row we disconnect the client.
	  				if (timeoutCounter>5*60*10)  // 5*60*10 here will give 5 min if readLine has 100 ms timeout.
	  				{
	  					throw new InterruptedException();
	  				}
	  				else
	  				{
		  				timeoutCounter++;	  					
	  				}
	  			}
	
				// Find changes in database.
	  			// But not too often since that would give us excessive CPU load
	  			long t_ms=System.currentTimeMillis();
	  			long delta_ms=t_ms-prevUpdate_ms;
	  			if (delta_ms>20)
	  			{
	  				findAndSendDbUpdatesToClient();
	  				prevUpdate_ms=t_ms;
	  			}

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
			// TODO: Since MirrorServer is handling the notifications it should add and remove subscription. This cleanup should be moved to class MirrorServer somehow. 
			debug("client disconnecting");
			close();

			w.playerDisconnected(avatarPlayerReferences);
			avatarPlayerReferences=null;
		}
		
		
		
	}

	// We override this since for HMEG we don't want any custom script, at least not for now. Comment this method out if custom script shall be used.
	@Override
    public void configureGame()
    {
		
    	int minSize=20;
    	int maxSize=80;
    	
    	int size = minSize;
    	
    	if (maxSize>minSize)
    	{    	
    		size = stc.promptInt("world_size", "How large shall the world be? "+minSize+"=min, "+maxSize+"=max", minSize, maxSize);
    	}
    			
    	FlameWorld ew = (FlameWorld)worldBase;
		
		ew.generateSelf(size);   	
    }

	
	
	@Override
	public WorldBase createWorld() {
		return new FlameWorld();
	}
	
	
}