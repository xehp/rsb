//
//Copyright (C) 2013 Henrik Björkman www.eit.se
//
//History:
//Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;

import se.eit.d1_pkg.*;
import se.eit.web_package.*;



public class EmpireUnitType extends EmpireBase {

    final static public int InfiniteBuildCostEconomical=9999;
    final static public int InfiniteBuildCostCapacity=9999;
    final static public int InfiniteMoveCost=9999;
    final static public int InfiniteMass=9999;


	public String typeName;        // Name of this unit type
	private int unitIncome;        // If positive value: how much money the unit gives, if negative: how much it will cost to have this unit.
	private int buildMoneyCost;     // How much money it will cost to start building this unit
	private int buildCapacityCost;  // How move points (not money) it will buildMoneyCost to start building this unit
	public int landMoveCost;       // How many move points the unit needs to move on land (speed is the inverse of this value)
	public int seaMoveCost;        // How many move points the unit needs to move at sea
	private int unitMass;           // How heavy the unit is
	public int landCarryCapacity;  // How much mass the unit can carry on land (it must also carry itself)
	public int seaCarryCapacity;   // How much mass the unit can carry at see
	public int maxHealth;          // How many hits this unit can receive without being destroyed.
	public int attackProbability;  // Probability of hitting other units. The higher value the more likely to score a hit.
	public int attackStrength;     // How many hits (health loss) this unit will inflict if it scores a hit
	public int landDefenseMoving;  // Probability of avoiding hits by other units at land, the higher value the more likely to avoid a hit.
	public int landDefenseStill;   // Probability of avoiding hits by other units at land, the higher value the more likely to avoid a hit.
	public int seaDefence;         // Probability of avoiding hits by other units at sea
	private int personnelSupport;   // If >0 How much personnel the unit supports, if <0 how much personnel that is tied up inside the unit also how much crew points it provides, units need more crew points then "crewNeeded" to function.
	private int crewNeeded;         // How much crew it needs to function
	public String degradesTo;        // What the unit becomes if degraded or scrapped
	public String canTransformInto;        // What the unit turns into if transformed, TODO this and transformationMaterial is perhaps not needed, we have possibleAssemblies/disassemblesInto instead
	public String transformationMaterial; // Material needed to transform unit into something else.
	public String disassemblesInto;        // What the unit turns into if disassembled         
	public String possibleOrders;  // What orders the unit can do
	public String possibleBuilds;  // What the unit can build (if it can build see possibleOrders)
	public String buildMaterialNeeded;  // Material needed to build this type of object
	public String requiredTerrain;  // In which type of sectors the object can reside
	public String possibleAssemblies;

	
	public static final int MaxMovePoints=100;          // How many move points a unit can have, it will gain on point per tick
	public static final int minMovePointsToMove=0;      // How many move points a unit must have to make a move
	public static final int minMovePointsToBuild=0;     // How many move points a unit must have to build something
	public static final int movePointsUntilUnlinked=100; // If a unit was destroyed, keep it a few game ticks before unlinking it.
	public static final int MaxAttackPoints=10;         // How many attack points a unit can have
	public static final int attackCost=10;              // How many attack points it costs to attack
	public static final int minAttackPointsToAttack=10; // How many attack points that are needed to perform an attack 	
	public static final int MovePointCostForHealth=100; // How many move points it cost to increase health with one point.
	
