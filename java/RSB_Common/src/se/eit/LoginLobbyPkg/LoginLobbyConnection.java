// LoginServerConnection.java
//
// Copyright (C) 2015 Henrik Bj��rkman www.eit.se/hb
//
// History:
// Created by Henrik 2015 

package se.eit.LoginLobbyPkg;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;

import se.eit.d1_pkg.D1List;
import se.eit.d1_pkg.D1GlobalConfig;
import se.eit.rsb_package.UserAccount;
import se.eit.rsb_package.Version;
import se.eit.web_package.MyBlockingQueue;
import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

public class LoginLobbyConnection implements Runnable {

	D1GlobalConfig globalConfig;
	
	private Socket socket = null;
	private PrintStream out = null;
	private BufferedReader in = null;

	//private int avatarId=-1;
	
	//public DbList<NotificationData> notificationDataList=new DbList<NotificationData>();
	
	// queueList can be used from other threads. So synchronized must be used when handling common data.
	// This is a list with queues. It didn't need to be queues, just a string would have been enough.
	public D1List<MyBlockingQueue<String>> queueList=new D1List<MyBlockingQueue<String>>();
	
	//private ReentrantReadWriteLock rwl=null;

	
	// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
	public static String className()
	{	
		return LoginLobbyWorld.class.getSimpleName();	
	}
	
	
	public String getType()
	{	
		return this.getClass().getSimpleName(); // Note! This gives the name of the extended class, that is this does not always give this class.
	}
	
	
	// Just for debugging.
	void debug(String s)
	{
	    WordWriter.safeDebug(getType() + ": " + s);   // Do we want getType or className here?
	}
	
	void error(String s)
	{
		System.out.flush();
	    System.err.println(getType() + ": " + s);
		Thread.dumpStack();
	    System.exit(1);
	}

	
	public LoginLobbyConnection(D1GlobalConfig globalConfig)
	{
		this.globalConfig=globalConfig;
		//rwl=new ReentrantReadWriteLock();
	}
	
	// Returns true if there was a problem
	protected boolean queryPrompt(WordReader wr)
	{
		/*String queryType=*/wr.readWord();
		String ref=wr.readWord();
		
		if (ref.equals("login_or_reg"))
		{
			out.println("Login");
		}
		else if (ref.equals("enter_player_name"))
		{
			if (globalConfig.loginServerUsername!=null)
			{
				out.println("\""+globalConfig.loginServerUsername+"\"");
			}
			else
			{
				out.println("\"rsbgameserver\"");
			}
		}
		else if (ref.equals("player_not_found"))
		{
			return true;		
		}
		else if (ref.equals("enter_player_pw"))
		{
			if (globalConfig.loginServerUserPw!=null)
			{
				out.println("\""+globalConfig.loginServerUserPw+"\"");
			}
			else
			{
				out.println("\"ohnowhatisthis\""); // TODO same as user name above
			}
		}
		else if (ref.equals("login_ok"))
		{
			System.out.println("login OK");
			out.println("OK");
		}
		else if (ref.equals("2d_or_3d_support")) // This is deprecated
		{
			out.println("2d");
		}
		else if (ref.equals("game_support"))
		{
			// Tell server which service we want.
			out.println("\"lobby\""); // Use same name as used in gamesSupportedByServer and serverFactory
		}
		else if (ref.equals("join_or_create"))
		{
			out.println("\"Continue game\"");
		}
		else if (ref.equals("list_enter_game_type_name"))
		{
			out.println("\"mainLobby\""); // TODO It should be possible to choose lobby from command line options
		}
		else if (ref.equals("no_worlds_available"))
		{
			//out.println("\"OK\"");
			return true;
		}
		else if (ref.equals("list_enter_world_name"))
		{
			//out.println("LoginServerWorld");
			out.println("yukigassenLobby");  // lobby with name after the name of the game
		}
		else if (ref.equals("list_can_not_find"))
		{
			return true;
		}
		else if (ref.equals("joining_world"))
		{
			out.println("OK");
		}
		else if (ref.equals("openLoginWorld"))
		{
			// Ignore
		}
		else if (ref.equals("selectType"))
		{
			out.println("server");
		}
		/*else if (ref.equals("serverUrl"))
		{
			if (globalConfig.myHostname!=null)
			{
				out.println("\"/"+globalConfig.myHostname+":"+globalConfig.myPort+"\"");
			}
			else
			{
				out.println("Cancel");
			}
		}*/
		/*else if (ref.equals("serverIp"))
		{
			if (globalConfig.myHostname!=null)
			{
				out.println("\""+globalConfig.myHostname+"\"");
			}
			else
			{
				out.println("Cancel");
			}
		}*/
		else if (ref.equals("serverPort"))
		{
			if (globalConfig.port!=-1)
			{
				out.println("\""+globalConfig.port+"\"");
			}
			else
			{
				out.println("Cancel");
			}
		}
		else if (ref.equals("lobbyJoinedSuccessfully"))
		{
			String msg=wr.readString();
			System.out.println("Successfully joined lobby '"+msg+"'");
			out.println("OK");
		}
		else if (ref.equals("alreadyPlaying"))
		{
			String msg=wr.readString();
			System.out.println("Could not join lobby '"+msg+"'");
			out.println("Cancel");
			return true;
		}
		else
		{
			debug("unknown query");
		}
			
		return false;
	}

