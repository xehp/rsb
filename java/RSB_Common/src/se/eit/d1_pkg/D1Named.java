/*
DbNamed.java

This is an object that can reside in the database, it can contain other objects in its turn and
it has a name.

Perhaps DbNamed would have been a better name for this class?



Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2013-02-27
Created by Henrik Bjorkman (www.eit.se/hb)

*/


package se.eit.d1_pkg;

import java.io.PrintWriter;

import se.eit.rsb_factory_pkg.RsbFactory;
import se.eit.web_package.*;


public class D1Named extends D1Container {
	
	public static final int TICK_PHASE_LAST=0;
	
	
	private String name;


	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return D1Named.class.getSimpleName();	
	}
	
	public D1Named()
	{	
		super();
	}
	
	/*
	public DbNamed(String name)
	{
		super();
		
		this.name=name;
	}
	*/

	/*
	public DbNamed(final DbNamed namedObj)
	{
		super(namedObj);
		
		this.name=namedObj.name;
	}
	*/

	// don't think this is used.
	/*
	public DbNamed deepCopy() {
		return new DbNamed(this);
	}
	*/
	
	
	@Override
	public void readSelf(WordReader wr)
	{
		super.readSelf(wr);
		
		final String tmp=wr.readString();
				
		setName(tmp);
	}
	
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);

  		ww.writeString(name);  			
	}
	

	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("name"))
		{
			name=wr.readString();
			setUpdateCounter();

			return 1;
		}
		return super.setInfo(wr, infoName);
	}

	// Get the name of an object, or its index if it does not have a name.
	@Override
	public String getNameOrIndex()
	{
		if (name==null)
		{
			return ""+getIndex();
		}
		else if (name.equals("null"))
		{
			debug("name null should not be used");
			return ""+getIndex();
		}			
		return name;
	}

	@Override
	public String getName()
	{
		return name;
	}
	
	// This will set an objects name.
	// It will also register the object with its parent if it has any. 
	// Therefore in many places when an object is created it shall be added to its parent before it is registered.
	@Override
	public void regName(String name)
	{
		if (this.name!=null)
		{
			error("already have a name "+this.name);
		}
		else
		{
			this.name=name;
			D1Container parent = this.getParent();
	
			if (parent==null)
			{
				error("objects must be linked before name is registered"); // at least for now...
			}
			else if (name.charAt(0)=='_')
			{
				// Register with parent, needed if this named object is one of its needed own child objects 
				// An object with a reserved name was added, tell parent.
				if (parent.regNamedChildObject(this)==false)
				{
					error("failed to register child object with reserved name '"+name+"'");
				}
			}
			
			// Now we take the opportunity to create our own child objects.
			// When we get our own name set, that is when we also create those child objects with reserved names that we always must have
			createNamedObjects();
	
			setUpdateCounter();
		}
	}

	// This sets name.
	public void setName(String name)
	{
		if ((name!=null) && (name.length()>0) && (name.charAt(0)=='_') && (this.getParent()!=null))
		{
			// Register with parent, needed if this named object is one of its needed own child objects
			regName(name);
		}		
		else
		{
			this.name=name;
		}
	}

	@Override
	public String getNameAndPath(String separator)
	{
		// Is there an object above this one.
		if (containingParentObj!=null)
		{
			final String p=getContainingObj().getNameAndPath(separator);
			final String n=getNameOrIndex();
			return p + separator +n;				
		}
		return /*separator+ separator+*/ getName();
	}


	@Override
	public String getNameAndPathInternal(String separator)
	{
		// Is there an object above this one.
		if ((containingParentObj!=null) && (!(containingParentObj instanceof D1SubRoot)))
		{
			final String p=containingParentObj.getNameAndPathInternal(separator);
			final String n=getNameOrIndex();
			return p + separator +n;				
		}
		return separator + getName();
	}

	// Searches all sub objects for a game object with the given name.
	// First found is returned even if there are more than one.
	// Databases should be read lock before calling this.
	@Override
	public D1Base findDbNamedRecursive(String name, int recursionDepth)
	{
		//debugReadLock();
		final D1SubRoot r=getDbSubRoot();
		
		// This is just for dubugging, can be removed later.
		if (r==null)
		{
			error("No root");
		}
				
		r.debugReadLock();

		if (listOfStoredObjects!=null)
		{			
			for (D1Storable g : listOfStoredObjects)
			{
				if (name.equals(g.getName()))
				{
					return g;
				}
	
				// Perhaps recursively in a sub object?
				if (recursionDepth>0)
				{
					D1Base s = g.findDbNamedRecursive(name, recursionDepth-1);
					if (s!=null)
					{
						return s;
					}
				}				
			}
		}

		return null;
	}


	// Searches all sub objects for a game object with the given name and type
	// First found is returned even if there are more than one.
	// Databases should be read lock before calling this.
	// http://stackoverflow.com/questions/3397160/how-can-i-pass-a-class-as-parameter-and-return-a-generic-collection-in-java
	/*
	@Override
	public DbBase findDbNamedRecursive(String name, int recursionDepth, Class<?> cls)
	{
		//debugReadLock();
		final DbSubRoot r=getDbSubRoot();
		r.lockRead(); // read locking in a recursive call is very inefficient. Should do the read lock in calling method instead.
		try
		{		
			if (listOfStoredObjects!=null)
			{			
				for (DbStorable g : listOfStoredObjects)
				{
					if ((g.getClass() == cls) && (name.equals(g.getName())))
					{
						return g;
					}
		
					// Perhaps recursively in a sub object?
					if (recursionDepth>0)
					{
						DbBase s = g.findDbNamedRecursive(name, recursionDepth-1,cls);
						if (s!=null)
						{
							return s;
						}
					}
				}		
			}
		}
		finally
		{
			r.unlockRead();
		}
		return null;
	}
*/
	
	@Override
	public D1Base findDbNamedRecursive(String name)
	{
		return findDbNamedRecursive(name, Integer.MAX_VALUE);
	}


	@Override
	public D1Base findGameObjNotRecursive(String name)
	{
		return findDbNamedRecursive(name, 0);
	}
	
	// Search from the top
	/*
	public DbNamed findGameObjRecursiveFromTop(String name)
	{
		DbNamed go = findDbNamedRecursive(name, 0);
		if (go!=null) 
		{
			return go;
		}
		go = findDbNamedRecursive(name, 1);
		if (go!=null) 
		{
			return go;
		}
		go = findDbNamedRecursive(name, 2);
		if (go!=null) 
		{
			return go;
		}		
		return findDbNamedRecursive(name, Integer.MAX_VALUE);
	}
	*/
	
	// deprecated, use 	addDbObj instead
	public int addGameObj(D1Base gameObjToAdd)
	{
		return addObject(gameObjToAdd);
	}
	
	
	// not thread safe, non recursive count of sub objects
	public final int countSubGamObjNonRecursive()
	{
		debugReadLock();
		return this.getNSubObjects();
	}
	
	// get a non recursive list of names of all sub objects
	public String[] getAllNamesNonRecursive()
	{
		debugReadLock();
		
		debug("getAllNamesNonRecursive: "+this.getName());
		
		int c=0;
		int n=countSubGamObjNonRecursive();
		String[] sa = new String[n];

		if (listOfStoredObjects!=null)
		{
			for (D1Storable d : listOfStoredObjects)
			{
				String name=d.getName();
				debug("getAllNamesNonRecursive("+c+"): "+name);
				sa[c++]=name;
			}
		}			
		return sa;
	}
	

	public String getObjInfoPathNameEtc()
	{
		String info="";
		
		int id=getId();
		if (id >= 0)
		{			
			info+="id=~"+id;		
		}

		
		info+=", name="+getNameOrIndex();

		info+=", index="+this.getIndex();

		/*if (name!=null)
		{
			info+=" name="+name;
		}
		else
		{
			info+=" "+getIndex();
		}*/
		
		info+=", type="+getType();
		
		final String nap=getNameAndPath("/");			
		info+=", path=/"+nap;
		
		final D1SubRoot ro=getDbSubRoot();
		if (ro!=null)
		{
			final String db=ro.getNameAndPath("/");
			final String idx=getIndexPathWithinDbSubRoot("/");
			info+=", db=/"+db;
			info+=", indexPath=/"+idx;
		}
				
		return info;
	}

	
	@Override
	public void listNameAndPath(PrintWriter pw, int recursionDepth, String prefix)
	{
		if (listOfStoredObjects!=null)
		{
			for (D1Storable bo : listOfStoredObjects)
			{
				String npe=bo.getNameAndPath("/");
				pw.println(prefix+npe);
				pw.flush();
				if (recursionDepth>0)
				{
					bo.listNameAndPath(pw, recursionDepth-1, prefix+"  ");
				}
			}
		}
	}


	// List available variables within this object.
	// This method needs to be implemented by sub classes if they contain variables that shall be listed in the server console by "list" command. Those overriding methods shall also call "super.listInfo".
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		
		pw.println("type"+" "+getType());
		pw.println("name"+" "+getName());
		
	}
	
	@Override
	public int getInfo(WordWriter ww, String parameterName, WordReader wr)
	{
		if (parameterName.equals("name"))
		{
			ww.writeWord(getName());
			return 1;
		}
		
		return super.getInfo(ww, parameterName, wr);
	}	
	
	public String toShortFormatedString()
	{
		final int index=getIndex();
		final int id=getId();

		if (this.containingParentObj!=null)
		{
			if (this.containingParentObj.getObjFromIndex(index)!=this)
			{
				debug("index is not correct: "+index);
			}
		}
		
		if ((this.containingParentObj!=null) && (id>0))
		{
			if (this.containingParentObj.getDbIdObj(id)!=this)
			{
				debug("id is not correct");
			}
		}

		
		return String.format("  %-8s ~%-10s %-20s %-32s", index, id, getType(), getNameOrIndex()); 		
	}
	
	
	@Override
	public void listSubObjects(WordWriter ww)
	{
		final D1SubRoot db=this.getDbSubRoot();
		db.lockRead();
		try
		{
			if (listOfStoredObjects!=null)
			{
				// Looping can be done in two ways. As in comment below or as in the code below. One looks better the other is faster.
				/*
				for (D1Storable no : listOfStoredObjects)
				{
					System.out.println(no.toShortFormatedString());
				}
				*/
				final int n=listOfStoredObjects.getCapacity();
				for(int i=0; i<n; i++)
				{
					D1Base d= listOfStoredObjects.get(i);
					if (d!=null)
					{
						ww.writeLine(d.toShortFormatedString());
					}
				}
				
			}
		}
		finally
		{
			db.unlockRead();
		}
	}
	
	
	// To find an object relative to this object using a name or index.	
	// deprecated use findObjectByNameIndexOrId instead
	// Paths starting on '/' should have been absolute but that is not handled here, it must be taken care of before calling this.
	@Override
	public D1Base findRelativeFromNameOrIndex(String nameOrIndex)
	{
		WordReader wr2=new WordReader(nameOrIndex);				
		D1Base obj=findObjectByNameIndexOrId(wr2);
		return obj;
	}
	
	/*
	@Override
	public void tickSelfMs(final int deltaMs)
	{
	
	}
	*/

	// See also findOrCreateChildObject (if object shall be created if it does not already exit).
	@Override
	public D1Base findObjectByNameAndType(String objName, String objType)
	{
		if (this.listOfStoredObjects!=null)
		{
			for (D1Base b: this.listOfStoredObjects)
			{
				if (((objName==null) || (objName.equals(b.getName())))  && ((objType==null) || (objType.equals(b.getType()))))
				{
					return b;
				}
			}
		}
		return null;
	}

	// Find and object, create it if not found, non recursive, thread safe (if object is stored in a database)
	// See also findObjectByNameAndType (if object is wanted only if it exist).
	@Override
	public D1Base findOrCreateChildObject(String objName, String objType)
	{
		if (this.listOfStoredObjects!=null)
		{
			for (D1Base b: this.listOfStoredObjects)
			{
				if ((objName==null) || (objName.equals(b.getName())))
				{
					if ((objType==null) || (objType.equals(b.getType())))
					{
						return b;
					}
					else
					{
						debug("findOrCreateChildObject: Object exist but is wrong type, name="+objName+", type="+objType + ", found:"+b.getType());
					}
				}
			}
		}
		
		// object was not found, create it
		D1SubRoot r = this.getDbSubRoot();
		{		
			try
			{
				r.lockWrite();
			
				D1Base db = RsbFactory.createObj(objType);
				db.linkSelf(this);
				db.regName(objName);
				return db;
			}
			finally
			{
				r.unlockWrite();			
			}
		}
	}

	@Override
	public D1SubRoot getDbSubRoot() {
		
		return containingParentObj.getDbSubRoot();
	}

	@Override
	public String getNameAndId()
	{
		final String name=getName();
		return name+"~-1";
	}
	
	@Override
	public void linkSelf(D1Container parentObj)
	{
		super.linkSelf(parentObj);
		if ((name!=null) && (name.length() > 0) && (name.charAt(0)=='_'))
		{
			if (parentObj.regNamedChildObject(this)==false)			
			{
				error("failed to register child object with reserved name '"+name+"'");
			}
		}
	}

	
	@Override
	public void unlinkSelf()
	{
		if ((name!=null) && (name.length()>0) && (name.charAt(0)=='_'))
		{
			// TODO: Some things are unlinked when a player disconnects. Parts of the avatar body for example. So made this a debug log instead of error for now.
			//debug("fixed sub objects shall not be unlinked");
		}
		
		super.unlinkSelf();
	}
	
	// The following override is only for debugging and can be removed/commented out later.
	@Override
	public int relinkSelf(D1Container to)
	{
		// This is for debugging, it shall not happen
		if ((name!=null) && (name.length()>0) && (name.charAt(0)=='_'))
		{
			// Objects with a reserved name are fixed sub objects and may not be moved to other rooms
			error("fixed sub objects may not move between rooms");
		}
		
		return super.relinkSelf(to);
	}
}
