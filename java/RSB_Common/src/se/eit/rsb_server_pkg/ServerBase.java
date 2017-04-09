// ServerBase.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 


package se.eit.rsb_server_pkg;

//import se.eit.rsb_factory_pkg.GlobalConfig;
import se.eit.rsb_package.UserAccount;
import se.eit.rsb_package.WorldBase;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;


/*
 * All game server threads inherit this class.
 * To create a new type of game:
 * Create a new class extending this class.
 * Add it in PlayerConnectionThread.startNewGame and in PlayerConnectionThread.playWorld
 * Create a new game base object extending WorldBase. 
 * Add that new class in DbContainer.createObj
 */


public abstract class ServerBase {

	protected WorldBase worldBase;
	protected D1GlobalConfig config;
	protected ServerTcpConnection stc;
	protected UserAccount userAccount;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return ServerBase.class.getSimpleName();	
	}	
	
    public void debug(String str)
	{
    	WordWriter.safeDebug(className()+"("+stc.getTcpInfo()+"): "+str);
	}

    public static void error(String str)
	{
    	WordWriter.safeError(className()+": "+str);
	}
	
	
	public ServerBase()
	{
	}
	
    public static boolean isStringOkAsWorldName(String name)
    {
    	return WordWriter.isNameOk(name,1); // We shall perhaps require world names to be longer eventually.
    }
	
	public void joinWorld(WorldBase worldBase, D1GlobalConfig config, UserAccount userAccount, ServerTcpConnection stc)
	{
		this.worldBase=worldBase;
		this.config=config;
		this.userAccount=userAccount;
		this.stc=stc;
		joinWorld();
	}

	
	// This is called from PlayerConnectionThread.startNewGame when a new game shall be started.
	// The terrain etc for the new game needs to be filled in.
	public WorldBase createAndStore(D1GlobalConfig config, UserAccount userAccount, ServerTcpConnection stc)
	{
		this.config=config;
		this.stc=stc;
		this.userAccount=userAccount;
		
		if (worldBase!=null)
		{
			error("worldBase is not null");
		}
		
		String worldName=null;
		while((stc.isOpen() && (worldName==null)))
		{
	    	worldName=stc.enterNameForNewGame();

	    	if (worldName==null)
	    	{
	    		// user did cancel or disconnect.
	    		break;	    		
	    	}
	    	
	    	if (!isStringOkAsWorldName(worldName))
    		{    			
    			stc.alertBox("name_not_accepted", "name not accepted, try another name with only letters and digits");
    			worldName=null;
    		}
	
	    	if (worldName!=null)
	    	{
	    		D1SubRoot wdb=stc.findOrCreateGameDb();
	    		wdb.lockWrite();
	    		try
	    		{
	    			if (wdb.findGameObjNotRecursive(worldName)!=null)
	    			{
	    				debug("Name already exist '"+worldName+"'");
	    	    		stc.alertBox("name_already_taken", "name already taken");
	    				worldName=null;
	    			}	    
	    			else
	    			{
	    				debug("Name '"+worldName+"' is not yet used");
	    				
    		    		worldBase = createWorld();
    		    		worldBase.linkSelf(wdb);
    		    		worldBase.regName(worldName);
    		    		worldBase.setCreatedBy(userAccount.getName());
	    		    	
	    				
	    			}
	    		}
	    		finally
	    		{
	    			wdb.unlockWrite();
	    		}
	    	}
		}		

		
    	if (worldBase!=null)
    	{
    		worldBase.lockWrite();
	    	try {
	    		configureGame();
	    		//worldBase.setGlobalConfig(config);
				worldBase.saveRecursive();
	    	}
			finally
			{
				worldBase.unlockWrite();
			}
    	}
    	

    	
		return worldBase;		
		
	}

	public void close()
	{
		if (stc.isOpen())
		{
			stc.close();
		}
	}
	
	//abstract protected String createAndStore(String worldName);
	
	abstract public WorldBase createWorld();
	
	// This method needs to be overridden if game needs to be configured before generated. 
	// In the overriding method it is possible to interact with the client using the stc object. 
	public void configureGame()
	{
		worldBase.generateSelf();
	}

	abstract public void joinWorld();
	
	/*
	// Tell here what type of client is needed for the game
	abstract public boolean need2dSupport();

	// Tell here what type of client is needed for the game
	abstract public boolean need3dSupport();
	*/


	public boolean onlyOnMainServer()
	{
		return false;
	}

}
