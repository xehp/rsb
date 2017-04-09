/*
DbSubRoot.java

This is an object that can reside in the database, it can contain other objects in its turn,
it has a name and it can be saved to file. 

This object is called rootObj but since there can be more than one that is not a perfect name.
Perhaps something like "object that when saving to file is saved as its own file" but that is too long.
This is the root of objects that are saved to one file so in that way the name is ok.


Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
2013-02-27
Created by Henrik Bjorkman (www.eit.se/hb)

*/


package se.eit.d1_pkg;
//import se.eit.DeJniPkg.DeProxy; // This is not needed in 2d versions such as flame & empire
import se.eit.rsb_factory_pkg.RsbFactory;
import se.eit.rsb_package.Version;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import se.eit.web_package.*;


// This is intended to be the root object for the database.
// It contains a ReentrantReadWriteLock to be used when reading or writing in the database.
// 

public class D1SubRoot extends D1ThreadSafe {

	private ReentrantReadWriteLock rwl=null;
		
	//boolean needSave=true; // save to disk should be needed only if this is true. This variable is probably not needed.

	// This counter is to be incremented every time a data base is write locked. 
	// That way we can know if it has changed by only looking at this counter. 
	// Further all objects that are changed shall have their changed counters 
	// set to the same value as this. That way we can also know which objects 
	// have been updated since last update and which have not.  
	public int rootUpdateCounter=0; 
	public int rootUpdateCounterLastSaved=-1; 
	