	protected void isClientCodeOkReply(WordReader wr)
	{
		// Compare with sending isClientCodeOkReply in LoginLobbyServer.joinLobby
		final int questionIndex = wr.readInt();				

		final String reply = wr.readString();				

		/*NotificationData notificationData = notificationDataList.get(questionIndex); 
		
		notificationData.doNotify(0);
		
		notificationDataList.remove(questionIndex);*/										

		debug("isClientCodeOkReply: questionIndex="+questionIndex+", reply="+reply+"'");
		
		MyBlockingQueue<String> mbq = null;
		
		synchronized(this)
		{
			// queueList can be used from other threads. So synchronized must be used when handling common data.
			mbq = queueList.get(questionIndex);
		}

		if (mbq!=null)
		{
			// This is the reply from main server. Put it in the queue for the task that needed it.
			mbq.put(reply);
		}
		else
		{
			debug("did not find mbq, questionIndex="+questionIndex);
		}
		
	}

	// Note mirrorUpdate and mirrorUpdated are different things. This command shall be renamed to something better.
	public void mirrorUpdated(WordReader wr)
	{
		final int mirrorServersSeqnr=wr.readInt();
		/*final int mirrorAckedClientsSeqnr=*/wr.readInt();
		out.println("mirrorAck"+" "+mirrorServersSeqnr);
	}

	// Returns true when it is time to close the connection
	public boolean commandInterpreter(String line)
	{
		WordReader wr = new WordReader(line);

		String cmd=wr.readWord();
		
		char ch=cmd.charAt(0);
		
		debug("commandInterpreter: line: '"+line+"'");
		
		switch(ch)
		{
			case 'a': 
				if (cmd.equals("avatarId"))
				{
					//avatarId = wr.readInt();
					return false;
				}	
				break;
			case 'c':
				if (cmd.equals("close"))
				{
					System.out.println("server sent close");
					return true;
				}
				break;
			case 'i':
				if (cmd.equals("isClientCodeOkReply"))
				{
					// This is a reply to one of our "isClientCodeOk" questions sent to server
					isClientCodeOkReply(wr);
					return false;
				}
			case 'l': 
				if (cmd.equals("listClear"))
				{
					debug("ignored listClear");
					return false;
				}
				break;
			case 'q': 
				if (cmd.equals("qp")) // query prompt
				{
					return queryPrompt(wr);
				}
				break;
			case 'm': 
				if ((cmd.equals("ma")) || (cmd.equals("mirrorAdd")))
				{
					// Ignore
					return false;
				}		
				if ((cmd.equals("mu")) || (cmd.equals("mirrorUpdate")))
				{
					// Ignore
					return false;
				}		
				if ((cmd.equals("mr")) || (cmd.equals("mirrorRemove")))
				{
					// Ignore
					return false;
				}	
				if (cmd.equals("mirrorUpdated"))
				{
					mirrorUpdated(wr);
				}
				break;
			case 'p': 
				if (cmd.equals("playerPreference"))
				{
					// ignore these
					return false;
				}
				break;
				
				
			default: 
				break;
		}
		
		debug("unknown command: '"+line+"'");
		return false;
	}

