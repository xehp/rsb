/*
Main.java

To test this use:
websocket-client.html
websocket-client.js




Copyright (C) 2016 Henrik Bj��rkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2013-05-08
Created by Henrik Bjorkman (www.eit.se/hb)

*/



package se.eit.rsb_srv_main_pkg;

import java.awt.Desktop;
import java.io.File;
import java.io.IOException;
//import java.io.InputStream;

import se.eit.rsb_factory_pkg.RsbFactory;
import se.eit.rsb_srv_main_pkg.FileServer;
import se.eit.rsb_srv_main_pkg.SocketServer;
import se.eit.rsb_server_pkg.TickThread;

import java.io.PrintWriter;
import java.net.URI;
import java.net.URISyntaxException;

import se.eit.LoginLobbyPkg.LoginLobbyConnection;
import se.eit.LoginLobbyPkg.LoginLobbyServer;                    // This is not needed in flame version
import se.eit.d1_pkg.*;
import se.eit.flameengine_package.FlameRoom;
import se.eit.web_package.*;



public class Main {

	D1GlobalConfig globalConfig=new D1GlobalConfig();
	D1TickList dbRoot = null;
	D1Base defaultObj=dbRoot; 
	WebServer webServer = null;
	TickThread tickThread = null;
	WordReader wr = null;
	
	public static void debug(String str)
	{
		WordWriter.safeDebug("Main: " + str);
	}