	//StringFifo updateFifos[]=new StringFifo[8]; 	

	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return D1SubRoot.class.getSimpleName();	
	}

	public D1SubRoot()
	{	
		super();
		rwl=new ReentrantReadWriteLock();
	}

	/*
	public DbSubRoot(DbBase parent, String name, GlobalConfig globalConfig)
	{
		super();
		this.setName(name);
		linkSelf(parent);
		this.globalConfig=globalConfig;
		rwl=new ReentrantReadWriteLock();
	}
	*/
	
	
	@Override	
	public void readSelf(WordReader wr)	
	{
		debugWriteLock();
		try 
		{
			super.readSelf(wr);
			rootUpdateCounter=wr.readInt();
			rootUpdateCounterLastSaved=rootUpdateCounter;
		}
		finally
		{
			//unlockWrite();
		}
	}
	
	
	
	@Override	
	public void writeSelf(WordWriter ww)
	{		
		lockRead();		
		try 
		{
			super.writeSelf(ww);
			ww.writeInt(rootUpdateCounter);
		}
		finally
		{
			unlockRead();
		}
	}
	
	
	
	@Override	
	public void readSelfRecursiveAndLink(D1Container parent, WordReader wr)
	{
		lockWrite();		
		try 
		{
			super.readSelfRecursiveAndLink(parent, wr);
		}
		finally
		{
			unlockWrite();
		}
	}
	
	
	public void readChildrenRecursive(WordReader wr) throws NumberFormatException
	{
		//lockWrite();		
		debugWriteLock();
		try 
		{
			super.readChildrenRecursive(wr);
		}
		finally
		{
			//unlockWrite();
		}
	}

	public void readRecursive(WordReader wr) throws NumberFormatException
	{
		lockWrite();		
		try 
		{
			super.readRecursive(wr);
		}
		finally
		{
			unlockWrite();
		}
	}

	
	@Override	
	public void writeRecursive(WordWriter ww)
	{		
		lockRead();		
		try 
		{
			super.writeRecursive(ww);
		}
		finally
		{
			unlockRead();
		}
	}
	
	/*
	public DbNamed findRelativeFromNameOrIndex(String nameOrIndex)
	{
		DbNamed no;
		lockRead();		
		try 
		{
			no=super.findRelativeFromNameOrIndex(nameOrIndex);
		}
		finally
		{
			unlockRead();
		}
		return no;
	}
	*/

	@Override	
	public D1SubRoot getDbSubRoot()
	{
		return this;
	}	

	// It is the responsibility on everyone calling this method to also call the unlockRead
	public void lockRead()
	{
		//debug("lockRead");
		rwl.readLock().lock();
	}

	public void unlockRead()
	{
		//debug("unlockRead");
		rwl.readLock().unlock();
	}

	// It is the responsibility on everyone calling this method to also call the unlockWrite
	public void lockWrite()
	{
		//debug("lockWrite");
		rwl.writeLock().lock();
		incDbSubRootUpdateCounter();
	}

	public void unlockWrite()
	{
		//debug("unlockWrite");
		rwl.writeLock().unlock();
	}
	
	
	public int getNOfDbSubRoot()
	{
		int n=0;
		if (listOfStoredObjects!=null)
		{
			for (D1Storable d : listOfStoredObjects)
			{
				if (d instanceof D1SubRoot)
				{
					++n;
				}
			}
		}
		return n;		
	}
	
	
	// save all sub root objects
	public void saveRecursive()
	{
		lockRead();
		try {			
			
			if (getNOfDbSubRoot()>0)
			{	
				// There are other DbSubRoot so let those save themselves instead
				for (D1Storable go : listOfStoredObjects)
				{
					try
					{
						D1SubRoot ro = (D1SubRoot)go;
						ro.saveRecursive();
					}				
					catch (ClassCastException e)
					{
						error("mixed DbSubRoot and non DbSubRoot in "+ getName()+" "+e);
					}
				}
			}
			else
			{
				// There are no other DbSubRoot so save this
				saveSelf();
			}
		}
		finally
		{
			unlockRead();
		}			
	}
	
	public String getParentPath()
	{
		D1Container dc=this.getParent();
		if (dc!=null)
		{
			return dc.getNameAndPath("/");
		}
		else
		{
			return ".";
		}
		
		
	}
	
	public void saveSelf(File file) throws IOException
	{

		// Create the FileWriter/WordWriter
		FileWriter fw = new FileWriter(file);
		WordWriter fww = new WordWriterPrintWriter(new PrintWriter(fw));

  		final String programNameAndVersion=Version.fileFormatVersion();
  		
		// Saving in JSON format.
		// To verify if the json file syntax is OK try for example:
		// http://www.jsoneditoronline.org/

  		// In JSON everything is enclosed in {}, this is first
  		fww.writeBegin();
  		
  		// Write the version info string, this is used so that if program is changed so that old save files can not be read it can ignore those gracefully.
  		fww.writeString("versionInfo");					
  		fww.writeWord(":");
  		fww.writeString(programNameAndVersion);					
  		fww.writeWord(",");
  		

  		// Write type of object
  		fww.writeString(getType());					
  		
  		// Dump objects internal data
    	writeRecursive(fww);
    				  		
		// In JSON everything is enclosed in {}, this is last
  		fww.writeEnd();
			  
		// End of saving in JSON format.
		
			
		
    	
    	// Close the writer and stream
    	fww.close();
    	fw.close();   	
	}

	
	// This saves a sub root object to disk.
	// Loading sub roots is done in Main.loadExistingWorldsAndPlayers and Main.parse_file. The naming and format here and there need to match.
	public void saveSelf()
	{
		if (getGlobalConfig()==null)
		{
			error("no globalConfig");
		}
		
		
		lockRead();
		try {
			// Check if this object needs to be saved
			if (rootUpdateCounterLastSaved==rootUpdateCounter)
			{
				// It is not changed since previous save or load
				//System.out.println("Did not save '" + getNameAndPath(".") + "', it is not changed.");
			}
			else
			{
				// It needs to be saved
				// http://docs.oracle.com/javase/6/docs/api/java/io/File.html

				// Get saved games folder
				File savesRootDir = new File(getGlobalConfig().savesRootDir);
				final String savesRootPath=savesRootDir.getPath();
				
				// Store in the folder of parent object
				String parentPath = getParentPath();
				File parentDir = new File(savesRootPath+"/"+parentPath);
				final String path=parentDir.getPath();

				// Make sure that folder exist
				parentDir.mkdirs();
				
				// Figure out the filename to use
				final String name=this.getNameOrIndex();
				final String fileExtension=".json";
				String fileName=path+"/"+name+fileExtension;
				System.out.println("saving to " + fileName);

				// Not writing to that name directly, first saving to temporary file name "..._part~".
				File f1 = new File(fileName+"_part~");

				saveSelf(f1);			
	        	
	        	// Now that the file is saved remove old backup file if any.
				File f2 = new File(fileName+"~");
				if (f2.exists())
				{
					f2.delete();
				}

	        	// Rename old file to the backup name and the new _part file to the main name.
				File f3 = new File(fileName);
				File f4 = new File(fileName+"~");
				f3.renameTo(f4);
				f1.renameTo(f3);
	        	
	        	
	        	System.out.println("path " + f3.getAbsolutePath());
	        	
	        	rootUpdateCounterLastSaved=rootUpdateCounter;
			}
		} catch (IOException e) {
			System.err.println("\nSave to file failed");
			e.printStackTrace();       	
		}
		finally
		{
			unlockRead();
		}			
	}


	// deprecated, use findGameObjNotRecursive instead
	/*
	public GameObj findGameObj(String name)
	{
		debug("findGameObj is deprecated");
		return findGameObjNotRecursive(name);		
	}
	*/
	

	
	// Searches all sub objects for a game object with the given name.
	// First found is returned even if there are more than one.
	// Databases should be read lock before calling this.
	public D1SubRoot findDbSubRootRecursive(String name, int recursionDepth)
	{
		debugReadLock();
		
		if (listOfStoredObjects!=null)
		{
			for (D1Storable g : listOfStoredObjects)
			{
			  if (g instanceof D1SubRoot)
			  {
				D1SubRoot r = (D1SubRoot)g;
				if (name.equals(g.getName()))
				{
					return r;
				}
	
				// Perhaps recursively in a sub object?
				if (recursionDepth>0)
				{
					D1SubRoot s = r.findDbSubRootRecursive(name, recursionDepth-1);
					if (s!=null)
					{
						return s;
					}
				}
			  }
			}		
		}
		return null;
	}

	/*
	private DbSubRoot findDbSubRootRecursive(String name)
	{
		return findDbSubRootRecursive(name, Integer.MAX_VALUE);
	}
	*/


	public D1SubRoot findDbSubRootNotRecursive(String name)
	{
		return findDbSubRootRecursive(name, 0);
	}
	
	public D1SubRoot findDbSubRootNotRecursiveReadLock(String name)
	{
		lockRead();
		try
		{
			return findDbSubRootRecursive(name, 0);		
		}
		finally
		{
			unlockRead();			
		}
	}

	
	// To find a database (alias root object). 
	// Thread safe, not recursive.
	public D1SubRoot findDb(String name)	
	{
		D1SubRoot db = null;

		lockRead();
		try
		{
			db = findDbSubRootNotRecursive(name);
		}
		finally
		{
			unlockRead();
		}
		
		if (db==null)
		{
			debug("did not find DbSubRoot "+name);
		}
		
		return db;		
	}
	

	public int addObjectAndSave(D1Base gameObjToAdd)
	{
		int r=0;
		lockWrite();
		try {
			r=super.addObject(gameObjToAdd);
			saveRecursive();
		}
		finally
		{
			unlockWrite();
		}
	
		return r;		
	}
	
	/*
	public DbNamed findDbNamedRecursive(String name, int recursionDepth)
	{
		lockRead();
		try {
			return super.findDbNamedRecursive(name, recursionDepth);
		}
		finally
		{
			unlockRead();
		}			
	}
	*/
	
	@Override	
	public void listChangedObjects(int previousUpdateCounter, WordWriter ww)
	{
		lockRead();
		try {
			super.listChangedObjects(previousUpdateCounter, ww);
		}
		finally
		{
			unlockRead();
		}			
	}
	
	@Override	
	public void listSubObjects(WordWriter ww)
	{
		lockRead();
		try {
			super.listSubObjects(ww);
		}
		finally
		{
			unlockRead();
		}			
	}

	
	/*
	public int addGameObj(DbBase gameObjToAdd)
	{
		int r=0;
		try {
			lockWrite();
			r=super.addGameObj(gameObjToAdd);
		}
		finally
		{
			unlockWrite();
		}
	
		return r;		
	}
	*/
	// Call this from methods for which a read lock should have been made before calling.
	@Override	
    public void debugReadLock()
    {
    	// These code lines can be commented out when the code has been tested.
    	if (D1GlobalConfig.DEBUG_READ_LOCKS)
    	{
	    	if ((rwl!=null) && (!rwl.isWriteLockedByCurrentThread()) && (rwl.getReadHoldCount()==0))
	    	{
	    		//debug("it should be read locked but was not "+getName());
	    		System.out.flush();
	    		System.err.println(className()+" it should be read locked but was not '"+getName()+"'");
	    		Thread.dumpStack();	    		
	    	}
    	}
    }
	
	// Call this from methods for which a write lock should have been made before calling.
	@Override	
    public void debugWriteLock()
    {
    	// This code can be commented out when the code has been tested.
    	if ((rwl!=null) && (!rwl.isWriteLockedByCurrentThread()))
    	{
    		error("it should be write locked but was not '"+getName()+"'");
    	}
    }
    
    
    private void incDbSubRootUpdateCounter()
    {
    	rootUpdateCounter++;
    }
    
	public int getDbSubRootUpdateCounter()
    {
    	return rootUpdateCounter;
    }
    
    
	@Override	
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		pw.println("rootUpdateCounter"+" "+rootUpdateCounter);
	}
    
	//public abstract void notifySubscribers(int id);

	@Override
	public String getNameAndPathInternal(String separator)
	{
		return separator;
	}



	
	
	// The files we parse here is written from D1SubRoot.saveSelf so if something is changed there it may need changing here also.	
	private void parse_file(File f, final String name) throws FileNotFoundException
	{
	
		// This is the final part of the full file name
		debug("parsing '" + f.getAbsolutePath()+"', '" + name + "', to be stored in '"+ getNameAndPath("/")+"'");

		// Open the file and create a WordReader to parse it.
		BufferedReader is = new BufferedReader(new FileReader(f));
		WordReader wr = new WordReaderBufferedReader(is);
		
		// Make sure the saved file format is the expected format and version.
		final String expectedProgramNameAndVersion=Version.fileFormatVersion();
		
		if (wr.isNextBegin())
		{
			// The file began with a '{' (alias begin) so this is perhaps JSON
			wr.readWord();

			if (wr.isNextString())
			{
				String v=wr.readString();
				
				if (v.equals("versionInfo"))
				{
					String pnav=wr.readString();
					if (pnav.equals(expectedProgramNameAndVersion))
					{
						String t=wr.readString();
				
						D1Base o=RsbFactory.createObj(t);			
	
						try
						{
							this.lockWrite();
							try
							{
								o.linkSelf(this);
	
							}
							finally
							{
								this.unlockWrite();
							}
							o.readRecursive(wr);
						}
						catch (NumberFormatException e)
						{
							WordWriter.safeError("readRecursive: NumberFormatException: '" + e.getMessage() + "'");
						}
					}
					else
					{
						// Currently we can only read files generated by same version of the program (we do not always update the version string though, in fact usually we do not, so this is not expected to happen often).
						System.out.println("old file version: '"+pnav+"', ignored: '" + f.getAbsolutePath() + "'");
					}
				}
				else
				{	
					// The first object in our save files is expected to be the version info. 
					System.out.println("unknown version, ignored: '" + f.getAbsolutePath() + "'");							
				}
			}
			else
			{
				// This was not the expected file save format, ignore this file.
				System.out.println("not the expected file format, ignored: '" + f.getAbsolutePath()+"'");
			}
		}
		else
		{
			// This was not the expected file save format, ignore this file.
			System.out.println("unknown file format, ignored: '" + f.getAbsolutePath() + "'");				
		}
	}


	// This is not needed in 2d versions such as flame & empire
	/*
	void loadDeJni(File f, final String name) throws IOException
	{
		DeProxy o = new DeProxy();
		try
		{
			this.lockWrite();
			try
			{
				o.linkSelf(this);
			}
			finally
			{
				this.unlockWrite();
			}
			o.regName(name);
			try
			{
				o.loadFile(name, f.getCanonicalPath());
			}
			catch (IOException e)
			{
				System.out.println("loadDeJni: failed to load "+name+" from "+f.getAbsolutePath());
				this.lockWrite();
				try
				{
					o.unlinkSelf();
				}
				finally
				{
					this.unlockWrite();
				}
			}
		}
		catch (NumberFormatException e)
		{
			WordWriter.safeError("readRecursive: NumberFormatException: '" + e.getMessage() + "'");
		}
	}
	*/

	// Saving sub roots is done in DbSubRoot.saveSelf. Then naming here and there need to match.	
	public void loadFolder(File folder) throws IOException
	{
		// Folder exist, now get the list of all files in it.
		File[] listOfFiles = folder.listFiles();
		
		//File f = new File("ugl_save_world.txt");
		
		for (int i = 0; i < listOfFiles.length; i++) 
		{
			// For each file in the folder, check if it is a file, ignore directories.
			File f = listOfFiles[i];										
			if (f.isFile()) 
			{
				// This is a file, get its name
				final String fileName = f.getName();

				/*if (fileName.endsWith(".csrv.json"))
				{
					final String n = path + fileName.substring(0, fileName.length()-10); // remove .json part of file name
					parse_csrv(f, n, dbRoot);
				} else */							
				if (fileName.endsWith(".json"))
				{
					final String n = fileName.substring(0, fileName.length()-5); // remove .json part of file name					
					/*if ((this.getGlobalConfig().useDeJni==true) && this.getName().equals("yukigassen"))
					{
						// This is not needed in 2d versions such as flame & empire
						loadDeJni(f, n);
					}
					else*/
					{
						parse_file(f, n);
					}
				}
				else if (fileName.endsWith(".json~"))
				{
					// silently ignore this, its a backup file.
				}
				else
				{
					debug("ignored file '"+ f.getName()+ "' (not a .json file)");
				}
			}
			else if (f.isDirectory())
			{
				final String dirName = f.getName();
				D1SubRoot subRoot = (D1SubRoot) findOrCreateChildObject(dirName, "D1NoSaveRoot");
				subRoot.loadFolder(f);
			}
			else
			{
				debug("ignored "+ f.getName());
			}

		}
	}

	
	
	@Override
	public boolean needIdList()
	{
		return false;
	}

	
	
}