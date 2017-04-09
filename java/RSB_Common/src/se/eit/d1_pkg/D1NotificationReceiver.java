// NotificationReceiver.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 


package se.eit.d1_pkg;

public interface D1NotificationReceiver {

	// subscribersRef (aka yourRef) is the reference number given by the subscriber when subscribing (if subscriber only subscribe on notifications from one class this is typically zero). So from perspective of receiving class your is my sort of.
	// sendersRef (aka myRef) is a reference number given by the class that generated the event. Typically an objId identifying a changed object. 
	public void notifyRef(int subscribersRef, int sendersRef);
	
	// The object sending notifications is being unlinked (deleted).
	public void unlinkNotify(int subscribersRef);

}
