/*
DbBase.java

The data base object (DbBase) is that base object for objects that can be stored in our data base.
If they need to be able to contain other data base objects then the extension DbContainer can be used.


Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2013-02-14
Created by Henrik Bjorkman (www.eit.se)

*/


package se.eit.d1_pkg;

import java.io.IOException;
import java.io.PrintWriter;

import se.eit.web_package.*;


//import javax.media.opengl.GL2;

// DbBase is the base class for DbStorable.
//
// In DbBase we put methods that shall be implemented by extending (alias sub, specialized, derived) classes.
// Don't put variables in this class. Create a sub class for that.
// In this class put only:
// * methods that don't need class variables. 
// * abstract methods
// * methods returning a default value
// * methods giving an error message if not overridden by derived class.
// Most classes that extend this class shall also be listed in RsbFactory so that they can be read/parsed from file.

// TODO Db was not a very good name. Want to rename all Db to Mirror.

public abstract class D1Base {


	public static void static_error(String str)
	{
		WordWriter.safeError(className()+": "+str);
		System.exit(1);
	}

	public static void static_debug(String str)
	{
		WordWriter.safeDebug(className()+": "+str);
	}
	
	protected String debugPrefix()
	{
		String prefix=getType();
		prefix+="("+this.getNameAndId()+")";
		return prefix;
	}
	
	public void debug(final String str)
	{
		WordWriter.safeDebug(debugPrefix()+": " + str);
	}
	
	
	public void error(String str)
	{
		WordWriter.safeError(className()+" "+getNameAndPath("/")+" "+debugPrefix()+": "+ str);
		System.err.println("will do System.exit(1) now");
		System.exit(1);
	}
	
