// DbThreadSafe.java
//
// Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;


import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;


public abstract class D1ThreadSafe extends D1ChangedCounter {

	public D1ThreadSafe() {
		super();	
	}

	public D1Base[] getListOfSubObjectsThreadSafe()
	{	
		final D1SubRoot db=getDbSubRoot();
		db.lockRead();
		try
		{
			return getListOfSubObjects();
		}
		finally
		{
			db.unlockRead();
		}
	}

	
	public int moveToRoomThreadSafe(D1Container to)
	{
		
		final D1SubRoot db=getDbSubRoot();
		db.lockWrite();
		try
		{
			return relinkSelf(to);
		}
		finally
		{
			db.unlockWrite();
		}
		
	}


	public void addObjectThreadSafe(D1Storable obj)
	{
		final D1SubRoot db=getDbSubRoot();
		db.lockWrite();
		try
		{
			//obj.linkSelf(this);
			this.addObject(obj);
			obj.setUpdateCounter();  // This should be superfluous but there is some problem somewhere, without this the JS client will not get order updates so this is still needed but it should not be. Hopefully not needed now.
		}
		finally
		{
			db.unlockWrite();
		}
	}
	
	// Same as above but used if object is to be stored on a specific position.
	// Deprecated. Do setIndex on the object to add and then use "public void addObjectThreadSafe(DbStorable obj)". It will use the index the object wants.
	/*
	public void addObjectThreadSafe(DbStorable obj, int index)
	{
		final DbSubRoot db=getDbSubRoot();
		db.lockWrite();
		try
		{
			//obj.linkSelf(this, index);
			this.addAndRegObjAtIndex(obj, index);
		}
		finally
		{
			db.unlockWrite();
		}
	}
	*/


	public void delObjectThreadSafe(D1Base obj)
	{
		final D1SubRoot db=getDbSubRoot();
		db.lockWrite();
		try
		{
			obj.unlinkSelf();
		}
		finally
		{
			db.unlockWrite();
		}
	}

	
	public final int getNSubObjectsThreadSafe()
	{	
		final D1SubRoot db=getDbSubRoot();
		db.lockRead();
		try
		{
			return getNSubObjects();
		}
		finally
		{
			db.unlockRead();
		}
	}

    public void clearStoredThreadSafe()
    {	
		final D1SubRoot db=getDbSubRoot();
		db.lockWrite();
		try
		{
			clearStored();
		}
		finally
		{
			db.unlockWrite();
		}
    }

    public void postMessageThreadSafe(String str)
    {
		final D1SubRoot dsr=getDbSubRoot();
		// Tell other players that this player joined the game
		dsr.lockWrite();
		try
		{
			//avatar.textureIndex=team;
			
			/*DbBase dd = avatarPlayerReferences.getParent();
			if (dd==null)
			{
				dd=w;
			}
			dd.postMessage(avatar.getName()+" joined team "+avatarPlayerReferences.teamIndex);
			
			*/
			
			this.postMessageToThis(str);
		}
		finally
		{
			dsr.unlockWrite();
		}
    }
    
    
    @Override 
    public void readSelf (WordReader wr) 
    {
    	super.readSelf(wr);
    	wr.readBoolean();
    }
    
    @Override
    public void writeSelf (WordWriter ww)
    {
    	super.writeSelf(ww);
    	ww.writeBoolean(true);
    }
    
    public void generateSelf()
    {
    	// unless overridden this does nothing
    }


	
}

