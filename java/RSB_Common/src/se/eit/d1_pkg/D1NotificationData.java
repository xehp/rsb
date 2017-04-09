/*
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license
*/
package se.eit.d1_pkg;

public class D1NotificationData
{
	int ref=0;
	D1NotificationReceiver rcv=null;

	public D1NotificationData(D1NotificationReceiver rcv, int ref)
	{
		this.rcv=rcv;
		this.ref=ref;
	}
	
	public void doNotify(int idOfSenderOrEventId)
	{
		rcv.notifyRef(ref, idOfSenderOrEventId);
	}
}