	protected static void error(String str)
	{
		WordWriter.safeError("Main: " + str);	
		System.exit(1);
	}

	
	public static String getIdIfAny(D1Base no)
	{
		final int id=no.getId();
		
		if (id>=0)
		{
			return " (id="+id+")";
		}
		return "";
	}
	   
	
	protected void commandParser(WordReader cmdReader, WordWriter ww) throws IOException
	{
		String cmd=cmdReader.readWord();
		ww.incIndentation();
		//ww.indentString=" .. ";

		if (cmd.equals("help"))
		{
			ww.writeLine("help : Show this text");
			ww.writeLine("");

			if (defaultObj!=null)
			{
				String subCmd = cmdReader.readWord();				
				String hist=" "+cmd;
				defaultObj.helpCommand(subCmd, wr, ww, hist);
			}
			
			ww.writeLine("Main commands:");
			ww.incIndentation();
			ww.println("cd <name or id> : Set default object, to move up do 'cd ..'");
			ww.println("rm : delete default object (be carefull, there will be no warning)");
			ww.println("mv <id> : move default object to the room with id <id>");
			ww.println("sa : Save all databases in server");
			ww.println("save : Save default database (the one where default object is residing)");
			//ww.println("show <index path> : index base path shall be a string sourounded with \"\" like \"3 2 5\"");
			ww.println("ta : set time acceleration (or 0 for pause)");
			ww.println("set <tag> <value> : change a property of default object");
			ww.println("rls <depth>: recursively list sub objects");
			ww.println("quit : Shut down server");
			ww.println("note! Sometimes the argument is a name sometimes and id, sometimes a name is a number but that name/number is not same as id");
			ww.decIndentation();
		}
		else if ((defaultObj!=null) && (defaultObj.interpretCommand(cmd, cmdReader, ww)))
		{
			//debug("Command was handled by default object");
		}	
		else if (cmd.equals("sa"))
		{
			debug("saving all");
			//dbRoot.setGlobalConfig(globalConfig);
			dbRoot.saveRecursive();
		}
		else if (cmd.equals("save"))
		{
			D1SubRoot db=defaultObj.getDbSubRoot();
			debug("saving "+db.getName());
			//db.setGlobalConfig(globalConfig);
			db.saveRecursive();
			
		}
		else if (cmd.equals("cd"))
		{
			String n=cmdReader.readWord();
			
			// Check that input was given.
			if (!n.equals(""))
			{
				D1Base currObj=defaultObj;
				try
				{
					currObj.lockRead();

					D1Base newDefaultObj=null;
					
					if (n.charAt(0) == '/')
					{
						newDefaultObj=dbRoot.findRelativeFromNameOrIndex(n);
					}
					else
					{
						newDefaultObj=defaultObj.findRelativeFromNameOrIndex(n);							
					}
					
					if (newDefaultObj!=null)
					{
						defaultObj=newDefaultObj;
					}
				}
				finally
				{
					currObj.unlockRead();
				}
			}
			else
			{
				// Set the dir to default
				defaultObj = dbRoot;
			}
			//debug("sd "+defaultObj.getNameAndPath());		
		}
		else if (cmd.equals("show"))
		{
			ww.println(defaultObj.getObjInfoPathNameEtc());
		}
		else if (cmd.equals("rm"))
		{
			String n=cmdReader.readWord();
			D1Base objToDelete=null;
			if (n.equals(".."))
			{
				// Can not remove super object
			}
			else if ((n.equals("")) || (n.equals(".")))
			{
				objToDelete=defaultObj;
				defaultObj=defaultObj.getContainingObj();
				// Can not remove selected object	
			}
			else if (WordReader.isInt(n))
			{
				int i=Integer.parseInt(n);
				objToDelete=(D1Named)defaultObj.getObjFromIndex(i);
			}
			else
			{
				objToDelete=(D1Named)defaultObj.findGameObjNotRecursive(n);
			}

			if (objToDelete!=null)
			{
			
				D1SubRoot db=defaultObj.getDbSubRoot();
				db.lockWrite();
				try
				{
					objToDelete.unlinkSelf();
				}
				finally
				{
					db.unlockWrite();
				}
			}
			else
			{
				ww.println("did not find object to delete "+n);
			}
				
		}
		else if (cmd.equals("rls"))
		{
			//debug("list " + dbRoot.getName());
			if (cmdReader.isNextIntOrFloat())
			{
				final int recursionDepth=cmdReader.readInt();
				defaultObj.listNameAndPath(new PrintWriter(System.out), recursionDepth, "  ");
			}
			else
			{
				//String path=WordReader.getWord(line);
				//DbBase bo=dbRoot.getObjFromIndexPathWithinDbSubRoot(path);
				defaultObj.listNameAndPath(new PrintWriter(System.out), 0, "  ");
			}
		}
		else if (cmd.equals("mv"))
		{
			String n=cmdReader.readWord();
			
			D1Named from=(D1Named)defaultObj.getContainingObj();
   			
			if (from!=null)
			{
				if (WordReader.isInt(n))
				{
					int id=Integer.parseInt(n);
					D1SubRoot db=defaultObj.getDbSubRoot();
					if (db instanceof D1IdList)
					{
						D1IdList il=(D1IdList)db;
						D1Named to = il.getDbIdObj(id);
						
						db.lockWrite();
						try
						{
							//defaultObj.moveBetweenRooms(from, to);
							defaultObj.relinkSelf(to);
						}
						finally
						{
							db.unlockWrite();
						}					
					}
					else
					{
						ww.println("default object is not in an DbIdList");
					}
				}
				else
				{
					ww.println("argument did not look like a number");
				}
			}
			else
			{
				ww.println("can't move root object");
			}
		}
		else if (cmd.equals("find"))
		{
			String name=cmdReader.readWord();
							

			
			D1Base newDefaultObj;
			D1SubRoot r=defaultObj.getDbSubRoot();
			r.lockRead();
			try
			{
				newDefaultObj=defaultObj.findDbNamedRecursive(name);
			}
			finally
			{
				r.unlockRead();
			}



			if (newDefaultObj!=null)
			{
				defaultObj=newDefaultObj;
			}
			else
			{
				ww.println("did not find "+name);
			}
			
		}
		else if (cmd.equals("set"))
		{
			D1SubRoot db=defaultObj.getDbSubRoot();
			db.lockWrite();
			try
			{
				final String tag=cmdReader.readWord();

				final int result=defaultObj.setInfo(cmdReader, tag);
				if (result==0)
				{
					ww.println("did not find "+tag+ " or it is a read only property");											
				}
				else
				{
					defaultObj.setUpdateCounter();
				}
			}
			finally
			{
				db.unlockWrite();
			}
		}
		
		else if (cmd.equals("quit"))
		{
			debug("close tcp ip server");
			webServer.close();
			webServer=null;
			
			debug("stop tick");
			tickThread.setDone();
			tickThread=null;
			
			// Save all databases
			// Or not, we don't want to save all, only those changed.
			//debug("save all databases");
			//dbRoot.save();
			//dbRoot.clear();
			
			wr.close();
			wr=null;
			
			ww.println("exit");
			System.exit(0);
		}
		else if (cmd.equals("ta"))
		{
			tickThread.timeAcceleration=cmdReader.readInt();
			ww.println("timeAcceleration set to: "+tickThread.timeAcceleration);
		}
		else
		{
			debug("Main command parser: Unknown command");
			ww.println("Unknown command " + cmd + " " + cmdReader.readLine() + ", try: help");
		}
		ww.decIndentation();
	}
	
	   
	public void go()
	{
	
		globalConfig.logConfig(System.out);

   	
		
		debug("load existing worlds");
		dbRoot = loadExistingWorldsAndPlayers();

		
		// Create the login lobby (if not loaded from disk)
        // This is not needed in flame version
		if (globalConfig.allowExternal)
		{
			LoginLobbyServer.createAndStoreLobby(dbRoot);
		}
		
		
		debug("Create and start tick, the thread that updates the world");
		tickThread = new TickThread(globalConfig, dbRoot);
		Thread ttt = new Thread(tickThread);
	 	ttt.start();

	 	
        // This is not needed in flame version
	 	LoginLobbyConnection loginServerConnection=null;
	 	if (globalConfig.loginServerHostname!=null)
	 	{
	 		// Connect to login server
	 		loginServerConnection=new LoginLobbyConnection(globalConfig);
			Thread loginServerConnectionThread = new Thread(loginServerConnection);
			loginServerConnectionThread.start(); 		
	 	}
	 	
	 	FileServer fileServer=new FileServer(globalConfig, dbRoot);
	 	WebSocketServer webSocketServer= new SocketServer(globalConfig, dbRoot, loginServerConnection);         // loginServerConnection is not needed in flame version


	 	
		debug("Create and start the thread that accepts new connections");
		webServer = new WebServer(globalConfig.port, globalConfig.httpRootDir, webSocketServer, fileServer);   	
		Thread stt = new Thread(webServer);
		stt.start();
		
		if (globalConfig.startWebBrowser==true)
		{
			debug("start web browser");
			if(Desktop.isDesktopSupported())
			{
			  try 
			  {
				Desktop.getDesktop().browse(new URI("http://localhost:"+globalConfig.port));
			  } catch (IOException e) {
					error("IOException "+e);
					e.printStackTrace();
			  } catch (URISyntaxException e) {
					error("IOException "+e);
					e.printStackTrace();
			  }
			}
			else
			{
				debug("desktop is not supported");
			}
		}
		
		//debug("check input from standard input");
		defaultObj=dbRoot;
		wr=new WordReaderInputStream(System.in);
		while (wr.isOpenAndNotEnd())
		{
			//debug("default object: "+defaultObj.getNameAndPath("/")+getIdIfAny(defaultObj));
			String cmdLine=wr.readLine();
			WordReader cmdReader=new WordReader(cmdLine);
			
			try {
				//PrintWriter pw = new PrintWriter (System.out);			
				//WordWriter ww=new WordWriterPrintWriter(pw);
				WordWriter ww = new WordWriter();
				commandParser(cmdReader, ww);
				String m=ww.getString();
				System.out.println(m);
				//ww.flush();
				//pw.flush();
			} catch (IOException e) {
				error("IOException "+e);
				e.printStackTrace();
			}
			
		}
		debug("closed console");

	}
	


	
	
	
	// Saving sub roots is done in DbSubRoot.saveSelf. Then naming here and there need to match.	
	public D1TickList loadExistingWorldsAndPlayers()
	{
		D1TickList root = new D1TickList("wap", globalConfig); 
		
		root.lockWrite();
		try {

			try {
				// Will examine all files in current directory (alias folder)
				File folder = new File(globalConfig.savesRootDir+"/wap");
				System.out.println("loading worlds from " + folder.getAbsolutePath());
				if (folder.exists())
				{			
					root.loadFolder(folder);
				}
				else
				{
					System.out.println("Folder \".\" not found at "+folder.getAbsolutePath());
				}			
				
			} catch (IOException e) {
					e.printStackTrace();
			}
			finally
			{
			}
		}
		finally
		{
			root.unlockWrite();
		}
		return root;
	}

	
	public static void help()
	{
		System.out.println("Usage: ./server.jar <options> [options]");
		System.out.println("");
		System.out.println("Where [options] are:");
		System.out.println("-p <port>                    : tcpip port number"); // Specify the IP port for the server. Clients shall connect to this port. Typically -p 8080.
		System.out.println("-d <path>                    : javascript directory");
		System.out.println("-s <path>                    : game save directory");
		System.out.println("-l <path>                    : script/lua directory");
		System.out.println("-r <path>                    : use a general root directory for the 3 above"); // Often the folders web, lua and saved_games are located in a common folder, so instead of using -d -s- l this can be used. 
		System.out.println("-b                           : if set launch a web browser"); // Useful when testing, a pure servers should not use this.
		System.out.println("-g <host> <port> <user> <pw> : game server only (no user data)"); // This server shall connect to the main server, players will be sent over from there.
		System.out.println("-e                           : allow external servers"); // Set this switch if this is the main login server. External servers use -g to connect to this server.
		System.out.println("-n <pw>                      : set a password for creating new games"); // If this is a login server this needs to be set to some good password.
		System.out.println("");
		System.out.println("Examples:");
		System.out.println("-p 8080 -r ../../");
		System.out.println("./server.jar -p 8080 -d /home/henrik/git/RoboticsSandBox/web -s /home/henrik/saved_games -w");  	
	}
	
	
	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		System.out.println("Welcome to Drekkar games");

		
		debug("java.library.path" + " " + System.getProperty("java.library.path"));
		
