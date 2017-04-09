//
//Copyright (C) 2013 Henrik Björkman www.eit.se
//
//History:
//Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;



//import java.util.Random;


//import se.eit.rsb_package.*;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;


public class EmpireUnitTypeList extends EmpireBase {
	
    final int nTypes=3;
    final static public int InfE=EmpireUnitType.InfiniteBuildCostEconomical;
    final static public int InfCC=EmpireUnitType.InfiniteBuildCostCapacity;
    final static public int InfMC=EmpireUnitType.InfiniteMoveCost;
    final static public int InfM=EmpireUnitType.InfiniteMass;
    final static public int cr=-9999; // cr= calculate recursively from parts
    
	// If exceeding its available work force then state must pay money for the difference.	
    // This defines how much.
    final static public int workforceDeficitCostPerUnit=10;
    
    /*
    public final EmpireUnitType types[]={
    		new EmpireUnitType("city", 0, 1, 10, 999, 999, 0),
    		new EmpireUnitType("inf", 10, 0, -1,   5, 15, 0),
    		new EmpireUnitType("ship",10, 0, -2, 999,   1, 0)
    };*/

    
	public static String className()
	{	
		return EmpireUnitTypeList.class.getSimpleName();	
	}

	public EmpireUnitTypeList(D1Base parent, String name) 
	{
		super();
		parent.addObject(this);
		this.regName(name);

		generateTypes();
	}

