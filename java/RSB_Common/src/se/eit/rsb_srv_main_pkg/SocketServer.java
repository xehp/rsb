// SocketServer.java
//
// Copyright (C) 2015 Henrik Björkman www.eit.se/hb
//
// History:
// Created by Henrik 2015 

package se.eit.rsb_srv_main_pkg;

import se.eit.LoginLobbyPkg.LoginLobbyConnection;
import se.eit.d1_pkg.D1GlobalConfig;
import se.eit.d1_pkg.D1SubRoot;
import se.eit.rsb_srv_main_pkg.PlayerConnectionThread;
import se.eit.web_package.WebConnection;
import se.eit.web_package.WebSocketConnection;
import se.eit.web_package.WebSocketServer;

public class SocketServer implements WebSocketServer {

    private D1GlobalConfig config;
	private D1SubRoot dbRoot;
	private LoginLobbyConnection loginServerConnection;


	public SocketServer(D1GlobalConfig config, D1SubRoot dataBase, LoginLobbyConnection loginServerConnection) 
    {
    	this.config=config;
    	this.dbRoot=dataBase;
    	this.loginServerConnection=loginServerConnection;
    }
	
	
	@Override
	public WebSocketConnection newSocketServer(WebConnection webConnection) {
		return new PlayerConnectionThread(config, webConnection, dbRoot, loginServerConnection);
	}

}
