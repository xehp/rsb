//
//Copyright (C) 2013 Henrik Bjorkman www.eit.se
//
//History:
//Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;

import se.eit.d1_pkg.*;
import se.eit.web_package.*;




public class EmpireUnit extends EmpireActiveObject {
	
	static final long MinimumMoneyRequiredForUnitsHealthRecoveryFactor = -100;
	static final long MinimumMoneyRequiredToMoveUnitsFactor = -1000;
	static final long MinMoneyForMaintainingUnitsFactor = -10;

	static final long dismissFactor=1000; // How often a worker is dismissed if state has to many, higher value here for less often. For example 10 here will give one scrapped roughly every 10 game time unit.
	
	/*class typeProperties
	{
	}*/
	
	
	

    public int unitHealth;
	public int unitType;
	public int attackPoints;   // needed for unit to attack enemy units. 
    public int movePoints;     // needed for unit to move.
    

    
    // temporary variable, not saved in file, it is only used during a tick
    //public int receivedHits;
    
    //typeProperties types[]=new typeProperties[10];
    
    
    

	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireUnit.class.getSimpleName();	
	}


	public EmpireUnit(D1Base parent, String name) 
	{
		super();
		parent.addObject(this);
		this.regName(name);

		generateUnit();
		
		// will this work, will units have full parent hierarchy here?
		/*if (parent!=null)
		{
			final EmpireWorld ew=getEmpireWorld();
			//ew.setTickTimeCallback(this);
		}*/
	}

	public EmpireUnit()
	{	
		super();
	}

	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		unitHealth=wr.readInt();
		unitType=wr.readInt();
		attackPoints=wr.readInt();
		movePoints=wr.readInt();
	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(unitHealth);
		ww.writeInt(unitType);
		ww.writeInt(attackPoints);
		ww.writeInt(movePoints);
	}	
	
	@Override
	public void listInfo(WordWriter ww)
	{
		super.listInfo(ww);
		ww.println("unitHealth "+unitHealth);		
		ww.println("unitType "+unitType);		
		ww.println("attackPoints "+attackPoints);		
		ww.println("movePoints "+movePoints);

	}

	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("unitType"))
		{
			unitType=wr.readInt();
			return 1;
		}
		else
		{
			return super.setInfo(wr, infoName);
		}
	}

	
	/*
	public EmpireTerrain getEmpireTerrain()
	{
		return (EmpireTerrain)getContainingObjOfClass(EmpireTerrain.class);
	}
	*/

	
	
	public void generateUnit()
	{
		//final EmpireWorld ew=getEmpireWorld();
		//final EmpireTerrain et=ew.getEmpireTerrain();

		
		owner=0;
		//pos=ew.generator.nextInt(et.sizeX*et.sizeY);
	}
	
	@Override
	public int getPosRecursive()
	{
		/*if (pos>=0)
		{
			return pos;
		}
		else
		{
			DbBase co=this.getContainingObj();
			if (co instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)co;
				return eu.getPosRecursive();
			}
		}*/
		
		D1Base co=this.getContainingObj();
		if (co instanceof EmpireUnit)
		{
			EmpireUnit eu=(EmpireUnit)co;
			return eu.getPosRecursive();
		}
		else if (co instanceof EmpireSector)
		{
			EmpireSector es=(EmpireSector)co;
			return es.getPosXY();
		}

		return -1;
	}

	// returns: 
	// XY coordinates if the unit is not aboard another unit
	// -1 if it is.
	public int getPosXY()
	{
		
		D1Base co=this.getContainingObj();
		if (co instanceof EmpireSector)
		{
			EmpireSector es=(EmpireSector)co;
			es.getPosXY();
		}

		return -1;
		
	}
	
	/*
	public EmpireOrder findOrder()
	{
		DbBase bo=iterateStoredObjects(null, EmpireOrder.class);
		return (EmpireOrder)bo;		
	}
	*/

	
	public int calculateStackMass()
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(this.unitType);
		
		int m=0;
		
		// Units own mass
		m+=eut.getMass();
		
		// Add mass recursively
		// Mass of all child units (and their child units...)
		D1Base sol[]=getListOfSubObjects();
		for(int i=0; i<sol.length; i++)
		{
			D1Base so = sol[i];
			if (so instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)so;
				m+=eu.calculateStackMass();
			}
		}
		
		return m;
	}

	public long calculateOwnCrewContribution()
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(this.unitType);

		// It does not need crew if it is just cargo but it will contribute if it can
		if (isUnitCargo())
		{
			return eut.getWorkforceProvided();
		}
		
		return eut.getWorkforceProvided()-eut.getWorkforceNeeded();
	}
		
	public long calculateAvailableCrewMinusUsed()
	{

		// Units own crew contribution
		long m=calculateOwnCrewContribution();
		
		
		// Add crew contribution from sub units
		// all child units (and their child units...)
		D1Base sol[]=getListOfSubObjects();
		for(int i=0; i<sol.length; i++)
		{
			D1Base so = sol[i];
			if (so instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)so;
				m+=eu.calculateAvailableCrewMinusUsed();
			}
		}
		
		return m;
	}

	
	// This is used to know if the move can be made, 
	// It can tell if the move can be done now or ever, if not ever, its better to give up the move right away.
	public int calculateStackCarryCapacityNowOrEver(int fromTerrain, int toTerrain, boolean ever)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(this.unitType);
				
		int c=0;
		
		// Units own carryCapacity regardless of current move points
		if ((eut.getMoveCost(fromTerrain)<EmpireUnitType.InfiniteMoveCost) && (eut.getMoveCost(toTerrain)<EmpireUnitType.InfiniteMoveCost))
		{
			if (ever || ((movePoints>=EmpireUnitType.minMovePointsToMove)))
			{
				c += eut.getCarryCapacity(fromTerrain, toTerrain);
			}
		}
		
		// Add carry capacity recursively
		for (D1Base so: this.listOfStoredObjects)
		{
			if (so instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)so;
				if (!eu.isUnitCargo())
				{
					c+=eu.calculateStackCarryCapacityNowOrEver(fromTerrain, toTerrain, ever);
				}
			}
		}

		return c;
	}
	
	
	
	public void payMoveCost(int fromTerrain, int toTerrain)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(this.unitType);
	
		
		final int moveCost=eut.getMoveCost(fromTerrain, toTerrain);
		
		// Units own move cost
		// Pay units own move cost (only )
		if (movePoints>=EmpireUnitType.minMovePointsToMove)
		{
			// if it had enough move points to contribute carry capacity
			movePoints-=moveCost;
		}
		else
		{
			// Set to zero if it was cargo.
			movePoints = 0;
		}
		
		// pay move cost recursively
		D1Base sol[]=getListOfSubObjects();
		for(int i=0; i<sol.length; i++)
		{
			D1Base so = sol[i]; 
			if (so instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)so;
				eu.payMoveCost(fromTerrain, toTerrain);
			}
		}
	}
	
	
	public void lookAround(int radius)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireTerrain et=ew.getEmpireTerrain();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireState en=enl.getEmpireNation(owner);
		final int curPosIndiex=getPosRecursive();
		final EmpireSector es=et.getSector(curPosIndiex);
		final int k=es.getIndex();
		final int nSectors=EmpireTerrain.getNSectorNeighbors(radius);

		if (en.getMoney()>EmpireState.MinPossibleAmmountOfMoney)
		{
			for(int d=EmpireTerrain.SAME_SECTOR; d<nSectors;d++)
			{
				final int j=et.getSectorIndexByDirection(k,d);
				//final int st=et.getTerrainMask(j);
				
				EmpireSector e = et.getSector(j);
	
				if (!en.canSeeSector(j))
				{
					en.setCanSeeSector(j);
					//DbBase b=et.getObjFromIndex(j);
					if (e!=null)
					{
						//EmpireSector s=(EmpireSector)b;
						//s.setUpdateCounter(); // TODO perhaps there is a better way to make this client see this sector, this solution might be sending update to also they who already see it.
						//e.setUpdateCounter();
						ew.notifySubscribers(e.getId()); // TODO: This will notify all not just the state that saw this sector. It will get filtered out in EmpireServer.
						
						// Notify also about sub objects, TODO: Should we notify recursively down?
						if (e.listOfStoredObjects!=null)
						{
							for(D1Base b: e.listOfStoredObjects)
							{
								ew.notifySubscribers(b.getId());
							}
						}
					}
					else
					{
						debug("Sector is not created yet, we should create it here perhaps "+j);
					}
				}
			}
		}
		else
		{
			// If player have very little money its units go blind.
		}
	}

		
	// Returns null if OK, an error message if move can't be performed.
	public String moveUnit(int destSector)
	{
		final int curPosXY=getPosRecursive();
		if (curPosXY!=destSector)
		{
			final EmpireWorld ew=getEmpireWorld();
			final EmpireTerrain et=ew.getEmpireTerrain();
			
			//final int terrainDest=et.getTerrain(destSector);				
			final int terrainCurrMask=et.getTerrainMask(curPosXY);				

			final int d = et.getDirection(curPosXY, destSector, this.unitType, this.owner);			
			final int nextPos = et.getSectorIndexByDirection(curPosXY, d);
			final int terrainNextPosMask=et.getTerrainMask(nextPos);
			
			final int stackMass=calculateStackMass();
			
			final int carryCapacityEver=calculateStackCarryCapacityNowOrEver(terrainCurrMask, terrainNextPosMask, true);
			
			final long availableCrewMinusUsed = calculateAvailableCrewMinusUsed();
			
			if (stackMass>carryCapacityEver)
			{
				// Unit is overloaded, it will never make the move.
				final String str="Giving up on move order, to heavy or can't move there.";
				debug(str);
				return str;
			}
			else if (availableCrewMinusUsed<0)
			{
				// insufficient crew
				final String str="Giving up on move order, insufficient crew";
				debug(str);
				return str;
			}
			else
			{
				final int carryCapacityCurrent=calculateStackCarryCapacityNowOrEver(terrainCurrMask, terrainNextPosMask, false);
				if (carryCapacityCurrent>=stackMass)
				{
					final EmpireSector es=et.getSector(nextPos);

					if (es.isNeutral(this.owner))
					{
						return("Its not allowed to move onto sectors owned by neutral states.");
						
					}
					else
					{
						// remove unit from its old position and add it on its new position.
						relinkSelf(es);
						
						payMoveCost(terrainCurrMask, terrainNextPosMask);
	
						// Look around
						lookAround(3);
						
						this.setUpdateCounter();
					}
				}
			}
			
		}
		return null;
	}
	
	
	public void takeHit(int attackStrength)
	{
		//final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		//final EmpireUnitType eut=eul.getUnitType(unitType);

		unitHealth-=attackStrength;

		if (unitHealth<=0)
		{
			final EmpireWorld ew=getEmpireWorld();
			ew.setTickCleanupCallback(this);
		}

		setUpdateCounter();
	}
	
