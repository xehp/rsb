//
//Copyright (C) 2013 Henrik Björkman www.eit.se
//
//History:
//Adapted for use with RSB. Henrik 2013-05-04


package se.eit.empire_package;


//import java.util.Random;


//import se.eit.rsb_package.*;
import se.eit.d1_pkg.*;
import se.eit.rsb_package.Misc;
import se.eit.web_package.*;


public class EmpireStatesList extends EmpireBase {
	

	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return EmpireStatesList.class.getSimpleName();	
	}

	public EmpireStatesList(D1Base parent, String name, int nStates) 
	{
		super();
		parent.addObject(this);
		this.regName(name);
		

		generateList(nStates);
	}

	public EmpireStatesList()
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
	
	public void generateList(int nStates)
	{
	    for(int i=0;i<nStates;i++)
	    {
			new EmpireState(this, "State"+i);
	
	    	/*
	    	EmpireState en = new EmpireState(this, "State"+i);
	    	en.postMessage("Welcome to EIT Empire");
	    	
			final EmpireWorld ew=getEmpireWorld();
			if (ew.gameSpeed==0)
			{
				en.postMessage("Game is paused. (Game admin can use command: 'go' to start.)");
			}
	    	*/
	    }
	}
	
    // Post message to all states
	public void postMessageToThis(String str)
	{
		debugWriteLock();

		D1Base sol[]=getListOfSubObjects();
		for(int i=0; i<sol.length; i++)
		{
			D1Base so = sol[i]; 
			try
			{
				EmpireState en = (EmpireState)so;
				en.postMessageToThis(str);
			}
			catch (ClassCastException e)
			{
				debug("not an EmpireState "+e);
			}
		}
	}
	
	public EmpireState getNationByOwner(String PlayerName)
	{
		final D1SubRoot db=getDbSubRoot();
		db.lockRead();
		try
		{
			if (listOfStoredObjects!=null)
			{	
				for (D1Storable d : listOfStoredObjects)
				{
					try
					{
						EmpireState es=(EmpireState)d;
						
						if (es.headOfState.equals(PlayerName))
						{
							return es;
						}
					}
					catch (ClassCastException e)
					{
						debug("not an EmpireState "+e);
					}
				}
			}
		}
		finally
		{
			db.unlockRead();
		}
		return null;
	}
	
	public EmpireState getNationByCoRuler(String PlayerName)
	{
		final D1SubRoot db=getDbSubRoot();
		db.lockRead();
		try
		{
			if (listOfStoredObjects!=null)
			{
				for (D1Storable d : listOfStoredObjects)
				{
					try
					{
						EmpireState es=(EmpireState)d;
						
						if (Misc.getNWordInList(PlayerName, es.coRuler)>0)
						{
							// This player is a co ruler for this state.
							return es;
						}
					}
					catch (ClassCastException e)
					{
						debug("not an EmpireState "+e);
					}
				}
			}
		}
		finally
		{
			db.unlockRead();
		}
		return null;
	}

	public EmpireState takeNation(String PlayerName)
	{
		final D1SubRoot db=getDbSubRoot();
		db.lockWrite();
		try
		{
			if (listOfStoredObjects!=null)
			{
				for (D1Storable d : listOfStoredObjects)
				{
					try
					{
						EmpireState es=(EmpireState)d;
						if ((es.headOfState.length()==0) && (es.getPlayerState()==false))
						{
							es.headOfState = PlayerName;
							es.setPlayerState();
							return es;
						}
					}
					catch (ClassCastException e)
					{
						debug("not an EmpireState "+e);
					}
				}
			}	
		}
		finally
		{
			db.unlockWrite();
		}
		return null;
	}

	
	public EmpireState getEmpireNation(int n)
	{
		if (n<0)
		{
			return null;
		}
		
		D1Base db = getObjFromIndex(n);
		try
		{
			return (EmpireState)db;
		}
		catch (ClassCastException e)
		{
			error("getEmpireNation: internal error, wrong object type "+n+" "+e);
			return null;
		}
	}
	
}