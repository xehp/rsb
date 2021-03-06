//
//Copyright (C) 2014 Henrik Björkman www.eit.se
//
//History:
//Created by Henrik 2014-08-02


package se.eit.empire_package;


import se.eit.d1_pkg.D1Base;
//import java.util.Random;
import se.eit.web_package.*;


// Only objects of type "EmpireUnit" shall be stored in objects of this class.

public class EmpireSector extends EmpireUnitOrSector {

	
	int terrain=0;

	
	int newOwner=NO_OWNER;
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireSector.class.getSimpleName();	
	}

	public EmpireSector()
	{	
		super();
	}
	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		terrain=wr.readInt();
	}

	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(terrain);
	}	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		pw.println("terrain"+" "+terrain);		
	}

	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("terrain"))
		{
			terrain=wr.readInt();
			return 1;
		}
		else
		{
			return super.setInfo(wr, infoName);
		}
	}
	
	
	public int getTerrain()
	{
		return terrain;
	}

	public void setTerrain(int terrain)
	{
		this.terrain=terrain;
	}
	
	// not a good name on this one. it return an index that is x+sizeX*y
	public int getPosXY()
	{
		return getIndex();
	}
	
	public void setNewOwner(int newOwner)
	{
		if (newOwner!=this.newOwner)
		{
			if (this.newOwner==NO_OWNER)
			{
				this.newOwner=newOwner;
			}
			else
			{
				this.newOwner=CONTESTED_PROPERTY;
			}
			final EmpireWorld ew=getEmpireWorld();
			ew.setTickCleanupCallback(this);
		}
	}

	public void setOwner(int owner)
	{
		this.owner=owner;			
	}
	
	public int getOwner()
	{
		return this.owner;			
	}
	
	@Override	
	public void gameTickCleanup()
	{
		if ((this.newOwner>=0) && (this.newOwner!=owner))
		{
			// This sector has a new owner
			this.owner=this.newOwner;
			setUpdateCounter();
		}
		this.newOwner=NO_OWNER;
	}
	
	
	// Mass of all child units (and their child units...)		
	public int calculateMassInSector()
	{
		int m=0;
		
		for (D1Base so: this.listOfStoredObjects)
		{
			if (so instanceof EmpireUnit)
			{
				// Add mass recursively
				EmpireUnit eu=(EmpireUnit)so;
				m+=eu.calculateStackMass();
			}
		}
		
		return m;
	}

	@Override
	public int getPosRecursive()
	{
		return this.getPosXY();
	}

	
	@Override
	public boolean isVisibleTo(D1Base observingObj)
	{
		if (observingObj instanceof EmpireState)
		{
			EmpireState empirestate = (EmpireState)observingObj;
			final EmpireWorld ew=getEmpireWorld();
			final EmpireStatesList enl = ew.getEmpireStatesList();
			final int stateIndex = empirestate.getIndex();
			final EmpireState ownerState=enl.getEmpireNation(owner);
			
			
			//if (owner<=0)
			//{
			//	return true;
			//}
	
			if (owner==stateIndex)
			{
				return true;
			}
					
			if ((ownerState!=null) && (ownerState.isAlly(empirestate)))
			{
				return true;
			}
			
			// Get sector index
			//final int curPosXYIndex=getPosRecursive();
			final int curPosXYIndex=this.getIndex();
			
			if (curPosXYIndex<0)
			{
				debug("how does this happen?");
				return true;
			}
			
			if (empirestate.canSeeSector(curPosXYIndex))
			{
				return true;
			}
			
			return false;
		}
		else
		{
			return super.isVisibleTo(observingObj);
		}
	}
	
	
}