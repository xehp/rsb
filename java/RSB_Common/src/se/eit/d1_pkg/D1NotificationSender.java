// NotificationSender.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;

import java.util.Iterator;

// TODO: Perhaps DbTickReceiver and NotificationSender should swap places in the hierarchy. And if last NotificationReceiver disconnects then ticks are canceled.

public abstract class D1NotificationSender extends D1IdList {

	
	public D1List<D1NotificationData> notificationDataList=new D1List<D1NotificationData>();;

	
	// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
	public static String className()
	{	
		return D1NotificationSender.class.getSimpleName();	
	}


	public D1NotificationSender()
	{	
		super();
	}

	// Objects that need notification when something is changed in the database shall call this method to register a subscription to notifications.
	// Parameters:
	// notificationReceiver the object to be notified when something has been changed.
	// notificationReference an optional reference, it can be used by caller to keep track of the notifications it receives, it is only needed if caller has more than one subscription, set to zero if no needed.
	// Returns an index to use when removing the notification request.
	// NOTE Remember to call removeNotificationReceiver when done.
	public int addNotificationReceiver(D1NotificationReceiver notificationReceiver, int notificationReference)
	{
		D1NotificationData nd = new D1NotificationData(notificationReceiver, notificationReference);
		return notificationDataList.add(nd);
	}

	public void removeNotificationReceiver(int index)
	{
		notificationDataList.remove(index);
	}

	// Call this if notifications are no longer wanted.
	public int removeNotificationReceiver(D1NotificationReceiver notificationReceiver)
	{
		Iterator<D1NotificationData> i=notificationDataList.iterator();
		
		while (i.hasNext())
		{
			D1NotificationData nd = i.next();
		
			if (nd.rcv==notificationReceiver)
			{
				i.remove();
			}
		}
		
		return 0;
	}

	// Call this to tell subscribers that something has happened.
	public void notifySubscribers(int idOfSenderOrEventId)
	{
		for (D1NotificationData nd : notificationDataList)
		{
			nd.doNotify(idOfSenderOrEventId);
		}
	}
	
	
	@Override
	public void unlinkSelf()
	{
		notifySubscribers(-1);
		notificationDataList.clear();
		super.unlinkSelf();
	}
	
}
