/*
DbContainer.java

The data base object (DbBase) is that base object for objects that can be stored in our data base.
If they need to be able to contain other data base objects then this extension DbContainer can be used.
Perhaps this should be called Parent instead of Container?


Copyright (C) 2016 Henrik Bjorkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2013-02-27
Created by Henrik Bjorkman (www.eit.se/hb)

*/


package se.eit.d1_pkg;

import se.eit.rsb_factory_pkg.RsbFactory;
import se.eit.web_package.*;



public abstract class D1Container extends D1Storable {

	
	public D1List<D1Storable> listOfStoredObjects=null;
	

	
	public static void static_error(String str)
	{
		System.out.flush();
		System.err.println(className()+": static_error: "+str);
		Thread.dumpStack();
		System.exit(1);
	}
	
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return D1Container.class.getSimpleName();	
	}

	
	public D1Container()
	{
		super();	
	}

	// copy constructor, remember to create new objects of everything (dont't just simply copy references)
	public D1Container(final D1Container org)
	{	
		super(org);
	
		error("Not implemented yet, all sub objects are ignored");		
		/*
		listOfStoredObjects=new DbBase[org.listOfStoredObjects.length];
		
		for(int i=0;i<org.listOfStoredObjects.length;i++)
		{
			listOfStoredObjects[i]=org.listOfStoredObjects[i].deepCopy();
			listOfStoredObjects[i].superObj=this;
			listOfStoredObjects[i].superObjIndex=i;
		}
		*/
	}
	
	public void readChildrenRecursive(WordReader wr) throws NumberFormatException
	{
		clearStored();
		
		String m = wr.readToken("");

		
		// Temporary workaround, this should not be needed, remove later...
		/*if (m.equals(""))
		{
			debug("expected begin marker '"+WordWriter.BEGIN_MARK+"' but found empty string");
			m = wr.readWord();
		}
		else if (m.equals(","))
		{
			// In JSON this means there is more data to come, such as child objects
			m = wr.readToken("");			
		}
		else*/
			
		if (m.equals(WordWriter.BEGIN_MARK))
		{
			// This indicates that the file is in our own proprietary format (not JSON)
			while (!wr.isNextEnd())
			{
				//int index = wr.readInt();				

				// get type of object to parse
				String t = wr.readString(); 

				// create an object of the desired type
				D1Base bo=RsbFactory.createObj(t);
				
				// Let the object read its own data
				bo.readSelfRecursiveAndLink(this,wr);
			}
			final String e = wr.readWord();
			if (!e.equals(WordWriter.END_MARK))
			{
				debug("expected '"+WordWriter.END_MARK+"' but found '"+e+"'");
			}		
		}
		else if (m.equals("]"))
		{
			// In JSON this marks the end of an array, the first ] would be the one ending the objects own properties.
			
			// Now we expect one more ] or a ,
			// If there are child objects we should get a , here.			
			m = wr.readToken("");
			if (!(m.equals("]") || (m.equals(","))))  
			{
				debug("expected ] or ,");
			}

			// If it was a , then we expect an array with child objects enclosed with [] and after all that a ] to end the array of properties and child objects  
			if (m.equals(","))
			{
				// The object did have child objects
			
				// we expect a [ here, it marks the beginning of the array with child objects-
				m = wr.readToken("");
				if (!m.equals("[")) 
				{
					debug("expected [");
				}
	
				// now loop until we got the ] that marks the end of the array of child objects
				while(!m.equals("]"))
				{
	
					// we expect a { here, it marks the beginning of an object (all children are objects, no string etc here)
					m = wr.readToken("");
					if (!m.equals("{")) 
					{
						debug("expected {");
					}
					
					// get type of object to parse
					String t = wr.readString(); 
	
					// create an object of the desired type
					D1Base bo=RsbFactory.createObj(t);
					
					// Let the object read its own data
					bo.readSelfRecursiveAndLink(this, wr);
	
					// we expect a } here, this ends the object.
					m = wr.readToken("");;
					if (!m.equals("}")) 
					{
						debug("expected }");
					}
					
					// Now we expect one more ] or ,
					// If there are child objects we should get a , here.
					m = wr.readToken("");
					if (!(m.equals("]") || (m.equals(","))))  
					{
						debug("expected ] or ,");
					}
				}
				// Now we expect one more ] to end the array of properties and child objects.
				m = wr.readToken("");
				if (!m.equals("]")) 
				{
					debug("expected ]");
				}
			}
			
			
			
		}
		else
		{
			debug("expected begin marker '"+WordWriter.BEGIN_MARK+"' but found '"+m+"'");			
		}
	}

	
	
	// read self and all child objects
	@Override
	public void readSelfRecursiveAndLink(D1Container parent, WordReader wr) throws NumberFormatException
	{
		clearStored();

		readSelf(wr);
		
		linkSelf(parent);
				
		readChildrenRecursive(wr);
	}


	public void readRecursive(WordReader wr) throws NumberFormatException
	{
		if (this.listOfStoredObjects!=null)
		{
			debug("object should not have child objects when doing readRecursive");
			clearStored();
		}
			
		readSelf(wr);
		
		//linkSelf(parent);
				
		readChildrenRecursive(wr);	
	}
	
	/*
	public static DbBase staticParse(WordReader wr) throws NumberFormatException
	{
		DbBase bo = null;
		String t = wr.readWord(); // get type of object to parse
		bo=RsbFactory.createObj(t);
		bo.readRecursive(wr);
		return bo;
	}
*/
	
		
	
	
	// serialize to ww
	// write all objects stored in this object
	// The object type is to have been written before this is called.
	@Override
	public void writeRecursive(WordWriter ww)
	{		
		//super.write(ww);
		//writeSelf(ww);

		debugReadLock();

		
		ww.writeWord(":[[");
		ww.needSpace=false;
		ww.spaceString=",";
		writeSelf(ww);
		ww.needSpace=false;
		ww.spaceString=" ";
		ww.writeWord("]");
		ww.needSpace=false;
		if (listOfStoredObjects!=null)
		{
			if (listOfStoredObjects.size()!=0)
			{			
				boolean needComma=false;
					ww.writeWord(",[");

					// get the size of the list of stored objects
					final int c = listOfStoredObjects.getCapacity();
					
					for(int i=0;i<c;i++)
					{
						D1Base d = listOfStoredObjects.get(i);
						if (d != null)
						{
							if (needComma)
							{
								ww.writeWord(",");
							}
							needComma=true;
								
				  			ww.writeBegin();

				  			//DbContainer c=(DbContainer)d;
		
							//ww.writeInt(i);
		
							ww.writeString(d.getType());
				  			ww.needSpace=false;
							d.writeRecursive(ww);
				  			ww.needSpace=false;
							//ww.writeWord("]");
				  			ww.needSpace=false;

							ww.writeEnd();
							
						}
					}
					ww.writeWord("]");
	  		}
		}
		ww.writeWord("]");
		//ww.writeEnd();
  		
		
		
	}
		
	
	// Gives number of objects stored in this object,
	// not recursive
	public final int getNSubObjects()
	{
		debugReadLock();
		if (listOfStoredObjects!=null)
		{
			return listOfStoredObjects.size();
		}
		return 0;
	}
	
	
	// For most cases its better to use go.getIndex() directly than this.
	public int getIndexOfObj(D1Base go)
	{		
		// data base shall be read locked when this method is called. But this is called very often so the debug check can be commented out here for performance reasons.
		debugReadLock();

		// Most of these code lines are just for debugging (but not all). We need to know if we have inconsistencies in the database but none of these error should happen of course.
		if (go!=null)
		{
			final int index = go.getIndex();
			
			if ((index!=NO_INDEX))
			{
				if ((index>=0) && (index<listOfStoredObjects.getCapacity()))
				{
					if (listOfStoredObjects.get(index)!=go)
					{
						// This should NOT happen if code is bug free. Perhaps if there is a multi thread problem it can still happen?
						error("index not correct "+index);						
					}
				}
				else
				{
					// This should NOT happen if code is bug free. Perhaps if there is a multi thread problem it can still happen?
					error("index not in range "+index);
				}
				
				return index;
			}
			else
			{
				// This should NOT happen if code is bug free. Perhaps if there is a multi thread problem it can still happen?
				error("index is not set");
			}
		}
		else
		{
			//debug("getIndexOfObj null"); // it is allowed to ask for index of null. It will return NO_INDEX which must be -1.
		}		
		return NO_INDEX;
	}

	
	
	
	
	
	// Remove and delete all sub objects, recursively, perhaps a better name would be unlinkStoredObjects?
	public void clearStored()
	{	
		debugWriteLock();
		
		//super.clear(); // if super class need clear remember to call it here.
		
		if (listOfStoredObjects!=null)
		{
			for (D1Storable s : listOfStoredObjects)
			{
				s.unlinkRecursive();
			}
			
			listOfStoredObjects=null;
		}

	}
	
	public void unlinkSelf()
	{
		//debugWriteLock(); // check not needed as long as clearStored will check

		clearStored();  // Sometimes it is desired to move all child objects to parent instead of deleting them. If so use an override of unlinkSelf that move all child objects. See commented out method below. 
		super.unlinkSelf();
	}
	
	/*
	// When some object are deleted it is not desired that all objects it contained are also deleted.
	// If so use an override of unlinkSelf that moves the child objects.
	// It could look like this.  
	@Override
	public void unlinkSelf()
	{
		if (this.listOfStoredObjects!=null)
		{
			for (DbStorable ds : this.listOfStoredObjects)
			{
				if (ds instanceof DbContainer)
				{
					DbContainer dc=(DbContainer)ds;
					dc.moveBetweenRooms(this.getParent());
				}
			}
		}
		super.unlinkSelf();
	}
    */


	// Deprecated, use unlinkSelf instead.
	public void unlinkRecursive()
	{
		clearStored();
		unlinkSelf();
	}
	
	
	protected void makeNewDbListIfNeeded()
	{
		if (listOfStoredObjects==null)
		{
			listOfStoredObjects = new D1List<D1Storable>();
		}
	}

	// Returns index if ok, 
	// <0 if not ok
	// The caller must set the index in objToAdd to the returned value.
	// Caller must also set containingObj
	protected int addDbStorableToContainer(D1Storable objToAdd)
	{
		debugWriteLock(); // not thread safe, lock database for write before calling

		makeNewDbListIfNeeded();
		
		try
		{
			// Check if the object has an index already.
			int index=objToAdd.getIndex();
			if (index!=NO_INDEX)
			{				
				// It does try to add it at that slot.
				listOfStoredObjects.add(index, objToAdd);
			}
			else
			{
				// It did not, store it with a new index.
				index = listOfStoredObjects.add(objToAdd);
			}
			return index;
		}
		catch (NullPointerException e)
		{
			error("addGameObj null");
			return -1;			
		}
	}
	
	
	// Same as addDbStorableToContainer but will also set containingObj and containingIndex
	protected int addDbStorable(D1Storable objToAdd)
	{
		final int i = addDbStorableToContainer(objToAdd);
		objToAdd.setContainingObjIndex(this, i);
		return i;
	}

	
	// deprecated, the name is misleading since it sounds like the opposite of addObject which this is not. Use removeDbStorable instead.
	@Override
	protected int removeObject(D1Storable obj_to_remove) 
	{
		return this.removeDbStorable(obj_to_remove);
	}
	

	// Non recursive remove of an object.
	// This is not "deleteDbBase". It is not specialized in other classes that keeps track of objects Most importantly the DbIdList.
	// It shall only be used when moving objects between containers (AKA rooms, AKA parent objects).
	// Returns <0 if not ok >=0 if ok
	@Override
	protected int removeDbStorable(D1Storable obj_to_remove) 
	{
		debugWriteLock(); // not thread safe, lock database for write before calling
		
		final int i=obj_to_remove.getIndex();
		
		if ((i >= 0) && (i<listOfStoredObjects.getCapacity()))
		{
			if (listOfStoredObjects.get(i)!=obj_to_remove)
			{
				error("wrong object in list " + i + " "+obj_to_remove.getType());				
			}
			else
			{
				listOfStoredObjects.remove(i);
				
				// If this was the last stored object and the list is big then this is an opportunity to reduce memory usage.
				if ((listOfStoredObjects.size()==0) && (listOfStoredObjects.getCapacity()>8))
				{
					listOfStoredObjects=null;
				}

			}
			//notifySubObjects(obj_to_remove.name + " left the room", obj_to_remove);
		}
		else if (i == NO_INDEX)
		{
			error("no index " + obj_to_remove.getType());
		}
		else
		{
			error("object to remove outside list: " + i+ " "+obj_to_remove.getType());
		}

		obj_to_remove.setContainingObjIndex(null, NO_INDEX);


		
		return 0;		
	}


	// Move this object from one DbContainer to another.
	// Returns zero
	// Special care needs to be taken regarding when this method is called or else things like iterating the list of objects during a tick can get messed up.
	// Derived classes may need to override this method in order to translate things like positions and velocity vectors.
	// deprecated use: 'moveBetweenRooms(DbContainer to)' instead.
	/*
	public int moveBetweenRooms(DbContainer from, DbContainer to)
	{
		if (from!=getContainingObj())
		{
			error("can we move from a room we are not in?");
		}
		else
		{
			moveBetweenRooms(to);
		}
		return 0;
	}
	*/
	
	// Same as moveBetweenRooms but to be used if the object need to be placed at a specified slot in the destination container. Is this used?
	/*
	public int moveBetweenRooms(DbContainer from, DbContainer to, final int newIndex)
	{
		debugWriteLock();

		from.removeObject(this);
		this.setIndex(newIndex);
		to.addDbStorableFull(this);
		return 0;
	}
	*/
	
	// Returns true if this object is in the list of recursive parents of object o
	@Override
	public boolean isRecursiveParentOf(D1Container o)
	{
		D1Container dc=o.getParent();
		if (dc==null)
		{
			return false;
		}				
		if (dc==this)
		{
			return true;
		}
		return isRecursiveParentOf(dc);
	}
	
	// Move this object from its current container to another container.
	// Returns zero if OK
	@Override
	public int relinkSelf(D1Container to)
	{
		debugWriteLock();

		if (to == null)
		{
			debug("Can not move into nothing.");
			return -1;
		}

		
		if (to == this)
		{
			debug("Can not move into itself.");
			return -1;
		}
		
		// Make sure an object is not put into something inside itself.
		if (this.isRecursiveParentOf(to))
		{
			debug("Can not move into a child (or child of a child).");		
			return -1;
		}
		
		D1Base from=getContainingObj();
		from.removeDbStorable(this);
		to.addDbStorable(this);
		this.setUpdateCounter();
		return 0;
	}
	
	
	

	// This is the reverse of getIndexPathWithinDbSubRoot("/")
	// Finds an object from its path within a data base
	// The path shall be a series of indexes separated with '/', looking like this: "1/3/2/4"
	public D1Base getObjFromIndexPathWithinDbSubRoot(String path)
	{
		try
		{
			WordReader wr = new WordReader(path);
			return getObjFromIndexWithinDbSubRootWr(wr);
		}
		catch (NumberFormatException e)
		{
			debug("did not find object at given path '"+path+"'");
			return null;
		}
			
	}

	
	
	// help function for getObjFromIndexWithinDbSubRoot, 
	// Path must be a space or '/' separated list of indexes
	// The findObjectByNameIndexOrId can do the same as this and it allows relative paths an IDs to be used.
	private D1Base getObjFromIndexWithinDbSubRootWr(WordReader wr) throws NumberFormatException
	{
		int index=wr.readInt();
		D1Base bo = getObjFromIndex(index);

		if (!wr.isOpenAndNotEnd())
		{
			return bo;
		}
		else
		{
			D1Container c=(D1Container)bo;
			return c.getObjFromIndexWithinDbSubRootWr(wr);
		}	
	}	

	
	// Get a sub object for a given index.
	public D1Base getObjFromIndex(int index)
	{
		
		// This check should not be needed, remove later when things work more smoothly
		if (listOfStoredObjects==null)
		{
			debug("getObjFromIndex: listOfStoredObjects==null "+index);
			return null;
		}
		else if (index>=listOfStoredObjects.getCapacity())
		{
			//debug("getObjFromIndex: index>=listOfStoredObjects.length "+index+ " "+listOfStoredObjects.length);
			return null;			
		}
		else if (index < 0)
		{
			debug("getObjFromIndex: index<0 "+index);
			return null;						
		}
		
		
		return listOfStoredObjects.get(index);
	}
	
	// deprecated, this is inefficient
	public static D1Base findRelativeFromIndex(String nameOrIndex, D1Base sid[])
	{
		int index=Integer.parseInt(nameOrIndex);
		
		for(int i=0;i<sid.length; i++)
		{
			if (sid[i].getIndex()==index)
			{
				return sid[i];
			}
		}
		return null;
	}

	public static D1Base findRelativeFromName(String nameOrIndex, D1Base sid[])
	{
		for(int i=0;i<sid.length; i++)
		{
			if (sid[i].getName().equals(nameOrIndex))
			{
				return sid[i];
			}
		}
		return null;
	}

	
	// deprecated, this is inefficient, use getChildFromIndexOrName instead.
	public static D1Base findRelativeFromNameOrIndex(String nameOrIndex, D1Base sid[])
	{		
		if (WordReader.isInt(nameOrIndex))
		{
			return findRelativeFromIndex(nameOrIndex, sid);
		}
		else
		{
			return findRelativeFromName(nameOrIndex, sid);		
		}
		
	}

	// Similar to getObjFromIndexWithinDbSubRootWr but it allows .. for parent object and '#' or '~' for ID and it allows names or index to be used.
	// Most important difference is that this stays inside a game data base (not program wide root). 
	// If path begins with '/' it starts from the game root not program wide root (or at least that was intended but does it work?). No all '/' are ignored, but if first char is '/' it should indicate absolute path.
	// TODO: Paths starting on '/' should have been absolute but that does not work. Perhaps caller should check for that since inside a game the sub root is the top object but from console the main.dbRoot is absolute root object.
	public D1Base findObjectByNameIndexOrId(WordReader wr)
	{				
		if (!wr.isOpenAndNotEnd())
		{
			return this;		
		}
		
		String nameOrIndex = wr.readToken("/");
		
		if (nameOrIndex == null)
		{
			return this;			
		}
		
		if (nameOrIndex.equals(""))
		{
			return this;
		}
		else if (nameOrIndex.equals("~"))
		{
			final String idStr=wr.readToken("/");
			try
			{
				final int id=Integer.parseInt(idStr);

				D1SubRoot dr=this.getDbSubRoot();
				if (dr!=null)
				{
					D1Base defaultObj=dr.getDbIdObj(id);
					if (defaultObj==null) return null;
					return defaultObj.findObjectByNameIndexOrId(wr);
				}
				else
				{
					debug("no root object for ~"+this.getId());
					return null;
				}
			}
			catch(NumberFormatException e)
			{
				debug("NumberFormatException ~"+idStr);
				return null;		
			}
		}
		else if (nameOrIndex.equals(".."))
		{
			D1Base defaultObj = this.getContainingObj(); 
			if (defaultObj==null) return null;
			return defaultObj.findObjectByNameIndexOrId(wr);
		}
		else if (nameOrIndex.equals("."))
		{
			return this.findObjectByNameIndexOrId(wr);
		}
		else
		{		
			D1Base db = this.getChildFromIndexOrName(nameOrIndex);
			if (db==null)
			{
				return null;
			}
			return db.findObjectByNameIndexOrId(wr);
		}
	
	}
	
	public D1Base findObjectByNameIndexOrId(String path)
	{
		WordReader wr = new WordReader(path);
		return findObjectByNameIndexOrId(wr);
	}

	
	public D1Base getChildFromIndexOrName(String nameOrIndex)
	{
		if (WordReader.isInt(nameOrIndex))
		{
			int index=Integer.parseInt(nameOrIndex);
			D1Base db=this.getObjFromIndex(index);
			return db;
		}
		else
		{
			if (this.listOfStoredObjects!=null)
			{
				for (D1Base db : this.listOfStoredObjects)
				{
					if (db.getName().equals(nameOrIndex))
					{
						return db;
					}
				}
			}
			
		}
		return null;
	}
	
	
	// Searches recursively for an object with the given type
	// First found is returned even if there are more than one.
	// It will return only objects that match the fully extended class (if a base class is given in cls, objects extending that class is not found with this method)   
	// Databases should be read lock before calling this.
	// http://stackoverflow.com/questions/3397160/how-can-i-pass-a-class-as-parameter-and-return-a-generic-collection-in-java
	public D1Base findRecursive(int recursionDepth, Class<?> cls)
	{
		debugReadLock();
		
		if (listOfStoredObjects!=null)
		{
			for (D1Storable g : listOfStoredObjects)
			{
				if (g.getClass() == cls)
				{
					return g;
				}
	
				// Perhaps recursively in a sub object?
				// if ((recursionDepth>0) && (g instanceof DbContainer))
				if (recursionDepth>0)
				{
					D1Container c=(D1Container)g;
					D1Base s = c.findRecursive(recursionDepth-1, cls);
					if (s!=null)
					{
						return s;
					}
				}
			}		
		}
		return null;
	}
	
	// classes that have their own update counters shall implement this method (that method shall call super as usual)
	// previousUpdateCounter shall be the value root counter had when object update was sent previous time	
	// If a class adds its own listChangedObjects then a parse for the class name needs to be added in ClientThread where it interprets the "updatePartly" message.
	@Override
	public void listChangedObjects(int previousUpdateCounter, WordWriter ww)
	{
		debugReadLock();

		//super.listChangedObjects(previousUpdateCounter, ww);

		// Check sub objects
		if (listOfStoredObjects!=null)
		{
			for (D1Storable d : listOfStoredObjects)
			{
				d.listChangedObjects(previousUpdateCounter, ww);
			}
		}
	}
	

	// getSubObj
	// gives a list of all sub objects, alias child objects
	// If there are much to be done with the sub objects (such as sending data to a client) 
	// this can be better than iterating since this read lock only while making the list.	
	// Returns an array with references (not copies) to all sub objects.
	// Not thread safe, if thread safe is needed use getListOfSubObjectsThreadSafe.
	public D1Base[] getListOfSubObjects()
	{	
		debugReadLock();
		
		final int n=this.getNSubObjects();
		D1Base[] bol=new D1Base[n];

		if (n>0)
		{
			int i=0;
			for (D1Storable d : listOfStoredObjects)
			{
				bol[i]=d;
				i++;
			}
		}
		return bol;		
	}
	
	// Gives the current capacity of the listOfStoredObjects (capacity can increase)
	public int getListCapacity()
	{
		if (listOfStoredObjects!=null)
		{
			return listOfStoredObjects.getCapacity();
		}
		return 0;
	}

	
	// Deprecated, Not a good name on this method since it gives capacity, max index would actually be cpacity-1 
	public int getMaxIndex()
	{
		return getListCapacity();
	}


	
	/*
	@Override
	public void registerSelfAndSubObjectsInDbIdList(DbIdList idList)
	{	
		if (listOfStoredObjects!=null)
		{
			for (DbStorable bo : listOfStoredObjects)
			{
				bo.registerSelfAndSubObjectsInDbIdList(idList);
			}
		}
	}
	*/

	@Override
	public void registerInterferenceRecursive()
	{
		if (listOfStoredObjects!=null)
		for(D1Base db: listOfStoredObjects)
		{
			db.registerInterferenceRecursive();
		}
		
	}

	public int addObject(D1Base gameObjToAdd)
	{
		gameObjToAdd.linkSelf(this);
		return 0;
	}

	public void moveAllChildObjectsToParent()
	{
		if (this.listOfStoredObjects!=null)
		{
			for (D1Storable ds : this.listOfStoredObjects)
			{
				if (ds instanceof D1Container)
				{
					// move to parent object
					D1Container dc=(D1Container)ds;
					dc.relinkSelf(this.getParent());
				}
			}
		}
	}

	// This is just for debugging
	@Override
	public void linkSelf(D1Container parentObj)
	{
		if (this.listOfStoredObjects!=null)
		{
			error("an object can not have child objects befor it has been linked");
			// TODO we could allow this and register the children recursively.
		}
		super.linkSelf(parentObj);
	}
}