	@Override
	public void run() {
		// TODO: Connect with the login server
		
		connect(globalConfig.loginServerHostname, globalConfig.loginServerPort);
		
		try {
			
			// Send an empty line to server
			// This will tell server that we are not a web socket client.
			out.println("rsb_web_game");
			
			// wait for first message from server		
			String line=in.readLine();
				
			// It should say server version.

			// ServerVersion and ClientVersion
	  		//String serverNameAndVersion=WordReader.replaceCharacters(Version.getProgramNameAndVersion()+"/server", ' ', '_');
	  		//String clientNameAndVersion=WordReader.replaceCharacters(Version.getProgramNameAndVersion()+"/client", ' ', '_');
	  		String serverNameAndVersion=Version.serverVersion();
	  		String clientNameAndVersion=Version.clientVersion();
	  		
			if (line==null)
			{
				debug("got null");
				throw new IOException("got null");
			}
			if (line.length()==0)
			{
				debug("got empty string");
				throw new IOException("got empty string");
			}
			else if (line.equals(serverNameAndVersion))
	    	{
				//cc.writeLine("xehpuk.com/rsb/client/"+ Version.getVersion());
				out.println(clientNameAndVersion);							
	    	}
			else
			{
				error("expected server version \"" + serverNameAndVersion +"\" but got \""+line+"\"");
				throw new IOException("wrong version, expected \"" + serverNameAndVersion +"\" but got \""+line+"\"");
			}

			
				
			for(;;) 
			{
				line=in.readLine();
				
				if (line==null)
				{
					debug("received null");
					break;
				}
				
				if (line.length()==0)
				{
					debug("empty string");
				}
				else
				{
					debug("line to interpret: "+line);
					
					boolean r = commandInterpreter(line);
					
					if (r)
					{
						break;
					}
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		// TODO If wee still want connection we should try to reconnect.
		
		close();
		
		System.out.println("Closed connection with lobby server "+globalConfig.loginServerHostname +" "+ globalConfig.loginServerPort);
	}


	// This can be called from other threads. So synchronized must be used when handling common data.
	// Returns null if not OK
	public UserAccount isClientCodeOk(/*NotificationData notificationData,*/ int playerIndex, int playerCode) 
	{	
		// Create a queue in which the answer can be put.
		MyBlockingQueue<String> mbq = new MyBlockingQueue<String>(8);
		
		int questionIndex=-1;
		
		synchronized(this)
		{
			questionIndex = queueList.add(mbq);
		}

		debug("Ask main server, playerIndex='"+playerIndex+"', questionIndex="+ questionIndex);
		
		//int questionIndex = notificationDataList.add(notificationData);
		
		// Send a question to main login server if the client code is correct.
		out.println("isClientCodeOk" +" "+ playerIndex + " "+playerCode+" "+questionIndex);
		
		
		//GameUser playerData=null;
		UserAccount userAccount=null;
		
		// Now we wait for a PlayerData, it will be added by another task.
		try {
			final String str=mbq.take(60000);
			
			debug("Reply from main server was '"+str+"'");
			if (str.length()>0)
			{
				WordReader wr=new WordReader(str);
				final String resultStr=wr.readToken("");
				//final String playerDataStr=wr.readString();
				//final String userAccountStr=wr.readString();
				if (resultStr.equals("ok"))
				{
					debug("Code was correct, resultStr='"+resultStr+"'");
					
					// It must be same message format, type of objects here as sent in isClientCodeOkReply by LoginLobbyServer
					
					//WordReader pdwr=new WordReader(playerDataStr);
					
					//playerData=new GameUser(); 

					//playerData.readSelf(wr);
					//debug("playerData, name=" +playerData.getName());

					
					//WordReader uawr=new WordReader(userAccountStr);

					userAccount = new UserAccount();
					userAccount.lockWrite();
					try
					{
						userAccount.readSelf(wr);						
					}
					finally
					{
						userAccount.unlockWrite();					
					}
					debug("playerData, name=" +userAccount.getName());
				}
				else if (resultStr.equals("nok"))
				{
					debug("Code was not correct");
				}
				else
				{
					debug("Unknown reply from login server: "+resultStr);					
				}
			}
			
		} catch (InterruptedException e) {
			debug("Timeout, no reply from login server");
			e.printStackTrace();
		}
		
		synchronized(this)
		{
			// The queue is no longer needed, we go the answer 
			queueList.remove(questionIndex);
		}	
		
		return userAccount;
	}
	
	
	public void connect(String hostname, int port)
	{
	    if ((hostname!=null) && (port>0) && (socket==null))
	    {
	       debug("trying to connect to "+hostname+":"+port);
	       try 
	       {
	         // Create a socket to communicate to the specified host and port
	         socket = new Socket(hostname, port);
	    
	         // Create streams for reading and writing lines of text
	         // from and to this socket.
	         in = new BufferedReader(new InputStreamReader(socket.getInputStream()));         
	         out = new PrintStream(socket.getOutputStream());
	
	         System.out.println("Connected to " + socket.getInetAddress() + ":"+ socket.getPort());        
	       }      
	       catch (IOException e) 
	       {
	         error("connect failed "+e);
	       }
	       finally 
	       {
	         // Always be sure to close the socket if any
	         //try { if (socket != null) socket.close(); } catch (IOException e2) { ; }
	       }
	    }
	    else
	    {
	    	error("could not connect");
	    }
	}

	public synchronized void close()
	{
		try 
		{
			if (in!=null)
			{
				in.close();
				in=null;
			}
			if (out!=null)
			{
				debug("closing connection to main server");
				out.close();
				out=null;
			}
			if (socket!=null)
			{
				socket.close();
				socket=null;
			}
		}
		catch( IOException e ) 
		{
			error("reconnect " + e );
		}
	}
	
	
	public void finalize()
	{
		debug("finalize");
	    close();
	}
	
}
