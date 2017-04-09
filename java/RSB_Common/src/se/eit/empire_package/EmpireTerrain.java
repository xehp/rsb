//
//Copyright (C) 2013 Henrik Björkman www.eit.se
//
//History:
//Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;




import java.util.concurrent.atomic.AtomicInteger;

//import se.eit.rsb_package.*;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;


//Only objects of type "EmpireSector" shall be stored in objects of this class.



public class EmpireTerrain extends EmpireBase {
	

	//StringFifo stringFifo;	
	
	public int sizeX;
	public int sizeY;
	
	//public int terrain[];


	
	final static int INFINITE_MOVE_COST=0x7FFFFFFF;
	final static int N_SECTOR_NEIGHBORS_1=6;   // N_SECTOR_NEIGHBORS_2+6
	final static int N_SECTOR_NEIGHBORS_2=18;  // N_SECTOR_NEIGHBORS_2+12
	final static int N_SECTOR_NEIGHBORS_3=36;  // N_SECTOR_NEIGHBORS_2+18
	final static int N_SECTOR_NEIGHBORS_4=60;  // N_SECTOR_NEIGHBORS_3+24;
	final static int SAME_SECTOR=-1;
	final static int maxNations = 64;
	final static int ETERNITY = 1000000;
		 
	final static int N_CITYS = 8;
	final static int N_ISLANDS = 6;
	final static int N_BEACH_FACTOR = 5;
	final static int N_FIELDS_FACTOR = 10;
	final static int N_MINERAL_FACTOR = 30;
	
