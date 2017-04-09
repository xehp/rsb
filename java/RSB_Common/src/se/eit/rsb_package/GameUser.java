/*
Copyright (C) 2016 Henrik Bjorkman (www.eit.se/hb)
License: www.eit.se/rsb/license
*/
package se.eit.rsb_package;

import java.io.IOException;

import se.eit.d1_pkg.*;
import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

// This class keeps score and will tell where the players avatar is and which team.


public class GameUser extends D1ThreadSafe {

	final public static int MutedPlayerMode=4;
	
	public UserAccount player=null;
	private boolean isActive=false; // Did we need a separate variable for this, we could use avatarId=-1 for inactive?
	public int avatarId=-1; // If this is -1 then this game user does not have an avatar and is probably not playing in this game at the moment (not playing yet or no longer playing).
	public int teamIndex=-1;
	
	public int fragScore=0;
	public int lossScore=0;

	public int tmpPinCode=0; // Not saved to disk.

	public RsbRoundBuffer rsbRoundBuffer=null;

	public SmallFifoBuffer directMessageToClient=new SmallFifoBuffer();
	
	public long userMode=0;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return GameUser.class.getSimpleName();	
	}
	

	public GameUser()
	{	
		super();
	}
	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		avatarId=wr.readInt();
		isActive=wr.readBoolean();
		teamIndex=wr.readInt();
		fragScore=wr.readInt();
		lossScore=wr.readInt();

		isActive=false; // We only ever read AvatarPlayerReferences when we load from disk. At that point no player is active even if they were when saving to disk.
	}

	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(avatarId);
		ww.writeBoolean(isActive);
		ww.writeInt(teamIndex);
		ww.writeInt(fragScore);
		ww.writeInt(lossScore);
	}	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);					
		pw.println("avatarId "+avatarId);		
		pw.println("isActive "+isActive);		
		pw.println("teamIndex "+teamIndex);		
		pw.println("fragScore "+fragScore);		
		pw.println("lossScore "+lossScore);		
		pw.println("tmpPinCode "+tmpPinCode);		
	}
	
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("avatarId"))
		{
			avatarId=wr.readInt();
			this.setUpdateCounter();
			return 1;
		}
		if (infoName.equals("teamIndex"))
		{
			teamIndex=wr.readInt();
			this.setUpdateCounter();
			return 1;
		}
		if (infoName.equals("isActive"))
		{
			isActive=wr.readBoolean();
			this.setUpdateCounter();
			return 1;
		}
		if (infoName.equals("fragScore"))
		{
			fragScore=wr.readInt();
			this.setUpdateCounter();
			return 1;
		}
		if (infoName.equals("lossScore"))
		{
			lossScore=wr.readInt();
			this.setUpdateCounter();
			return 1;
		}
		if (infoName.equals("tmpPinCode"))
		{
			tmpPinCode=wr.readInt();
			this.setUpdateCounter();
			return 1;
		}
		
		return super.setInfo(wr, infoName);
	}
	
	@Override
	public int getInfo(WordWriter ww, String infoName, WordReader wr)
	{
		if (infoName.equals("isActive"))
		{
			ww.writeBoolean(isActive);
			return 1;
		}
		else if (infoName.equals("teamIndex"))
		{
			ww.writeInt(teamIndex);
			return 1;
		}
		else if (infoName.equals("fragScore"))
		{
			ww.writeInt(fragScore);
			return 1;
		}
		else if (infoName.equals("lossScore"))
		{
			ww.writeInt(lossScore);
			return 1;
		}
		else if (infoName.equals("tmpPinCode"))
		{
			ww.writeInt(tmpPinCode);
			return 1;
		}
		
		return super.getInfo(ww, infoName, wr);
	}
	
	public void setActive(boolean active)
	{
		if (isActive!=active)
		{
			isActive=active;
			this.setUpdateCounter();
		}
	}
	
	public boolean getActive()
	{
		return isActive;
	}

	public void setTeam(int team)
	{
		if (this.teamIndex!=team)
		{
			this.teamIndex=team;
			this.setUpdateCounter();
			
		     if (this.avatarId>=0)
		     {
				// Find the players avatar and set the team  for it also.
				D1SubRoot r = this.getDbSubRoot();		
				D1Base a = r.getDbIdObj(avatarId);		
				if ((a!=null) && (a instanceof AvatarInterface))
				{
					AvatarInterface avatar=(AvatarInterface)a;
					avatar.setTeam(team);
				}
		    }
		
		}
	}
		
	public void swapTeam()
	{
		D1SubRoot r = this.getDbSubRoot();		
		D1Base a = r.getDbIdObj(avatarId);
		
		if ((a!=null) && (a instanceof AvatarInterface))
		{
			//AvatarInterface avatar=(AvatarInterface)a;
					
			if (teamIndex<2)
			{
				setTeam(teamIndex+1);
			}
			else
			{
				setTeam(1);											
			}
			
			
			a.postMessageToThis("you are now in team "+teamIndex);
		}
		debug("not implemented yet");
	}


	// Find or create the message buffer
	public RsbRoundBuffer findOrCreateRsbRoundBuffer()
	{
		
		// Do we have a quick reference to it?
		if (rsbRoundBuffer!=null)
		{
			// We already have a short cut to it.
			return rsbRoundBuffer;
		}
		
		
		
		// Check if it already exits among our sub (AKA child) objects.
		// TODO: This should not be needed any longer, the reference is set by regNamedObject instead.
		if (this.listOfStoredObjects!=null)
		{
			for (D1Base db : this.listOfStoredObjects)
			{
				if (db instanceof RsbRoundBuffer)
				{
					debug("Did not think this was still used.");
					// Found it, remember it and return the reference.
					rsbRoundBuffer=(RsbRoundBuffer)db;
					return rsbRoundBuffer;
				}		
			}
		}
		
		// Create a new one.
		
		rsbRoundBuffer = new RsbRoundBuffer(this, "_rrb");
		//this.addObject(erb);
		//erb.setUpdateCounter(); // There is a problem with setting update counter, it does not work when addObject is done. When that is fixed this line can be removed.
		return rsbRoundBuffer;
	}

	
	// Register the objects created by getOrCreateSubRoom (or by createNamedObjects)
	@Override
	public boolean regNamedChildObject(D1Named addedObject)
	{
		if ((addedObject instanceof RsbRoundBuffer) && (addedObject.getName().charAt(0)=='_'))
		{
			rsbRoundBuffer = (RsbRoundBuffer)addedObject;
			return true;
		}
		else
		{
			return super.regNamedChildObject(addedObject);
		}
	}

	// This is called when a message to this player is to be posted.
	@Override
	public void postMessageToThis(String str)
	{
		debugWriteLock();

		
		// Perhaps here is a place we can tell client so play sounds also?
		// Or should it be done from checkCollisionWithAvatar? See comments in checkCollisionWithAvatar.
		// If so we should have something here that forwards the important messages to round buffer and the rest to consoleMessage.
		// For example message player <x> was hit by <y> can be a consoleMessage
		// But here we don't have a reference to the YukigassenServer/OpServer object.
		// Should we have a queue instead? The YukigassenServer/OpServer would then need to poll that queue.
		// See also comments about sound in YukigassenServer.joinYukigassen
		
		// Important messages shall go to the bigger round buffer while less important ones (such as sound) shall go to the smaller round buffer for direct messages

		if (!sendAsDirectMessage(str))
		{
			// find the round buffer and post message to it
			RsbRoundBuffer rrb = findOrCreateRsbRoundBuffer();
			rrb.postMessageToThis(str);
			//setUpdateCounter();
		}
	}

	
	public void incFragScore()
	{
		fragScore++;
		this.setUpdateCounter();
	}
	
	public void incLostScore()
	{
		lossScore++;
		this.setUpdateCounter();
	}


	@Override
	public void linkSelf(D1Container parentObj)
	{
		super.linkSelf(parentObj);
		WorldBase worldBase = (WorldBase)this.getDbSubRoot();
		tmpPinCode=worldBase.random.nextInt();
	}

			

	public boolean sendAsDirectMessage(String str)
	{
		char ch=str.charAt(0);
		{
			switch (ch)
			{
				case 'c':
				{
					WordReader wr=new WordReader(str);
					final String cmd=wr.readWord();

					if (cmd.equals("consoleMessage"))
					{
						// Information about other players can also be sent as direct.
						debug("sendAsDirectMessage "+cmd+" "+str);
						directMessageToClient.put(str);
						return true;
					}
					break;
				}
				case 'p':
				{
					WordReader wr=new WordReader(str);
					final String cmd=wr.readWord();
					
					if (cmd.equals("playSoundFile"))
					{
						debug("sendAsDirectMessage "+cmd+" "+str);
						directMessageToClient.put(str);
						return true;
					}
				
					/*
					if (cmd.equals("player"))
					{
						// Information about other players can also be sent as direct.
						debug("sendAsDirectMessage "+cmd);
						WordWriter ww=new WordWriter();
						ww.writeWord("consoleMessage");
						ww.writeString(str);
						directMessageToClient.put(ww.getString());
						return true;
					}*/
					break;
				}
				default:
					break;
			}
		}
		return false;
	}
	

	// Returns true if the command was found and performed.
	@Override
	public boolean interpretCommand(String cmd, WordReader wr, WordWriter ww) throws IOException
	{
		final char ch=cmd.charAt(0);
		switch(ch)
		{	
			case 's':
				if (cmd.equals("say"))
				{					
					if ((userMode&MutedPlayerMode)!=0)
					{
						ww.writeLine("You have been muted by op");
					}
					else
					{
						// TODO check for forbidden characters like bell and stuff.
						String line=wr.readLine();
						String str=WordWriter.safeStr(line, 1024);
						postConsoleMessageToAllGameUsers(this.getName()+": "+str);
					}
					return true;
				}
				break;

		}

		if (super.interpretCommand(cmd, wr, ww))
		{
			return true;
		}
		
		return false;
	}		

	public void postConsoleMessageToThisGameUser(String str)
	{
		WordWriter ww2=new WordWriter();
		ww2.writeWord("consoleMessage");
		ww2.writeString(this.getName()+": "+str);
		postMessageToThis(ww2.getString());
	}

	
	public void postConsoleMessageToAllGameUsers(String str)
	{
		D1Container p=this.getParent();
		WordWriter ww2=new WordWriter();
		ww2.writeWord("consoleMessage");
		ww2.writeString(str);
		p.postMessageToThis(ww2.getString());
	}

	public static String getTeamName(int i)
	{
		switch(i)
		{
			case 1: return "1(blue)"; 
			case 2: return "2(red)";
			default: break;
		}
		return "unknown";
	}
	
	public String getTeamName()
	{
		return getTeamName(teamIndex);
	}
}
