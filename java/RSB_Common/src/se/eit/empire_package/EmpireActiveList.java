//
//Copyright (C) 2014 Henrik Björkman www.eit.se
//
//History:
//Created by Henrik 2014-03-08

package se.eit.empire_package;


import se.eit.rsb_package.*;
import se.eit.d1_pkg.*;


public abstract class EmpireActiveList extends WorldBase {

	 //ArrayList<ActiveObject> activeObjects = new  ArrayList<ActiveObject>();
	D1List<EmpireActiveObject> activeObjects = new  D1List<EmpireActiveObject>();

	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireBase.class.getSimpleName();	
	}


	public EmpireActiveList()
	{
		super();
	}
		
	public void addActiveObject(EmpireActiveObject ao)
	{
		ao.activeIndex = activeObjects.add(ao);
	}

	public void removeActiveObject(EmpireActiveObject ao)
	{
		activeObjects.remove(ao.activeIndex);
	}

	public void tickActiveObjects(long timeInGameTimeUnits)
	{
		for (EmpireActiveObject s : activeObjects)
		{
			s.gameTick(timeInGameTimeUnits);
		}
	}
}
