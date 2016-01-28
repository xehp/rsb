// Empire.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb





function empireOpen(headingText)
{
	document.title = clientVersion;

	rootDiv=new EmpWin();
	rootDiv.init();

	// Register our own handler for data from server, need to be restored if exiting empire
	websocket.onmessage = function(evt) { empireOnMessage(evt) };

	rootDiv.mapInitData();
	rootDiv.mapHeadingText=headingText;
	//rootDiv.mapOpenCreateCanvas(headingText);
	rootDiv.mapSetShowState(0);
	rootDiv.mapClearSelections();

}


function empireClose()
{
	websocket.onmessage = function(evt) { onMessage(evt) };
}


function empireOnMessage(evt)
{
	var str=evt.data;

	if ((str==='undefined') || (str==null) || (str.length<=0))
	{
		console.log("empireOnMessage("+rootDiv.inputState+"): undefined, null or zero length"); 	
	}
	else
	{
		//console.log("empireOnMessage("+rootDiv.inputState+"): " + str); 	

		var arg=hlibSplitString(str);

		rootDiv.onMessageArg(arg);
	}
}





