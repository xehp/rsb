// RoboGameMain.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-03-11 by Henrik Bjorkman www.eit.se/hb




function RoboGameOpen(headingText)
{
	document.title = clientVersion;

	rootDiv=new RoboGameWin(headingText);
	rootDiv.init();

	// Register our own handler for data from server, need to be restored when we exit this game
	websocket.onmessage = function(evt) { RoboGameOnMessage(evt) };
}


function RoboGameClose()
{
	websocket.onmessage = function(evt) { onMessage(evt) };
}


function RoboGameOnMessage(evt)
{
	var str=evt.data;

	if ((str==='undefined') || (str==null) || (str.length<=0))
	{
		console.log("RoboGameOnMessage("+rootDiv.inputState+"): undefined, null or zero length"); 	
	}
	else
	{
		var arg=hlibSplitString(str);
		rootDiv.onMessageArg(arg);
	}
}






