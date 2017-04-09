//
//Copyright (C) 2013 Henrik Björkman www.eit.se
//
//History:
//Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;

import java.io.IOException;

import se.eit.d1_pkg.*;
import se.eit.rsb_package.BigBitMap;
import se.eit.rsb_package.Misc;
import se.eit.rsb_package.RsbBigBitMap;
import se.eit.web_package.*;



public class EmpireState extends EmpireActiveObject {
	
	final long inflationFactor=1000; // States loose 1/inflationFactor of their money each game tick.
	final long startMoney=EmpireUnitType.defaultStartMoneyForStates;
	final public static long MinPossibleAmmountOfMoney=-99999;
	
	// StateMode bit masks, state mode can be a combination of these
	final private static int PlayerState=1;
	//final private static int ComputerState=2;
	final private static int MutedPlayerState=4;

	//public long savedMoney=0;

    public String headOfState=""; // Which plater owns the state
    public BigBitMap allyList=new BigBitMap("0");
    public int homeSectorId=0; // Where the state capital is located
    public String coRuler="";

	// Money earned per game time unit
    private long moneyEarned=0; 
    
	// Money spent per game time unit
    private long moneySpent=0;
    
    //public long dailyIncome=0;
    //public long dailyExpences=0;
    
    // How much money the state made during current game tick
    private long tmpMoneyEarned=0;

    // How much money the state has spent during current game tick
    private long tmpMoneySpent=0; 
    
	protected long stateDeltaGameTime=0; // TODO: Perhaps move this to ActiveObject?
    
	private long StateMode=0;  // This is a bit mask, See PlayerState, ComputerState etc.

	//protected long previousMoneyChange=0;
	
	//BigBitMap visibleSectors=new BigBitMap("0");
	RsbBigBitMap visibleSectors=null;
	
	EmpireRoundBuffer empireRoundBuffer=null;
	EmpireMoney empireMoney=null;

	BigBitMap hostileToState=new BigBitMap("0");
	
	public long readyToTime=0;

	public String stateMotto="";
	
        // How much money the state made during current game tick
        private long tmpWorkforceAvailability=0;

        // How much money the state has spent during current game tick
        private long tmpWorkforceInUse=0; 
    