	protected void println(String str)
	{
		System.out.println(className()+": " + str);
	}

	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return D1Base.class.getSimpleName();	
	}

	public String getType()
	{	
		return this.getClass().getSimpleName(); // This gives the name of the extended class, that is this does not always give string "DbBase".
	}

	public abstract String getDbSubRootNameAndPath(String separator);
	/*
	public String getDbSubRootNameAndPath(String separator)
	{	
		DbSubRoot ro = this.getDbSubRoot();
		if (ro!=null)
		{
			return ro.getNameAndPath(separator);
		}
		return "unknown";
	}
    */
	
	// Gives full path to the absolute root of all data bases loaded
	public String getNameAndPath(String separator)
	{
		error("getNameAndPath");
		return null;
	}
	
	// Gives full path within a data base.
	public String getNameAndPathInternal(String separator)
	{
		error("getNameAndPathToRoot");
		return null;
	}
  	
	// deprecated, use getIndexPathWithinDbSubRoot("/");
	public String getIndexPathWithinDbSubRoot()
	{
		return getIndexPathWithinDbSubRoot(".");
	}
	
	// If separator is given as " " then this is the inverse of getObjFromIndexPathWithinDbSubRoot or findObjectByNameIndexOrId.
	public abstract String getIndexPathWithinDbSubRoot(String separator);
  	
	public abstract String getIndexPath(String separator);

	/*
	public static String getTypeStatic()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return DbBase.class.getSimpleName();
	}
	*/
		
		
	/*
	public String getTypePath()
	{
		return getType();
	}
	*/


	/*public final String getName()
	{
		return name;
	}*/
	

	public D1Base()
	{
	}
	
	// Copy constructor, is this used at all?
	public D1Base(final D1Base org)
	{
		error("is this used?");
	}
	
	
	
	// deserialise from wr
	// sub classes with additional member variables that need 
	// to be saved to disk shall implement this method.
	// Classes overriding this shall make sure to call the same in super class.
	public abstract void readSelf(WordReader wr);	

	// Serialise to ww
	// Write all available variables within this object.
	// Sub classes with additional member variables that need to be saved to disk must implement this method.
	// Those overriding methods shall also call "super.write".
	// Classes overriding this shall make sure to call the same in super class.
	public abstract void writeSelf(WordWriter ww);

	
	public abstract void writeRecursive(WordWriter ww);
	
	public abstract void readRecursive(WordReader wr);
	public abstract void readSelfRecursiveAndLink(D1Container parent, WordReader wr);

	// This is a clever way to make a string from a stream, will keep it for reference.
	// http://c2.com/cgi/wiki?StringBuffer
	/*
	public String toString()
	{
		StringWriter stringWriter = new StringWriter();
		PrintWriter out = new PrintWriter (stringWriter);			
		WordWriter ww=new WordWriter(out);
		writeSelf(ww);
		ww.close();
		return stringWriter.toString();
	}
	*/
	
	public String toString()
	{
		WordWriter ww=new WordWriter();
		writeSelf(ww);
		return ww.getString();
	}
	
	// Gives the index (alias slot) at which this object is stored in the containing obj.
	public abstract int getIndex();
	
	// The root of all root objects should also be the tick list so for now (when this comment was written) supper root and tick list are the same. 
	public abstract D1TickList getDbSuperRoot();

	public abstract D1SubRoot getDbSubRoot();

	public abstract D1IdList getDbIdList();

	public abstract void debugReadLock();
   
    public abstract void debugWriteLock();
    
	// This must be overridden in an extended class.
    public void incDbChangedCounter()
    {
		error("incDbChangedCounter");    	
    }
    
	public abstract boolean isChanged(int rootCounter);
	
	// Gives the object in which this object is stored.
	public abstract D1Container getContainingObj();
	
	public D1Container getParent()
	{
		return getContainingObj();
	}
	
	protected abstract void setContainingObjIndex(D1Base bo, int i);
	
	// Classes overriding this shall make sure to call the super.listInfo()
	public abstract void listInfo(WordWriter pw);
	
	// returns 1 if info to set was found, -1 if not found.
	// Classes overriding this shall make sure to call the super.setInfo() if the info was not yet found.
	public int setInfo(WordReader wr, String infoName)
	{
		return -1;
	}
	
	// To be defined in class DbNamed
	public abstract String getName();
	
	// Class DbIdObj is expected to override this.
	public int getId()
	{
		return -1;
	}
	
	/*
	// This must be overridden in an extended class.
	public void writeIdOrPath(WordWriter ww)
	{
		ww.writeString(getIndexPathWithinDbSubRoot(" "));
		error("incDbChangedCounter");    	
	}
	*/
	
	// getObjectById, This must be overridden in an extended class.
	public D1IdObj getDbIdObj(int i)
	{
		error("getDbIdObj");    	
		return null;
	}
	
	// This must be overridden in an extended class.
	/*public D3ObjWithPos getContainingD3ObjWithPos()
	{
		error("getContainingD3ObjWithPos");    	
		return null;		
	}*/
	
	//public abstract void tickSelfMs(int deltaMs);
	//public abstract void tickRecursiveMs(int deltaMs);
	
	/*
	public void tickMs(long tickMs)
	{
		error("tickMs");    	
	}
	*/
	
	// Remove and delete all sub objects, recursively
	public abstract void clearStored();
	
	//protected abstract DbBase getObjFromIndexWithinDbSubRootWr(String path, WordReader wr);
	
	/*public int moveBetweenRooms(final D3ObjWithPos from, final D3ObjWithPos to)
	{
		error("moveBetweenRooms");    	
		return -1;		
	}*/
	
	// Find a sub object of given class.
	// A Call to this can look like this: EmpireTerrain et=(EmpireTerrain) bo.findRecursive(0, EmpireTerrain.class);
	public abstract D1Base findRecursive(int recursionDepth, Class<?> cls);
	
	// Unfortunately this can only find objects that are exactly of the given class 
	// That is if an object is of an extended class then it is not found when looking for one of its base classes.
	//public abstract DbBase getContainingObjOfClass(Class<?> cls);
	
	public abstract void listChangedObjects(int previousUpdateCounter, WordWriter ww);

	
	// This method is supposed to be overridden in an extended class.
	/*
	public void registerInDbIdList()
	{
		error("registerInDbIdList");
	}
	*/
	
	
	// Removes an object stored in this object. But this does not remove it from all lists such as DbIdList.
	// If the stored object is to be deleted and never used again use deleteObject instead.
	// deprecated, the name is misleading since it sounds like the opposite of addObject which this is not. Use removeDbStorable instead.
	protected abstract int removeObject(D1Storable obj_to_remove);
	
	// Removes an object stored in this object. But this does not remove it from all lists such as DbIdList.
	// If the stored object is to be deleted and never used again use deleteObject instead.
	protected abstract int removeDbStorable(D1Storable obj_to_remove);
	
	
	// Removes an object from all container and IdList
	protected void deleteObject(D1Base obj_to_remove)
	{
		if (obj_to_remove.getParent()!=this)
		{
			error("deleteObject: not my child: "+obj_to_remove.getName());
		}
		obj_to_remove.unlinkSelf();
	}
	
	
	// link this object with all DbContainer and DbIdList etc that it shall be registered in.
    // objects stored this way shall do unlinkSelf if they are to be removed.
	// Classes like DbIdObj and DbStorable shall have a specialization of this.
	// Classes overriding this shall make sure to call the same in super class.
	public abstract void linkSelf(D1Container parentObj);	
	
	// unlink this object from all list it is part of for example DbContainer and DbIdList
	// All objects that have been stored with a call to storeSelf shall use this method to unlink themselves when they are to be removed.
	// Classes like DbIdObj and DbStorable shall have a specialization of this.
	// Classes overriding this shall make sure to call the same in super class.
	public abstract void unlinkSelf();	
	
	public abstract void unlinkRecursive();	

	
	public abstract int addObject(D1Base gameObjToAdd);

	public abstract String getNameOrIndex();
	
	public abstract D1Base findDbNamedRecursive(String name);
	
	public abstract D1Base findDbNamedRecursive(String name, int recursionDepth);

	//public abstract DbBase findDbNamedRecursive(String name, int recursionDepth, Class<?> cls);	

	public abstract void listNameAndPath(PrintWriter pw, int recursionDepth, String prefix);

	public abstract D1Base getObjFromIndex(int index);
	
	// An alternative to this is findObjectByNameAndType if name and type is known.
	public abstract D1Base findGameObjNotRecursive(String name);
		
	public abstract D1Base findRelativeFromNameOrIndex(String nameOrIndex);	
	
	public abstract String getObjInfoPathNameEtc();
	
	public abstract void listSubObjects(WordWriter ww);
			
	// This must be overridden in an extended class.
	public void registerSelfInDbIdListAndAddUsingLockWrite(D1Base container)
	{
		error("registerSelfInDbIdListAndAddUsingLockWrite");		
	}

	public void setUpdateCounter()
	{
		error("setUpdateCounter");
	}
	
	public int getDbChangedCounter()
	{
		error("getDbChangedCounter");
		return -1;
	}
	
	
	//public abstract DbBase getObjFromIndexPathWithinDbSubRoot(String path);
	//public abstract DbBase getObjFromIndexPath(String path);

	public abstract D1Base findObjectByNameIndexOrId(WordReader wr);
	public abstract D1Base findObjectByNameIndexOrId(String path);

	
	// getSubObj
	// gives a list of all sub objects, alias child objects
	// If there are much to be done with the sub objects (such as sending data to a client) 
	// this can be better than iterating since this read lock only while making the list.	
	// Returns an array with references (not copies) to all sub objects.
	public abstract D1Base[] getListOfSubObjects();

	public D1Base[] getListOfSubObjectsThreadSafe()
	{
		error("getListOfSubObjectsThreadSafe");	
		return null;		
	}
	
	public abstract long getGameTime();
	
	public abstract String toShortFormatedString();
	
	public abstract boolean isThisOrAnyParentAnIdList();

	public boolean isVisibleTo(D1Base observingObj)
	{
		return true;
	}

	public abstract boolean isRecursiveParentOf(D1Container o);

	// Most objects shall have a name set. 
	// When a named object is registered it is may also create its fixed child objects.
	// The object must be linked before calling regName. Since this triggers creation of fixed child objects anything needed for that must also have been set before calling this. 
	public abstract void regName(String name);

	// This will try to find a child object with a given name and type. If it is not found it is created  so this should always return an object of given type.
	public abstract D1Base findOrCreateChildObject(String objName, String objType);

	// To search for something given by name only use findGameObjNotRecursive
	// This returns null if object is not found. If it is desired that object is created if not found then use findOrCreateChildObject.
	public abstract D1Base findObjectByNameAndType(String objName, String objType);
	
	public String getStringValue()
	{
		return null;
	}
	
	// This is called to send an ascii message to this object. That is to say something to this object. Some objects will pass the message on to their children, others will queue it to be sent to a client.
	// NOTE don't confuse this with this object sending a message (to someone else).
	public void postMessageToThis(String str)
	{
		//debug ("postMessage ingnored str: "+str);
		return;
	}
	
	// This moves the object form current parent to another.
	public abstract int relinkSelf(D1Container to);

	// Makes a relative path from an absolute
	public String makeRelativePath(String path)
	{
		String relPath="";
		String myPath=this.getIndexPathWithinDbSubRoot("/");

		WordReader wr1=new WordReader(myPath);
		WordReader wr2=new WordReader(path);
		
		while((wr1.isOpenAndNotEnd())&&(wr2.isOpenAndNotEnd()))
		{
			int i1=wr1.readInt();
			int i2=wr2.readInt();
			if (i1!=i2)
			{
				while(wr1.isOpenAndNotEnd())
				{
					wr1.readInt();
					relPath+="../";
				}
				relPath+="../"+i2;
				
				while(wr2.isOpenAndNotEnd())
				{
					int i=wr2.readInt();
					relPath+="/"+i;
				}
				
				break;
			}
		}
		
		
		return relPath;

	}

	public abstract void registerInterferenceRecursive();
	
	public abstract String getNameAndId();
	
	public D1GlobalConfig getGlobalConfig()
	{
		if (this.getParent()!=null)
		{
			return this.getParent().getGlobalConfig();
		}
		return null;
	}
	
	// Classes overriding this shall make sure to call the super.getInfo() if the info was not yet found and set.
	public int getInfo(WordWriter ww, String parameterName, WordReader wr)
	{
		if (parameterName.equals("type"))
		{
			ww.writeString(getType());
			return 1;
		}
		return 0;
	}

	
	// This is called when an object with a reserved name is registered.
	// Reserved names are names that begin with '_'.
	// Extending classes shall override this if they need to know when a named object was added (or renamed).
	// Typically this works in cooperation with createNamedObjects. All objects that can be created in initAll shall be taken care of in this method.
	// Classes overriding this shall make sure to call the same in super class.
	// Returns true if registered. Some error reaction shall be taken if not found.
	public boolean regNamedChildObject(D1Named addedObject)
	{
		return false;
	}

	// When this is called the object may create its own fixed sub objects. It is called from method "regName" in "DbNamed".
	// Typically this works in cooperation with regNamedObject. Objects that can be registered by regNamedObject shall be created here.
	// Classes overriding this shall make sure to call the same in super class.
	public void createNamedObjects()
	{
	}

	public D1Base findObjectByNameIndexOrId(WordReader wr, WordWriter ww)
	{
		String n=wr.readWord();
		WordReader wr2=new WordReader(n);				
		D1Base obj=findObjectByNameIndexOrId(wr2);
		if (obj==null)
		{
			ww.writeLine("Did not find "+n);			
		}
		return obj;
	}
	
	// extending classes shall call super
	// Returns true if command was found.
	public boolean interpretCommand(String cmd, WordReader wr, WordWriter ww) throws IOException
	{
		// do nothing
		if (cmd.equals("cat")) 
		{
			/*D1Base obj=findObjectByNameIndexOrId(wr, ww);
			if (obj!=null)
			{
				obj.listInfo(ww);				
			}*/
			listInfo(ww);
			return true;
		}
		
		if (cmd.equals("pwd"))
		{
			ww.println(getNameAndPath("/"));
			return true;
		}

		if (cmd.equals("ls"))
		{
			/*D1Base obj=findObjectByNameIndexOrId(wr, ww);
			if (obj!=null)
			{
				obj.listSubObjects(ww);
			}*/
			listSubObjects(ww);
			return true;
		}
		
		if ((cmd.equals("ts")) ||  (cmd.equals("dump")))
		{
			ww.println(toString());
			return true;
		}
		if (cmd.equals("dr"))
		{
			writeRecursive(ww);
			return true;
		}
		if (cmd.equals("ds"))
		{
			writeSelf(ww);
			return true;
		}

		return false;
	}

	/*
	public void assignIdRecursively()
	{
	}
	*/

	public boolean needIdList()
	{
		return true;
	}
	
	public void lockWrite()
	{
	}

    
	public void unlockWrite()
	{
	}

	public void lockRead()
	{
	}

	public void unlockRead()
	{
	}

	// Returns true if help for the given command was found
    public boolean helpCommand(String cmd, WordReader wr, WordWriter ww, String hist) throws IOException
    {
		if ((cmd==null) || (cmd.length()==0))
    	{
			ww.writeLine("Base object commands:");
			ww.incIndentation();
			ww.writeLine("cat : list available info about an object");				
			ww.writeLine("ls : List sub objects");
			ww.writeLine("ts : show all data in default object as string (ts=ToString)");
			ww.writeLine("dr : dump recursively");
			ww.writeLine("ds : dump self");
			ww.writeLine("");
			ww.decIndentation();
    	}
    	
		if (cmd.equals("cat")) 
		{
			ww.println("list available info about an object");
			return true;
		}
    	
    	return false;
    }

    public boolean isPartOfSubRoot(D1Base subRoot)
    {
    	return false;
    }
}

