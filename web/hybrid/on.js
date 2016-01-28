/*
on.js

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

Licence: You may use, copy, modify and redistribute this code as long as you keep the History below and add state there if you did any changes.

History:
Created 2014-03-14 by Henrik Bjorkman www.eit.se/hb

*/


//var pingTimer=null;
var websocket;

function onTestWebSocket(wsUrl)
{
	// open the web socket
	websocket = new WebSocket(wsUrl);
	
	// This registers callbacks for web socket events
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };
}


// this is called by system when connection is up
function onOpen(evt)
{
	console.log("CONNECTED: "+ Date());

	a_result = Module.ccall('mainHandleStringEvent', 'string', ['number', 'string'], [1, evt.data]); // 1 = StringEventOnOpen

    // Tell sever we wish to use web game protocol.
	//doSend("rsb_web_game");
	//websocket.send("rsb_web_game");
}

// this is called by system if there is a problem with the connection
function onError(evt)
{
	console.log("onError: " + evt.data);
	a_result = Module.ccall('mainHandleStringEvent', 'string', ['number', 'string'], [2, evt.data]); // 2 = StringEventOnError
}

// This is perhaps the most interesting function in this file.
// It receives commands from the server program.
function onMessage(evt)
{
	//var str=evt.data;
	a_result = Module.ccall('mainHandleStringEvent', 'string', ['number', 'string'], [3, evt.data]); // 3 = StringEventOnMessage
}

// this is called by system when connection is closed
function onClose(evt)
{
	console.log("DISCONNECTED: "+ Date());
	a_result = Module.ccall('mainHandleStringEvent', 'string', ['number', 'string'], [4, evt.data]); // 4 = StringEventOnClose
	//$("body").empty();
	//$("body").append("DISCONNECTED: "+ Date());
}





