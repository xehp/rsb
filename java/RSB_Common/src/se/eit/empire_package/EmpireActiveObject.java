package se.eit.empire_package;

import se.eit.d1_pkg.D1Container;
import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

// TODO this shall implement ActiveObjectInterface
public abstract class EmpireActiveObject extends EmpireUnitOrSector /*implements ActiveObjectInterface*/ {

	/*
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireBase.class.getSimpleName();	
	}
	*/

	// An index telling where in EmpireActiveList this object is listed.
	public int activeIndex=-1;
	
	// Last update (call to gameTick) for this object in game time units
	public long localTime=0; 

	public EmpireActiveObject() 
	{
		super();
	}

	// TODO move localTime from EmpireUnit and EmpireState to here.

	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		localTime=wr.readLong();
	}

	// serialize to ww
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeLong(localTime);
	}	
	
	@Override
	public void listInfo(WordWriter ww)
	{
		super.listInfo(ww);					
		ww.println("localTime"+" "+localTime);		
	}

	
	@Override
	public void unlinkSelf()
	{
		final EmpireWorld ew=getEmpireWorld();
		if (ew!=null)
		{
			ew.removeActiveObject(this);
		}
		super.unlinkSelf();
	}


	
	@Override
	public void linkSelf(D1Container parentObj)
	{
		super.linkSelf(parentObj);

		if (this.getDbSubRoot()!=null)
		{
			final EmpireWorld ew=getEmpireWorld();
			ew.addActiveObject(this);
		}
	}

	// During game tick update only internal variables. Don't move, add or remove units.
	// This method can be overridden in extending classes but those overrides shall call super.gameTick();
	public void gameTick(long timeInGameTimeUnits)
	{
		localTime=timeInGameTimeUnits;
	}

	
}
