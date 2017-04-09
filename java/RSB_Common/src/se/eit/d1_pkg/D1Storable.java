/*
DbStorable.java

The data base object (DbStorable) is that base object for objects that can be stored in our data base.
If they need to be able to contain other data base objects then the extension DbContainer can be used.


Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
2013-02-14
Created by Henrik Bjorkman (www.eit.se)

*/


package se.eit.d1_pkg;

//import se.eit.rsb_package.SubRoot;
import se.eit.web_package.*;




// DbStorable is the base class for all objects that can be stored in a DbContainer.



public abstract class D1Storable extends D1Base {

	
    static final int NO_INDEX = -1; // must be -1, it is assumed so in several places
    
	// Objects are stored in other objects
	protected D1Base containingParentObj=null;  // This is a reference to the object that this object is stored in.
	private int containingIndex=NO_INDEX; // This tells in which slot in the parent object that this object is stored.

	//int changedCounter=0;


	public D1Storable()
	{
		super();	
	}
	
	public D1Storable(final D1Storable org)
	{
		super(org);	
		
		// DbContainer copy constructor must set these.
		// don't really know at this point what to set them to though.
		this.containingIndex=NO_INDEX; // org.superObjIndex;
		this.containingParentObj=null; // org.superObj
	}
	
	
	
	// deserialise from wr
	// sub classes with additional member variables that need 
	// to be saved to disk shall implement this method.
	@Override
	public void readSelf(WordReader wr)	
	{
		// The super class of DbStorable does not have a readSelf method so here a call to super.readSelf is not needed. But all derived classes shall call "super.read".
		//super.read(wr);

		final int i = wr.readInt();
		setIndex(i);
	}

	// Serialise to ww
	// Write all available variables within this object.
	// Sub classes with additional member variables that need to be saved to disk must implement this method.
	// Those overriding methods shall also call "super.write".
	@Override
	public void writeSelf(WordWriter ww)
	{
		// The super class of DbStorable does not have a writeSelf method so here a call to super.writeSelf is not needed. But all derived classes shall call "super.write".
		//super.write(ww);
		
		ww.writeInt(containingIndex);
	}

	@Override
	public void listInfo(WordWriter pw)
	{
		//super.listInfo(pw, prefix);					
		pw.println("containingIndex"+" "+containingIndex);		
	}

	
	
	// This shall be called from DbContainer only.
	protected void setContainingObjIndex(D1Base parentObj, int indexInParent)
	{
		containingIndex = indexInParent;
		containingParentObj = parentObj;
	}
	
	public String getDbSubRootNameAndPath(String separator)
	{	
		D1ThreadSafe ro = this.getDbSubRoot();
		if (ro!=null)
		{
			return ro.getNameAndPath(separator);
		}
		return "unknown";
	}

	// This gives full path all the way to the root of the database.
	public String getNameAndPath(String separator)
	{	
        if (containingParentObj!=null)
		{	
			return containingParentObj.getNameAndPath(separator)+separator+containingIndex;
		}
		
		return separator+ separator +containingIndex;
	}
	
	// This is similar to getNameAndPath but gives the path within a game data base, not all the way to program wide root.
	public String getNameAndPathToRoot(String separator)
	{
        if ((containingParentObj!=null) && (!(containingParentObj instanceof D1SubRoot)))
		{	
			return containingParentObj.getNameAndPath(separator)+separator+containingIndex;
		}
		
		return separator + containingIndex;
	}
	
  	/*
	public String getIndexPathWithinDbSubRoot()
	{
		return getIndexPathWithinDbSubRoot("/");
	}
	*/
	
	// If separator is given as "/" then this is the inverse of getObjFromIndexPathWithinDbSubRoot
	// This is similar to getIndexPath but gives the path within a game data base, not all the way to program wide root.
	public String getIndexPathWithinDbSubRoot(String separator)
	{
        if (containingParentObj!=null)
		{	
        	// If there was a getIndexPathWithinDbSubRoot in class DbSubRoot we would perhaps not need this instanceof?
        	if (containingParentObj instanceof D1SubRoot)
        	{
        		return ""+containingIndex;
        	}
			return containingParentObj.getIndexPathWithinDbSubRoot(separator)+separator+containingIndex;
		}
		return ""+containingIndex;
	}
  	
