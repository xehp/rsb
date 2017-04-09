// DbIterator.java
//
// Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
// License: www.eit.se/rsb/license
//
// History:
// Created by Henrik 2015 

package se.eit.d1_pkg;


import java.util.Iterator;
import java.util.NoSuchElementException;


public class D1Iterator <T> implements Iterator<T> {

	D1List<T> dbList;
	int index=-1;
	int nextIndex=0;
	
	public D1Iterator(D1List<T> dbList)
	{
		this.dbList=dbList;
	}
	
	
    public boolean hasNext() {
    	final int m = dbList.getCapacity();
    	nextIndex = index + 1;
    	while (nextIndex<m)
    	{
    		T n=dbList.get(nextIndex);
    		if (n!=null)
    		{
    			return true;
    		}
    		++nextIndex;
    	}
    	return false;
    }

    public T next() {
    	final int m = dbList.getCapacity();

    	index = nextIndex;
    	while (index<m)
    	{
    		T n=dbList.get(index);
    		if (n!=null)
    		{
    			nextIndex=index+1;
    			return n;
    		}
    		++index;
    	}
    	throw(new NoSuchElementException());
    }

    public void remove() {
		dbList.remove(index);
    }
}