/*
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license
*/
package se.eit.LoginLobbyPkg;

import se.eit.d1_pkg.D1Named;
import se.eit.d1_pkg.D1ThreadSafe;
import se.eit.d1_pkg.D1TickList;
import se.eit.rsb_package.*;

// This is a place where servers and players connect.

// TODO move this to se.eit.LoginLobbyPkg
public class LoginLobbyWorld extends /*ActiveList*/ WorldWithPlayers {

	static public final String nameOfExternalServersDb="_externalServers";
	static public final String nameOfActivePlayersList="_activePlayers";
	// Numbers here must match buttonNames in LoginLobbyServer.chooseType so if changing here changed there also.
	static public final int ServerType=0;
	static public final int PlayerType=1;
	static public final int UnknownType=-1;
	
	D1TickList superRoot=null;
	//D1SubRoot playersList = null;
	RsbString externalServers = null;
	RsbString activePlayers = null;
	
	// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
	public static String className()
	{	
		return LoginLobbyWorld.class.getSimpleName();	
	}
	
	@Override
	public void generateSelf() {

		// Find reference to all Player Data.
		
		superRoot = getDbSuperRoot();

		//playersList = (D1SubRoot)superRoot.findGameObjNotRecursive(UserAccount.nameOfPlayersDb);
		
		externalServers = (RsbString)this.findOrCreateChildObject(nameOfExternalServersDb, "RsbString");
		activePlayers = (RsbString)this.findOrCreateChildObject(nameOfActivePlayersList, "RsbString");
		
	}

	// This is called when a server wants to know if a player is known.
	public GameUser isClientCodeOk(int playerIndex, int playerCode)
	{
		GameUser gameUser=(GameUser)listOfUsers.getObjFromIndex(playerIndex);
		
		if (gameUser!=null)
		{
			if (gameUser.tmpPinCode == playerCode)
			{
				return gameUser;
			}
			else
			{
				debug("Pin code did not match: playerIndex="+playerIndex+", playerCode="+playerCode+", name="+gameUser.getName()+", tmpPinCode="+gameUser.tmpPinCode);
				return null;
			}
		}
		debug("GameUser not found "+playerIndex);
		return null;
	}
	

	// When a server has logged in it shall call this to list itself.
	// TODO should get the IP of the server also.
	/*
	public void addServer(UserAccount playerData)
	{
		LuaBase sr = findSpawnRoom();
		sr.findOrCreateChildObject(playerData.getName(), "RsbString");
	}
	*/

	
	@Override
	public void saveSelf()
	{
		//debug("saveSelf: don't save this one");
	}	

	// In this case a user can be a server or a player is in the other end.
	@Override
	public D1ThreadSafe playerJoinedFindOrCreateAvatar(GameUser gameUser)
	{
		// In this world servers are team 0 and players are team 1.

		D1ThreadSafe spawnRoom = findSpawnRoom(gameUser);
		
		final String name = gameUser.getName();
		final String className = LoginLobbyAvatar.className();
		
		LoginLobbyAvatar a = (LoginLobbyAvatar)(spawnRoom.findOrCreateChildObject(name, className));
		
		gameUser.avatarId = a.getId();
		
		return a;
	}

	// This returns the name of the server object that clients shall use to play this world.
	@Override
	public String serverForThisWorld()
	{
		return LoginLobbyServer.className(); // The name used needs to match that in serverFactory
	}

	@Override
	public D1ThreadSafe findSpawnRoom(GameUser gameUser) 
	{
		if (gameUser.teamIndex==0)
		{
			return externalServers;		
		}
		else
		{
			return activePlayers;
		}
	}


	@Override
	public boolean regNamedChildObject(D1Named addedObject)
	{
		final String n=addedObject.getName();
		if ((addedObject instanceof RsbString) && (n.equals(nameOfExternalServersDb)))
		{
			
			// the following few lines are for debugging, can be removed later
			if ((externalServers!=null) && (addedObject!=externalServers))
			{
				error("not allowed to change this child object");
			}
			else if (!(addedObject instanceof RsbString))
			{
				error("wrong type for child object");
			}

			externalServers=(RsbString)addedObject;
			return true;
		}
		else if ((addedObject instanceof RsbString) && (n.equals(nameOfActivePlayersList)))
		{
			
			// the following few lines are for debugging, can be removed later
			if ((activePlayers!=null) && (addedObject!=activePlayers))
			{
				error("not allowed to change this child object");
			}
			else if (!(addedObject instanceof RsbString))
			{
				error("wrong type for child object");
			}

			activePlayers=(RsbString)addedObject;
			return true;
		}
		else
		{
			return super.regNamedChildObject(addedObject);
		}
	}
	
	
	RsbString getExternalServers()
	{
		return externalServers;
	}
	
	
}