	// If separator is given as " " then this is the inverse of getObjFromPath
	// This is same as getIndexPathWithinDbSubRoot but gives the path all the way up to program wide root
	public String getIndexPath(String separator)
	{
        if (containingParentObj!=null)
		{	
        	/*if (containingIndex<0)
        	{
        		return "";
        	}*/
			return containingParentObj.getIndexPath(separator)+separator+containingIndex;
		}
		return "";
	}
	
	

	
	@Override
	public int getIndex()
	{
		return containingIndex;
	}
	
	
	public void setIndex(int index)
	{
		if (containingIndex!=-1)
		{
			// TODO this happens to the top object when we read from file. Since when saving we write the index and while reading the top object we use another index. When that is fixed somehow this can be changed to be an error.
			debug("object already has an index, "+containingIndex +"!="+ index);
		}
		else
		{
			containingIndex = index;
		}
	}


	

    // alternative, but not tested version of debugReadLock
    public void debugReadLock()
    {
    	// These code lines can be commented out when the code has been tested.
    	if (D1GlobalConfig.DEBUG_READ_LOCKS)
    	{
    		if (containingParentObj!=null)
    		{
    			containingParentObj.debugReadLock();
    		}
    	}
    }
    
    


    
    // alternative, but not tested version of debugReadLock
	@Override
    public void debugWriteLock()
    {
    	// This code can be commented out when the code has been tested.
		if (containingParentObj!=null)
		{
			containingParentObj.debugWriteLock();
		}
    }

    
    
   
    
    
	@Override
	public boolean isChanged(int rootCounter)
	{
		return false;
	}
   
	@Override
	public D1Container getContainingObj()
	{		
		return (D1Container)containingParentObj;
	}
	
	public int getParentId() 
	{
		if (containingParentObj instanceof D1IdObj)
		{
			return ((D1IdObj)containingParentObj).getId();
		}
		return -1;
	}
		
	
	// Perhaps not correct place for this method? But it will do for now.
	// deprecated, do getDbSubRoot().getGameTime() instead (faster)
	public long getGameTime()
	{		
		if (getDbSubRoot()!=null)
		{
			return getDbSubRoot().getGameTime();
		}

		return -1;
	}



	@Override
	public void linkSelf(D1Container parentObj)
	{
		// This is for debugging
		if (this.containingParentObj!=null)
		{
			// If object is already linked it shall not be linked again. 
			// Method moveToRoom shall be used instead.
			error("object already linked");
		}
		
		
		try
		{
			containingIndex=parentObj.addDbStorableToContainer(this);
			containingParentObj=parentObj;
		}
		catch (ClassCastException e)
		{
			error("could not store object "+e);
		}
	}
	
	
	// Remove this object from the super objects list
	@Override
	public void unlinkSelf()
	{
		//super.unlinkSelf(); // not needed since this is the last in this call chain.
		try
		{
			containingParentObj.removeDbStorable(this);
			//containingObj=null; // removeDbStorable will call setContainingObjIndex which will set containingObj to null and containingIndex to NO_INDEX (NO_INDEX is -1); 
		}
		catch (NullPointerException e)
		{
			debug("no parent object");
			if (containingIndex!=NO_INDEX)
			{
				error("inconsistency containingObj/containingIndex");
			}
		}
	}
	
	@Override
	public D1TickList getDbSuperRoot()
	{
		try
		{
			return containingParentObj.getDbSuperRoot();
		}
		catch (NullPointerException e)
		{
			debug("no DbSuperRoot");
		}
		
		return null;
	}


	// This is just for debugging, it is used in DbIdList.
	@Override
	public boolean isThisOrAnyParentAnIdList()
	{
		if (this instanceof D1IdList)
		{
			return true;
		}
		else if (containingParentObj==null)
		{
			return false;
		}
		else
		{
			return containingParentObj.isThisOrAnyParentAnIdList();
		}
	}

	
	@Override
	public boolean isVisibleTo(D1Base observingObj)
	{
		if (containingParentObj != null)
		{
			return containingParentObj.isVisibleTo(observingObj);
		}
		else
		{
			return super.isVisibleTo(observingObj);
		}
	}

	@Override
	public D1IdList getDbIdList()
	{
		try
		{	
			return containingParentObj.getDbIdList();
		}
		catch (NullPointerException e)
		{
			error("no DbIdList");
		}
		
		return null;
	}

	
}