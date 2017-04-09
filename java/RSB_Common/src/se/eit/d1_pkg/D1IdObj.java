// DbIdObj.java
//
// Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;

import se.eit.web_package.*;


public class D1IdObj  extends D1Named 
{
    public static final boolean useId=true;
	
	private int id=-1;
	public D1IdList idList=null;
	
	
	
	public D1IdObj()
	{
		super();
	}
	
	
	@Override
	public int getId()
	{
		// TODO: Currently we assign ID when loading but we could defer doing it until one is asked for. That is we could do it here...
		/*
		if (id<0)
		{
			id=this.idList.addIdObj(this);
		}
		*/
		
		return id;
	}
	
	
	// to be called from DbIdList only.
	public void setIdFromDbIdList(int id, D1IdList idList)
	{
		// Just for debugging, this if can be removed later
		if ((this.id!=-1) && (this.id!=id) && (id!=-1))
		{
			error("already have id "+this.id+" "+id);
		}
		
		this.id=id;
		this.idList=idList;
	}
	

	@Override
	public void readSelf(WordReader wr)
	{
		super.readSelf(wr);
		
		id=wr.readInt();
	}

	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(id);	
	}
	
	
	// The object is linked into a hierarchy of objects. It is also assigned an ID.
	@Override
	public void linkSelf(D1Container parentObj)
	{		
		// A problem here, base class DbChangedCounter needs this.idList (it uses getDbSubRoot) but is called (in super.linkSelf) before it is set here. One solution is to change the order of base classes so that this class i base for DbChangedCounter instead. Or we could move the line "this.idList=getDbIdList(parentObj);" to before "super.linkSelf(parentObj);"
		
		super.linkSelf(parentObj);
		
		if ((this.idList==null) || (id==-1))
		{	
			// Find our DbIdList and register our selves in it.
			
			if (this instanceof D1IdList)
			{
				// The object is an IdList itself
				this.idList=(D1IdList)this;
			}
			else
			{
				// Get the Id list of parent
				this.idList=parentObj.getDbIdList();
			}
			
			try
			{
				id=this.idList.addIdObj(this);
			}
			catch (NullPointerException e)
			{
				// This can happen for PlayerData UserAccount since that is an object that is not an IdList and its not stored in one either
				if (this.needIdList())
				{
					debug("linkSelf no idList");
				}
			}
		}
		else
		{
			// this should be registered in the IdList already so do nothing.
			
			// This is for debugging only, can be commented out when code works perfectly.
			if (this.idList.getDbIdObj(id)!=this)
			{
				error("it was not registered in idList");
			}
		}
	}


	

	
	
	/*
	@Override
	public void writeIdOrPath(WordWriter ww)
	{
		if (useId)
		{
			ww.writeInt(getId());
		}
		else
		{
			ww.writeString(getIndexPathWithinDbSubRoot(" "));
		}
	}
	*/
	
	// This is deprecated, use getNameAndId instead
	/*
	public String getIdAndName()
	{
		final String name=getName();
		if (name!=null)
		{
			return "~"+getId()+":"+getName();
		}
		return "~"+getId();
	}
	*/

	// For debugging it is easier if both name and ID is displayed. 
	@Override
	public String getNameAndId()
	{
		final String name=getName();
		if (name!=null)
		{
			final int id = getId();
			if (id>=0)
			{
				return getName()+"~"+getId();
			}
			return getName()+"~";
		}
		return "null~"+getId();
	}
	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);					
		pw.println("id"+" "+getId());		
	}
	
	@Override
	public D1IdList getDbIdList()
	{
		return idList;		
	}
	
	// unlink alias delete or unregister
	// probably we should also do the same with all objects stored in this object.
	// and this method and clear should be the same.
	@Override
	public void unlinkSelf()
	{
		// Small problem here, if we do idList.unregDbIdObj before super.unlinkSelf then notify changes will not work since that is done in unlinkSelf but needs to know which ID list it is in. Will try doing unlinkSelf before unregDbIdObj...  
		super.unlinkSelf();
		if (idList==null)
		{
			// TODO: This happens, it should not, do not know why yet. But it happens when gameTickCleanup deletes a NothingType in empire. 
			debug("idList was null, "+this.toString());
			//throw(new NullPointerException("idList was null, "+this.toString()));
		}
		else
		{
			idList.unregDbIdObj(this);
			idList=null;
		}
	}

	
	@Override
	public D1SubRoot getDbSubRoot()
	{
		return idList;
	}
	

	// Get reference to object using its ID
	public D1IdObj getDbIdObj(int id)
	{
		return idList.getDbIdObj(id);
	}
	
	
	/*
	// Make sure the object and all its child objects has been assigned IDs.
	// This is not tested
	@Override
	public void assignIdRecursively()
	{
			if (id<0)
			{
				idList.addIdObj(this);
			}
			
			
			// Are there any child objects?
			if (listOfStoredObjects!=null)
			{
				for (int i=0; i< listOfStoredObjects.getCapacity(); i++)
				{
					DbStorable s = listOfStoredObjects.get(i);
					if (s!=null)
					{
						s.assignIdRecursively();			
					}
				}
			}
			
		
	}
	*/

	


	public void debugReadLock()
	{
		final D1SubRoot r = this.getDbSubRoot();
		r.debugReadLock();
	}

    @Override
	public void lockWrite()
	{
		getDbSubRoot().lockWrite();
	}

    @Override
	public void unlockWrite()
	{
		getDbSubRoot().unlockWrite();
	}

    @Override
	public void lockRead()
	{
		getDbSubRoot().lockRead();
	}

    @Override
	public void unlockRead()
	{
		getDbSubRoot().unlockRead();
	}

    @Override
    public boolean isPartOfSubRoot(D1Base subRoot)
    {
    	if (idList==subRoot)
    	{
    		return true;
    	}
    	return false;
    }

}