	// TODO: Perhaps we can move these to EmpireSector.java
	// Bitmasks for the property of a sector (max 32 bits, since int is used to store this)
	final static int SEA_TERRAIN_MASK = 1; // If true ships can move here
	final static int LAND_TERRAIN_MASK = 2;    // If true land units can move here
	final static int CITY_TERRAIN_MASK = 4; // If true a city can be here  
	final static int FIELD_TERRAIN_MASK = 8; // If true a farm can be placed here   
	final static int MINERAL_TERRAIN_MASK = 0x10; // If true a metal production can be placed here
 
 
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireTerrain.class.getSimpleName();	
	}


	public EmpireTerrain(D1Base parent, String name, int size, int nStates) 
	{
		super();
		parent.addObject(this);
		this.regName(name);
		generateTerrain(size, nStates);
	}

	public EmpireTerrain()
	{	
		super();
	}

	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		//debug("created unit " + name + " at "+id);
		// It seems we should check that database is locked here. It is not always...
		sizeX=wr.readInt();
		sizeY=wr.readInt();		
		debug("readSelf "+sizeX+" "+sizeY);
		
		/*terrain=new int[sizeX*sizeY];
		for(int i=0;i<sizeX*sizeY;i++)
		{
			terrain[i]=wr.readInt();
		}*/
	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(sizeX);
		ww.writeInt(sizeY);
		
		/*for(int i=0;i<sizeX*sizeY;i++)
		{
			ww.writeInt(terrain[i]);
		}*/
	}	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);					
		pw.println("sizeX "+sizeX);		
		pw.println("sizeY "+sizeY);		
	}

	
	public static int getNSectorNeighbors(int distance)
	{
		switch(distance)
		{
		case 0: return 0;
		case 1: return N_SECTOR_NEIGHBORS_1;
		case 2: return N_SECTOR_NEIGHBORS_2;		
		case 3: return N_SECTOR_NEIGHBORS_3;
		case 4: return N_SECTOR_NEIGHBORS_4;
		default: return getNSectorNeighbors(distance-1)+distance*6;
		}
	}

	public static int getDistanceFromNeighborNr(int nn)
	{
		int d=0;

		while(true)
		{			
			if (nn<getNSectorNeighbors(d))
			{
				return d;
			}	
			++d;
		}
	}

	
	public boolean isNextToTerrain(int pos, int terrainMask, int distance)
	{
		final int n=getNSectorNeighbors(distance);
		
		for(int i=0;i<n;i++)
		{
			int sid=getSectorIndexByDirection(pos, i);
			if ((getTerrainMask(sid) & terrainMask)!=0)
			{
				return true;
			}
		}
		return false;
	}
	
	
	
	public void makeCanalToSea(int pos)
	{
		final EmpireWorld ew=getEmpireWorld();
		
		for(int i=0;i<100;i++)
		{			
			int direction=ew.generator.nextInt(N_SECTOR_NEIGHBORS_1); // try randomly

			int sid=getSectorIndexByDirection(pos, direction);
			if (isNextToTerrain(sid, SEA_TERRAIN_MASK, 1))
			{
				setTerrainMask(sid,SEA_TERRAIN_MASK);
				return;
			}
		}
	}

	
	public void generateTerrain(int size, int nStates)
	{
		final EmpireWorld ew=getEmpireWorld();

		ew.lockWrite();

		try
		{
		
			sizeX=size;
			sizeY=size;
	
			// Generate the terrain
			//terrain = new int[sizeX*sizeY];
	
			/*for(int x=0;x<sizeX;x++)
			{
				for(int y=0;y<sizeY;y++)
				{
					new EmpireSector(this, "x"+x+"y"+y);
				}
			}*/
			
			int landSectors=0;
			
			// create some islands/continents
			for(int n=0;n<size/5;n++)
			{
				//int x=sizeX/4+ew.generator.nextInt(sizeX/2);
				//int y=sizeY/4+ew.generator.nextInt(sizeY/2);
				//int sid0=getSectorId(x,y);
				int sid0=ew.generator.nextInt(sizeX*sizeY);

				
				for(int i=-1;i<N_SECTOR_NEIGHBORS_3;i++)
				{
					int sid=getSectorIndexByDirection(sid0, i);
					if ((getTerrainMask(sid) & LAND_TERRAIN_MASK) == 0)  // not land here
					{
						// this was sea, now it is land
						clrTerrainMask(sid,SEA_TERRAIN_MASK);
						setTerrainMask(sid,LAND_TERRAIN_MASK);
						landSectors++;
					}
				}
			}
	
			
			// create more land if needed, shall be at least 25%
			while ((landSectors*4)<(sizeX*sizeY))
			{
				// Find a sea sector that is next to some land
				int j=1000;
				while (--j>=0)
				{
					int sid=ew.generator.nextInt(sizeX*sizeY);
					if ((getTerrainMask(sid) & LAND_TERRAIN_MASK) == 0)  // not land here
					{						
						// It must be next to at least one land sector
						if (isNextToTerrain(sid, LAND_TERRAIN_MASK, 1))
						{
							// this was sea, now it is land
							clrTerrainMask(sid,SEA_TERRAIN_MASK);
							setTerrainMask(sid, LAND_TERRAIN_MASK);
							landSectors++;
							break;
						}
					}
				}
			}

			
			// Count how many possible beaches we have.
			int nCoastal=0;
			for(int pos=0; pos<sizeX*sizeY; pos++)
			{
				// Is it land
				if ((getTerrainMask(pos) & LAND_TERRAIN_MASK) > 0)
				{
					// this is land
					
					// is it next to at least one sea sector
					if (isNextToTerrain(pos, SEA_TERRAIN_MASK, 1))
					{
						// it is	
						nCoastal++;
					}
				}
				
			}
			
			// Add some beaches
			for(int i=0; i<nCoastal/N_BEACH_FACTOR; i++)
			{
				// Find a land sector
				int j=1000;
				while (--j>=0)
				{
					int pos=ew.generator.nextInt(sizeX*sizeY);
					if ((getTerrainMask(pos) & LAND_TERRAIN_MASK) > 0)
					{
						// this is land
						
						// It must be next to at least one sea sector
						if (isNextToTerrain(pos, SEA_TERRAIN_MASK, 1))
						{
							// this is land, now with a river or canal	
							setTerrainMask(pos, SEA_TERRAIN_MASK);
							break;
						}
					}
				}
			}

			
			// Add some fields
			for(int i=0; i<landSectors/N_FIELDS_FACTOR; i++)
			{
				// Find a land sector
				int j=1000;
				while (--j>=0)
				{
					int pos=ew.generator.nextInt(sizeX*sizeY);
					if ((getTerrainMask(pos)&LAND_TERRAIN_MASK) != 0)
					{
						// this is land, now with a field	
						setTerrainMask(pos, FIELD_TERRAIN_MASK);
						break;
					}
				}
			}
			
		
			// Add some mineral deposits
			for(int i=0; i<landSectors/N_MINERAL_FACTOR; i++)
			{
				// Find a land sector
				int j=1000;
				while (--j>=0)
				{
					int pos=ew.generator.nextInt(sizeX*sizeY);
					if ((getTerrainMask(pos)&LAND_TERRAIN_MASK) != 0)
					{
						// this is land, now with a mineral deposit	
						setTerrainMask(pos, MINERAL_TERRAIN_MASK);
						break;
					}
				}
			}
			

			
			// Add some terrain where cities can be built, twice as many as number of states
			for(int i=0; i<nStates*2; i++)
			{
				// Find a land sector
				int j=10000; // used to prevent eternal loop, if something went wrong
				while (--j>=0)
				{
					int pos=ew.generator.nextInt(sizeX*sizeY);
					if ((getTerrainMask(pos) & LAND_TERRAIN_MASK) != 0)
					{
						// this was land
						
						// don't want to be too close to other cities, if possible
						if ((j<100) || (!isNextToTerrain(pos, CITY_TERRAIN_MASK, 1)))
						{				
							setTerrainMask(pos, SEA_TERRAIN_MASK);
							setTerrainMask(pos, CITY_TERRAIN_MASK);
	
							// If city is not next to sea check make a canal/river if it is not too far from sea.
							if ((!isNextToTerrain(pos, SEA_TERRAIN_MASK, 1)) && (isNextToTerrain(pos, SEA_TERRAIN_MASK, 2)))
							{
								makeCanalToSea(pos);						
							}
														
							break;
							
						}			
					}
				}
			}

						
			
			
			// Add some castles, one for each state.
			for(int i=0; i<nStates; i++)
			{
				final EmpireStatesList enl = ew.getEmpireStatesList();
				final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
				final EmpireState es=enl.getEmpireNation(i);
				
				es.setMoney(EmpireUnitType.defaultStartMoneyForStates);
				es.setOwner(i);
				
				// Find a city sector
				int j=1000000; // to avoid eternal loop
				while (--j>0)
				{
					int pos=ew.generator.nextInt(sizeX*sizeY);
					if (  ((getTerrainMask(pos)&CITY_TERRAIN_MASK)!=0)   ||   ((j<1000) && ((getTerrainMask(pos)&LAND_TERRAIN_MASK)!=0))  )
					{
						// this was land possible to put a city on
							
						EmpireSector s = (EmpireSector)getObjFromIndex(pos);

						// Is it free
						if ((s.getNSubObjects()==0) || (j<1000))
						{							
							setTerrainMask(pos, CITY_TERRAIN_MASK);
							s.setOwner(i);
							es.homeSectorId=s.getId();
							
							// There must be some initial units in the city
							
							// Add a castle alias tower.
							EmpireUnit eu2=new EmpireUnit();
							eu2.linkSelf(s);
							eu2.regName("t"+eu2.getId());
							eu2.unitType=EmpireUnitTypeList.TowerType;
							eu2.owner=i;
							eu2.unitHealth=eul.getUnitType(EmpireUnitTypeList.TowerType).maxHealth;
							eu2.lookAround(3);

							EmpireUnit eu4=new EmpireUnit();
							eu4.linkSelf(eu2);
							eu4.regName("c"+eu4.getId());
							eu4.unitType=EmpireUnitTypeList.CrewType;
							eu4.owner=i;

							/*EmpireUnit eu3=new EmpireUnit(eu, "i"+i);
							eu3.unitType=EmpireUnitTypeList.InfType;
							eu3.owner=i;
							
							EmpireUnit eu4=new EmpireUnit(eu, "i"+i);
							eu4.unitType=EmpireUnitTypeList.InfType;
							eu4.owner=i;*/

							for(int i2=-1;i2<N_SECTOR_NEIGHBORS_1;i2++)
							{
								int sid2=getSectorIndexByDirection(pos, i2);
								EmpireSector s2 = (EmpireSector)getObjFromIndex(sid2);
								s2.owner=i;
							}
							
							
							break;
							
						}
							
					}
				}
				if (es.homeSectorId<0)
				{
					debug("Failed to find a city for nation "+i);
				}
			}
			
			
		}
		finally
		{
			ew.unlockWrite();
		}

		/*
		for(int i=0;i<sizeX/2;++i)
		{
			debug("getNSectorNeighbors "+i+" "+getNSectorNeighbors(i));
		}
		*/
		/*
		for(int i=0;i<168;++i)
		{
			debug("getDistanceFromNeighborNr "+i+" "+getDistanceFromNeighborNr(i));
		}*/
		//debugLogSectorIndexByDirection(getSectorId(sizeX/2, sizeY/2));
	}

	// sidx is sector index not object ID
	public EmpireSector getSector(int sidx)
	{
		EmpireSector s = (EmpireSector)getObjFromIndex(sidx);
		
		if (s==null)
		{
			// if there is no object for this index create one
			s = new EmpireSector();
			s.setIndex(sidx); // We want this object to be at the given index
			s.setTerrain(SEA_TERRAIN_MASK); // Initially all sectors are sea
			this.addObject(s);
			s.regName("x"+getSectorX(sidx)+"y"+getSectorY(sidx));
			s.setUpdateCounter();
		}
		
		return s;
	}
	

	public int getTerrainMask(int sidx)
	{
		EmpireSector s = (EmpireSector)getObjFromIndex(sidx);

		if (s==null)
		{
			return SEA_TERRAIN_MASK; // All undefined sectors are sea
		}
		
		return s.getTerrain();
	}
	
	/*
	public void setTerrain(int sid, int terrain)
	{
		//return terrain[sid];
		EmpireSector s = getSector(sid);
		s.setTerrain(terrain);
	}
	*/

	public void setTerrainMask(int sidx, int terrainMask)
	{
		//return terrain[sid];
		EmpireSector s = getSector(sidx);
		s.setTerrain(s.getTerrain() | terrainMask);
	}
	
	// this is not tested yet
	public void clrTerrainMask(int sidx, int terrainMask)
	{
		//return terrain[sid];
		EmpireSector s = getSector(sidx);
		s.setTerrain(s.getTerrain() & ~terrainMask);
	}

	public int getTerrainMask(int x, int y)
	{
		return getTerrainMask(getSectorId(x,y));
	}
	
	// Id is not same as object ID, this Id is more like index
	public int getSectorId(int x, int y)
	{
		return x+sizeX*y;
	}
	
	public int getSectorX(int idx)
	{
		return idx%sizeX;
	}
	
	public int getSectorY(int idx)
	{
		return idx/sizeX;
	}
	
	// Returns -1 if no direction (from is same as to).
	// 0 = right
	// 1 = up-right
	// 2 = up-left
	// 3 = left
	// 4 = down-left
	// 5 = down-right
	public int getDirection(int from, int to)
	{
		final int fromX=getSectorX(from);
		final int fromY=getSectorY(from);
		final int toX=getSectorX(to);
		final int toY=getSectorY(to);
		int dX=toX - fromX;
		int dY=toY - fromY;
		
		// Wrap around X
		if (dX>sizeX/2)
		{
			dX-=sizeX;
		}
		else if (dX<-sizeX/2)
		{
			dX+=sizeX;
		}
		
		// Wrap around Y
		if (dY>sizeY/2)
		{
			dY-=sizeY;
		}
		else if (dY<-sizeY/2)
		{
			dY+=sizeY;
		}

		
		
		if (dY>0)
		{
			// 04  or 05
			if (dX>0)
			{
				return 5;
			}
			else if (dX<0)
			{
				return 4;
			}
			else
			{
				if ((fromY%2)==0)
				{
					return 5;					
				}
				else
				{
					return 4;					
				}				
			}
		}
		else if (dY<0)
		{
			// 02  or 01
			if (dX>0)
			{
				return 1;
			}
			else if (dX<0)
			{
				return 2;
			}
			else
			{
				if ((fromY%2)==0)
				{
					return 1;					
				}
				else
				{
					return 2;					
				}
			}
		}
		else
		{
			//  03, -1 or 00
			if (dX>0)
			{
				return 0;
			}
			else if (dX<0)
			{
				return 3;
			}
			else
			{
				return -1;
			}	
		}
	}

