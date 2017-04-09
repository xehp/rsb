// DbChangedCounter.java
//
//Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
//License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;

import se.eit.web_package.*;



public abstract class D1ChangedCounter extends D1IdObj {

	private int counter=0; // This class maintains its own changed counter.



	public D1ChangedCounter()
	{	
		super();	
	}	
	
	
	public void linkSelf(D1Container db)
	{
		super.linkSelf(db);
		setUpdateCounter();
	}
	
	public void unlinkSelf()
	{
		setUpdateCounter();
		super.unlinkSelf();
	}

	
	// To be called when something here has been changed. It shall be set only if something was changed.
	@Override
	public void setUpdateCounter()
	{
		D1SubRoot r=this.getDbSubRoot();
		if (r instanceof D1NotificationSender)
		{
			//r.debugWriteLock(); // TODO: This line should not need to be commented out. The RootUpdateCounter is only incremented when DB is locked. So we might end up setting same value as before and no update will get sent to clients. Added debugWriteLock to detect this but its commented out for now due to other problems.
			int rootCounter=r.getDbSubRootUpdateCounter(); // is it OK to use getUpdateCounter also outside tick or should we set a local flag end then do this in our tick method? Will do it quick and dirty for now.
			if (counter!=rootCounter)
			{
				counter=rootCounter;
				
				// TODO This solution is inefficient. Receivers will need to store this in linked queues, it is a costly operation to add to linked queues, it is done for all connected clients and its done with database write locked. A better way is to use a simple round buffer within the database. Only one round buffer is needed, clients only need a counter each so they know how far they have read in the round buffer. Clients can have their get changes from it while database is only read locked.
				D1NotificationSender ns = (D1NotificationSender)r;
				ns.notifySubscribers(this.getId());
			}
		}
		else
		{
			//debug("no root obj, could not set update counter"); 
		}
	}
	
	
	// Returns true if the properties of this class has been update since value given by rootCounter.  
	@Override
	public boolean isChanged(int previousUpdateCounter)
	{
		final int d=this.counter-previousUpdateCounter;
		if (d>0)
		{
			return true;
		}
		return false;
	}
	
	
	
	// previousUpdateCounter shall be the value root counter had when object update was sent previous time
	// If a class adds its own listChangedObjects then a parse for the class name needs to be added in ClientThread where it interprets the "updatePartly" message.
	// Format here must match that used in ClientThread.updateObj. If this method is changed or moved, remember to update there also.
	@Override
	public void listChangedObjects(int previousUpdateCounter, WordWriter ww)
	{
		
		if (isChanged(previousUpdateCounter))
		{
			// Command to client
			ww.writeWord("updateObj");
			
			// The type of the object to update
			ww.writeWord(this.getType());

			// id of this object
			//this.writeIdOrPath(ww);
			ww.writeInt(getId());
			
			// id of the objects super object (the room that this object is residing in).
			D1Base so=getContainingObj();
			if (so!=null)
			{
				//so.writeIdOrPath(ww);
				ww.writeInt(so.getId());
			}
			else
			{
				ww.writeString("");
			}

			
			// All data for this object (not including sub objects)
			this.writeSelf(ww);
			
			// End of line
			ww.writeLine("");
			
			ww.flush();
		}

		super.listChangedObjects(previousUpdateCounter, ww);
	}

	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		
		pw.println("counter"+" "+counter);
		
	}

	public int getDbChangedCounter()
	{
		return counter;
	}

	public void notifyOthers()
	{
        final D1SubRoot dr = getDbSubRoot();
        final D1NotificationSender ns = (D1NotificationSender)dr;
        ns.notifySubscribers(this.getId());
	}

}
