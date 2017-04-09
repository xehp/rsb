// DbTickList.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;






// This class works in pair with DbTickReceiver
// Typically there is only one DbTickList in the very root of the database system, under it there can be several DbSubRoots (those can be extensions of DbTickReceiver). 
// DbSubRoot objects shall register with this list if they need ticks. But other objects, like those stored in a database shall use ActiveObjectList.
// The reason for having two similar but not same classes for this is that objects inside a DbSubRoot get their ticks from the DbSubRoot so that the DbSubRoot can stop ticks if no players are connected for example.



public class D1TickList extends D1SubRoot {

	private int desired_frame_rate_ms = 1000; // in ms
	static final int MaintenanceTimeIntervallMs = 5000;
	
	public D1List<D1TickReceiver> tickList=new D1List<D1TickReceiver>();
	public D1List<D1TickReceiver> tickListMaintenance=new D1List<D1TickReceiver>();
	
	D1GlobalConfig globalConfig=null;
	
	int maintenanceCount=0;
	long nextMaintenance=0;
	
	public D1TickList()
	{	
		super();
	}

	
	public D1TickList(String name, D1GlobalConfig globalConfig)
	{
		super();
		this.setName(name); // TODO can we use regName instead?
		this.globalConfig = globalConfig;
	}
	
	// Using classes shall call this to subscribe to ticks.
	// Returns a reference to be used to cancel periodic tick
	public int addPeriodicTick(D1TickReceiver tickReceiver)
	{
		debugWriteLock();
		
		return tickList.add(tickReceiver);
	}
	
	// Called by using classes if they need more frequent ticks than default.
	public void requestPeriodicTickInterval(int desiredIntervalMs)
	{
		if (desiredIntervalMs<desired_frame_rate_ms)
		{
			debug("tick rate now at "+desiredIntervalMs + " ms");
			desired_frame_rate_ms = desiredIntervalMs;
		}	
	}
	
	// Using classes shall call this to cancel ticks, such as when object is unlinked.
	public void removePeriodicTick(int i)
	{
		debugWriteLock();
		if (i>=0)
		{
			tickList.remove(i);
		}
		else
		{
			debug("ignored remove on "+i);
		}
	}
	
	// This is to be called by the tick thread (TickThread) at hopefully regular intervals.
	// It will in turn tick all subscribers.
	public void tickMsSuper(long tickTimeMs)
	{
		for (D1TickReceiver s : tickList)
		{
			s.tickMsCallback(tickTimeMs);
		}
		
		// Maintenance tasks here, one DbTickReceiver gets to do maintenance once every 1 second or so (see MaintenanceTimeIntervallMs).
		final long timeToNextMaintenance = nextMaintenance - tickTimeMs;
		if (timeToNextMaintenance<0)
		{
			
			if (maintenanceCount>=tickListMaintenance.getCapacity())
			{
				maintenanceCount=0;
			}
			D1TickReceiver m = tickListMaintenance.get(maintenanceCount);
			if (m!=null)
			{
				m.tickMsCallbackMaintenance(tickTimeMs);
			}
			++maintenanceCount;
			nextMaintenance = tickTimeMs + MaintenanceTimeIntervallMs; // in ms
		}
		
	}
	
	/*
	public void tickSlowSuper(GlobalConfig config)
	{
		for (DbTickReceiver s : tickList)
		{
			s.tickGameSlow(config);
		}		
	}
	*/
	
	/*
	public void tickMsSuper(long tickTimeMs)
	{
		Iterator<DbTickReceiver> i=tickList.iterator();
		
		while (i.hasNext())
		{
			DbTickReceiver dtr = i.next();
		
			dtr.tickMsCallback(tickTimeMs);
		}
	}
	*/
	
	@Override
	public D1TickList getDbSuperRoot()
	{
		return this;
	}


	@Override
	public D1GlobalConfig getGlobalConfig()
	{
		return globalConfig;
	}

	public int get_desired_frame_rate_ms()
	{
		return desired_frame_rate_ms;
	}

	
	// Using classes shall call this to subscribe to maintenance ticks.
	// Returns a reference to be used to cancel maintenance tick
	public int addMaintenanceTick(D1TickReceiver tickReceiver)
	{
		debugWriteLock();
		
		return tickListMaintenance.add(tickReceiver);
	}
	
	// Using classes shall call this to cancel maintenance ticks, such as when object is unlinked.
	public void removeMaintenanceTick(int i)
	{
		debugWriteLock();
		if (i>=0)
		{
			tickListMaintenance.remove(i);
		}
		else
		{
			debug("ignored remove on "+i);
		}
	}
	
	
}

