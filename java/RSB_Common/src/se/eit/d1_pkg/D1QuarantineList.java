// DbQuarantineList.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;

import java.util.LinkedList;
import java.util.Queue;
import java.util.Iterator;

// This is a special version of DbList. It will avoid reusing a slot soon after it was released.
// It will also be faster if initialCapacity is big enough.
// this is not tested!

public class D1QuarantineList<E> extends D1List<E> {
	
    //protected Queue<E> queue = new LinkedList<E>();
    //protected ArrayList<E> array = new ArrayList<E>();

	protected Queue<Integer> freeIdList = new LinkedList<Integer>();  // A queue of unused IDs to replace latestIdGiven. The idea is to avoid reusing an ID too soon.
	
	protected int initialCapacity;
	
    public D1QuarantineList(int initialCapacity)
    {
    	super();
    	this.initialCapacity=initialCapacity;
    	clear();
    }
    
    
	
	// Make the list larger
	protected void makeArrayLarger()
	{
		final int n=a.length;
		final int n2=n*2;

		// Just a warning message in case something gets very big
		if (n>0x10000)
		{
			System.out.println("DbList: many objects, more than "+n);
		}

		// make a larger array
		Object newArray[] = new Object[n2];

		// copy data over to the new array
		for(int i=0;i<n;i++)
		{
			newArray[i]=a[i];
		}
		
		// add all new positions to the free list
		for(int i=n;i<n2;i++)
		{
			freeIdList.add(i);
		}
		
		// use the new array
		a=newArray;
	}


	// Find empty slot, create more slots if needed.
	// A problem is that an index in the free list might have been taken by the add(int index, E e) method. The solution is to take from free list and check that the position really is free.
	protected int findUnusedId()
	{		
        for (;;)
        {
			if ((freeIdList.size()*4) < a.length)
			{
				makeArrayLarger();
			}
			
			final int i = freeIdList.remove();
	
			// A problem is that when we load database from file objects shall keep the ID that had.
			// IDs in free list is not always free, if add(int index, E e) has been used.
			// So we let them keep it but then the slots in freeIdList are not all free. So we must check for that here.
        	if (a[i]==null)
        	{
    			return i;
        	}
        }
	}



    // Remove the object at position index
    public void remove(int index)
    {
    	super.remove(index);
    	freeIdList.add(index);
    }



	@Override
	public Iterator<E> iterator() {
		return new D1QuarantineIterator<E>(this);
	}
    
	
	public int getCapacity()
	{
		return a.length/2;
	}
	
	
    public void clear()
    {
    	a=new Object[initialCapacity*2];
    	
		for(int i=0;i<a.length;i++)
		{
			freeIdList.add(i);
		}

        size=0;
    }

}

