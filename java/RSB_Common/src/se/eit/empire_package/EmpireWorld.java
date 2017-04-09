// EmpireWorld.java
//
// Copyright (C) 2013 Henrik Björkman www.eit.se
//
// History:
// Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;

import java.util.LinkedList;
import java.util.Queue;



//import se.eit.rsb_package.*;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;


public class EmpireWorld extends EmpireActiveList {
	
    final static int INFINITE_MOVE_COST=0x7FFFFFFF;
    final static int N_SECTOR_NEIGHBORS_1=6;
    final static int N_SECTOR_NEIGHBORS_2=18;
    final static int N_SECTOR_NEIGHBORS_3=36;
    final static int SAME_SECTOR=-1;
    final static int maxNations = 64;
    final static int ETERNITY = 0x7FFFFFFF;
    
    final static long TickWorldSpeed=1000; // Milliseconds between tick in game, tick is not always same thing as incrementing gameTime
    
    // Maximum game time increment in one tick/update
	final long maxTimeIncrement=100;

	// TODO Is the comment here correct? 
	// milliseconds passed, when this reach the value given by gameSpeed then gameTime is incremented by one. 
	long gameTimeMsCounter=0; 
	
	// This is used to know how much time has passed since previous tick (previous call of tickMsCallback)
	long tickTimeMs=0;

	int tickState; // This feature is currently not used, unless it gets used this variable should be removed.

	// Game starts paused, the player who created the game must issue a "go" command. This will tell how many milliseconds of real time per game time unit.
	public int gameSpeed=0; 
	
	// milliseconds passed, when this reaches TickWorldSpeed an update (empire tick) is done, its not always same thing as incrementing gameTime.
	protected long gameTickMsCounter=0; 

	
	//Queue<EmpireBase> timeQueue = new LinkedList<EmpireBase>();
	Queue<EmpireBase> performOrdersQueue = new LinkedList<EmpireBase>();
	Queue<EmpireBase> interactQueue = new LinkedList<EmpireBase>();
	Queue<EmpireBase> cleanupQueue = new LinkedList<EmpireBase>();