		// If the call below does not work.
		// cd java/RSB_Common/src
		// sudo apt-get install  default-jdk
		// make.
		//se.eit.DeJniPkg.DeJNI.test(args);
		
		/*
		{
			System.out.println(""+Long.toString(-2, 16));
			System.out.println(""+Long.toString(255, 16));
			
			final long i=Long.parseLong("fe000000fc000000", 16);
			System.out.println(""+i);
		}
		*/
		
		/*
		BigBitMap bbm=new BigBitMap("10 8 9 80");
		for(int i=0;i<256;i++)
		{
			if (bbm.getBit(i)>0)
			{
				System.out.println(""+i);
			}
		}
		*/
		
		RsbFactory.registerClass(FlameRoom.class);
		
		Main m = new Main();

		
		//WebHttpServer w = new WebHttpServer();

		if (args.length<1)
		{
			help();
		}
		else
		{
			for(int i=0; i<args.length; i++)
			{
				if (args[i].equals("-p"))
				{
					i++;
					m.globalConfig.port=Integer.parseInt(args[i]);
					debug("using port "+ m.globalConfig.port);
				}
				else if (args[i].equals("-d"))
				{
					i++;
					m.globalConfig.httpRootDir=args[i];
					debug("using httpRootDir "+ m.globalConfig.httpRootDir);
				}
				else if (args[i].equals("-s"))
				{
					i++;
					m.globalConfig.savesRootDir=args[i];
					debug("using savesRootDir "+ m.globalConfig.savesRootDir);
				}
				else if (args[i].equals("-w"))
				{
					System.out.println("Option -w is replaced by -b");
					return;				
				}
				else if (args[i].equals("-b"))
				{
					m.globalConfig.startWebBrowser=true;
					debug("start web browser");
				}
				else if (args[i].equals("-g"))
				{
					i++;
					m.globalConfig.loginServerHostname=args[i];
					i++;
					m.globalConfig.loginServerPort=Integer.parseInt(args[i]);
					i++;
					m.globalConfig.loginServerUsername=args[i];
					i++;
					m.globalConfig.loginServerUserPw=args[i];
					debug("login server "+m.globalConfig.loginServerHostname+" "+m.globalConfig.loginServerPort);
				}
				else if (args[i].equals("-n"))
				{
					i++;					
					m.globalConfig.startGamePw=args[i];
					debug("password is required to start a new game");					
				}
				else if (args[i].equals("-l"))
				{
					i++;
					m.globalConfig.luaScriptDir=args[i];
					debug("using luaScriptDir "+ m.globalConfig.luaScriptDir);
				}
				else if (args[i].equals("-r"))
				{
					i++;
					String path=args[i];
					m.globalConfig.luaScriptDir=path+"/lua";
					m.globalConfig.httpRootDir=path+"/web";
					m.globalConfig.savesRootDir=path+"/saved_games";
					debug("using luaScriptDir "+ m.globalConfig.luaScriptDir);
					debug("using httpRootDir "+ m.globalConfig.httpRootDir);
					debug("using savesRootDir "+ m.globalConfig.savesRootDir);
				}
				else if (args[i].equals("-e"))
				{
					m.globalConfig.allowExternal=true;
					debug("start web browser");
				}
				else
				{
					help();
					//System.exit(1);
					return;
				}
				
			}

		 	// Check that the http root directory exist
		 	File theDir = new File(m.globalConfig.httpRootDir);
		 	if (!theDir.exists()) {
		 		System.out.println("\nThe http root directory was not found at: '"+m.globalConfig.httpRootDir+"', expanded to: '"+theDir.getAbsolutePath() + "'\n");
		 		System.out.println("Use switch -? for usage help");
				//System.exit(1);		 		  
				return;
		 	}
			
			
			// Start a web html server
			//Thread thread = new Thread(w);
			//thread.start();
			
			
			m.go();
		}
	}

}