/*
	@Override	
	public void gameTickEconomy()
	{

		
		
	}
	*/
	
	static public boolean isWordInString(String w, String s)
	{
		WordReader wr= new WordReader(s);
		while(wr.isOpenAndNotEnd())
		{
			String o=wr.readWord();
			if (o.equals(w))
			{
				return true;
			}
		}
		return false;
	}
	

	static int inc(int current, long delta, int max)
	{
		if (current==max)
		{
			return current;
		}		
		
		current+=delta;
		if (current>max)
		{
			return max;
		}
		
		return current;
	}

	
	
	
	// During game tick update only local properties of this object. Don't move, add or remove units. Don't touch/change other units. That is to be done during interact.
	@Override
	public void gameTick(long timeInGameTimeUnits)
	{		
		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireState en=enl.getEmpireNation(owner);
		final EmpireUnitType eut=eul.getUnitType(unitType);
		//final EmpireTerrain et=ew.getEmpireTerrain();
		//final EmpireSector pes=(this.getContainingObj() instanceof EmpireSector)?(EmpireSector)this.getContainingObj() : null;

		
		final long deltaGameTime=timeInGameTimeUnits-localTime; // Do this before calling super.gameTick otherwise deltaGameTime will be zero always.
		super.gameTick(timeInGameTimeUnits);

		/*if(deltaGameTime>10)
		{
			debug("deltaGameTime "+deltaGameTime);
		}*/
			
		if (en!=null)
		{
			final int dailyIncome = eut.getIncome();
			final long minMoneyForMaintainingUnits=en.getMoneyEarned()*MinMoneyForMaintainingUnitsFactor;
			
			if (dailyIncome>=0) 
			{
				// this is a unit with positive income, such as a city (town hall)
				en.addEarnedMoney(eut.getIncome());
			}
			else if (en.getMoney()>=minMoneyForMaintainingUnits) 
			{
				// This state have enough money, so no problem.
				en.addSpentMoney(-eut.getIncome());
			}
			else
			{				
				// unit owner is out of money and this is a unit which cost money in maintenance. 
				// Instead of paying some units will be randomly scraped. Probability of scraping will depend on cost price ratio.
				for (int i=0;i<deltaGameTime;i++)
				{
					final int dailyCost = -dailyIncome;
					final int buyingCost = eut.getBuildMoneyCost(); // How much it cost to build this unit
					final int scrapRatio = (buyingCost * 99) / (dailyCost * 100);
					final int r = ew.generator.nextInt(scrapRatio);
					
					if (r==0)
					{
						// Scraping unit due to lack of money for maintenance
						this.unitType=EmpireUnitTypeList.NothingType;
						this.unitHealth=0;
						this.unitType=EmpireUnitTypeList.NothingType;
						movePoints=0;
						unitHealth=0;
						tellOwner("was scrapped due to lack of money");
						setUpdateCounter();
						break;
					}
				}	
			}
			
			final long workForceSupportProduced=eut.getPersonnelSupportProduced();
			final long workForceSupportNeeded=eut.getPersonnelSupportConsumed();
			
			// This turned out to resulted in a division by zero sometimes, will make it cost money instead (done in EmpireState).
			/*
			final long workforceAvailability=en.getWorkforceAvailability();
			final long workforceInUse=en.getWorkforceInUse();
			
			// scrap work force units if to little work force is supported by the cities
			// TODO we should find IDLE units and scrap those first, or charge money
			if ((this.unitType!=EmpireUnitTypeList.NothingType) && (workForceSupportNeeded>0) && (workforceInUse>workforceAvailability))
			{
				// This state have more workers than it can support, dismiss some
				for (int i=0;i<deltaGameTime;i++)
				{
					long ratio=(dismissFactor * workforceInUse) / workforceAvailability;
					long r=ew.generator.nextInt((int)ratio);
					if (r==0) 
					{
						// degrading unit due to lack of populated areas to support it
						EmpireUnitType newType=eul.getUnitType(eut.degradesTo);
						this.unitType=newType!=null?newType.getIndex():EmpireUnitTypeList.NothingType;
						this.unitHealth=0;
						movePoints=0;
						unitHealth=0;
						tellOwner("was scrapped due to lack of population to support it");
						setUpdateCounter();
						break;
						// TODO instead of the above, give ourselves and order to removeWorkers instead.
					}
				}
			}
			*/
			
			// Workforce
			en.addWorkforceAvailability(workForceSupportProduced);
			en.addWorkforceUsage(workForceSupportNeeded);
			
		}
		
		
		if (attackPoints<EmpireUnitType.MaxAttackPoints)
		{
			attackPoints=inc(attackPoints, deltaGameTime, EmpireUnitType.MaxAttackPoints);
			//setUpdateCounter(); // Don't set update counter for fully deterministic routine changes, client will have to calculate these itself.
		}

		
		//debug("gameTime "+gameTime);
		if (movePoints<EmpireUnitType.MaxMovePoints)
		{
			// Unit has moved (or is new), recover move points.
			movePoints = inc(movePoints, deltaGameTime, EmpireUnitType.MaxMovePoints);
			//setUpdateCounter(); // Don't set update counter for fully deterministic routine changes, client will have to calculate these itself.
		}
		else
		{
			// Unit has 100% move points, does it need to recover health?
			if (unitHealth<eut.maxHealth)
			{
				// if it also has max attack points then perhaps it can recover health.
				if ((attackPoints==EmpireUnitType.MaxAttackPoints))
				{
					// perhaps it can recover health
					// Units recovers health only if it is not moving and not fighting (have full move points and full attack points).
					if (isCorrectTerrainForRecovery())
					{
						if (en.getMoney() > en.getMoneyEarned()*MinimumMoneyRequiredForUnitsHealthRecoveryFactor)
						{
							unitHealth = inc(unitHealth, deltaGameTime, eut.maxHealth);
							movePoints-=EmpireUnitType.MovePointCostForHealth;
							setUpdateCounter();
						}
						else
						{
							// Unit owner has so little money it can not recover health. 
						}
					}
				}
			}
		}
		

        // This unit might have orders to do
		if (this.getNSubObjects()>0)
		{
			ew.setTickPerformOrdersQueueCallback(this);
		}
			
		// This unit may want to do some attack also.
		if (attackPoints>=EmpireUnitType.minAttackPointsToAttack)
		{
			ew.setTickInteractCallback(this);
		}

		
		// This unit needs to do cleanup. Or we could fix the owner issue here instead of in cleanup.
		if ((this.owner<0) || (this.unitType==EmpireUnitTypeList.NothingType))
		{
			ew.setTickCleanupCallback(this);
		}

	}
	
	public int getUnitTypeNumberFromUnitTypeName(String name)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		return eul.getUnitTypeIndex(name);
	}
	
    // Returns null if all was found
	// A message if not all was found
	public String checkOrUseBuildMaterial(String materialNames, boolean justCheckIfAvailable, boolean inkSelfAsRawMaterial)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final int n=eul.getListCapacity(); // how many different types of units are there
		String materialName="";
		
		// Get the list of materials as an array of strings
		String materialNeededNames[]=WordReader.split(materialNames);
				
		// arrays for all possible materials
		int materialAvailableOrUsed[]=new int[n];
		int buildMaterialNeeded[]=new int[n];
		
		// Now check what we need
		for(int i=0;i<materialNeededNames.length;++i)
		{
			int j=getUnitTypeNumberFromUnitTypeName(materialNeededNames[i]);
			if (j>=0)
			{
				buildMaterialNeeded[j]++;
			}
			else
			{
				return "unknown material needed "+materialNeededNames[i];
			}
		}

		// TODO what about health and move points?
		
		// Perhaps use the unit itself also
		if (inkSelfAsRawMaterial)
		{
			int ut=this.unitType;

			// Do we need more of this?
			if (buildMaterialNeeded[ut]>materialAvailableOrUsed[ut])
			{
				if (!justCheckIfAvailable)
				{
					this.unitType=EmpireUnitTypeList.NothingType;
					this.setUpdateCounter();
					materialAvailableOrUsed[ut]++;
				}
				else
				{
					materialAvailableOrUsed[ut]++;
				}
			}
		}
		
		
		// Now check what we have
		for(D1Base db: this.listOfStoredObjects)
		{
			if (db instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)db;
				int ut=eu.unitType;
				if (ut>=0)
				{
					if (!justCheckIfAvailable)
					{
						// Do we need more of this?
						if (buildMaterialNeeded[ut]>materialAvailableOrUsed[ut])
						{
							eu.unitType=EmpireUnitTypeList.NothingType;
							eu.setUpdateCounter();
							materialAvailableOrUsed[ut]++;
						}
					}
					else
					{
						materialAvailableOrUsed[ut]++;
					}
				}
			}
		}
		
		// Check also for material in parent, but not this again
		for(D1Base db: this.getParent().listOfStoredObjects)
		{
			if (db instanceof EmpireUnit)			
			{
				if (db!=this)
				{
					EmpireUnit eu=(EmpireUnit)db;
					int ut=eu.unitType;
					if (ut>=0)
					{
						if (!justCheckIfAvailable)
						{
							// Do we need more of this?
							if (buildMaterialNeeded[ut]>materialAvailableOrUsed[ut])
							{
								eu.unitType=EmpireUnitTypeList.NothingType;
								eu.setUpdateCounter();
								materialAvailableOrUsed[ut]++;
							}
						}
						else
						{
							materialAvailableOrUsed[ut]++;
						}
					}
				}
			}
		}

		
		// Check that everything was found
		for(int i=0;i<n;++i)
		{
			if (buildMaterialNeeded[i]>materialAvailableOrUsed[i])
			{
				// This was not found, or not enough of it.
				materialName+=eul.getObjFromIndex(i).getName()+" ";
			}
		}
		
		// Everything needed was found
		if (materialName.length()!=0)
		{
			return materialName;
		}
		return null;
	}

	
	public boolean sectorOwnerOrElseClaimSector(final EmpireUnitType euttb)
	{
		if (euttb.getMass()<EmpireUnitType.InfiniteMass)
		{
			// The unit to build is not infinite mass, don't need to own sector
			return true;
		}

		EmpireSector es=getEmpireSectorRecursivelyForUnit();

		if (es.owner==this.owner)
		{
			return true;
		}
		
		
		return false;

	}

	
	
	
	
	public boolean isCorrectTerrain(final EmpireUnitType euttb)
	{
		String rt=euttb.requiredTerrain;
		
		if (rt.length()==0)
		{
			// This type of object can be built in any terrain
			return true;
		}
		
		int m=EmpireTerrain.getTerrainTypeMaskFromTerrainTypeNames(rt);
		
		EmpireSector es=getEmpireSectorRecursivelyForUnit();
		
		// all required terrain bits must be present
		if ((m&es.terrain)!=m)
		{		
			// some required terrain is missing
			return false;
		}
		
		return true;
	}
	
	public boolean isCorrectTerrain()
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(unitType);
		return isCorrectTerrain(eut);
	}
	
	public boolean isCorrectTerrainForRecovery()
	{
		EmpireSector es=getEmpireSectorRecursivelyForUnit();
		
		// To recover health a unit must rest in a city, docks or something.
		//if ((EmpireTerrain.CITY_TERRAIN_MASK & es.terrain)!=0)
		if (isCityDocksOrOtherBuildingsInSector())
		{		
			// It must not be an enemy city.
			if (!isEnemy(es.owner))
			{
				return true;
			}
		}

		return false;
	}

	public boolean isCityDocksOrOtherBuildingsInSector()
	{
		EmpireSector es=getEmpireSectorRecursivelyForUnit();
		
		if (es.calculateMassInSector()>=EmpireUnitType.InfiniteMass)
		{
			return true;
		}
		return false;
	}
	
	
	public boolean isThereRoomInSectorForBuilding(final EmpireUnitType euttb)
	{	
		if (euttb.getMass()<EmpireUnitType.InfiniteMass)
		{
			// The unit to build is not infinite mass
			return true;
		}

		// The unit to build is infinite mass, it can only be built in a sector with no other such units (such as cities)
		
		if (!isCityDocksOrOtherBuildingsInSector())
		{
			return true;
		}
		
		return false;
	}
	
	// TODO: is this duplicate of getTerrainMaskRecursive?
	public EmpireSector getEmpireSectorRecursivelyForUnit()
	{
		if (this.getContainingObj() instanceof EmpireSector)
		{
			return (EmpireSector)this.getContainingObj();
		}
		else if (this.getContainingObj() instanceof EmpireUnit)
		{
			EmpireUnit eu=(EmpireUnit)this.getContainingObj();
			return eu.getEmpireSectorRecursivelyForUnit();
		}
		else
		{
			return null;
		}
	}

	private void createNewUnit(int m2)
	{
		EmpireUnit eu = new EmpireUnit(this.getParent(), this.getName());
		eu.unitType=m2;
		eu.owner=owner;
		eu.attackPoints=0;
		eu.movePoints=this.movePoints;							
	}

	public void transformUnit(WordReader mwr)
	{
		while (mwr.isOpenAndNotEnd())
		{
			int m2=getUnitTypeNumberFromUnitTypeName(mwr.readName());

			if (m2>=0)
			{
				if (this.unitType==EmpireUnitTypeList.NothingType)
				{
					// Transform this unit into new type
					this.unitType=m2;
				}
				else
				{
					createNewUnit(m2);
				}
		
			}
			else
			{
				debug("unknown unit to create");
			}
		}
	}

	
	public void buildSomething(String whatToBuild, EmpireUnitType euttb, EmpireState en, int unitType)
	{
		// now it will build something
		EmpireUnit eu = new EmpireUnit();
		eu.linkSelf(this.getParent());
		//eu.linkSelf(this); // wanted to dut the new object in "this" instead of parent but then all new objects get destroyed, very strange. 
		eu.regName(whatToBuild.substring(0, 1)+eu.getId()); // +"_from_"+getName()
		eu.unitType=unitType;
		eu.owner=owner;
		//eu.pos=pos;
		eu.attackPoints=0;
		eu.movePoints=euttb.getInitialMovePoints(); // or shall we have a separate variable for this?
		movePoints-=euttb.getBuildCapacityCost();
		checkOrUseBuildMaterial(euttb.buildMaterialNeeded, false, false);
		en.addMoney(-euttb.getBuildMoneyCost()); //en.moneyChange-=euttb.buildMoneyCost;
		eu.localTime=localTime;
		//useMaterialToBuild(euttb.buildMaterialNeeded);
		//tellOwner("building unit "+whatToBuild); // Don't tell owner for now, it might be to much messages.
		this.setUpdateCounter();
		en.setUpdateCounter();
	}
	
	private void buildSomething(EmpireOrder bo, WordReader wr)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireState en=enl.getEmpireNation(owner);
		final EmpireUnitType eut=eul.getUnitType(unitType);
		//final EmpireTerrain et=ew.getEmpireTerrain();
		//final EmpireSector pes=(this.getContainingObj() instanceof EmpireSector)?(EmpireSector)this.getContainingObj() : null;

		if ((en!=null) && (en.getMoney()>0))
		{
			String whatToBuild=wr.readWord();
			
			if (!isWordInString(whatToBuild, eut.possibleBuilds))
			{
				tellOwner("This unit can not build "+whatToBuild);
				bo.unlinkSelf();			
			}
			else
			{
				//debug("order " + bo.getId() + " build "+ whatToBuild);
				int i=getUnitTypeNumberFromUnitTypeName(whatToBuild);
	
				if (i>=0)
				{
					
					// This is just for debugging, can be removed later
					if (!eul.getUnitTypeName(i).equals(whatToBuild))
					{
						error("eul.getUnitTypeName(i) != whatToBuild "+ i+ " "+eul.getUnitTypeName(i)+" "+whatToBuild);
					}
					
					
					final EmpireUnitType euttb = eul.getUnitType(i); // Empire Unit Type To Build
					final long workForceSupportNeeded=euttb.getPersonnelSupportConsumed();
					final long workforceAvailability=en.getWorkforceAvailability();
					final long workforceInUse=en.getWorkforceInUse();
	
					if (euttb.getBuildCapacityCost()>=EmpireUnitType.InfiniteBuildCostCapacity)
					{
						tellOwner("can't build "+whatToBuild);
						bo.unlinkSelf();													
					}
					else if (en.getMoney()<0)
					{
						tellOwner("can't build, no money");
						bo.unlinkSelf();													
					}
					else if ((workForceSupportNeeded>0) && (workforceInUse>=workforceAvailability))
					{
						// This is a worker unit or a unit with an included work force and the state can not support more personnel.
						tellOwner("can't support more personnel units");
						bo.unlinkSelf();										
					}
					else
					{
						if (isCorrectTerrain(euttb))
						{
							if (isThereRoomInSectorForBuilding(euttb))
							{
								if (sectorOwnerOrElseClaimSector(euttb))
								{
									if (en.getMoney()>euttb.getBuildMoneyCost())
									{
										// this state has enough money
									
										if (movePoints>=EmpireUnitType.minMovePointsToBuild)
										{
											// the city has enough build points
											String materialMissing=checkOrUseBuildMaterial(euttb.buildMaterialNeeded, true, false);
											if (materialMissing==null)
											{
												// it has enough materials
												buildSomething(whatToBuild, euttb, en, i);
												bo.unlinkSelf();
											}
											else
											{
												debug("order " + bo.getId() + " materialMissing="+materialMissing);
												tellOwner("to build "+whatToBuild + " you need more "+materialMissing);
												bo.unlinkSelf();					
											}
										}
										else
										{
											//debug("order " + bo.getId() + " not enough move points to build yet "+ whatToBuild+", have="+movePoints+", need="+EmpireUnitType.minMovePointsToBuild);
										}
									}
									else
									{
										//debug("order " + bo.getId() + " not enough money to build yet "+ whatToBuild+ " state " + en.getName() +" have " + en.savedMoney+ " need "+euttb.buildMoneyCost);
									}
								}
								else
								{
									//tellOwner("could not build "+whatToBuild + " here, it belongs to someone else, will try to claim it.");
									//bo.unlinkSelf();												
									
									// Will try to take sector
									EmpireSector es=getEmpireSectorRecursivelyForUnit();
									es.setNewOwner(this.owner);
								}
							}
							else
							{
								tellOwner("can't build "+whatToBuild + " here, there is already something here");
								bo.unlinkSelf();																										
							}
						}
						else
						{
							tellOwner("can't build "+whatToBuild + " here, it needs to be in: "+euttb.requiredTerrain);
							bo.unlinkSelf();															
						}
					}
				}
				else
				{
					tellOwner("Unknown unit to build "+whatToBuild);
					bo.unlinkSelf();					
				}
			}
		}
	}

	// Returns true if order can now be unlinked
	private boolean goTo(int idOfDestObj)
	{
		D1IdList il=this.getDbIdList();					

		D1Base destObj=il.getDbIdObj(idOfDestObj);

		if (destObj instanceof EmpireUnitOrSector)
		{
			final int curPosXY=getPosRecursive();
			EmpireUnitOrSector deu=(EmpireUnitOrSector)destObj;
	
			if (curPosXY==deu.getPosRecursive())
			{
				// Both units are at some location
				int r= this.relinkSelf(deu);
				if (r!=0)
				{
					tellOwner("tried to board unit "+idOfDestObj+" but could not");								
				}
				deu.setUpdateCounter();
				this.setUpdateCounter();
				return true;
			}
			else
			{
				// Not at same location, so move in that direction
				int dest=deu.getPosRecursive();
				if (dest>=0)
				{
					moveUnit(dest);
				}
				else
				{
					// perhaps destination unit is destroyed? Cancel order.
					tellOwner("tried to board unit "+idOfDestObj+" but could not find it");
					return true;
				}
			}
		}
		else
		{
			tellOwner("this unit can not go to ~"+idOfDestObj);
			return true;
		}
		return false;
	}
	


	private void goTo(EmpireOrder bo, WordReader wr)
	{
		
		int idOfDestObj=wr.readInt();
	
		//debug("order id " + bo.getId() + ", unit "+ getId()+ ", goTo id "+idOfDestObj);
		if (goTo(idOfDestObj))
		{
			bo.unlinkSelf();
		}
	}
	
	private void moveTo(EmpireOrder bo, WordReader wr)
	{	
		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		//final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireState en=enl.getEmpireNation(owner);
		//final EmpireUnitType eut=eul.getUnitType(unitType);
		//final EmpireTerrain et=ew.getEmpireTerrain();
		//final EmpireSector pes=(this.getContainingObj() instanceof EmpireSector)?(EmpireSector)this.getContainingObj() : null;
	
		if ((en!=null) && (en.getMoney()>en.getMoneyEarned()*MinimumMoneyRequiredToMoveUnitsFactor))
		{				
			int dest=wr.readInt();
	
			final String r = moveUnit(dest);			
	
			final int pos=getPosRecursive();
			if ((pos==dest) || (r!=null))
			{
				// destination reached or move is not possible, remove this order and tell owner
				bo.unlinkSelf();
				if (r!=null)
				{
					// something went wrong, move was not possible
					tellOwner(r);
				}
			}					
		}
		else
		{
			bo.unlinkSelf();
			tellOwner("can't move, no money");				
		}
	}

	public void scrapUnit(EmpireOrder bo, String cmd, WordReader wr)
	{
		final EmpireWorld ew=getEmpireWorld();
		//final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		//final EmpireState en=enl.getEmpireNation(owner);
		final EmpireUnitType eut=eul.getUnitType(unitType);
		//final EmpireTerrain et=ew.getEmpireTerrain();
		//final EmpireSector pes=(this.getContainingObj() instanceof EmpireSector)?(EmpireSector)this.getContainingObj() : null;

		debug("order " + bo.getId() + " scrap/transform/disassemble unit "+ getId());
	
		//int dest=wr.readInt();
	
		bo.unlinkSelf();
		
		WordReader mwr;
		
		if (cmd.equals("disassemble"))
		{
			this.unitType=EmpireUnitTypeList.NothingType;
			mwr=new WordReader(eut.disassemblesInto);
			tellOwner("was disassembled as ordered");
			transformUnit(mwr);
		}
		else if (cmd.equals("transform"))
		{
			String materialMissing=checkOrUseBuildMaterial(eut.transformationMaterial, true, true);
			if (materialMissing==null)
			{
				// All need material was available
				mwr=new WordReader(eut.canTransformInto); // It can transform into more than one object
				checkOrUseBuildMaterial(eut.transformationMaterial, false, true);  // When we transform we require then the material we get from transformationMaterial is available
				tellOwner("was transformed as ordered");
				transformUnit(mwr);
			}
			else
			{
				tellOwner("need "+materialMissing+" to transform");
				return;
			}
		}
		else if (cmd.equals("scrap"))
		{
			this.unitType=EmpireUnitTypeList.NothingType;
			mwr=new WordReader("");
			tellOwner("was scrapped as ordered");
		}
		/*else if (cmd.equals("removeWorkers"))
		{
			this.unitType=EmpireUnitTypeList.NothingType;
			mwr=new WordReader(eut.disassemblesInto);
			tellOwner("was disassembled as ordered");
			transformUnit(mwr);
		}*/
		else
		{
			tellOwner("something went wrong with order '"+cmd+"'");
			return;
		}
	
		this.setUpdateCounter();
	}
	
	public void assembleSomething(EmpireOrder bo, WordReader wr)
	{
		final EmpireWorld ew=getEmpireWorld();
		//final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		//final EmpireState en=enl.getEmpireNation(owner);
		//final EmpireUnitType eut=eul.getUnitType(unitType);
		//final EmpireTerrain et=ew.getEmpireTerrain();
		//final EmpireSector pes=(this.getContainingObj() instanceof EmpireSector)?(EmpireSector)this.getContainingObj() : null;

		String whatToAssemble=wr.readWord();
		final EmpireUnitType eutToAssemble=eul.getUnitType(whatToAssemble);
	
		if (eutToAssemble!=null)
		{
			
			debug("order " + bo.getId() + " assemble " + eutToAssemble.getName() + " unit ~"+ getId());
	
			//int dest=wr.readInt();
	
			bo.unlinkSelf();
			
			
			String materialMissing=checkOrUseBuildMaterial(eutToAssemble.disassemblesInto, true, true);  // When assembling we require that the material we get from disassemblesInto is available
			if (materialMissing==null)
			{
				// All need material was available
				WordReader mwr=new WordReader(eutToAssemble.getName());
				checkOrUseBuildMaterial(eutToAssemble.disassemblesInto, false, true);
				tellOwner("was assembled as ordered");
	
				transformUnit(mwr);
				this.setUpdateCounter();
			}
			else
			{
				tellOwner("need more "+materialMissing+" to assembled a "+ whatToAssemble);
				return;
			}
	
		}
		else
		{
			tellOwner("Can't assemble '"+ whatToAssemble+"'");							
		}
	}
	
	// unload all stored EmpireUnit units
	private void unloadUnit(EmpireOrder bo, WordReader wr)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireTerrain et=ew.getEmpireTerrain();

		debug("order " + bo.getId() + " unload "+ getId());
		
		// Get sector that this unit is in, unloaded units are to be placed in the sector
		EmpireSector es=et.getSector(getPosRecursive());

		// Get a list of all stored objects
		D1Base[] list=getListOfSubObjects();
		
		for(int i=0;i<list.length;i++)
		{
			if (list[i] instanceof EmpireUnit)
			{
				EmpireUnit eus=(EmpireUnit)list[i];
				eus.relinkSelf(es);
				eus.setUpdateCounter();
				//break;
			}
		}
	
		// order hopefully performed so remove it.
		bo.unlinkSelf();
	}
	
	/*
	public D1IdObj findHeaviestUnit(D1List<EmpireUnitOrSector> idList)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eutl=ew.getEmpireUnitTypeList();
		return eutl.findHeaviestUnit(idList);
	}
	
	// mergeGroup is to be deprecated, use groupUnits instead
	// TODO remove this.
	private void mergeGroup(EmpireOrder bo, WordReader wr)
	{
		D1IdList il=this.getDbIdList();					
		D1List<EmpireUnitOrSector> idList=new D1List<EmpireUnitOrSector>();

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
					idList.add(euos);
				}
				else
				{
					tellOwner("Not your unit ~"+ id);							
				}
			}
		}
		
		if (idList.size()<=1)
		{
			tellOwner("empty merge ignored");	
		}
		else
		{
			D1IdObj heaviestFoundUnit=findHeaviestUnit(idList);
			
			if (heaviestFoundUnit==null)
			{
				tellOwner("merge target not clear");	
			}
			else
			{
				// Now move all units to target
				for(int i=0;i<idList.getCapacity();i++)
				{
					EmpireUnitOrSector euos=idList.get(i);
					if ((euos!=null) && (euos instanceof EmpireUnit))
					{
						EmpireUnit eu=(EmpireUnit)euos;
						eu.goTo(heaviestFoundUnit.getId());
					}
				}
			}
		}
		bo.unlinkSelf();
	}
	*/
	
	// returns true if order was found
	private boolean performOrder(EmpireOrder eo, String cmd, WordReader wr)
	{
		final char ch=cmd.charAt(0);
		switch(ch)
		{
			case 'a':
			{
				if (cmd.equals("assemble"))  
				{
					assembleSomething(eo, wr);
					return true;
				}
				break;
			}
			case 'b':
			{
				if (cmd.equals("build"))
				{
					buildSomething(eo, wr);
					return true;
				}
				if (cmd.equals("board")) // board is deprecated, use goTo instead
				{
					goTo(eo, wr);
					return true;
				}
				break;
			}
			case 'g':
			{
				if (cmd.equals("goTo"))
				{
					goTo(eo, wr);
					return true;
				}
				break;
			}
			case 'm':
			{
				if (cmd.equals("moveTo")) // TODO: moveTo is deprecated, it is possible to use 'goTo' to move to a sector, the difference is that goTo expects an ID as parameter while 'move' expects a position on the map. 
				{
					moveTo(eo, wr);
					return true;
				}
				// mergeGroup is to be deprecated, use groupUnits instead
				/*if (cmd.equals("mergeGroup"))
				{
					mergeGroup(eo, wr);
					return true;
				}*/
				break;
			}
			case 'd':
			{
				if (cmd.equals("disassemble"))
				{
					scrapUnit(eo, cmd, wr);
					return true;
				}
				break;
			}
			case 's':
			{
				if (cmd.equals("scrap"))
				{
					scrapUnit(eo, cmd, wr);
					return true;
				}
				break;
			}
			case 't':
			{
				if (cmd.equals("transform"))
				{
					scrapUnit(eo, cmd, wr);
					return true;
				}
				break;
			}
			case 'u':
			{
				if (cmd.equals("unload"))
				{
					unloadUnit(eo, wr);
					return true;
				}
				break;
			}
			default: break;
		}
		
		tellOwner("unknown command:"+cmd);
		eo.unlinkSelf();
		return false;
	}	
	
	// This is called during "doOrders" phase of game tick/update if a unit has an order to be performed.
	@Override	
	public void gameTickPerformOrders()
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(unitType);
		final EmpireSector es=(this.getContainingObj() instanceof EmpireSector)?(EmpireSector)this.getContainingObj() : null;

	
		
		// check if there is an order to perform for this unit
		for (D1Storable bo : this.listOfStoredObjects)
		{

  		    if (bo instanceof EmpireOrder)
		    {
  		    	try
  		    	{
  		    	
					EmpireOrder eo = (EmpireOrder)bo;
					WordReader wr=new WordReader(eo.getOrder());
					String cmd=wr.readWord();
					
					if (this.unitType==EmpireUnitTypeList.NothingType)
					{
						// nothing can not perform any orders
						bo.unlinkSelf();					
					}
					// mergeGroup is to be deprecated, use groupUnits instead, so eventually the quick and dirty fix here can be removed
					//else if ((!isWordInString(cmd, eut.possibleOrders)) && (!cmd.equals("mergeGroup")))  // Quick and dirty fix for mergeGroup
					else if (!isWordInString(cmd, eut.possibleOrders))
					{
						tellOwner("Can not do that order "+cmd);
						bo.unlinkSelf();					
					}
					else
					{
						performOrder(eo, cmd, wr);
					}
  		    	}
  		    	catch (NumberFormatException e)
  		    	{
					tellOwner("failed command:"+e);
					bo.unlinkSelf();
  		    	}
			}
		}
		
		// Check if this unit shall claim the sector it is in (change sector owner)
		// If its a city (castle or town hall) it must own the sector it is in, for other units that is optional.
		if (es !=null)
		{
			//if (eut.attackStrength>0)
			if (eut.getIncome()>0)
			{
				es.setNewOwner(this.owner);
			}
		}
		
	}
	

	public int getTerrainMaskRecursive()
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireTerrain et=ew.getEmpireTerrain();
		final int pos=getPosRecursive();
		return et.getTerrainMask(pos);		
	}

	
	// Calculate defenders strength
	public int calculateEnemyDefence()
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireTerrain et=ew.getEmpireTerrain();
		final int pos=getPosRecursive();
		
		int defStrength=0;
		
		// loop, look at nearby sectors, count number of defenders.
		for(int d=EmpireTerrain.SAME_SECTOR; d<EmpireTerrain.N_SECTOR_NEIGHBORS_1;d++)
		{
			final int sid=et.getSectorIndexByDirection(pos, d);
			
			defStrength+=et.calcDefensiveStrengthAtSector(sid, owner);
		}
		return defStrength;
	}
		
	public boolean isUnitCargo()
	{	
		final D1Container parent = this.getParent();

		if (parent instanceof EmpireSector)
		{
			// It is not cargo if it is in a sector (then it is not aboard another unit)
			return false;
		}
		
		if (parent instanceof EmpireUnit)
		{
			// This unit is aboard another unit but if that is a unit of same type its not cargo
			
			EmpireUnit eu = (EmpireUnit)parent;
			
			if (eu.unitType != this.unitType)
			{
				// It is aboard a unit of another type so it is cargo.
				return true;
			}			
			else
			{
				// Same type of unit, check if that unit is cargo
				return eu.isUnitCargo();
			}
		}
		
		// This is not in a sector and not in a unit. What is it in.
		debug("unexpected cargo");
		return true;
	}
	
	// During tick interact units interact with other units. They are not allowed to do that during gameTick or gameTickCleanup only during this phase.
	@Override	
	public void gameTickInteract()
	{		
		final EmpireWorld ew=getEmpireWorld();
		final EmpireTerrain et=ew.getEmpireTerrain();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(unitType);

		// has it reloaded? If not it will fight later
		if (attackPoints>=EmpireUnitType.minAttackPointsToAttack)
		{
			// yes, this unit have enough attack points to fight
				
			// Check if this unit can attack other units
			final int as=eut.attackProbability;
			if (as>0)
			{			
				// Units does not fight if they are loaded onto other units.
				if (!isUnitCargo())
				{		
					// This unit can attack if it is on appropriate terrain
					// Is it on suitable ground or water for the unit to fight from.
					final int terrainMask=getTerrainMaskRecursive();
					final int carryCapacity=eut.getCarryCapacity(terrainMask);
					if ((isCorrectTerrain()) && (carryCapacity>=eut.getMass()))
					{
						// This unit is in a sector terrain from which it can fight/attack

						// Are there enemies nearby to attack?
						final int defStrength=calculateEnemyDefence();	
				
						// was there units to attack?
						if (defStrength>0)
						{
							// There are enemies within range
							
							// Does it have enough crew			
							final long availableCrewMinusUsed = calculateAvailableCrewMinusUsed();
							if (availableCrewMinusUsed>=0)
							{			
								// It does have crew
								
								attackPoints-=EmpireUnitType.attackCost;
								
								final int ap=ew.generator.nextInt(as*100);
								final int dp=ew.generator.nextInt(defStrength*100);
								
								final int pos=getPosRecursive();
								
								if (ap >= dp)
								{
									// hit successfully
			
									final int damage=ew.generator.nextInt(eut.attackStrength);
									
									tellOwner("at sector "+pos+ " scored a hit, damage="+damage+", probability="+as+":"+defStrength);


									// loop nearby sectors and increment hit count
									for(int d=EmpireTerrain.SAME_SECTOR; d<EmpireTerrain.N_SECTOR_NEIGHBORS_1;d++)
									{
										final int sid=et.getSectorIndexByDirection(pos, d);
										
										
										et.hitSector(sid, owner, damage);					
									}
								}
								else
								{
									// miss
									debug("#"+this.getId()+" "+this.getName() + " at sector "+pos+ " failed its attack "+as+" "+defStrength);
			
									tellOwner("at sector "+pos+ " missed, "+as+":"+defStrength);
								}
							}
							else
							{
								//tellOwner("has insufficient crew to fight");
							}
						}
						else
						{
							// no units to attack
						}
					}
					else
					{
						//debug("the unit can not fight from this terrain");
					}
				}
				else
				{
					// This unit is loaded onto other units, so it can not fight now.
				}
			}
			else
			{
				// this unit can not attack other units
			}
		}
	}
	

	
	@Override	
	public void gameTickCleanup()
	{

		if (this.unitType==EmpireUnitTypeList.NothingType)
		{
			// All units inside a "nothing" are moved to parent.
			moveAllChildObjectsToParent();
			
			
			// This unit is destroyed let it stay a few ticks so that players can see where their unit is.
			if (movePoints>=EmpireUnitType.movePointsUntilUnlinked)
			{
				// TODO; All unit aboard needs to be moved to parent.
				this.unlinkSelf();
			}
		}
		else
		{
			final EmpireWorld ew=getEmpireWorld();
			final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
			final EmpireUnitType eut=eul.getUnitType(unitType);
			final EmpireTerrain et=ew.getEmpireTerrain();

			
			// Does this unit have an owner? 
			if (owner<0)
			{
				// No, is this a city (or other unit that earns money)?
				if (eut.getIncome()>0)
				{
					// city has no owner, city can be captured.
					// Get owner of the sector the city is in
					int newOwner=et.getSectorOwner(getPosRecursive());
					if (newOwner>=0)
					{
						setOwner(newOwner);				
						tellOwner("was captured");
						setUpdateCounter();
					}
				}
			}
			
			// Was this unit shot at
			if (unitHealth<=0)
			{
				// is it a city
				// Either cities should require a crew or they shall need to be rebuilt if captured. Have not decided yet. Cities requiring a crew is not implemented yet anyway.
				/*if (eut.income>0)
				{
					// Yes, cities are not destroyed, only change owner
					if (owner!=-1)
					{
		                tellOwner("was lost");
						owner=-1;
					}
					unitHealth=0;
					setUpdateCounter();
				}
				else*/
				{
					// no, destroy this unit (unless it is already destroyed)
					if (this.unitType!=EmpireUnitTypeList.NothingType)
					{
						tellOwner("was destroyed");
						this.unitType=EmpireUnitTypeList.NothingType;
						//unitHealth=0;
						movePoints=0;
						unitHealth=0;
					}
				}
			}

		}
		 
	}
	

	@Override	
	public void moveAllChildObjectsToParent()
	{
		if (this.listOfStoredObjects!=null)
		{
			for (D1Storable ds : this.listOfStoredObjects)
			{
				if (ds instanceof D1Container)
				{
					if (ds instanceof EmpireOrder)
					{
						// but not if it is just an order
						ds.unlinkSelf();
					}
					else
					{
						// move to parent object
						D1Container dc=(D1Container)ds;
						dc.relinkSelf(this.getParent());
					}
				}
			}
		}
	}

	boolean canBeGivenAway()
	{		
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(unitType);


		if (eut.getWorkforceProvided()>0)
		{
			// Units containing personnel can not be given away (to others than allies)
			// TODO perhaps we should not allow personnel to be given away at all?
			return false;
		}												

		if (eut.getIncome()<0)
		{
			// Units costing money can not be given away (to others than allies)
			return false;
		}												
		
		return super.canBeGivenAway();
	}

}

