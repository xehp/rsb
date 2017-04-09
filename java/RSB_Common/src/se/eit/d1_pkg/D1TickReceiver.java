// DbTickReceiver.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 


package se.eit.d1_pkg;




// This class works in pair with DbTickList, for documentation see comment in the beginning of that class.

public class D1TickReceiver extends D1SubRoot {
	int tickRef=-1;
	int tickRefMaintenance=-1;

	long lastSaveTime=0;
	
	@Override
	public void linkSelf(D1Container parentObj)
	{
		super.linkSelf(parentObj);
		D1TickList dtl=this.getDbSuperRoot();
		if (dtl==null)
		{
			error("no super root");
		}
		else
		{
			dtl.lockWrite();
			try
			{
				tickRefMaintenance = dtl.addMaintenanceTick(this);
			}
			finally
			{
				dtl.unlockWrite();
			}
		}
	}

	@Override
	public void unlinkSelf()
	{
		cancelTick();
		if (tickRefMaintenance!=-1)
		{
			D1TickList dsr=this.getDbSuperRoot();
			dsr.lockWrite();
			try
			{
				dsr.removeMaintenanceTick(tickRefMaintenance);
			}
			finally
			{
				dsr.unlockWrite();
				tickRef=-1;
			}
		}				
		super.unlinkSelf();		
	}

	// This will be called every game tick if tick has been requested (call requestTick).
	// by default do error, extending classes shall override this method if the do request tick.
	public void tickMsCallback(long tickTimeMs) 
	{
		error("useless tick request");
	}


	public void requestTick(int desiredIntervalMs)
	{
		D1TickList dsr=this.getDbSuperRoot();
		if (tickRef==-1)
		{
			if (dsr!=null)
			{
				dsr.lockWrite();
				try
				{
					tickRef = dsr.addPeriodicTick(this);
				}
				finally
				{
					dsr.unlockWrite();
				}
			}
		}
		else
		{
			debug("ticks are already requested "+tickRef);
		}
		dsr.requestPeriodicTickInterval(desiredIntervalMs);
	}
	
	public void cancelTick()
	{
		if (tickRef!=-1)
		{
			D1TickList dsr=this.getDbSuperRoot();
			dsr.lockWrite();
			try
			{
				dsr.removePeriodicTick(tickRef);
			}
			finally
			{
				dsr.unlockWrite();
				tickRef=-1;
			}
		}		
	}
	
	// This is called less frequently than tickMsCallback.
	// Extending classes can override this if they have additional maintenance tasks to perform or wish to do them differently.
	public void tickMsCallbackMaintenance(long tickTimeMs)
	{
		saveSelf();
	}
	
	
}