/*
Sector numbering if size is 32x32 
00  01  02  04 ...
  32  33  34  35 ...  
64  65  66  67 ...
  96  97  98  99 ...
.
.
.


Directions
        24  23  22  21
      25  10  09  08  20
    26  11  02  01  07  19
  27  12  03  -1  00  06  18
    28  13  04  05  17  35
      29  14  15  16  34
        30  31  32  33


*/
	
	
	public int getSectorToRight(int id)
	{
		int x=id%sizeX;
		int y=id/sizeX;
		if (x>=sizeX-1)
		{
			x=0;
		}
		else
		{
			x++;
		}
		return x+y*sizeX;
	}

	public int getSectorToLeft(int id)
	{
		int x=id%sizeX;
		int y=id/sizeX;
		if (x<=0)
		{
			x=sizeX-1;
		}
		else
		{
			x--;
		}
		return x+y*sizeX;
	}

	private int getSectorToUpp(final int id)
	{
		int x=id%sizeX;
		int y=id/sizeX;
		if (y==0)
		{
			y=sizeY-1;
		}
		else
		{
			y--;
		}
		return x+y*sizeX;
	}
	
	private int getSectorToDown(final int id)
	{
		int x=id%sizeX;
		int y=id/sizeX;

		if (y==sizeY-1)
		{
			y=0;
		}
		else
		{
			y++;
		}	
		return x+y*sizeX;
	}
	
	
	public int getSectorToUppRight(final int id)
	{
		//int x=id%sizeX;
		int y=id/sizeX;

		// If we are at even row then move just up.
		if ((y%2)==0)
		{
			return getSectorToUpp(id);
		}
		
		// Otherwise move left and up		
		return getSectorToRight(getSectorToUpp(id));
	}

	public int getSectorToUppLeft(final int id)
	{
		//int x=id%sizeX;
		int y=id/sizeX;

		// If we are at uneven row then move just up.
		if ((y%2)!=0)
		{
			return getSectorToUpp(id);
		}
		
		// Otherwise move left and up		
		return getSectorToLeft(getSectorToUpp(id));
	}

	public int getSectorToLowLeft(final int id)
	{
		//int x=id%sizeX;
		int y=id/sizeX;

		// If we are at uneven row then move just down.
		if ((y%2)!=0)
		{
			return getSectorToDown(id);
		}
		
		// Otherwise move left and up		
		return getSectorToLeft(getSectorToDown(id));
	}
	
	public int getSectorToLowRight(final int id)
	{
		//int x=id%sizeX;
		int y=id/sizeX;

		// If we are at uneven row then move just down.
		if ((y%2)==0)
		{
			return getSectorToDown(id);
		}
		
		// Otherwise move left and up		
		return getSectorToRight(getSectorToDown(id));
	}

	// The "i" here is what is called "index" in most other parts of this program. 
	public int getSectorIndexByDirection(final int i, final int a)
	{
		switch (a)
		{
			case -1: return i; 
			case 0: return getSectorToRight(i);
			case 1: return getSectorToUppRight(i);
			case 2: return getSectorToUppLeft(i);
			case 3: return getSectorToLeft(i);
			case 4: return getSectorToLowLeft(i);
			case 5: return getSectorToLowRight(i);

			case 6: return getSectorToRight(getSectorToRight(i));
			case 7: return getSectorToRight(getSectorToUppRight(i));
			case 8: return getSectorToUppRight(getSectorToUppRight(i));
			case 9: return getSectorToUppLeft(getSectorToUppRight(i));
			case 10: return getSectorToUppLeft(getSectorToUppLeft(i));
			case 11: return getSectorToLeft(getSectorToUppLeft(i));
			case 12: return getSectorToLeft(getSectorToLeft(i));
			case 13: return getSectorToLeft(getSectorToLowLeft(i));
			case 14: return getSectorToLowLeft(getSectorToLowLeft(i));
			case 15: return getSectorToLowLeft(getSectorToLowRight(i));
			case 16: return getSectorToLowRight(getSectorToLowRight(i));
			case 17: return getSectorToRight(getSectorToLowRight(i));					

			case 18: return getSectorToRight(getSectorToRight(getSectorToRight(i)));
			case 19: return getSectorToRight(getSectorToRight(getSectorToUppRight(i)));
			case 20: return getSectorToRight(getSectorToUppRight(getSectorToUppRight(i)));
			case 21: return getSectorToUppRight(getSectorToUppRight(getSectorToUppRight(i)));
			case 22: return getSectorToUppRight(getSectorToUppRight(getSectorToUppLeft(i)));
			case 23: return getSectorToUppRight(getSectorToUppLeft(getSectorToUppLeft(i)));
			case 24: return getSectorToUppLeft(getSectorToUppLeft(getSectorToUppLeft(i)));
			case 25: return getSectorToLeft(getSectorToUppLeft(getSectorToUppLeft(i)));
			case 26: return getSectorToLeft(getSectorToLeft(getSectorToUppLeft(i)));
			case 27: return getSectorToLeft(getSectorToLeft(getSectorToLeft(i)));
			case 28: return getSectorToLeft(getSectorToLeft(getSectorToLowLeft(i)));
			case 29: return getSectorToLeft(getSectorToLowLeft(getSectorToLowLeft(i))); 								
			case 30: return getSectorToLowLeft(getSectorToLowLeft(getSectorToLowLeft(i))); 					
			case 31: return getSectorToLowLeft(getSectorToLowLeft(getSectorToLowRight(i))); 					
			case 32: return getSectorToLowLeft(getSectorToLowRight(getSectorToLowRight(i))); 					
			case 33: return getSectorToLowRight(getSectorToLowRight(getSectorToLowRight(i))); 					
			case 34: return getSectorToRight(getSectorToLowRight(getSectorToLowRight(i))); 					
			case 35: return getSectorToRight(getSectorToRight(getSectorToLowRight(i)));			
			
			default:
			{
				//debug("a "+a);
				if (a<N_SECTOR_NEIGHBORS_4)
				{
					int a4=a-N_SECTOR_NEIGHBORS_3;
					//debug("a4 "+a4);

					// a4 is now a number between 0 and 24
					// But we need a number between 0 and 18 to use in recursion
					int a5=(a4*3)/4;
					int a6=a5+N_SECTOR_NEIGHBORS_2;
					int s1=getSectorIndexByDirection(i, a6);
					// Now what is left after that?
					int a7=(a4-a5)%6;
					int s2=getSectorIndexByDirection(s1, a7);
					return s2;
				}		
				else
				{
					int d=getDistanceFromNeighborNr(a);
					//int n=getNSectorNeighbors(d);
					int nm1=getNSectorNeighbors(d-1);
					int nm2=getNSectorNeighbors(d-2);
					int a4=a-nm1;
					int a5=(a4*(d-1))/d;
					int a6=a5+nm2;
					int s1=getSectorIndexByDirection(i, a6);
					// Now what is left after that?
					int a7=(a4-a5)%6;
					int s2=getSectorIndexByDirection(s1, a7);
					return s2;
					
				}
				//break;
			}
		}

		//return -1;
	}

	

	
	
	public EmpireUnit findUnitAt(int x, int y)
	{
		int pos=getSectorId(x,y);
		D1Base sol[]=getListOfSubObjects();
		for(int i=0; i<sol.length; i++)
		{
			D1Base so = sol[i];
			if (so instanceof EmpireUnit)
			{
				EmpireUnit eu=(EmpireUnit)so;
				if (eu.getPosRecursive()==pos)
				{
					return eu;
				}
			}
		}
		return null;
	}
	
	
	/*
	public EmpireWorld getEmpireWorld()
	{
	    DbBase bo = this.getContainingObj();
	    
	    if (bo instanceof EmpireWorld)
	    {
			return (EmpireWorld)bo;
	    }
	    else  if (bo instanceof EmpireTerrain)
	    {
	    	EmpireTerrain et=(EmpireTerrain)bo;
			return et.getEmpireWorld();
	    }
	    return null;
	}
*/

	
	
	// Calculate defensive strength in a given sector 
	public int calcDefensiveStrengthAtSector(int sid, int attacker)
	{
		int defStrength=0;
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();

		final int st=getTerrainMask(sid);
		
		// look for enemies
		
		EmpireSector es = getSector(sid);
		if (es.getNSubObjects()>0)
		{
		  for (D1Storable bo : es.listOfStoredObjects)
		  {
			if (bo instanceof EmpireUnit)
			{
				final EmpireUnit eu=(EmpireUnit)bo;
				
				// Cargo does not contribute to defense.
				if (!eu.isUnitCargo())
				{
					// Is it an enemy
					if (eu.isEnemy(attacker))
					{
						final int t=eu.unitType;
						final EmpireUnitType eut=eul.getUnitType(t);
						
						if (eu.movePoints < EmpireUnitType.MaxMovePoints)
						{
							defStrength+=eut.getDefenceMoving(st);
						}
						else
						{
							defStrength+=eut.getDefenceStill(st);							
						}
					}
				}
			}
		  }				
		}

		return defStrength;
	}
	
	
	public void hitSector(int sid, int attacker, int attackStrength)
	{
		//final EmpireWorld ew=getEmpireWorld();
		//final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();

		// look for enemies of attacker

		EmpireSector es = getSector(sid);
		if (es.getNSubObjects()>0)
		{
			for (D1Storable bo : es.listOfStoredObjects)
			{
				if (bo instanceof EmpireUnit)
				{
					final EmpireUnit eu=(EmpireUnit)bo;
					
					// Cargo does not get hit directly.
					if (!eu.isUnitCargo())
					{
						// Only enemy units are hit
						if (eu.isEnemy(attacker))
						{
							eu.takeHit(attackStrength);
						}
					}
				}
			}		
		}
	}


	// Returns >=0 if only one nation can claim the sector.
	// -1 if no one can
	// -2 if more than one can
	public int getSectorOwner(int sid)
	{
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		int owner=-1;



		EmpireSector es = getSector(sid);
		
		if (listOfStoredObjects!=null)
		{
			for (D1Storable bo : es.listOfStoredObjects)
			{
				if (bo instanceof EmpireUnit)
				{
					final EmpireUnit eu=(EmpireUnit)bo;
					//if (eu.getPosXY()==sid)
					{
						final EmpireUnitType eut=eul.getUnitType(eu.unitType);
						
						if (eut.attackProbability>0)
						{
							if (owner==-1)
							{
								// at least one possible owner. 
								owner=eu.owner;
							}
							else if (owner!=eu.owner)
							{
								// units from more than one country in sector -> no owner 
								return -2;
							}
						}		
					}
				}
			}
		}		
		return owner;
	}
	
	
	// one and only one word may be in the name when calling this
	public static int getTerrainTypeMaskFromTerrainTypeName(String name)
	{
		if (WordReader.isInt(name))
		{
			return Integer.parseInt(name);
		}
		else if (name.equals("sea"))
		{
			return SEA_TERRAIN_MASK;
		}
		else if (name.equals("land"))
		{
			return LAND_TERRAIN_MASK;			
		}
		else if (name.equals("city"))
		{
			return CITY_TERRAIN_MASK;			
		}
		else if (name.equals("field"))
		{
			return FIELD_TERRAIN_MASK;			
		}
		else if (name.equals("mineralDeposit"))
		{
			return MINERAL_TERRAIN_MASK;			
		}
		else
		{
			return -1;
		}
	}
	
	public static int getTerrainTypeMaskFromTerrainTypeNames(String names)
	{
		String a[]=WordReader.split(names);
		int m=0;
						
		for(int i=0;i<a.length;i++)
		{
			m|=EmpireTerrain.getTerrainTypeMaskFromTerrainTypeName(a[i]);
		}
		return m;
	}

	// one and only one bit may be set in the mask when calling this
	public static String getTerrainTypeNameFromTerrainTypeMask(int mask)
	{
		switch(mask)
		{
			case SEA_TERRAIN_MASK: return "sea";
			case LAND_TERRAIN_MASK: return "land";
			case CITY_TERRAIN_MASK: return "city";
			case FIELD_TERRAIN_MASK: return "field";
			case MINERAL_TERRAIN_MASK: return "mineralDeposit";
			default: return "";
		}
	}
	
	public static String getTerrainTypeNamesFromTerrainTypeMask(int mask)
	{
		String name="";
		for(int i=0;i<32;i++)
		{
			if (((1<<i) & mask) != 0)
			{
				if (name.length()!=0)
				{
					name+=" ";
				}
				name+=getTerrainTypeNameFromTerrainTypeMask(1<<i);
			}
		}
		return name;
	}

	public int getNextSectorTowardsDest(int curPosXY, int destSector)
	{
		final int d = getDirection(curPosXY, destSector);			
		final int nextPos = getSectorIndexByDirection(curPosXY, d);
		return nextPos;
	}
	

	public static int addMax(int a, int b, int max)
	{
		long s=a+b;
		return s<max?(int)s:max;
	}
	
	
	public void updateMoveCostIfLess(D1List<AtomicInteger> moveCostList, int sectorIndex, int newCost)
	{
		AtomicInteger moveCost=moveCostList.get(sectorIndex);

		if (moveCost==null)
		{
			if (newCost<EmpireUnitType.InfiniteMoveCost)
			{
				moveCostList.add(sectorIndex, new AtomicInteger(newCost));
			}
		}
		else
		{
			if (newCost<moveCost.get())
			{
				moveCost.set(newCost);
			}
		}
	}
	
	// TODO Don't route units over neutral territory
	public int getMoveCost(int fromIndex, int toIndex, final EmpireUnitType eut, int stateIdx)
	{
		final EmpireSector es=getSector(toIndex);
		if (es.isNeutral(stateIdx))
		{
			return EmpireUnitType.InfiniteMoveCost;
		}

		final int fromTerrainMask=getTerrainMask(fromIndex);				
		final int toTerrainMask=getTerrainMask(toIndex);				
		final int stepCost=eut.getMoveCost(fromTerrainMask, toTerrainMask);
		
		
		
		return stepCost;
	}

	public void examineNeighbors(D1List<AtomicInteger> moveCostList, int sectorIndex, final EmpireUnitType eut, int stateIdx)
	{
		AtomicInteger myCost=moveCostList.get(sectorIndex);

		for(int i=0; i<N_SECTOR_NEIGHBORS_1;++i)
		{
			final int neighborIndex=getSectorIndexByDirection(sectorIndex, i);

			final AtomicInteger neighborCost=moveCostList.get(neighborIndex);
			
			if (neighborCost!=null)
			{
				final int stepCost=getMoveCost(sectorIndex, neighborIndex, eut, stateIdx);
				if (stepCost<EmpireUnitType.InfiniteMoveCost)
				{
					final int c=neighborCost.get() + stepCost;
					
					// Small CPU optimize here instead of calling updateMoveCostIfLess(moveCostList, sectorIndex, c) do it here (avoids some calls to moveCostList.get)
					if (myCost==null)
					{
						if (c<EmpireUnitType.InfiniteMoveCost)
						{
							myCost=new AtomicInteger(c);
							moveCostList.add(sectorIndex, myCost);
						}
					}
					else
					{
						if (c<myCost.get())
						{
							myCost.set(c);
						}
					}
				}
			}
		}
	}

	public static String extendString(String s, int n)
	{
		while(s.length()<(n-1))
		{
			s=' '+s+' ';
		}
		while(s.length()<n)
		{
			s+=' ';
		}		
		return s;
	}
	
	public void debugLogMoveCost(D1List<AtomicInteger> moveCostList)
	{
		for(int y=0;y<this.sizeY;y++)
		{
			String str="";
			if (y%2!=0)
			{
				str+="   ";
			}
			for(int x=0;x<this.sizeX;x++)
			{
				int index=getSectorId(x,y);
				
				final AtomicInteger c=moveCostList.get(index);
				if (c==null)
				{
					str+="  --  ";
				}
				else
				{
					str+=extendString(c.toString(),6);
				}
				
				//str+=extendString(""+index,6);
			}
			debug("line "+extendString(""+y,6)+" "+str);
		}
		
	}
	
	public void debugLogSectorIndexByDirection(int sectorIndex)
	{
		D1List<AtomicInteger> moveCostList=new D1List<AtomicInteger>(this.listOfStoredObjects.getCapacity());

		for(int i=-1; i<getNSectorNeighbors(5);++i)
		{
			final int neighborIndex=getSectorIndexByDirection(sectorIndex, i);
			if (neighborIndex>=0)
			{
				updateMoveCostIfLess(moveCostList, neighborIndex,i);
			}
		}
		debugLogMoveCost(moveCostList);
	}
	
	public int calculateDistanceBetweenSectors(int from, int to)
	{
		int n=0;
		int thisIndex=to;
		while(thisIndex!=from)
		{
			final int closerSector=getNextSectorTowardsDest(thisIndex, from);
			thisIndex=closerSector;
			++n;
		}
		return n;
	}
	
	public void searchRoutesAroundSector(D1List<AtomicInteger> moveCostList, int sectorIndex, final EmpireUnitType eut, int stateIdx, int radius)
	{
		final int n=getNSectorNeighbors(radius);
		for(int i=-1; i<n;++i)
		{
			final int neighborIndex=getSectorIndexByDirection(sectorIndex, i);
			examineNeighbors(moveCostList, neighborIndex, eut, stateIdx);
		}
	}

	public void searchRoutesAroundSectorRev(D1List<AtomicInteger> moveCostList, int sectorIndex, final EmpireUnitType eut, int stateIdx, int radius)
	{
		final int n=getNSectorNeighbors(radius);
		for(int i=n-1; i>=-1;--i)
		{
			final int neighborIndex=getSectorIndexByDirection(sectorIndex, i);
			examineNeighbors(moveCostList, neighborIndex, eut, stateIdx);
		}
	}

	public void searchRoutesAroundSectorPerimeter(D1List<AtomicInteger> moveCostList, int sectorIndex, final EmpireUnitType eut, int stateIdx, int radius)
	{
		final int n1=getNSectorNeighbors(radius-1);
		final int n2=getNSectorNeighbors(radius);
		for(int i=n1; i<n2;++i)
		{
			final int neighborIndex=getSectorIndexByDirection(sectorIndex, i);
			examineNeighbors(moveCostList, neighborIndex, eut, stateIdx);
		}
	}
	
	// This does a rather thorough search but only within some distance (radius) from the direct path.
	public void narrowSearch(D1List<AtomicInteger> moveCostList, int curPosXY, int destSector, final EmpireUnitType eut, int stateIdx, int radius)
	{
		// Now examine neighbors
		// Will start at destination and step our way back to current location
		int thisIndex=destSector;

		searchRoutesAroundSector(moveCostList, thisIndex, eut, stateIdx, radius);
		thisIndex=getNextSectorTowardsDest(thisIndex, curPosXY);

		while((thisIndex!=curPosXY))
		{			
			// For each step look around the perimeter for a less costly way
			searchRoutesAroundSectorPerimeter(moveCostList, thisIndex, eut, stateIdx, radius);

			thisIndex=getNextSectorTowardsDest(thisIndex, curPosXY);
		}
		
		// For current position search routes reverse, that is from outer radius and inwards, so that current sector is the last one to be routed to.
		searchRoutesAroundSectorRev(moveCostList, curPosXY, eut, stateIdx, radius);
	}

	// This does a rather coarse search but it searches the entire world.
	public void wideSearch(D1List<AtomicInteger> moveCostList, int curPosXY, int destSector, final EmpireUnitType eut, int stateIdx)
	{
		// Now examine neighbors, 
		// This will scan the entire world
		// so this may take some processing time
		// For each step look at around for a less costly way
		final int n=this.sizeX*this.sizeY;
		for(int s=0; s<n; ++s)
		{
			examineNeighbors(moveCostList, s, eut, stateIdx);
		}
		// search from other end also
		for(int s=n-1; s>=0; s--)
		{
			examineNeighbors(moveCostList, s, eut, stateIdx);
		}
		
		// For current position search routes reverse, that is from radius and inwards, so that current sector is the last one to be routed to.
		searchRoutesAroundSectorRev(moveCostList, curPosXY, eut, stateIdx, 6);	
	}

	public int getDirectionFromMoveCostList(D1List<AtomicInteger> moveCostList, int curPosXY, final EmpireUnitType eut, int stateIdx)
	{
		final EmpireWorld ew=getEmpireWorld();
	
		int lowestCostDirection=-1;
		int nSameCost=0;
		if (moveCostList.get(curPosXY)!=null)
		{
			int lowestCost=EmpireUnitType.InfiniteMoveCost;
			for(int i=0; i<N_SECTOR_NEIGHBORS_1;++i)
			{
				final int neighborIndex=getSectorIndexByDirection(curPosXY, i);
				final AtomicInteger neighborCost=(moveCostList.get(neighborIndex));
				if (neighborCost!=null)
				{
					// Get the cost to that sector and add it
					final int stepCost=getMoveCost(curPosXY, neighborIndex, eut, stateIdx);
					if (stepCost<EmpireUnitType.InfiniteMoveCost)
					{
						final int c=neighborCost.get() + stepCost;
										
						if (c<=lowestCost)
						{
							if (c==lowestCost)
							{
								nSameCost++;
							}
							else
							{
								nSameCost=0;
							}
							
							int r=ew.generator.nextInt(nSameCost+1); // try randomly
		
							if (r==0)
							{
								lowestCost=c;
								lowestCostDirection=i;
							}
						}
					}
				}
			}
		}
		return lowestCostDirection;
	}

	
	public int getDirection(int curPosXY, int destSector, int unitType, int stateIdx)
	{
		D1List<AtomicInteger> moveCostList=new D1List<AtomicInteger>(this.listOfStoredObjects.getCapacity());
		final EmpireWorld ew=getEmpireWorld();
		final EmpireUnitTypeList eul=ew.getEmpireUnitTypeList();
		final EmpireUnitType eut=eul.getUnitType(unitType);
		final int dist=calculateDistanceBetweenSectors(curPosXY, destSector);
		int lowestCostDirection=-1;
	
		//debugLogSectorIndexByDirection(destSector);
		
		// Start with zero cost at destination
		moveCostList.add(destSector, new AtomicInteger(0));
		
		
		//debugLogMoveCost(moveCostList);

		if (dist>1)
		{
			narrowSearch(moveCostList, curPosXY, destSector, eut, stateIdx, 6);
			lowestCostDirection=getDirectionFromMoveCostList(moveCostList, curPosXY, eut, stateIdx);
			
			if (lowestCostDirection==-1)
			{
				wideSearch(moveCostList, curPosXY, destSector, eut, stateIdx);
				lowestCostDirection=getDirectionFromMoveCostList(moveCostList, curPosXY, eut, stateIdx);
			}
	
			if (lowestCostDirection==-1)
			{
				wideSearch(moveCostList, curPosXY, destSector, eut, stateIdx);
				lowestCostDirection=getDirectionFromMoveCostList(moveCostList, curPosXY, eut, stateIdx);
			}
		}

		// if no direction was found above just try the the old way (the birds way)
		if (lowestCostDirection==-1)
		{
			lowestCostDirection=getDirection(curPosXY, destSector);
		}
		
		return lowestCostDirection;
	}

}