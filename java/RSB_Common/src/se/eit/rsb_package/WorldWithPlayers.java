// WorldWithPlayers.java
//
// Copyright (C) 2016 Henrik Bjorkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.rsb_package;

import se.eit.d1_pkg.*;

// This is the class for the world object.
// The world object is special in that for a game there shall only be one world object and it must be the root object in that game.
// The client might fail if server does something else.

public abstract class WorldWithPlayers extends ActiveObjectList  {
	
	static final String listOfUsersName="_listOfUsers";
	//public DbList<PlayerData> listOfActivePlayers=null;

	protected ActivePlayerList listOfUsers=null;
	
	public WorldWithPlayers()
	{
		super();
		//listOfActivePlayers=new DbList<PlayerData>();		
	}
	
	
	// Call this to add the player to the list of active players
	// Remember to call removePlayer when leaving
	public GameUser playerConnectedFindOrCreateGameUser(UserAccount player, int team)
	{
		getListOfActivePlayers();
		
		final String playerName=player.getName();
		
		// First check that player is not already playing here
		GameUser apr = (GameUser)listOfUsers.findObjectByNameAndType(playerName, GameUser.className());

		D1SubRoot ro = this.getDbSubRoot();
		ro.lockWrite();
		try
		{

			if (apr!=null)			
			{
				// It is a know player. Is this player already playing?
				if (apr.getActive()==false)
				{
					// This is a returning player, the player can now start playing
					
					apr.setTeam(team);
					apr.setActive(true);

				}
				else
				{
					// This player is already playing in at this world, can not connect one more.
					return null;
				}
			}
			else
			{
				// This is a new player, create a new GameUser.
				apr = playerJoinedCreateApr(player, team);					
				apr.setActive(true);
			}

			// Now create an avatar also
			playerJoinedFindOrCreateAvatar(apr);				

		}
		finally
		{
			ro.unlockWrite();
		}
		
		
		return apr;
	}
	
	
	public void playerDisconnected(GameUser apr)
	{				
		
		apr.setActive(false);
			
		D1SubRoot ro = this.getDbSubRoot();
		ro.lockWrite();
		try
		{
			D1IdObj a = ro.getDbIdObj(apr.avatarId);
			apr.avatarId=-1;
			if (a!=null)
			{
				a.unlinkSelf(); // TODO instead of unlink perhaps it should be moved to the apr object. Or in some other way remember its position, so it can reenter at same place.
			}
		}
		finally
		{
			ro.unlockWrite();
		}

		
		//apr.unlinkSelf();
	}		
	
	/*
	public PlayerData getPlayerAvatar(int playerIndex)
	{
		return listOfActivePlayers.get(playerIndex);		
	}
	*/
	
	
	public D1Named getListOfActivePlayers()
	{
		if (listOfUsers==null)
		{
			debug("This should not happen since regNamedChildObject should have taken care of it already");
			listOfUsers = (ActivePlayerList)findOrCreateChildObject(listOfUsersName, ActivePlayerList.className());
		}
		
		return listOfUsers;
	}

	
	// Register the objects created by getOrCreateSubRoom (or by createNamedObjects)
	@Override
	public boolean regNamedChildObject(D1Named addedObject)
	{
		if (addedObject.getName().equals("_listOfUsers"))
		{
			listOfUsers=(ActivePlayerList)addedObject;
			return true;
		}
		else
		{
			return super.regNamedChildObject(addedObject);
		}
	}
	
	public int getNActivePlayers()
	{
		getListOfActivePlayers();
		return listOfUsers.getNActivePlayers();
	}

	
	// Creates the AvatarPlayerReferences (APR) for a player
	// Will also create the avatar for the player.
	// When player leaves playerDisconnected shall be called.
	protected GameUser playerJoinedCreateApr(UserAccount userAccount, int team)
	{
		final String playerName=userAccount.getName();
		final String className=GameUser.className();
		GameUser apr = (GameUser)listOfUsers.findOrCreateChildObject(playerName, className); 
		apr.player=userAccount;
		apr.setTeam(team);
			
		return apr;
	}

	// extending classes 
	public abstract D1ThreadSafe playerJoinedFindOrCreateAvatar(GameUser apr);
	/*{
		// Game worlds where players have avatars must override this method so it creates one.
		return null;
	}*/
	
	public void setTeam(GameUser apr, int team)
	{
		debug(apr.getNameAndId()+" joins "+team);
	}

	public abstract D1ThreadSafe findSpawnRoom(GameUser gameUser);


	
	@Override
	public void generateSelf()
	{
		//super.generateSelf();
		listOfUsers = (ActivePlayerList)findOrCreateChildObject(listOfUsersName, ActivePlayerList.className());
	}
}