	public static final int defaultMsPerTick=500; // The recommended game speed when game is on. Low value means game moves faster.
	public static final int defaultStartMoneyForStates = 20000;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireUnitType.class.getSimpleName();	
	}


	public EmpireUnitType(D1Base parent, String typeName, int buildMoneyCost, int buildCapacityCost, int income, int landMoveCost, int seaMoveCost, int unitMass, int landCarryCapacity, int seaCarryCapacity, int maxHealth, int attackProbability, int attackStrength, int landDefenseMoving, int landDefenseStill, int seaDefence, int crewContribution, int crewNeeded, String degradesTo, String canTransformInto, String transformationMaterial, String disassemblesInto, String possibleOrders, String possibleBuilds, String buildMaterialNeeded, String requiredTerrain, String possibleAssemblies) 
	{
		super();
		parent.addObject(this);
		this.regName(typeName);

		this.typeName=typeName;
		this.buildMoneyCost=buildMoneyCost;		
		this.buildCapacityCost=buildCapacityCost;
		this.unitIncome=income;
		this.landMoveCost=landMoveCost;
		this.seaMoveCost=seaMoveCost;
		this.unitMass=unitMass;
		this.landCarryCapacity=landCarryCapacity;
		this.seaCarryCapacity=seaCarryCapacity;
		this.maxHealth=maxHealth;
		this.attackProbability=attackProbability;
		this.attackStrength=attackStrength;
		this.landDefenseMoving=landDefenseMoving;
		this.landDefenseStill=landDefenseStill;
		this.seaDefence=seaDefence;
		this.personnelSupport=crewContribution;
		this.crewNeeded=crewNeeded;
		this.degradesTo=degradesTo;
		this.canTransformInto=canTransformInto;
		this.transformationMaterial=transformationMaterial;
		this.disassemblesInto=disassemblesInto;
		this.possibleOrders=possibleOrders;
		this.possibleBuilds=possibleBuilds;
		this.buildMaterialNeeded=buildMaterialNeeded;
		this.requiredTerrain=requiredTerrain;
		this.possibleAssemblies=possibleAssemblies;
	}

	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);					
		pw.println("typeName "+typeName);		
		pw.println("buildMoneyCost "+buildMoneyCost);		
		pw.println("buildCapacityCost "+buildCapacityCost);		
		pw.println("income "+unitIncome);		
		pw.println("landMoveCost "+landMoveCost);		
		pw.println("seaMoveCost "+seaMoveCost);		
		pw.println("unitMass "+unitMass);		
		pw.println("landCarryCapacity "+landCarryCapacity);		
		pw.println("seaCarryCapacity "+seaCarryCapacity);		
		pw.println("maxHealth "+maxHealth);				
		pw.println("attackProbability "+attackProbability);		
		pw.println("attackStrength "+attackStrength);		
		pw.println("landDefenseMoving "+landDefenseMoving);		
		pw.println("landDefenseStill "+landDefenseStill);		
		pw.println("seaDefence "+seaDefence);	
		pw.println("personnelSupport "+personnelSupport);	
		pw.println("crewNeeded "+crewNeeded);	
		pw.println("degradesTo "+degradesTo);
		pw.println("canTransformInto "+canTransformInto);
		pw.println("transformationMaterial "+transformationMaterial);
		pw.println("disassemblesInto "+disassemblesInto);	
		pw.println("possibleOrders "+possibleOrders);		
		pw.println("possibleBuilds "+possibleBuilds);
		pw.println("buildMaterialNeeded "+buildMaterialNeeded);
		pw.println("requiredTerrain "+requiredTerrain);
		pw.println("possibleAssemblies "+possibleAssemblies);
		
	}

	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("typeName"))
		{
			typeName=wr.readString();
			return 1;
		}
		else if (infoName.equals("buildMoneyCost"))
		{
			buildMoneyCost=wr.readInt();
			return 1;
		}
		else if (infoName.equals("buildCapacityCost"))
		{
			buildCapacityCost=wr.readInt();
			return 1;
		}
		else if (infoName.equals("income"))
		{
			unitIncome=wr.readInt();
			return 1;
		}
		else if (infoName.equals("landMoveCost"))
		{
			landMoveCost=wr.readInt();
			return 1;
		}
		else if (infoName.equals("seaMoveCost"))
		{
			seaMoveCost=wr.readInt();
			return 1;
		}
		else if (infoName.equals("unitMass"))
		{
			unitMass=wr.readInt();
			return 1;
		}
		else if (infoName.equals("landCarryCapacity"))
		{
			landCarryCapacity=wr.readInt();
			return 1;
		}
		else if (infoName.equals("seaCarryCapacity"))
		{
			seaCarryCapacity=wr.readInt();
			return 1;
		}
		else if (infoName.equals("maxHealth"))
		{
			maxHealth=wr.readInt();
			return 1;
		}
		else if (infoName.equals("attackProbability"))
		{
			attackProbability=wr.readInt();
			return 1;
		}
		else if (infoName.equals("attackStrength"))
		{
			attackStrength=wr.readInt();
			return 1;
		}
		else if (infoName.equals("landDefenseMoving"))
		{
			landDefenseMoving=wr.readInt();
			return 1;
		}
		else if (infoName.equals("landDefenseStill"))
		{
			landDefenseStill=wr.readInt();
			return 1;
		}
		else if (infoName.equals("personnelSupport"))
		{
			personnelSupport=wr.readInt();
			return 1;
		}
		else if (infoName.equals("crewNeeded"))
		{
			crewNeeded=wr.readInt();
			return 1;
		}
		else if (infoName.equals("degradesTo"))
		{
			degradesTo=wr.readString();
			return 1;
		}
		else if (infoName.equals("canTransformInto"))
		{
			canTransformInto=wr.readString();
			return 1;
		}
		else if (infoName.equals("transformationMaterial"))
		{
			transformationMaterial=wr.readString();
			return 1;
		}
		else if (infoName.equals("disassemblesInto"))
		{
			disassemblesInto=wr.readString();
			return 1;
		}
		else if (infoName.equals("possibleOrders"))
		{
			possibleOrders=wr.readString();
			return 1;
		}
		else if (infoName.equals("possibleBuilds"))
		{
			possibleBuilds=wr.readString();
			return 1;
		}
		else if (infoName.equals("buildMaterialNeeded"))
		{
			buildMaterialNeeded=wr.readString();
			return 1;
		}
		else if (infoName.equals("requiredTerrain"))
		{
			requiredTerrain=wr.readString();
			return 1;
		}
		else if (infoName.equals("possibleAssemblies"))
		{
			possibleAssemblies=wr.readString();
			return 1;
		}
		
		return super.setInfo(wr, infoName);
	}

	static public boolean isMixedLandAndSea(int terrainMask)
	{
		final int m=EmpireTerrain.SEA_TERRAIN_MASK | EmpireTerrain.LAND_TERRAIN_MASK;
		return (terrainMask & m)==m;
	}
	
	static public boolean isOpenSea(int terrainMask)
	{
		return (terrainMask & EmpireTerrain.SEA_TERRAIN_MASK)!=0;
	}
	
	static public boolean isDryLand(int terrainMask)
	{
		return (terrainMask & EmpireTerrain.LAND_TERRAIN_MASK)!=0;
	}

	public int getMoveCost(int terrainMask)
	{
		if (isMixedLandAndSea(terrainMask))
		{
			// Both land and sea units can move in this sector (river, lake or canal)
			return Math.min(landMoveCost,seaMoveCost);			
		}
		else if (isOpenSea(terrainMask))
		{
			// Only sea units can move in this sector (open sea, no bridge)
			return seaMoveCost;
		}
		else if (isDryLand(terrainMask))
		{
			// Only land units can move in this sector (land without rivers or canals)
			return landMoveCost;
		}
		else
		{
			return Math.max(landMoveCost,seaMoveCost);
		}
	}
	
	public int getCarryCapacity(int terrainMask)
	{
		if (isMixedLandAndSea(terrainMask))
		{
			// Both land and sea units can move in this sector (river, lake or canal)
			return Math.max(landCarryCapacity,seaCarryCapacity);			
		}
		else if (isOpenSea(terrainMask))
		{
			// Only sea units can move in this sector (open sea, no bridge)
			return seaCarryCapacity;
		}
		else if (isDryLand(terrainMask))
		{
			// Only land units can move in this sector (land without rivers or canals)
			return landCarryCapacity;
		}
		else
		{
			return Math.min(landCarryCapacity,seaCarryCapacity);
		}
	}
	
	public int getMoveCost(int fromTerrainTypeMask, int toTerrainTypeMask)
	{
		final int moveCostFromCurr=this.getMoveCost(fromTerrainTypeMask);
		final int moveNextToPos=this.getMoveCost(toTerrainTypeMask);
		
		//final int moveCost=(moveCostFromCurr+moveNextToPos)/2;
		// Or like this if the destination terrain shall be weighted more than the current terrain
		final int moveCost=(moveCostFromCurr+moveNextToPos*3)/4;

		return moveCost;
	}
	
	public int getCarryCapacity(int fromTerrain, int toTerrain)
	{
		final int carryCapacityFromCurr=this.getCarryCapacity(fromTerrain);
		final int carryCapacityToPos=this.getCarryCapacity(toTerrain);
		
		final int carryCapacity=Math.min(carryCapacityFromCurr,carryCapacityToPos);
		return carryCapacity;
	}
	
	
	public int getDefenceMoving(int terrainMask)
	{
		if (isMixedLandAndSea(terrainMask))
		{
			// Both land and sea units can move in this sector (river, lake or canal)
			return Math.max(landDefenseMoving,seaDefence);			
		}
		else if (isOpenSea(terrainMask))
		{
			// Only sea units can move in this sector (open sea, no bridge)
			return seaDefence;
		}
		else if (isDryLand(terrainMask))
		{
			// Only land units can move in this sector (land without rivers or canals)
			return landDefenseMoving;
		}
		else
		{
			return Math.min(landDefenseMoving,seaDefence);
		}
	}

	public int getDefenceStill(int terrainMask)
	{
		if (isMixedLandAndSea(terrainMask))
		{
			// Both land and sea units can move in this sector (river, lake or canal)
			return Math.max(landDefenseMoving,seaDefence);			
		}
		else if (isOpenSea(terrainMask))
		{
			// Only sea units can move in this sector (open sea, no bridge)
			return seaDefence;
		}
		else if (isDryLand(terrainMask))
		{
			// Only land units can move in this sector (land without rivers or canals)
			return landDefenseStill;
		}
		else
		{
			return Math.min(landDefenseStill,seaDefence);
		}
	}
	
	public EmpireUnitType()
	{	
		super();
	}

	
	@Override
	public void readSelf(WordReader wr)	throws NumberFormatException
	{
		super.readSelf(wr);

		typeName=wr.readString();        
		buildMoneyCost=wr.readInt();             
		buildCapacityCost=wr.readInt();    
		unitIncome=wr.readInt();           
		landMoveCost=wr.readInt();     
		seaMoveCost=wr.readInt();      
		unitMass=wr.readInt();         
		landCarryCapacity=wr.readInt();
		seaCarryCapacity=wr.readInt(); 
		maxHealth=wr.readInt();		   
		attackProbability=wr.readInt();
		attackStrength=wr.readInt();   
		landDefenseMoving=wr.readInt();      
		landDefenseStill=wr.readInt();      
		seaDefence=wr.readInt();
		personnelSupport=wr.readInt();
		crewNeeded=wr.readInt();
		degradesTo=wr.readString();
		canTransformInto=wr.readString();
		transformationMaterial=wr.readString();
		disassemblesInto=wr.readString();
		possibleOrders=wr.readString();
		possibleBuilds=wr.readString();
		buildMaterialNeeded=wr.readString();
		requiredTerrain=wr.readString();
		possibleAssemblies=wr.readString();
	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		
		ww.writeWord(typeName);
		ww.writeInt(buildMoneyCost);
		ww.writeInt(buildCapacityCost);
		ww.writeInt(unitIncome);
		ww.writeInt(landMoveCost);
		ww.writeInt(seaMoveCost);
		ww.writeInt(unitMass);
		ww.writeInt(landCarryCapacity);
		ww.writeInt(seaCarryCapacity);
		ww.writeInt(maxHealth);
		ww.writeInt(attackProbability);
		ww.writeInt(attackStrength);
		ww.writeInt(landDefenseMoving);
		ww.writeInt(landDefenseStill);
		ww.writeInt(seaDefence);
		ww.writeInt(personnelSupport);
		ww.writeInt(crewNeeded);
		ww.writeString(degradesTo);
		ww.writeString(canTransformInto);
		ww.writeString(transformationMaterial);
		ww.writeString(disassemblesInto);
		ww.writeString(possibleOrders);
		ww.writeString(possibleBuilds);
		ww.writeString(buildMaterialNeeded);
		ww.writeString(requiredTerrain);
		ww.writeString(possibleAssemblies);		
	}	

	public EmpireUnitTypeList getEmpireUnitTypeList()
	{
		D1Container p=this.getParent();
		return (EmpireUnitTypeList)p;
	}
	
	// Add but don't let sum exceed max
	static public int addMax(int a, int b, int max)
	{
		long c=a+b;
		if (c>max) 
		{
			return max;
		}
		return (int)c;
	}
	
	public int getMass()
	{
		if (unitMass==EmpireUnitTypeList.cr)
		{
			int sum=0;
			if (disassemblesInto.length()>0)
			{
				EmpireUnitTypeList empireUnitTypeList = getEmpireUnitTypeList();
				
				String materialNeededNames[]=WordReader.split(disassemblesInto);
								
				// Now check what we need
				for(int i=0;i<materialNeededNames.length;++i)
				{
					EmpireUnitType j=empireUnitTypeList.getUnitType(materialNeededNames[i]);
					if (j!=null)
					{
						sum=addMax(sum, j.getMass(), EmpireUnitTypeList.InfM);
					}
					else
					{
						sum=EmpireUnitTypeList.InfM;
					}
				}
			}
			else
			{
				sum=EmpireUnitTypeList.InfM;
			}
			unitMass=sum;
		}
		return unitMass;
	}


	public int getIncome()
	{
		if (unitIncome==EmpireUnitTypeList.cr)
		{
			int sum=0;
			if (disassemblesInto.length()>0)
			{
				EmpireUnitTypeList empireUnitTypeList = getEmpireUnitTypeList();
				
				String materialNeededNames[]=WordReader.split(disassemblesInto);
								
				// Now check what we need
				for(int i=0;i<materialNeededNames.length;++i)
				{
					EmpireUnitType j=empireUnitTypeList.getUnitType(materialNeededNames[i]);
					if (j!=null)
					{
						sum+=j.getIncome();
					}
				}
			}
			this.unitIncome=sum;
		}
		return this.unitIncome;
	}

	
	// To have workers (or other units containing personnel, see getWorkforceProvided) the state must have some population to support it, cities (town halls and castles) provide this support. This tells how much support this unit produces.
	public int getPersonnelSupport()
	{
		if (personnelSupport==EmpireUnitTypeList.cr)
		{
			int sum=0;
			if (disassemblesInto.length()>0)
			{
				EmpireUnitTypeList empireUnitTypeList = getEmpireUnitTypeList();
				String materialNeededNames[]=WordReader.split(disassemblesInto);
								
				// Now check what we need
				for(int i=0;i<materialNeededNames.length;++i)
				{
					EmpireUnitType j=empireUnitTypeList.getUnitType(materialNeededNames[i]);
					if (j!=null)
					{
						sum+=j.getPersonnelSupport();
					}
				}
			}
			// Now that we calculated the value we can keep it for future use.
			this.personnelSupport=sum;
		}
		return this.personnelSupport;
	}
	
	// To have workers (or other units containing personnel, see getWorkforceProvided) the state must have some population to support it, cities (town halls and castles) provide this support. This tells how much support this unit produces.
	public long getPersonnelSupportProduced()
	{
		final long ps=getPersonnelSupport();
		return (ps>0)?ps:0;
	}

	// This will tell how much work force support that is needed to have this unit
	public long getPersonnelSupportConsumed()
	{
		final long ps=getPersonnelSupport();
		return (ps<0)?-ps:0;
	}

	
	// To have workers (or other units containing personnel, see getWorkforceProvided) the state must have some population to support it, cities (town halls and castles) provide this support. This tells how much support this unit produces.
	public int getCrewNeeded()
	{
		if (crewNeeded==EmpireUnitTypeList.cr)
		{
			int sum=0;
			if (disassemblesInto.length()>0)
			{
				EmpireUnitTypeList empireUnitTypeList = getEmpireUnitTypeList();
				String materialNeededNames[]=WordReader.split(disassemblesInto);
								
				// Now check what we need
				for(int i=0;i<materialNeededNames.length;++i)
				{
					EmpireUnitType j=empireUnitTypeList.getUnitType(materialNeededNames[i]);
					if (j!=null)
					{
						sum+=j.getCrewNeeded();
					}
				}
			}
			// Now that we calculated the value we can keep it for future use.
			this.crewNeeded=sum;
		}
		return this.crewNeeded;
	}
	
	
	// Units containing personnel produce work force, this tells how much personnel the unit contains and how much work force it gives. 
	// A unit with 1 here provides 1 work to other units but consumes one personnel support at the same time.
	public long getWorkforceProvided()
	{
		// work force provided us same as personnel support consumed
		return getPersonnelSupportConsumed();
	}


	// Many units need a crew to function/perform orders, this tells how much crew the unit needs. See also getWorkforceProvided.
	public long getWorkforceNeeded()
	{
		return getCrewNeeded();
	}
	
	
	public int getBuildMoneyCost()
	{
		if (buildMoneyCost==EmpireUnitTypeList.cr)
		{
			int sum=0;
			if (disassemblesInto.length()>0)
			{
				EmpireUnitTypeList empireUnitTypeList = getEmpireUnitTypeList();
				String materialNeededNames[]=WordReader.split(disassemblesInto);
								
				// Now check what we need
				for(int i=0;i<materialNeededNames.length;++i)
				{
					EmpireUnitType j=empireUnitTypeList.getUnitType(materialNeededNames[i]);
					if (j!=null)
					{
						sum+=j.getBuildMoneyCost();
					}
				}
			}
			// Now that we calculated the value we can keep it for future use.
			this.buildMoneyCost=sum;
		}
		return this.buildMoneyCost;
	}

	
	public int getBuildCapacityCost()
	{
		if (buildCapacityCost==EmpireUnitTypeList.cr)
		{
			int sum=0;
			if (disassemblesInto.length()>0)
			{
				EmpireUnitTypeList empireUnitTypeList = getEmpireUnitTypeList();
				String materialNeededNames[]=WordReader.split(disassemblesInto);
								
				// Now check what we need
				for(int i=0;i<materialNeededNames.length;++i)
				{
					EmpireUnitType j=empireUnitTypeList.getUnitType(materialNeededNames[i]);
					if (j!=null)
					{
						sum+=j.getBuildCapacityCost();
					}
				}
			}
			// Now that we calculated the value we can keep it for future use.
			this.buildCapacityCost=sum;
		}
		return this.buildCapacityCost;
	}

	public int getInitialMovePoints()
	{
		return -getBuildCapacityCost();
	}


}
