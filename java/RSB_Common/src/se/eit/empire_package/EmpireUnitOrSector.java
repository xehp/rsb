package se.eit.empire_package;

import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;


// TODO: We should probably rename this to EmpireProperty class since more than EmpireUnits can be owned

public class EmpireUnitOrSector extends EmpireBase {

	static final int NO_OWNER=-1;
	static final int CONTESTED_PROPERTY=-2;

	// TODO Currently owner is the index of a state/nation in EmpireStatesList, but perhaps this should be an ID instead so that an object can belong to whatever other object there are, not just nations/states.
	int owner=NO_OWNER; // Will try to move away from this and use ownerUnit instead

	//int ownerId=NO_OWNER; // The ID of the unit owning this unit, this can be recursive, a unit can belong to a city which belong to a state/nation
	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		owner=wr.readInt();
		//ownerId=wr.readInt();
	}

	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(owner);
		//ww.writeInt(ownerId);
	}	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);					
		pw.println("owner "+owner);		
		//pw.println("ownerId "+ownerId);		
	}

	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("owner"))
		{
			owner=wr.readInt();
			return 1;
		}
		else if (infoName.equals("ownerId"))
		{
			owner=wr.readInt();
			return 1;
		}
		else
		{
			return super.setInfo(wr, infoName);
		}
	}
	
	// This is to be overridden in EmpireUnit and EmpireSector
	public int getPosRecursive()
	{
		return -1;
	}

	
	public void setOwner(int newOwner)
	{
		// has owner changed?
		if (owner!=newOwner)
		{
			owner=newOwner;
			/*if (owner<0)
			{
				final EmpireWorld ew=getEmpireWorld();
				ew.setTickCleanupCallback(this);
			}*/
			setUpdateCounter();
		}
	}

	/*
	public void setOwnerId(int newOwnerId)
	{
		// has owner changed?
		if (ownerId!=newOwnerId)
		{
			ownerId=newOwnerId;
			setUpdateCounter();
		}
	}
	*/

	public int getOwner()
	{
		return owner;
	}

	/*
	public int getOwnerId()
	{
		return ownerId;
	}
	*/
	
    public void tellOwner(String str)
    {
		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireState en=enl.getEmpireNation(owner);
		if (en!=null)
		{
			en.postMessageToThis("Unit ~"+this.getId()+" ("+this.getName()+") "+str);
		}
		setUpdateCounter();					
    }

    /*
    public EmpireBase getOwningStateRecursively()
    {
    	D1IdObj idObj = this.getDbIdObj(ownerId);

    	if (idObj instanceof EmpireState)
    	{
    		return (EmpireState)idObj;
    	}
    	else if (idObj instanceof EmpireUnitOrSector)
    	{
    		EmpireUnitOrSector eb=(EmpireUnitOrSector)idObj;
    		return eb.getOwningStateRecursively();
    	}
    	
    	// This unit did not have an owner
    	return null;
    }


    public void tellOwnerId(String str)
    {
    	EmpireBase eb=getOwningStateRecursively();
    	
    	if (eb instanceof EmpireState)
    	{
			eb.postMessageToThis("Unit ~"+this.getId()+" ("+this.getName()+") "+str);	
    	}
    	else
    	{
    		debug("did not find an owner to tell: '"+str+"'");
    	}
    	
    }
    */
    
    /*
    public void tellOwnerId(String str)
    {
    	D1IdObj idObj = this.getDbIdObj(ownerId);

    	if (idObj instanceof EmpireState)
    	{
    		idObj.postMessageToThis("Unit ~"+this.getId()+" ("+this.getName()+") "+str);	
    	}
    	else if (idObj instanceof EmpireUnitOrSector)
    	{
    		((EmpireUnitOrSector) idObj).tellOwnerId(str);
    	}
    	else
    	{
    		debug("did not find an owner to tell: '"+str+"'");
    	}
    }
    */
    
    
    /*
	public boolean ownOrClaimSector(int owner)
	{
		if (this.owner==owner)
		{
			// This state already own this sector or unit
			return true;
		}
		
		if (this.owner==EmpireUnitOrSector.NO_OWNER)
		{
			// The sector is not claimed by anyone, will claim it now
			setOwner(this.owner);
			return true;
		}
		
		// Someone else own this sector or unit
		setOwner(CONTESTED_PROPERTY);
		
		return false;
	}

	@Override
	public void gameTickCleanup()
	{
		if (this.owner==CONTESTED_PROPERTY)
		{
			setOwner(NO_OWNER);
		}	
	}
*/
    
	// It is enemy if it is not own, not nobody's and not ally
	public boolean isEnemy(int attacker)
	{
		if (owner==attacker)
		{
			return false;
		}
			
		if (owner==-1)
		{
			return false;
		}

		// Is it a non ally
		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireState a=enl.getEmpireNation(attacker);
		final EmpireState o=enl.getEmpireNation(owner);

		// if attacker is hostile it is an enemy
		if (a.isEnemy(o))
		{
			return true;
		}

		// if this is hostile it is an enemy
		if (o.isEnemy(a))
		{
			return true;
		}
	
		/*
		if (!a.isAlly(o))
		{
			return true;
		}
		if (!o.isAlly(a))
		{
			return true;
		}
		*/

		return false;
	}

	public boolean isNeutral(int attacker)
	{
		// if this is same as attacker then it is not neutral
		if (owner==attacker)
		{
			return false;
		}
		
		// If this have no owner it is not neutral
		if (owner==-1)
		{
			return false;
		}

		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireState a=enl.getEmpireNation(attacker);
		final EmpireState o=enl.getEmpireNation(owner);

		
		// If this is friendly to attacker it is not neutral
		if (o.isAlly(a))
		{
			return false;
		}

		// If attacker is hostile to this it is not neutral
		if (a.isEnemy(o))
		{
			return false;
		}

		return true;
	}
	

	public boolean isAlly(int attacker)
	{
		// if this is same as attacker then it is an ally
		if (owner==attacker)
		{
			return true;
		}
		
		// If this have no owner it is not an ally
		if (owner==-1)
		{
			return false;
		}

		final EmpireWorld ew=getEmpireWorld();
		final EmpireStatesList enl = ew.getEmpireStatesList();
		final EmpireState a=enl.getEmpireNation(attacker);
		final EmpireState o=enl.getEmpireNation(owner);

		// if this is not ally it is not ally
		if (!o.isAlly(a))
		{
			return false;
		}

		// If attacker is not ally it is not ally
		if (!a.isEnemy(o))
		{
			return false;
		}

		return true;
	}
	
	/*
	@Override
	// When a unit or sector is unlinked the objects inside are moved to the parent object.
	// On second thought,  will do this in gameTickCleanup before unlink instead.
	public void unlinkSelf()
	{
		if (this.listOfStoredObjects!=null)
		{
			for (DbStorable ds : this.listOfStoredObjects)
			{
				if (ds instanceof DbContainer)
				{
					DbContainer dc=(DbContainer)ds;
					dc.moveBetweenRooms(this.getParent());
				}
			}
		}
		super.unlinkSelf();
	}
    */
	
	boolean canBeGivenAway()
	{
		return true;
	}
	
}
