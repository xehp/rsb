package se.eit.empire_package;

import se.eit.d1_pkg.D1Base;
import se.eit.rsb_package.RsbLong;

public class EmpireMoney extends RsbLong{

	public static String className()
	{	
		return EmpireMoney.class.getSimpleName();	
	}
	
	public EmpireMoney()
	{	
		super();
	}

	@Override
	public boolean isVisibleTo(D1Base observingObj)
	{
		return ((observingObj!=null) && (observingObj.isRecursiveParentOf(this)));
	}

	
	
}
