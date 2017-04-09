// DbReservedList.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;

import java.util.Iterator;

// Warning using DbReservedList can easily lead to bad performance if the number of reserved indexes is large and the number of other objects stored is small. In general it is probably better to make an container object with two lists instead of using this.

public class D1ReservedList<E> extends D1List<E> {
	
    private int reservedIndexRange=0;
    
    public D1ReservedList(int reservedIndexRange)
    {
    	super();
    	setReservedIndexRange(reservedIndexRange);
    }
    
    // inc wrap outside reserved range
    protected int incWrap(int i)
	{
		if (++i==a.length)
		{
			i=reservedIndexRange;
		}
		return i;
	}
	


	// Find empty slot, create more slots if needed.
    protected int findUnusedId()
	{
        final int len =  a.length;
		// find an unused position in existing list
		for(int i=reservedIndexRange;i<len;i++)
		{
			lastest=incWrap(lastest);

			// This is just for debugging, can be removed later if all works
			if (lastest<reservedIndexRange)
			{
				System.out.println("error: lastest<reservedIndexRange");
			}
			
			if (a[lastest]==null)
			{
				// An unused slot was found
				return lastest;
			}
		}

		if (a.length<reservedIndexRange)
		{
			lastest=reservedIndexRange;
		}
		else
		{
			lastest=a.length;
		}
					
		return lastest;
	}

    
    // Returns the index at which the element was added
    public int add(E e)
    {        
    	// First try to find a free slot
        final int i = findUnusedId();

    	while(i>=a.length)
    	{
    		makeArrayLarger();
    	}

        a[i] = e;
        size++;
        
		return i;
    }
    



	@Override
	public Iterator<E> iterator() {
		return new D1Iterator<E>(this);
	}

    
    public void clear()
    {
    	a=new Object[4];
    	lastest=reservedIndexRange;
        size=0;
    }


    protected void setReservedIndexRange(int i)
    {
    	reservedIndexRange=i;
    	lastest=reservedIndexRange;
    }


    public int getReservedIndexRange()
    {
    	return reservedIndexRange;
    }
}