	EmpireStatesList empireStatesList;
	EmpireTerrain empireTerrain;
	EmpireUnitTypeList empireUnitTypeList;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireWorld.class.getSimpleName();	
	}


	public EmpireWorld(D1Base parent, String name, String createdBy) 
	{
		super();
		parent.addObject(this);
		this.regName(name);
		this.createdBy=createdBy;
	}

	public EmpireWorld()
	{	
		super();
	}

	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		gameTimeMsCounter=wr.readLong();
		tickState=wr.readInt();
		gameSpeed=wr.readInt();
		//gameTime=wr.readInt();
	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeLong(gameTimeMsCounter);
		ww.writeInt(tickState);
		ww.writeInt(gameSpeed);
		//ww.writeInt(gameTime);
	}
	
	@Override
	public void listInfo(WordWriter ww)
	{
		super.listInfo(ww);
		ww.println("gameTimeMsCounter "+gameTimeMsCounter);		
		ww.println("tickState "+tickState);
		ww.println("gameSpeed "+gameSpeed);
		//ww.println(prefix+"gameTime "+gameTime);
	}
	
	// returns 1 if info to set was found, -1 if not found.
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("gameSpeed"))
		{
			gameSpeed=wr.readInt();
			return 1;
		}
		return super.setInfo(wr, infoName);
	}

	@Override
	public void generateSelf()
	{
		error("need parameters");
	}
	
	// Generate a new world
	public void generateSelf(int nStates, int size)
	{
        try
        {
        	this.lockWrite();
        	
        	//findGameTime();
        	
        	empireUnitTypeList = new EmpireUnitTypeList(this, "TypesList");

        	empireStatesList = new EmpireStatesList(this, "StatesList", nStates);
	
		    empireTerrain = new EmpireTerrain(this, "Terrain", size, nStates);
			
        }
        finally
        {
        	this.unlockWrite();
        }
	}
	
	public EmpireUnitTypeList getEmpireUnitTypeList()
	{
		if (empireUnitTypeList==null)
		{	
			if (listOfStoredObjects!=null)
			{		
				for (D1Storable s : this.listOfStoredObjects)
				{
					if (s instanceof EmpireUnitTypeList)
					{
						empireUnitTypeList=(EmpireUnitTypeList)s;
					}
				}
			}
		}
		
		return empireUnitTypeList;
	}

	
	public EmpireTerrain getEmpireTerrain()
	{
		if (empireTerrain==null)
		{	
			if (listOfStoredObjects!=null)
			{		
				for (D1Storable s : this.listOfStoredObjects)
				{
					if (s instanceof EmpireTerrain)
					{
						empireTerrain=(EmpireTerrain)s;
					}
				}
			}
		}
		
		return empireTerrain;
	}
	
	
	public EmpireStatesList getEmpireStatesList()
	{
		// TODO use reserved object name (a name starting on _) instead and of looping and looking for it here.
		if (empireStatesList==null)
		{	
			if (listOfStoredObjects!=null)
			{		
				for (D1Storable s : this.listOfStoredObjects)
				{
					if (s instanceof EmpireStatesList)
					{
						empireStatesList=(EmpireStatesList)s;
					}
				}
			}
		}
		
		return empireStatesList;
	}

	/*
	protected void doEconomyAndMovement()
	{
		if (timeQueue.isEmpty())
		{
			// time queue is empty, perhaps units have not yet registered for tick, this gives them a chance to to so.
			final int n=this.getDbIdListLength();
			// Loop over all ID objects in the database for this game
			for (int i=0; i<n; i++)
			{
				final DbIdObj io=this.getDbIdObj(i);
				if (io instanceof EmpireBase)
				{
					EmpireBase eb = (EmpireBase)io;
					eb.gameTick(gameTime);
				}
			}			
		}
		else
		{
			// Hopefully we only need to do tick on the units in our queue, this will save CPU time.
			Queue<EmpireBase> tmpTimeQueue = timeQueue;
			timeQueue=new LinkedList<EmpireBase>(); // new queue since units will re register for next tick during this tick (don't want eternal loop here)

	        while (!tmpTimeQueue.isEmpty()) {
	        	EmpireBase eb = tmpTimeQueue.remove();
	        	eb.gameTick(gameTime);
	        }
		}
			
	}
	*/
	
	// Process the list of orders.
	// Units that have an order to be performed will register themselves in 
	// the performOrdersQueue during tick. 
	protected void doOrders()
	{
        while (!performOrdersQueue.isEmpty()) {
        	EmpireBase eb = performOrdersQueue.remove();
			eb.gameTickPerformOrders();
        }
	}
	
	// During doOrders some units will affect other units
	protected void doInteract()
	{
		/*
		final int n=this.getDbIdListLength();
		
		for (int i=0; i<n; i++)
		{
			final DbIdObj io=this.getDbIdObj(i);
			if (io instanceof EmpireBase)
			{
				EmpireBase eb = (EmpireBase)io;
				eb.gameTickInteract();
			}
		}	
		*/
        while (!interactQueue.isEmpty()) {
        	EmpireBase eb = interactQueue.remove();
			eb.gameTickInteract();
        }
	}
	
	protected void doCleanup()
	{
		/*
		final int n=this.getDbIdListLength();
		
		for (int i=0; i<n; i++)
		{
			final DbIdObj io=this.getDbIdObj(i);
			if (io instanceof EmpireBase)
			{
				EmpireBase eb = (EmpireBase)io;
				eb.gameTickCleanup();
			}
		}	
		*/
        while (!cleanupQueue.isEmpty()) {
        	EmpireBase eb = cleanupQueue.remove();
			eb.gameTickCleanup();
        }
	}

	// gameTimeDiff is in game time not a real world time unit as milliseconds or seconds
	// the game tick is divided into some sub phases since all units shall be updated equally (not affected by the order in which they are listed in the active unit list)
	// So in first phase it will update its economy and other counters such as move points
	// In next phase units perform their orders such as moving 
	// Then after all units have been moved they can interact such as shooting at other units.
	// Units that have been hit are taken care of after all units have had a chance to get their shots in the cleanup phase.
	public void tickGame(long gameTimeDiff)
	{
		final long timeInGameTimeUnits=getGameTime();
		switch(tickState)
		{
			/*case 0:
			{
				tickActiveObjects(timeInGameTimeUnits);
				doOrders();
				doInteract();
				doCleanup();
				tickState++;
				break;
			}*/
			default:
			{
				tickActiveObjects(timeInGameTimeUnits);
				doOrders();
				doInteract();
				doCleanup();
				tickState=0;
				break;
			}
		}

	}
	
	// deltaMs is how many milliseconds that has passed since previous call.
	public void tickGameMs(int deltaMs)
	{
		//super.tickMs(deltaMs);
		// Game is not paused and there are players connected (notificationDataList is not empty)
				
		gameTimeMsCounter+=deltaMs;
		
		gameTickMsCounter+=deltaMs;
		if (gameTickMsCounter>=TickWorldSpeed)
		{
			// It is time to process an empire tick (happens typically every 1000 ms or so)
			gameTickMsCounter-=TickWorldSpeed;			
			
			lockWrite(); 
			try
			{
				final long gt=getGameTime();
				long allStatesReadyTime=allStatesReady();
				long d=Math.min(allStatesReadyTime-gt, maxTimeIncrement);
				if (d>0)
				{
					addGameTime(d);
					tickGame(maxTimeIncrement);					
				}
				else
				{
					// How much we increment game time per game tick
					final long gameTimeDiff=Math.min(gameTimeMsCounter/gameSpeed, maxTimeIncrement);
					if (gameTimeDiff>0)
					{
						addGameTime(gameTimeDiff);
						gameTimeMsCounter-=gameSpeed*gameTimeDiff;				
					}
					tickGame(gameTimeDiff);
				}
				
			}
			finally
			{
				unlockWrite();
			}		
		}
		//setUpdateCounter(); // With this all of this object would be sent to all clients.
	}

	
	public void setGameSpeed(int gameSpeed)
	{
		this.gameSpeed=gameSpeed;
		this.setUpdateCounter(); // TODO: This is in the root object for the game, it might not work with update counter since its unclear what ID to use. Shall it use ID it would have in the database it is placed or shall it have a fixed value of say 0? Or shall we avoid putting anything that changes in the root object?

		/*final EmpireNationsList enl = this.getEmpireNationsList();

		enl.postMessage("game speed "+gameSpeed);*/

	}
	
		
	// Units that have orders to be performed shall register themselves in this list
	public void setTickPerformOrdersQueueCallback(EmpireBase eb)
	{
		performOrdersQueue.add(eb);
	}
	
	// If an order involves interaction with other usits such as hitting them then they shall register in this list during the do orders phase of a gie tick.
	public void setTickInteractCallback(EmpireBase eb)
	{
		interactQueue.add(eb);
	}
	
	// Units that need a tick call during final (called cleanup) phase of a game tick update shall register by calling this.
	public void setTickCleanupCallback(EmpireBase eb)
	{
		cleanupQueue.add(eb);
	}

	// Override of a method from D1TickReceiver, this is the callback from D1TickList, 
	@Override
	public void tickMsCallback(long tickTimeMs)
	{
		final long deltaTimeMs = tickTimeMs - this.tickTimeMs;
		this.tickTimeMs += deltaTimeMs;
		
		// Progress time only if game is not paused and there are players connected
		if ((gameSpeed!=0) && (notificationDataList.size()!=0))
		{
			tickGameMs((int)deltaTimeMs);
		}
	}



	/*
	// Find or create the money object
	public RsbLong findGameTime()
	{
		if (gameTime!=null)
		{
			return gameTime;
		}
		
		if (this.listOfStoredObjects!=null)
		{
			for (DbBase b: this.listOfStoredObjects)
			{
				if ((b instanceof RsbLong) && (b.getName().equals("gameTime")))
				{
					gameTime=(RsbLong)b;
					return gameTime;
				}
			}
		}
		
		gameTime = new RsbLong();
		gameTime.setName("gameTime");
		gameTime.linkSelf(this);
	    
		return gameTime;
	}

	public long getGameTime()
	{
		return findGameTime().getValue();
	}
	
	public void setGameTime(long m)
	{
		findGameTime().setValue(m);
	}

	public void addGameTime(long m)
	{
		setGameTime(getGameTime()+m);
	}
	*/
	
	// This returns the name of the server object that clients shall use to play this world.
	@Override
	public String serverForThisWorld()
	{
		return "EmpireServer"; // The name used needs to match that in serverFactory
	}
	
	public long allStatesReady()
	{
		// check with all states if they are readyToTime if so move time faster
		long allStatesReadyTime=Long.MAX_VALUE;
		
		EmpireStatesList esl=getEmpireStatesList();
		
		if (esl.listOfStoredObjects!=null)
		{		
			for (D1Storable s : esl.listOfStoredObjects)
			{
				if (s instanceof EmpireState)
				{
					EmpireState es=(EmpireState)s;
					
					if (es.getPlayerState())
					{					
						long t=es.getReadyToTime();
						if (t<allStatesReadyTime)
						{
							allStatesReadyTime=t;
						}
					}
				}
			}
		}

		return allStatesReadyTime;
	}
}