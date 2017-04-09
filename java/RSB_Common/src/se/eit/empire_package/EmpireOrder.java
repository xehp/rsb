//
// Copyright (C) 2014 Henrik Björkman www.eit.se
//
// History:
// Created by Henrik 2014-03-08


package se.eit.empire_package;


//import java.util.Random;
import se.eit.d1_pkg.*;
import se.eit.web_package.*;

// TODO Could we have used te RsbString class instead of this one?

public class EmpireOrder extends EmpireBase {

	String order;
  
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireOrder.class.getSimpleName();	
	}

	public EmpireOrder(D1Base parent, String name, String order) 
	{
		super();
		parent.addObject(this);
		this.regName(name);

		this.order=order;
	}
	

	public EmpireOrder()
	{	
		super();
	}
	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		order=wr.readString();
	}

	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeString(order);
	}	
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);				
		pw.println("order"+" "+order);		
	}
	
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("order"))
		{
			order=wr.readString();
			return 1;
		}
		return super.setInfo(wr, infoName);
	}
	
	
	public String getOrder()
	{
		return order;
	}

	public void setOrder(String order)
	{
		if (this.order!=order)
		{
			setUpdateCounter();
			this.order=order;
		}
	}
	
	
	@Override
	// When an order is unlinked the objects inside (other orders) are moved to the parent object	
	public void unlinkSelf()
	{
		if (this.listOfStoredObjects!=null)
		{
			for (D1Storable ds : this.listOfStoredObjects)
			{
				try
				{
					D1Container dc=(D1Container)ds;
					dc.relinkSelf(this.getParent());
				}
				catch (ClassCastException e)
				{
					error("not a DbContainer "+e);
				}
			}
		}
		super.unlinkSelf();
	}

}