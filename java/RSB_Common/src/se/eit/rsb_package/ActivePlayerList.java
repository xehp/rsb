// ActivePlayerList.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.rsb_package;

import se.eit.d1_pkg.D1Storable;
import se.eit.d1_pkg.D1ThreadSafe;
import se.eit.web_package.WordWriter;

public class ActivePlayerList extends D1ThreadSafe {

	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return ActivePlayerList.class.getSimpleName();	
	}
	

	public ActivePlayerList()
	{	
		super();
	}
	

	public int getNActivePlayers()
	{
		return listActivePlayers(null);
	}


	// Returns the number of active players
	public int listActivePlayers(WordWriter ww)
	{
		int n=0;
		if (listOfStoredObjects!=null)
		{
			for (D1Storable s : listOfStoredObjects)
			{
				GameUser apr = (GameUser)s;
				if (apr.getActive())
				{
					if (ww!=null)
					{
						ww.writeWord(s.getName());
					}
					n++;
				}
			}
		}
		return n;
	}

	// This is called to send an ascii message to this object.	
	@Override
	public void postMessageToThis(String msg)
	{
		// pass the message on to those in the room.
		if (this.listOfStoredObjects!=null)
		{
			for(D1Storable ds : this.listOfStoredObjects)
			{
				ds.postMessageToThis(msg);
			}
		}
	}
	
}