	public EmpireUnitTypeList()
	{	
		super();
	}
	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
	}	

	// If EmpireUnitType are rearranged in generateTypes this may need to be updated.
    final static public int NothingType=0;
    final static public int TowerType=1;
    //final static public int CityType=2;
    final static public int CrewType=6;
	
	public void generateTypes()
	{
		//EmpireUnitType c=null;
		//                         type               build     build   income     land      sea    mass   carry   carry       max    attack   attack     land    land      sea    personnel   crew  	degrades    can transform into            material needed for                          disassembles                                                                    possible                                              possible builds    materials needed       required terrain     possible 		
		//                         name                cost  capacity              move     move          capacity   cap    health       hit strength  defense defense  defense    support    needed	to		    more than one type                transformation                                   to	                                                                    orders                      list here what units this unit can build         to be built	           to build	   assemblies
	 	//                                            money      cost              cost     cost            land     sea           probability    max   moving   still                        				        can be listed here                                                                                                                                                                                      
/* 0 */ new EmpireUnitType(this, "nothing",           InfE,    InfCC,       0,   InfMC,   InfMC,      0,      0,      0,        1,        0,       0,       0,      0,       0,      0,          0,         "", 	           "",                                    "",                                    "",                                                                          "",                                                          "",                 "",                    "",      "");
/* 1 */ new EmpireUnitType(this, "castle",            InfE,    InfCC,      50,   InfMC,   InfMC,   InfM,   InfM,      0,       80,      100,      10,     100,    200,       1,      4,          0,         "",                "",                                    "",                                    "",                                                  "build unload cancelOrder",                                                      "crew",                 "",           "land city",      "");
/* 2 */ new EmpireUnitType(this, "townhall",         10000,      200,      50,   InfMC,   InfMC,   InfM,   InfM,      0,       10,      100,      10,     100,    200,       1,      4,          0,         "",	               "",                                    "",                                    "",                                                  "build unload cancelOrder",                                                      "crew",                 "",           "land city",      "");
/*   */ new EmpireUnitType(this, "docks",             2000,      200,       0,   InfMC,   InfMC,   InfM,      0,      0,       10,        0,       0,       1,     10,       1,      0,          0,         "",		           "",                                    "",                                    "",                                                   "build scrap cancelOrder", "shipAndCrew bigShipWithCrew frigateAndCrew airshipAndCrew",                 "",            "land sea",      "");
/*   */ new EmpireUnitType(this, "farm",              2000,      200,       0,   InfMC,   InfMC,   InfM,      0,      0,       10,        0,       0,       1,     10,       1,      0,          0,      	"",	               "",                                    "",                                    "",                                                   "build scrap cancelOrder",                                      "wagonAndCrew cavalry",                 "",          "land field",      "");
/*   */ new EmpireUnitType(this, "mine",              2000,      200,       0,   InfMC,   InfMC,   InfM,      0,      0,       10,        0,       0,       1,     10,       1,      0,          0,         "",                "",                                    "",                                    "",                                                   "build scrap cancelOrder",          "muskets infantry infantry4x cannon cannonAndCrew",                 "", "land mineralDeposit",      "");
/* 6 */ new EmpireUnitType(this, "crew",             10000,      200,     -10,     100,     500,     10,     15,     10,       10,       50,       5,      50,    200,      10,     -1,          0,         "",                "",                                    "",                                    "",           "build moveTo goTo unload assemble disassemble scrap cancelOrder",                                  "docks farm mine townhall",                 "",                    "", "cannonAndCrew cavalry wagonAndCrew shipAndCrew frigateAndCrew airshipAndCrew");
/*   */ new EmpireUnitType(this, "infantry",         10000,      200,     -10,     100,     500,     10,     15,      0,       10,      100,      10,     100,    200,      10,     cr,         cr,  "muskets",	     "infantry4x", "infantry infantry infantry infantry",                        "crew muskets",       "moveTo goTo unload transform assemble disassemble scrap cancelOrder",                                                          "",                 "",                "land", "infantry4x");
/*   */ new EmpireUnitType(this, "infantry4x",          cr,       cr,      cr,     100,     500,     cr,     60,      0,       10,      400,      10,     400,    800,      10,     cr,         cr,  "muskets",                "",                                    "", "infantry infantry infantry infantry",                "moveTo goTo unload transform disassemble scrap cancelOrder",                                                          "",                 "",                "land",      "");
/*   */ new EmpireUnitType(this, "horse",            10000,      200,     -10,      30,     500,     30,     45,      0,       10,      100,      10,     100,    200,      10,      0,          1,         "",	        "cavalry",                          "horse crew",                               "wagon",       "moveTo goTo unload transform assemble disassemble scrap cancelOrder",                                                          "",                 "",                "land", "cavalry");
/*   */ new EmpireUnitType(this, "wagon",             5000,      200,     -10,     100,     500,     30,     90,      0,       10,      100,      10,     100,    200,      10,      0,          1,         "",           "horse",                               "wagon",                               "horse",                "moveTo goTo unload transform disassemble scrap cancelOrder",                                                          "",                 "",                "land",      "");
/*   */ new EmpireUnitType(this, "cannon",            3000,      200,      -3,   InfMC,   InfMC,     40,     45,      0,       10,      100,      80,     100,    200,      10,      0,          1,         "",   "cannonAndCrew",                         "cannon crew",                                    "",                            "moveTo goTo unload transform scrap cancelOrder",                                                          "",                 "",                "land", "cannonAndCrew");
/*   */ new EmpireUnitType(this, "muskets",           1000,      100,      -1,   InfMC,   InfMC,     10,     11,      0,       10,      100,      10,     100,    200,      10,      0,          1,         "",	       "infantry",                        "muskets crew",                                    "",                            "moveTo goTo unload transform scrap cancelOrder",                                                          "",                 "",                "land",      "");
/*   */ new EmpireUnitType(this, "ship",              3000,      100,      -3,   InfMC,      30,    200,      0,    400,       10,      100,       5,     100,    100,     100,      0,          1,         "",		           "",                                    "",                                    "",                                      "moveTo goTo unload scrap cancelOrder",                                                          "",                 "",                 "sea",      "");
/*   */ new EmpireUnitType(this, "bigShip",          40000,      400,      -8,   InfMC,      20,    360,      0,    600,       40,      100,       5,     100,    100,     100,      0,          1,         "", "bigShipWithCrew",                        "bigShip crew",                           "ship ship",       "moveTo goTo unload transform assemble disassemble scrap cancelOrder",                                                          "",                 "",                 "sea", "bigShipWithCrew");
/*   */ new EmpireUnitType(this, "bigShipWithCrew",     cr,       cr,      cr,   InfMC,      20,     cr,      0,    600,       40,      100,       5,     100,    100,     100,     cr,         cr,  "bigShip",  "frigateAndCrew",       "bigShipWithCrew cannonAndCrew",                        "bigShip crew",       "moveTo goTo unload transform assemble disassemble scrap cancelOrder",                                                          "",                 "",                 "sea", "frigateAndCrew");
/*   */ new EmpireUnitType(this, "airship",          30000,      200,      -8,      20,      20,    200,    250,    250,       10,      100,      10,     100,    100,     100,      0,          1, 		"",                "",                                    "",                             "muskets",                          "moveTo goTo unload disassemble scrap cancelOrder",                                                          "",          "muskets",                    "",      "");
/*   */ new EmpireUnitType(this, "cavalry",             cr,       cr,      cr,      30,     500,     cr,     45,      0,       10,      100,      10,     100,    200,      10,     cr,         cr,    "horse",      "wagon crew",                             "cavalry",                          "horse crew",                "moveTo goTo unload transform disassemble scrap cancelOrder",                                                          "",                 "",                "land",      "");
/*   */ new EmpireUnitType(this, "cannonAndCrew",       cr,       cr,      cr,     500,   InfMC,     cr,     50,      0,       10,      100,      80,     100,    200,      10,     cr,         cr,    "canon",                "",                                    "",                         "cannon crew",                          "moveTo goTo unload disassemble scrap cancelOrder",                                                          "",                 "",                "land",      "");
/*   */ new EmpireUnitType(this, "wagonAndCrew",        cr,       cr,      cr,     100,     500,     cr,    105,      0,       10,      100,      10,     100,    200,      10,     cr,         cr,    "wagon",      "horse crew",                        "wagonAndCrew",                          "wagon crew",                "moveTo goTo unload transform disassemble scrap cancelOrder",                                                          "",                 "",                "land",      "");
/*   */ new EmpireUnitType(this, "shipAndCrew",         cr,       cr,      cr,   InfMC,      30,     cr,      0,    400,       10,      100,       5,     100,    100,     100,     cr,         cr, 	"ship",	               "",                                    "",                           "ship crew",                          "moveTo goTo unload disassemble scrap cancelOrder",                                                          "",                 "",                 "sea",      "");
/*   */ new EmpireUnitType(this, "frigateAndCrew",      cr,       cr,      cr,   InfMC,      20,     cr,      0,    600,       40,      100,      60,     100,    100,     100,     cr,         cr,  "bigShip",	               "",                                    "",       "bigShipWithCrew cannonAndCrew",                          "moveTo goTo unload disassemble scrap cancelOrder",                                                          "",           "cannon",                 "sea",      "");
/*   */ new EmpireUnitType(this, "airshipAndCrew",      cr,       cr,      cr,      20,      20,     cr,    250,    250,       10,      100,      10,     100,    100,     100,     cr,         cr,  "airship",    	           "",                                    "",                        "airship crew",                "moveTo goTo unload transform disassemble scrap cancelOrder",                                                          "",          "muskets",                    "",      "");
	//
	// Explanations:
	// crew: Positive number means the unit contributes manpower to the stack it is in. A negative means unit needs that much manpower to operate (build, move or attack).
	// required terrain: Unit will need all listed terrain properties to be built or operate (move, attack or build). Nothing here means unit can operate anywhere. 
	
	}

	
    public int getNUnitTypes()
    {
    	//return types.length;
    	return this.getNSubObjects();
    }

    public String getUnitTypeName(int i)
    {
    	//return types[i].getName();
    	return getUnitType(i).getName();
    }
	
    public EmpireUnitType getUnitType(int i)
    {
    	return (EmpireUnitType)this.getObjFromIndex(i);
   	
    }
    

    public EmpireUnitType getUnitType(String name)
    {
    	//D1Base db = this.findDbNamedRecursive(name, 1);
		D1Base db = this.getChildFromIndexOrName(name);

    	if (db instanceof EmpireUnitType)
    	{
    		return (EmpireUnitType)db;
    	}
    	
    	return null;
    }

    public int getUnitTypeIndex(String name)
    {
		D1Base db=this.getChildFromIndexOrName(name);
	
		if (db!=null)
		{
			return db.getIndex();
		}
		
		debug("Did not find unit type "+name);
		
		return -1;
    }

	public long getMoneyBonus()
	{
		EmpireUnitType eut=getUnitType(TowerType);
		return eut.getIncome()/2;
	}
	
	public long getPesonnelBonus()
	{
		EmpireUnitType eut=getUnitType(TowerType);
		return eut.getPersonnelSupportProduced()/2;		
	}


	// This should be moved to some other class, EmpireUnitTypeList perhaps?
	public D1IdObj findHeaviestUnit(D1List<EmpireUnitOrSector> idList)
	{
		//final EmpireWorld ew=getEmpireWorld();

		long heaviestFoundMass=-1;
		D1IdObj heaviestFoundUnit=null;
		
		for(int i=0;i<idList.getCapacity();i++)
		{
			EmpireUnitOrSector euos=idList.get(i);
			if ((euos!=null) && (euos instanceof EmpireUnit))
			{
				debug("testing ~"+euos.getId());
				EmpireUnit eu=(EmpireUnit)euos;
				final EmpireUnitType eut=this.getUnitType(eu.unitType);
				if (eut.getMass()>heaviestFoundMass)
				{
					heaviestFoundMass=eut.getMass();
					heaviestFoundUnit=euos;
				}
			}
		}
		
		return heaviestFoundUnit;
	}

}
