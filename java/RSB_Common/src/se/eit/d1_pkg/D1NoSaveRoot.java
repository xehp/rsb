/*
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license
*/
package se.eit.d1_pkg;



public class D1NoSaveRoot extends D1SubRoot {

	
	@Override
	public void saveSelf()
	{
		//debug("saveSelf: don't save this one");
	}	

	@Override
	public boolean needIdList()
	{
		return false;
	}
	
}
