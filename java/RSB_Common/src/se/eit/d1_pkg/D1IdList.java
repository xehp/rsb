// DbIdList.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2014 

package se.eit.d1_pkg;


import se.eit.web_package.*;


// Perhaps we could improve this code by using HashMap? 
// http://docs.oracle.com/javase/7/docs/api/java/util/HashMap.html

public abstract class D1IdList extends D1TickReceiver {

	// This variable is only used for debugging, can be removed later.
	//public boolean debugLoadingWorldInProgress=false;
	
	public D1List<D1IdObj> dbListId=new D1List<D1IdObj>();
	
	//Queue<Integer> unusedIds = new LinkedList<Integer>();  // A queue of unused IDs to replace latestIdGiven. The idea is to avoid reusing an ID too soon.
	
	
	public D1IdList()
	{
		super();
		
		// The IdList object will be object zero itself. First other object will be 1. An IdList can not reside in another IdList since then it would not be object with ID 0 in that list.
		setIdFromDbIdList(0, this);  
		dbListId.add(0, this);
	}





	// getObjectById, Get a reference to an object from its ID.
	@Override
	public D1IdObj getDbIdObj(int id)
	{
		if (id<0)
		{
			debug("no id "+id);
			return null;
		}
		try
		{
			return dbListId.get(id);
		}
		catch (ArrayIndexOutOfBoundsException e)
		{
			debug("did not find ~"+id+", " +e);
			return null;
		}
	}

	// Register ID object in list (unless it is already registered)
	// Returns id if ok, 
	// <0 if not ok
	// The caller must set the id in idObj to the returned value.
	public int addIdObj(D1IdObj idObj)
	{
		debugWriteLock(); // not thread safe, lock database for write before calling

		int id=idObj.getId();
		
		if (id>=0)
		{
			// The object already has an ID.
			dbListId.add(id, idObj);
		}
		else
		{
			// The object did not have an ID so it shall be assigned one.
			// TODO: It can be a problem if this happens while loading a file since we do not yet know which ID are already used. We need some way to tell which case it is. For now we require that all objects saved to file already have an ID. But if we are to import schematics then we need a way to load objects from file without IDs.
			
			// The following few lines are for debugging
			/*if (debugLoadingWorldInProgress)
			{
				error("Can not assign new ID while loading world");
			}*/
			
			id=dbListId.add(idObj);
		}
		
		return id;
	}
	

	
	
	
	// This will do tick for all DbIdObj. The objects that are not subclasses of DbIdObj get their tick from DbSubRoot instead.
	// Since we now allow objects to move between rooms and that happens during tick we need to call tick using DbIdList instead of sub objects. Otherwise iterating the world got messed up when objects where moved as they may do during a tick.
    // TODO: Is this still used? Yes but need to look into this because this will take a lot of CPU time for perhaps little use.
    /*
	@Override
	public void tickRecursiveMs(int ms)
	{		
		lockWrite();
		try
		{
			//tickSelfMs(ms); // don't do self here since ID=0 is also self and called in the loop just below. May need to change this if we remove self from idList.

			for (DbIdObj s : idList)
			{
				s.tickSelfMs(ms);
			}			
		}
		finally
		{
			unlockWrite();
		}		
	}
    */

	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		pw.println("idList.capacity"+" "+dbListId.getCapacity());
	}

	// This will recursively check that all sub objects are registered in the id list. If they are not registered they are registered.
	/*public void registerSubObjectsInDbIdList()
	{
		if (listOfStoredObjects!=null)
		{
			for (DbStorable bo : listOfStoredObjects)
			{
				bo.registerSelfAndSubObjectsInDbIdList(this);
			}
		}
	}*/
	
	// TODO rename this to getDbIdListCapacity if this method is at all needed.
	public int getDbIdListLength()
	{
		return dbListId.getCapacity();		
	}
	
	
	
	
	// Remove an id from the list
	// TODO: we should keep a queue of unused IDs so that we reuse only the one that has been unused the longest time.
	public void unregDbIdObj(D1IdObj idObj)
	{
		final int id=idObj.getId();

		// just for debugging, can be commented out later
		if ((idObj.getDbIdList()!=this) || (dbListId.get(id)!=idObj)) 
		{
			error("inconsistent db "+" "+id);
		}

		dbListId.remove(id);
		idObj.setIdFromDbIdList(-1, null);
	}

	
	
	// This is just for debugging.
	@Override
	public void linkSelf(D1Container parentObj)
	{
		// The following code is for debugging, can perhaps be commented out later.
		// An IdList can not reside in another IdList since then it would not be object with ID 0 in that list. So checking that here.		
		if (parentObj.isThisOrAnyParentAnIdList())
		{
			error("A DbSubRoot can not be stored in another DbSubRoot");
		}
		
		super.linkSelf(parentObj);
		
	}

	@Override
	public D1IdList getDbIdList()
	{
		return this;		
	}
	
	@Override
	public D1SubRoot getDbSubRoot()
	{
		return this;		
	}
	
	
	
	
}