	private long workforceAvailability=0;
	private long workforceInUse=0;

	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireState.class.getSimpleName();	
	}


	public EmpireState(D1Base parent, String name) 
	{
		super();
		parent.addObject(this);
		this.regName(name);
		generateNation();
	}
	

	public EmpireState()
	{	
		super();
	}

	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		moneyEarned=wr.readLong();
		moneySpent=wr.readLong();
		headOfState=wr.readString();
		allyList=new BigBitMap(wr.readString());
		homeSectorId=wr.readInt();
		StateMode=wr.readLong();
		coRuler=wr.readString();
		hostileToState=new BigBitMap(wr.readString());
		stateMotto=wr.readString();
		workforceAvailability=wr.readLong();;
		workforceInUse=wr.readLong();;

	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeLong(moneyEarned);
		ww.writeLong(moneySpent);
		ww.writeString(headOfState);
		ww.writeString(allyList.toString());
		ww.writeInt(homeSectorId);
		ww.writeLong(StateMode);		
		ww.writeString(coRuler);
		ww.writeString(hostileToState.toString());
		ww.writeString(stateMotto);
		ww.writeLong(workforceAvailability);
		ww.writeLong(workforceInUse);

	}	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		pw.println("moneyEarned "+moneyEarned);
		pw.println("moneySpent "+moneySpent);
		pw.println("headOfState "+headOfState);
		pw.println("allyList "+allyList.toString());
		pw.println("homeSectorId "+homeSectorId);
		pw.println("StateMode "+StateMode);
		pw.println("coRuler "+coRuler);
		pw.println("hostileToState "+hostileToState.toString());
		pw.println("stateMotto "+stateMotto);
		pw.println("workforceAvailability "+workforceAvailability);
		pw.println("workforceInUse "+workforceInUse);

	}
	
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		/*if (infoName.equals("savedMoney"))
		{
			savedMoney=wr.readLong();
			return 1;
		}
		else*/ if (infoName.equals("headOfState"))
		{
			headOfState=wr.readString();
			return 1;
		}
		else if (infoName.equals("allyList"))
		{
			allyList=new BigBitMap(wr.readString());
			return 1;
		}
		else if (infoName.equals("homeSectorId"))
		{
			homeSectorId=wr.readInt();
			return 1;
		}
		else if (infoName.equals("StateMode"))
		{
			StateMode=wr.readInt();
			return 1;
		}
		else if (infoName.equals("coRuler"))
		{
			coRuler=wr.readString();
			return 1;
		}
		else if (infoName.equals("hostileToState"))
		{
			hostileToState=new BigBitMap(wr.readString());
			return 1;
		}
		else if (infoName.equals("stateMotto"))
		{
			stateMotto=wr.readString();
			return 1;
		}
		else if (infoName.equals("workforceAvailability"))
		{
			workforceAvailability=wr.readLong();
			return 1;
		}
		else if (infoName.equals("workforceInUse"))
		{
			workforceInUse=wr.readLong();
			return 1;
		}
		
				
		else
		{
		  return super.setInfo(wr, infoName);
		}
	}
	
	
	public void generateNation()
	{
		setMoney(startMoney);
		homeSectorId=-1; // no home sector yet, shall be assigned by EmpireTerrain.generate.
	}
	
	
	@Override	
	public void gameTick(long timeInGameTimeUnits)
	{
		final EmpireWorld ew=getEmpireWorld();

		stateDeltaGameTime=timeInGameTimeUnits-localTime;
		super.gameTick(timeInGameTimeUnits);
		

		// Home sector bonus. The home sector will give its owner a little extra money.
		D1IdList il=this.getDbIdList();					
		D1Base db=il.getDbIdObj(homeSectorId);
		if ((db!=null) && (db instanceof EmpireSector))
		{
			EmpireSector es=(EmpireSector)db;
			if (es.owner==this.getIndex())
			{
				// The state does own its home sector. It will receive some extra money and workers for that.
				final EmpireUnitTypeList eutl=ew.getEmpireUnitTypeList();
				long moneyBonus=eutl.getMoneyBonus();
				long personnelBonus=eutl.getPesonnelBonus();
				addEarnedMoney(moneyBonus);
				addWorkforceAvailability(personnelBonus);
			}
		}
		
		//ew.setTickTimeCallback(this);
		
		// state always need to do some cleanup after the game tick/update
		ew.setTickCleanupCallback(this);
	}
	
	public static long divRoundUp(long a, long b)
	{		
		return (a + (b-1)) / b;
	}
	
	
	// This is not public, other objects shall call addEarnedMoney
	private void setMoneyEarned(long moneyEarned)
	{
		if (this.moneyEarned!=moneyEarned)
		{
			this.moneyEarned=moneyEarned;
			setUpdateCounter();
		}
	}
	
	// This is not public, other objects shall call addSpentMoney
	private void setMoneySpent(long moneySpent)
	{
		if (this.moneySpent!=moneySpent)
		{
			this.moneySpent=moneySpent;
			setUpdateCounter();
		}
	}

	
	

	private void setWorkforceAvailability(long newWorkforceAvailability)
	{
		if (this.workforceAvailability!=newWorkforceAvailability)
		{
			this.workforceAvailability=newWorkforceAvailability;
			setUpdateCounter();
		}
	}


	private void setWorkforceInUse(long newWorkforceInUse)
	{
		if (this.workforceInUse!=newWorkforceInUse)
		{
			this.workforceInUse=newWorkforceInUse;
			setUpdateCounter();
		}
	}

	// This is to be called during the cleanup phase of a game tick/update
	@Override	
	public void gameTickCleanup()
	{

		long workforceDeficit = tmpWorkforceInUse - tmpWorkforceAvailability; 
		if (workforceDeficit>0)
		{
			// If exceeding its available work force then state must pay money for the difference.	
			addSpentMoney(workforceDeficit*EmpireUnitTypeList.workforceDeficitCostPerUnit);
		}

		
		setWorkforceAvailability(tmpWorkforceAvailability);
		setWorkforceInUse(tmpWorkforceInUse);
		tmpWorkforceAvailability=0;
		tmpWorkforceInUse=0;		

		
		
		
		setMoneyEarned(tmpMoneyEarned);
		setMoneySpent(tmpMoneySpent);			
		tmpMoneyEarned=0;
		tmpMoneySpent=0;

		long m = getMoney();


		
		for (int i=0;i<stateDeltaGameTime;i++)
		{
			m+=moneyEarned;
			m-=moneySpent;
			if (m>0)
			{
				// Apply a little inflation so that players can't save up infinite amounts of money.
				final long inflationLoss=divRoundUp(m,inflationFactor);
				m-=inflationLoss;
			}
			else if (m<0)
			{
				// Apply a little interest rate cost for states with negative money
				final long interestLoss=divRoundUp(-m,inflationFactor);
				m-=interestLoss;
				
				if (m<MinPossibleAmmountOfMoney)
				{
					m=MinPossibleAmmountOfMoney;
				}
				
			}
			
		}
		setMoney(m);


		

		
		// Set update counter? No not for moneyChange
		// this.setUpdateCounter();
		//addMoney(moneyEarned);
		//addMoney(moneySpent);
		//moneyChange=0;
	}

	// Find or create the message buffer
	public EmpireRoundBuffer findEmpireRoundBuffer()
	{
		if (empireRoundBuffer!=null)
		{
			return empireRoundBuffer;
		}
		
		int n=this.getListCapacity();
		for(int i=0;i<n;i++)
		{
			D1Base b=this.getObjFromIndex(i);
			if (b instanceof EmpireRoundBuffer)
			{
				empireRoundBuffer=(EmpireRoundBuffer)b;
				return empireRoundBuffer;
			}
		}
		empireRoundBuffer = new EmpireRoundBuffer(this, "erb"+this.getIndex());
		//this.addObject(erb);
		//erb.setUpdateCounter(); // There is a problem with setting update counter, it does not work when addObject is done. When that is fixed this line can be removed.
		return empireRoundBuffer;
	}
	

	public long getMoney()
	{
		return empireMoney.getValue();
	}
	
	public void setMoney(long m)
	{
		empireMoney.setValue(m);
	}

	public void addMoney(long m)
	{
		setMoney(getMoney()+m);
	}

	//add to money earned per game time unit
	public void addEarnedMoney(long m)
	{
		tmpMoneyEarned+=m;
		// Not calling setUpdateCounter(); since this variable is only used during game tick and is set to zero at end of tick.
	}
	
	// add to money spent per game time unit
	public void addSpentMoney(long m)
	{
		tmpMoneySpent+=m;
		// Not calling setUpdateCounter(); since this variable is only used during game tick and is set to zero at end of tick.
	}
	
	public void postMessageToThis(String str)
	{
		debugWriteLock();

		// find the round buffer and post message to it
		EmpireRoundBuffer erb = findEmpireRoundBuffer();
		erb.postMessageToThis(str);
		setUpdateCounter();
	}
	
	public void addAlly(final EmpireState empireState)
	{
		//final String str=""+empireState.getIndex();		
		//allyList=Misc.addWordToList(str, allyList);
		allyList.setBit(empireState.getIndex());
	}
	
	public boolean isAlly(final EmpireState empireState)
	{
		//final String str=""+empireState.getIndex();
		//return Misc.getWordPosInList(str , allyList)>0;
		return allyList.getBit(empireState.getIndex())>0;
	}
	
	public int rmAlly(final EmpireState empireState)
	{
		//final String str=""+empireState.getIndex();
		//int n =  Misc.getNWordInList(str , allyList);
		//if (n>0)
		//{
		//	allyList = Misc.removeWordFromList(str, allyList);
		//}
		//return n;
		if (allyList.getBit(empireState.getIndex())>0)
		{
			allyList.clrBit(empireState.getIndex());
			setUpdateCounter();
			return 1;
		}
		return 0;
	}
	
	
	public void addCoRuler(String str)
	{
		coRuler=Misc.addWordToList(str, coRuler);
	}
	

	public int rmCoRuler(String str)
	{
		int n =  Misc.getNWordInList(str , coRuler);
		if (n>0)
		{
			coRuler = Misc.removeWordFromList(str, coRuler);
		}
		return n;
	}

	public void setCanSeeSector(int sectorIndex)
	{
		findVisibleSectors().setBit(sectorIndex);			
	}	
	
	public boolean canSeeSector(int sectorIndex)
	{
		return findVisibleSectors().getBit(sectorIndex)>0?true:false;
	}

	// Find or create the message buffer
	public RsbBigBitMap findVisibleSectors()
	{
		if (visibleSectors!=null)
		{
			return visibleSectors;
		}
		
		if (this.listOfStoredObjects!=null)
		{
			for (D1Base b: this.listOfStoredObjects)
			{
				if ((b instanceof RsbBigBitMap) && (b.getName().equals("visibleSectors")))
				{
					visibleSectors=(RsbBigBitMap)b;
					return visibleSectors;
				}
			}
		}
		
		visibleSectors = new RsbBigBitMap();
		visibleSectors.linkSelf(this);
		visibleSectors.regName("visibleSectors");
	    
		return visibleSectors;
	}
	
	/*public boolean isVisibleTo(DbBase observingObj)
	{
		if (observingObj==this)
		{
			return true;
		}
		
		return false;
	}*/

	public void addEnemy(final EmpireState empireState)
	{
		//final String str=""+empireState.getIndex();		
		//allyList=Misc.addWordToList(str, allyList);
		if (hostileToState.getBit(empireState.getIndex())==0)
		{
			hostileToState.setBit(empireState.getIndex());
			setUpdateCounter();
		}
	}
	
	public boolean isEnemy(final EmpireState empireState)
	{
		//final String str=""+empireState.getIndex();
		//return Misc.getWordPosInList(str , allyList)>0;
		return hostileToState.getBit(empireState.getIndex())>0;
	}
	
	public int rmEnemy(final EmpireState empireState)
	{
		//final String str=""+empireState.getIndex();
		//int n =  Misc.getNWordInList(str , allyList);
		//if (n>0)
		//{
		//	allyList = Misc.removeWordFromList(str, allyList);
		//}
		//return n;
		if (hostileToState.getBit(empireState.getIndex())>0)
		{
			hostileToState.clrBit(empireState.getIndex());
			setUpdateCounter();
			return 1;
		}
		return 0;
	}

	
	// this states offer to otherState
	public String relationSymbol(final EmpireState otherState)
	{
		if (this==otherState)
		{
			return "-";
		}
		else if (this.isEnemy(otherState))
		{
			return "e";
		}
		else if (this.isAlly(otherState))
		{
			return "a";			
		}
		return "n";
	}

	@Override
	public void createNamedObjects()
	{
		super.createNamedObjects();

		
		// The following few lines are for debugging, can be removed or commented out later
		if (empireMoney!=null)
		{
			error("empireMoney!=null");
		}
		
		// The empire money is stored in a separate object because it is changed so frequently.
		EmpireMoney tmp = new EmpireMoney();
		tmp.linkSelf(this);
		tmp.regName("_empireMoney");
	}
	
	@Override
	public boolean regNamedChildObject(D1Named addedObject)
	{
		final String n=addedObject.getName();
		if ((addedObject instanceof EmpireMoney) && (n.equals("_empireMoney")))
		{
			
			// the following few lines are for debugging, can be removed later
			if ((empireMoney!=null) && (addedObject!=empireMoney))
			{
				error("not allowed to change this child object");
			}

			empireMoney=(EmpireMoney)addedObject;
			return true;
		}
		else
		{
			return super.regNamedChildObject(addedObject);
		}
	}
	
	
	
	static public EmpireState getEmpireStateFromName(D1SubRoot r, String n)
	{
	
		final EmpireWorld ew=(EmpireWorld)r;
		final EmpireStatesList enl=ew.getEmpireStatesList();
		EmpireState en=null;

		{
			if (WordReader.isInt(n))
			{
				final int idx = Integer.parseInt(n);
				en=(EmpireState)enl.getObjFromIndex(idx);
			}

			if (en==null)
			{
				//en=(EmpireState)enl.findGameObjNotRecursive(n);
				//DbBase db=findObjectByNameOrIndex(n);
				D1Base db=enl.findObjectByNameIndexOrId(n);
				if (db instanceof EmpireState)
				{
					en = (EmpireState)db;
				}
			}

			if (en==null)
			{
				en=(EmpireState)enl.getNationByOwner(n);
			}

			if (en==null)
			{
				D1Base d= r.findObjectByNameIndexOrId(n);
				if (d instanceof EmpireState)
				{
					en=(EmpireState)d;
				}
				else if (d instanceof EmpireUnit)
				{
					EmpireUnit eu=(EmpireUnit)d;
					final int o=eu.getOwner();
					en=(EmpireState)enl.getObjFromIndex(o);									
				}
				else if (d instanceof EmpireSector)
				{
					EmpireSector es=(EmpireSector)d;
					final int o=es.getOwner();
					en=(EmpireState)enl.getObjFromIndex(o);									
				}
			}
		}
		return en;
	}
	
	public void unitOrder(int unitId, String order, WordWriter ww) throws IOException
	{
		D1SubRoot worldBase = this.getDbSubRoot();

		if (order==null)
		{
			debug("unitOrder missing for unit: "+unitId);
			ww.writeName("TextBoxAppend");
			ww.writeString("Did not get the full order");
		}
		else
		{
			debug("unitOrder: " + unitId + " '" + order + "'");
			D1Base bo = worldBase.getDbIdObj(unitId);
			
			if (bo instanceof EmpireBase)
			{
				final EmpireBase eb=(EmpireBase)bo;
				
				// Only unit owner can give orders to it
				if (eb.getOwner() == this.getIndex())
				{
					//worldBase.debugWriteLock();
					EmpireOrder eo = new EmpireOrder();
					eo.setOrder(order);
					//eb.addObjectThreadSafe(eo); // TODO: It would be better to add new orders last, so that orders are performed in the order they are given
					eb.addOrder(eo);
					eo.regName("o"+eb.getOwner());
					debug("order id "+eo.getId());
				}
				else
				{
					ww.writeLine("Unit ~"+unitId+" is not your unit.");
				}
			}
			else
			{
				ww.writeLine("~"+unitId+" is not and empire unit.");					
			}
		}		
	}
	
	// This is called when a user have sent an order for a unit
	public void unitOrder(WordReader wr, WordWriter ww) throws IOException
	{
		D1SubRoot worldBase = this.getDbSubRoot();
		
		worldBase.lockWrite();
		try
		{
			
			final int unitId=wr.readInt();
			final String order=wr.readString();
			unitOrder(unitId, order, ww);
			
		}
		finally
		{
			worldBase.unlockWrite();
		}
	}
	
	
	public void cancelOrder(WordReader wr, WordWriter ww)
	{
		D1SubRoot worldBase = this.getDbSubRoot();
		
		final int unitId=wr.readInt();
		debug("cancelOrder: "+unitId);
		final D1Base u = worldBase.getDbIdObj(unitId);
		if ((u!=null) && (u instanceof D1Container))
		{
			D1Container co = (D1Container)u;
			worldBase.lockWrite();
			try
			{
				// iterate
				/*
				DbBase bo=u.iterateStoredObjects(null);
				while(bo!=null)
				{
					if (bo instanceof EmpireOrder)
					{
						bo.clearStored();
						bo.unlinkSelf();
						break;
					}
					bo=u.iterateStoredObjects(bo);
				}
				*/
				int n=co.getMaxIndex();
				for(int i=0;i<n;i++)
				{
					D1Base bo=u.getObjFromIndex(i);
					if (bo instanceof EmpireOrder)
					{
						debug("canceling order "+((EmpireOrder)bo).getOrder());
						bo.clearStored();
						bo.unlinkSelf();						
					}
				}
			}
			finally
			{
				worldBase.unlockWrite();
			}
		}
		else
		{
			debug("did not find unit "+unitId);
		}
	}

	
	// Returns true if the command was found and help given.
	@Override
	public boolean helpCommand(String cmd, WordReader wr, WordWriter ww, String hist) throws IOException
	{
		if ((cmd==null) || (cmd.length()==0))
		{
			ww.writeLine("Player commands:");
			ww.incIndentation();
			ww.writeLine("list : List all players/states");
			ww.writeLine("say : Say something to another player/state");
			ww.writeLine("addAlly : Add a player/state to your list of allies");
			ww.writeLine("rmAlly : Remove a player/state to your list of allies");
			ww.writeLine("giveUnit : Give a unit to another player");
			ww.writeLine("giveMoney : Give money to another player");
			ww.writeLine("setName : Change name");
			ww.writeLine("setMotto : Set a short info string about yourself");
			ww.writeLine("addEnemy : Set status to hostile towards another player/state");
			ww.writeLine("rmEnemy : Set status to friendly towards another player/state");
			ww.decIndentation();
			//super.helpCommand((String)null, wr, ww, hist);
			if (hist!=null)
			{
				ww.writeLine("");
				ww.writeLine("For more help about one sub command try:");
				ww.writeLine("  help "+hist+" <command>");
				ww.writeLine("Example:");
				ww.writeLine("  help "+hist+" say");
			}
		}
		else if (cmd.equals("say"))
		{
			ww.writeLine("Say something to another player. Receiver can be player name or state number.");							
			ww.writeLine("Usage: say <receiver> <message>");							
			ww.writeLine("  Where <receiver> is a name or number");										
		}
		else if (cmd.equals("giveUnit"))
		{
			ww.writeLine("Give something to another player. Receiver can be player name or state number.");							
			ww.writeLine("Usage: give <receiver> <unit(s)>");							
			ww.writeLine("  Where <receiver> is a name or number of the state unit is to be give to.");										
			ww.writeLine("  Where <unit(s)> is an ID or path for one or more units to be given.");										
		}
		else if (cmd.equals("setName"))
		{
			ww.writeLine("Set the name for one of your objects.");							
			ww.writeLine("Usage: setName <unit> <the new name>");							
			ww.writeLine("  Where <unit> is an ID or path for a unit.");										
			ww.writeLine("  Where <the new name> is a name");										
		}
		else if (cmd.equals("giveMoney"))
		{
			ww.writeLine("Give money to another player. Receiver can be player name or state number.");							
			ww.writeLine("Usage: give <receiver> <amount>");							
			ww.writeLine("  Where <receiver> is a name or number of the state unit is to be give to.");										
			ww.writeLine("  Where <amount> how much money to give the other player.");										
		}
		else if (cmd.equals("addAlly"))
		{
			ww.writeLine("Add a player as an ally, use state name, player name or state number.");							
			ww.writeLine("Usage: addAlly <receiver>");							
			ww.writeLine("  Where <receiver> is a name or number");							
		}
		else if (cmd.equals("rmAlly"))
		{
			ww.writeLine("Remove a player as an ally, use state name, player name or state number.");							
			ww.writeLine("Usage: rmAlly <receiver>");							
			ww.writeLine("  Where <receiver> is a name or number");							
		}		
		else if (cmd.equals("list"))
		{
			ww.writeLine("Gives a list of players");							
		}		
		else if (cmd.equals("go"))
		{
			ww.writeLine("Start the game or change the game speed.");											
			ww.writeLine("Usage: go <tick time divider>");											
			ww.writeLine("  Where <tick time divider>  is number of milliseconds per game tick");	
			ww.writeLine("  A value between 100 an 1000 ms/tick is recomended.");	
			ww.writeLine("  100 is fast, 1000 is slow.");	
			ww.writeLine("To stop the game again use the pause command.");
		}
		else if (cmd.equals("pause"))
		{
			ww.writeLine("Pauses the game. Only an op can use this command");
			ww.writeLine("To start the game again use the go command.");
		}
		else if (cmd.equals("addEnemy"))
		{
			ww.writeLine("Add a player as an enemy (start war), use state name, player name or state number.");							
			ww.writeLine("Usage: addEnemy <receiver>");							
			ww.writeLine("  Where <receiver> is a name or number");							
		}
		else if (cmd.equals("rmEnemy"))
		{
			ww.writeLine("Remove a player as an enemy (offer to end war), use state name, player name or state number.");							
			ww.writeLine("Usage: rmEnemy <receiver>");							
			ww.writeLine("  Where <receiver> is a name or number");							
		}		
		/*else if (super.helpCommand(cmd, wr, ww, hist))
		{
			// ok
		}*/
		else
		{
			ww.writeLine("Sorry, no more help on '"+cmd+"'");		
			return false;
		}
		return true;
	}
	
	
    // this interprets a message from a client	
	public boolean interpretCommand(String cmd, WordReader wr, WordWriter ww) throws IOException
	{	
	
		debug("interpretCommand cmd: '"+cmd+"'");

		
		if (cmd.equals(""))
		{
			ww.writeLine("empty line");
			return true;
		}
		
		D1SubRoot worldBase = this.getDbSubRoot();
		
		final char ch=cmd.charAt(0);
		
		switch(ch)
		{
		case 'a':
		{
			if (cmd.equals("addAlly"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Name Or Index of state to say something to.
						
						if (this.StateMode==EmpireState.MutedPlayerState)
						{
							ww.writeLine("you have been muted by op/admin, so you can't make alliances");
						}
						else
						{
							EmpireState en=getEmpireStateFromName(worldBase, n);
							if (en!=null)
							{
								this.addAlly(en);
								ww.writeLine("State "+en.getName()+" added as ally");		
								en.postMessageToThis("State " + this.getName()+" offer alliance");					
							}
							else
							{
								ww.writeLine("state "+ n +" was not found");
							}
						}
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return true;
			}
			else if (cmd.equals("addEnemy"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Name Or Index of state to say something to.
						
						EmpireState en=getEmpireStateFromName(worldBase, n);
						if (en!=null)
						{
							this.addEnemy(en);
							ww.writeLine("State "+en.getName()+" added as enemy");		
							en.postMessageToThis("State " + this.getName()+" is hostile");					
						}
						else
						{
							ww.writeLine("state "+ n +" was not found");
						}
						return true;
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
			}
			else if (cmd.equals("addCoRuler"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						final String n=wr.readWord(); // Name Or Index of state to say something to.
						
					    this.addCoRuler(n);
						ww.writeLine("PlayerData "+n+" added as coRuler");		

						return true;
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
			}
			else
			{
				return false;
			}
			break;
		}			
		case 'c':
			if (cmd.equals("cancelOrder"))
			{					
				cancelOrder(wr, ww);
				return true;
			}
		case 'g':
			if (cmd.equals("giveUnit"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						final String n=wr.readWord(); // Name Or Index of state to give something to.
						
						//EmpireStatesList enl=ew.getEmpireNationsList();
						EmpireState en=null;
						
						if ((this.StateMode & EmpireState.MutedPlayerState) !=0)
						{
							ww.writeLine("you have been muted by op/admin, so you can't give anything.");
						}
						else
						{
							en=getEmpireStateFromName(worldBase, n); // who to give something to
								
							if (en!=null)
							{
								while (wr.isOpenAndNotEnd()) // loop, there may be more than one unit to be given
								{
									final String unitId=wr.readWord();
									
									//DbBase bo = r.getDbIdObj(unitId);
									final D1Base bo= findObjectByNameIndexOrId(unitId);

									if (bo instanceof EmpireUnitOrSector)
									{
										final EmpireUnitOrSector euos=(EmpireUnitOrSector)bo;
										
										// Only unit owner can give orders to it
										if (euos.getOwner() == this.getIndex())
										{
											// You can only give units that cost money to an allied state
											if ((en.isAlly(this)) || (euos.canBeGivenAway()))
											{
												euos.setOwner(en.getIndex());
												ww.writeLine("gave "+euos.getId()+" to "+en.getName());
												en.postMessageToThis("State "+ this.getName()+" gave you ~"+euos.getId());										
											}
											else
											{
												ww.writeLine("You can only give such units to an allied state.");										
											}
										}
										else
										{
											ww.writeLine("~"+ unitId + " is not your unit");				
										}
									}
									else
									{
										ww.writeLine("~"+ unitId + " was not found or it not an empire unit or sector");											
									}
									
								}
							}
							else
							{
								ww.writeLine("receiver state "+ n +" was not found");
							}
						}
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return true;
			}	
			if (cmd.equals("giveMoney"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						final String n=wr.readWord(); // Name Or Index of state to give something to.
						
						//EmpireStatesList enl=ew.getEmpireNationsList();
						EmpireState en=null;
						
						if ((this.StateMode & EmpireState.MutedPlayerState) !=0)
						{
							ww.writeLine("you have been muted by op/admin, so you can't give anything.");
						}
						else
						{
							en=getEmpireStateFromName(worldBase, n); // who to give something to
								
							if (en!=null)
							{
								while (wr.isOpenAndNotEnd()) // loop, there may be more than one unit to be given
								{
									int a=wr.readInt();
									EmpireState es = this;

									if (es.getMoney()>=a)
									{
										es.addMoney(-a);
										en.addMoney(a);

										ww.writeLine("gave "+a+" to "+en.getName());
										en.postMessageToThis("State "+ this.getName()+" gave you "+a);										
									}
									else
									{
										ww.writeLine("you do not have that much money.");										
									}
								}
							}
							else
							{
								ww.writeLine("receiver state "+ n +" was not found");
							}
						}
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return true;
			}	
			if (cmd.equals("groupUnits"))
			{
				groupUnits(wr,ww);
			}
			break;
		case 'i':
			if (cmd.equals("id"))  // is this command still needed?
			{
				final String n=wr.readWord(); // Name Or Index
				D1Base d= findObjectByNameIndexOrId(n);
				ww.writeLine(""+d.getId());
				return true;
			}
			break;
		case 'l':
			if (cmd.equals("list"))
			{				
				final EmpireWorld ew=(EmpireWorld)worldBase;
				final EmpireStatesList enl=ew.getEmpireStatesList();
				for(D1Base db: enl.listOfStoredObjects)
				{
					final EmpireState es2=(EmpireState)db;
					//final String info=es2.toString();
					final String a1=this.relationSymbol(es2);
					final String a2=es2.relationSymbol(this);
					ww.writeLine(String.format("%2s %3d %4s  %s  %s",a1+a2, es2.getIndex(), "~"+es2.getId(), es2.getName(), es2.stateMotto));
				}
				return true;
			}
			break;
		case 'r':
		{
			if (cmd.equals("readyToTime"))
			{
				readyToTime=wr.readLong();
				return true;
			}
			else if (cmd.equals("rmAlly"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Name Or Index of state to say something to.
						
						EmpireState en=getEmpireStateFromName(worldBase, n);
						if (en!=null)
						{
							final int r=this.rmAlly(en);
							if (r>0)
							{
								ww.writeLine("State "+en.getName()+" removed as ally");		
								en.postMessageToThis("State " + this.getName()+" no longer offer alliance");
							}
							else
							{
								ww.writeLine("state "+ n +" was not an ally");		
							}
						}
						else
						{
							ww.writeLine("state "+ n +" was not found");
						}
					}
					else
					{
						final int r=this.rmAlly(null);
						ww.writeLine("Removed all from list of allies. There was "+r+".");
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return true;
			}
			else if (cmd.equals("rmEnemy"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Name Or Index of state to say something to.
						
						if (this.StateMode==EmpireState.MutedPlayerState)
						{
							ww.writeLine("you have been muted by op/admin, so you can't end hostilities");
						}
						else
						{
							EmpireState en=getEmpireStateFromName(worldBase, n);
							if (en!=null)
							{
								final int r=this.rmEnemy(en);
								if (r>0)
								{
									ww.writeLine("State "+en.getName()+" offered peace");		
									en.postMessageToThis("State " + this.getName()+" is no longer hostile");
								}
								else
								{
									ww.writeLine("state "+ n +" was not an ally");		
								}
							}
							else
							{
								ww.writeLine("state "+ n +" was not found");
							}
						}
					}
					else
					{
						final int r=this.rmAlly(null);
						ww.writeLine("Removed all from list of enemies. There was "+r+".");
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return true;
			}
			else if (cmd.equals("rmCoRuler"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Name Or Index of state to say something to.
						
						final int r=this.rmCoRuler(n);
						if (r>0)
						{
							ww.writeLine("PlayerData "+n+" removed as coRuler");		
						}
						else
						{
							ww.writeLine("PlayerData "+ n +" was not a coRuler");		
						}
					}
					else
					{
						final int r=this.rmCoRuler(null);
						ww.writeLine("Removed all from list of coRulers. There was "+r+".");
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return false;
			}
			break;
		}			
		case 's':		
			if (cmd.equals("say")) // TODO rename this to tell and add a new command say that will say to all.
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Name Or Index of state to say something to.
						String m=wr.readLine();
						
						//EmpireStatesList enl=ew.getEmpireNationsList();
						EmpireState en=null;
						
						if ((this.StateMode & EmpireState.MutedPlayerState) !=0)
						{
							ww.writeLine("you have been muted by op/admin, so you can't say anything");
						}
						else
						{
							en=getEmpireStateFromName(worldBase, n);
							
							if (en!=null)
							{
								ww.writeLine("to "+en.getName()+": '"+m+"'");		
								en.postMessageToThis("from " + this.getName()+": '"+m+"'");					
							}
							else
							{
								ww.writeLine("receiver "+ n +" was not found");					
							}
						}
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return false;
			}
			else if (cmd.equals("setName"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readWord(); // Index or ID of object to rename
						String m=wr.readLine(); // New name
						
						final D1Base d= findObjectByNameIndexOrId(n);
						
						/*if (this==null)
						{
							ww.writeLine("you are only in observation mode");
						}
						else*/
						if ((this.StateMode & EmpireState.MutedPlayerState) !=0)
						{
							ww.writeLine("you have been muted by op/admin, so you can't rename anything");
						}
						else if (!WordWriter.isStringOkAsName(m))
						{
							ww.writeLine("Name must begin with a letter and contain only permitted characters.");
						}
						else if (m.length()>32)
						{
							ww.writeLine("to long");
						}
						else if (d instanceof EmpireBase)
						{
							final EmpireBase eb=(EmpireBase)d;
							
							// Only unit owner can give orders to it
							if (eb.getOwner() == this.getIndex())
							{
								eb.regName(m);
							}
							else
							{
								ww.writeLine("Unit "+n+" is not your unit.");
							}
						}
						else
						{
							ww.writeLine("Object "+n+" was not found or is not and empire unit.");					
						}
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return false;
			}
			else if (cmd.equals("setMotto"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String m=wr.readLine();

						// Instead of code below it would also be possible to use this: 
						//String m=WordWriter.safeStr(wr.readLine(), 120);

						if (m.length()>160)
						{
							ww.writeLine("to long motto");
						}
						else if (!WordWriter.isStringOk(m, " !$¤%'()+,-./\\=?@[]{}<>|_^~*#&", 0)) // TODO: We should allow all printable unicode/utf-8 characters.
						{
							ww.writeLine("denied due to non ascii");
						}
						else
						{
							this.stateMotto=m;
							this.setUpdateCounter();
						}
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return false;
			}
			else if (cmd.equals("stateOrder"))
			{
				try
				{
					worldBase.lockWrite();

					if (wr.isOpenAndNotEnd())
					{
						String n=wr.readString();
						
						final EmpireState en=getEmpireStateFromName(worldBase, n);
						
						if (en!=null)
						{							
							if (this.isEnemy(en))
							{
								this.rmEnemy(en);
								en.postMessageToThis("State " + this.getName()+" is no longer hostile");
							}
							else
							{
								this.addEnemy(en);									
								en.postMessageToThis("State " + this.getName()+" is now hostile");
							}
							
							this.setUpdateCounter();
						}
						else
						{
							ww.writeLine("did not find state '"+n+"'");								
						}
					}
					else
					{
						helpCommand(cmd, wr, ww, null);
					}
				}
				finally
				{
					worldBase.unlockWrite();
				}
				return false;
			}
			break;
		case 't':
			if (cmd.equals("type"))
			{
				String n=wr.readWord(); // Name Or Index
				D1Base d= worldBase.findObjectByNameIndexOrId(n);
				ww.writeLine(d.getType());
				return true;
			}
			else if (cmd.equals("ts")) // ToString
			{
				String n=wr.readWord(); // Name Or Index
				D1Base d= worldBase.findObjectByNameIndexOrId(n);
				ww.writeLine(d.toString());
				return true;
			}
			break;
		
		case 'u':
			if (cmd.equals("unitOrder"))
			{					
				unitOrder(wr, ww);
				return true;
			}
			break;
			
		default:
			{
				break;
			}									
		}
		
		if (super.interpretCommand(cmd, wr, ww))
		{
			return true;
		}
		
		return false;
	}
	
	
 	// To be called from empire unit during tick (alias update) to set how much workforce the cities (owned by this state) make available
 	public void addWorkforceAvailability(long wf)
	{
		tmpWorkforceAvailability+=wf;
		// this is a temporary variable (not saved to disk or sent to client) so not needing to set update counter
	}

	// To be called from empire units during tick (alias update) to set how much workforce is used by the state
	public void addWorkforceUsage(long workForceProvided)
	{
		tmpWorkforceInUse+=workForceProvided;
	}

	public long getMoneyEarned()
	{
		return moneyEarned;
	}


 	public long getWorkforceInUse() {
		return workforceInUse;
	}


	public long getWorkforceAvailability() {
		return workforceAvailability;
	}

	private D1List<EmpireUnitOrSector> parseUnitList(WordReader wr)
	{
		D1IdList il=this.getDbIdList();					
		D1List<EmpireUnitOrSector> unitList=new D1List<EmpireUnitOrSector>();

		while(wr.isOpenAndNotEnd())
		{
			int id=wr.readInt();
			D1IdObj io=il.getDbIdObj(id);
			debug("merge ~"+id);
			if (io instanceof EmpireUnitOrSector)
			{
				EmpireUnitOrSector euos=(EmpireUnitOrSector)io;
				if (euos.owner==this.owner)
				{
					unitList.add(euos);
				}
				else
				{
					tellOwner("Not your unit ~"+ id);							
				}
			}
		}
		return unitList;
	}
	
	// This is intended to replace mergeGroup
	// This is called when a user have sent an order to group units
	public void groupUnits(WordReader wr, WordWriter ww) throws IOException
	{
		D1SubRoot worldBase = this.getDbSubRoot();
		
		worldBase.lockWrite();
		try
		{
			final EmpireWorld ew=getEmpireWorld();
			final EmpireUnitTypeList eutl=ew.getEmpireUnitTypeList();

			D1List<EmpireUnitOrSector> unitList=parseUnitList(wr);
			
			if (unitList.size()<=1)
			{
				//tellOwner("need at least two units to make a group");
				ww.writeLine("need at least two units to make a group");
			}
			else
			{
				D1IdObj groupParent=eutl.findHeaviestUnit(unitList);
				
				if (groupParent==null)
				{
					ww.writeLine("need one unit as parent of the group");	
				}
				else
				{
					// Now move all units to target
					for(int i=0;i<unitList.getCapacity();i++)
					{
						EmpireUnitOrSector euos=unitList.get(i);
						if ((euos!=null) && (euos instanceof EmpireUnit) && (euos!=groupParent))
						{
							String order="goTo "+groupParent.getId();
							unitOrder(euos.getId(), order, ww);
						}
					}
				}
			}
		}		
		finally
		{
			worldBase.unlockWrite();
		}
	}

	public boolean getPlayerState()
	{
		return (StateMode&EmpireState.PlayerState) != 0;
	}

	public void setPlayerState()
	{
		StateMode|=EmpireState.PlayerState;
	}
	
	public void mutePlayer()
	{
		StateMode|=EmpireState.MutedPlayerState;		
	}

	public boolean isMutedPlayer()
	{
		return (StateMode&EmpireState.MutedPlayerState) !=0;		
	}

	public void unMutePlayer()
	{
		StateMode&=~EmpireState.MutedPlayerState;		
	}

	public long getReadyToTime()
	{
		return this.readyToTime;
	}
}
