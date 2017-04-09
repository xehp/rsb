// hmeg_win.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-19 by Henrik Bjorkman www.eit.se/hb



HmegWin.prototype = Object.create(DivBase.prototype);
HmegWin.prototype.constructor = HmegWin;







function HmegWin(headingText)
{
	DivBase.call(this, null); // call super constructor

	this.hmegImgList = new HmegImgList();

	this.hmegDb=new HmegDb(this);

	this.sectorWidth=32;
	this.sectorHeight=32;
	
	this.mouseDownPos;

	this.headingText=headingText;

	this.subWin = null;
	this.msgWin = null;
	
	this.avatarId = -1;
	//this.avatar = null; // This is for CPU optimize only

	this.avatarPlayerReferencesId = -1;
	//this.avatarPlayerReferences = null; // This is for CPU optimize only

	this.wantedRotationSpeed = 0; 
	this.wantedForwardSpeed = 0;

	this.wantedRotation = 0; 
	this.wantedX = 0;
	this.wantedY = 0;
	
	this.changedPositionCounter=-3;		
	
	this.userInputCounter = 0;
	
	this.updatedTimeMs = 0;
	
	rootDiv = this;
}	
	


HmegWin.prototype.init=function()
{
	this.subWin = new HmegMapWin(this);
	this.msgWin = new HmegWinMsg(this);
}	

// This creates (make HTML layout) for the main window page of this game
// The main window contains 3 sub windows (AKA DIVs)
// * header on the to of the window
// * middle, different things can be shown here such as the world map.
// * messages text area in the lower part
HmegWin.prototype.defineDiv=function()
{
	console.log("defineDiv: w="+window.innerWidth+", h="+window.innerHeight);

	var msgAreaHeight=120;
	var scrollbarSize=32;

	
	if (msgAreaHeight*4>window.innerHeight)
	{
		msgAreaHeight=Math.floor(window.innerHeight/4);
	}


	var subWinSize={x: window.innerWidth-scrollbarSize, y: window.innerHeight-msgAreaHeight-scrollbarSize};
	var msgWinSize={x: window.innerWidth-scrollbarSize, y: msgAreaHeight-scrollbarSize};


	var newPage='';


	document.title = this.headingText + ' - ' + clientVersion;

	

	console.log('defineDiv');

	


	newPage+='<section>';

	newPage+=this.subWin.defineDiv(subWinSize);

	newPage+=this.msgWin.defineDiv(msgWinSize);
	
	newPage+='</section>';


	$("body").empty();
	$("body").append(newPage);


		
		
	// Remember our main window	
	//rootDiv.element = document.getElementById('myCanvas');

}


HmegWin.prototype.addEventListenersDiv=function()
{
	this.subWin.addEventListenersDiv();
	this.msgWin.addEventListenersDiv();

	// If game need direct input from keyboard this is a good place to setup the callback.
	// This game need direct input from keyboard
	var t=this;
	
	document.onkeypress=function(event)
	{
		t.subWin.onkeypress(event);
		t.userInputCounter++;
	}
	
	document.onkeydown=function(event)
	{
		var i=parseInt(event.which);
		//console.log('onkeydown: ' + event.which+' '+i);
    	t.moveAvatar();
		switch(i)
		{
			case 65:
				t.wantedRotationSpeed=-0.05;  // unit here is degrees per ms.				
				break;
			case 68:
				t.wantedRotationSpeed=0.05;				
				break;
			case 83:
				//t.wantedForwardSpeed=-0.001;  // unit here is blocks per ms.
				var apr=t.getAvatarPlayerReferences();
				t.wantedForwardSpeed=-apr.BACKWARD_SPEED;
				break;
			case 87:
				//t.wantedForwardSpeed=0.002
				var apr=t.getAvatarPlayerReferences();
				t.wantedForwardSpeed=apr.FORWARD_SPEED;			
				break;
			default:
			    var ew=event.which;
			    console.log('keydown "'+ew+'" ~'+t.avatarId);
		        websocket.send('keydown "'+ew+'"');
		        break;
		}
		t.userInputCounter++;		
	}
	document.onkeyup=function(event)
	{
		t.userInputCounter++;

		var i=parseInt(event.which);
		//console.log('onkeyup: ' + event.which+' '+i);
    	t.moveAvatar();
		switch(i)
		{
			case 65:
				t.wantedRotationSpeed=0.0;
				break;
			case 68:
				t.wantedRotationSpeed=0.0;				
				break;
			case 83:
				t.wantedForwardSpeed=0.0;
				break;
			case 87:
				t.wantedForwardSpeed=0.0;				
				break;
			default:
		        websocket.send('keyup "'+event.which+'"');
		        break;
		}
		t.userInputCounter++;		
	}
	
	
    setInterval(
        function()
        {
        	//t.drawDiv();
        	
		    t.moveAvatar();
			t.subWin.drawDiv();
			//t.msgWin.drawDiv();
			
        }
    ,20);

	
    setInterval(
        function()
        {
        	//t.drawDiv();
        	
		    //t.moveAvatar();
			//t.subWin.drawDiv();
			t.msgWin.drawDiv();
			
        }
    ,500);
}


HmegWin.prototype.drawDiv=function()
{
	if (this.getAvatar() != null)
	{
    	this.moveAvatar();
		this.subWin.drawDiv();
		this.msgWin.drawDiv();
	}
	else
	{
		console.log("no avatar yet");
	}
}


HmegWin.prototype.defineAndDrawPage=function()
{
	this.defineDiv();
	this.addEventListenersDiv();
	this.drawDiv();
}

/*
HmegWin.prototype.onInputText=function(srcId)
{
	var theSrcTextBox = document.getElementById(srcId);
	var str=theSrcTextBox.value;
	doSend('consoleInput "'+str+'"');
	theSrcTextBox.value="";
}
*/
	
// This handles all input from server when this game is running in server.	
HmegWin.prototype.onMessageArg=function(arg)
{
	var i=0;
	var cmd=arg[0];
	var reply='';


	// Here we check what server want client program to do.
	// Commands sent by ConnectionThread need to be interpreted here.


	if (this.hmegDb.onMirrorMessage(arg)!=0)
	{
		// Message was handled by mirror
	}
	else if (cmd=="mirrorUpdated")
	{
		// The server sends this when it has finished sending a batch of updates.
		// When this is received it it time to redraw the map.			
		/*if (this.hmegDb!=null)
		{
			this.drawDiv();
		}*/
		doSend('mirrorAck ' + arg[1]);

		doSend('avatarPos ' + this.wantedX + ' ' + this.wantedY + ' ' + this.wantedRotation + ' ' +	this.changedPositionCounter);		
	}
	else if (cmd=="return")
	{
		console.log("onMessageArg: '" +arg+"'"); 
		empireClose();
	}
	else if (cmd=="consoleMessage")
	{
		//console.log("onMessageArg: consoleMessage: '" +arg+"'"); 
		var msg=hlibRemoveQuotes(arg[1]);
		
		if (this.subWin instanceof HmegTerminal)
		{
			this.subWin.addText(msg);
		}
		else
		{
			this.msgWin.addText(msg);
		}
	}
	else if (cmd=="showWorld")
	{
		console.log("onMessageArg: showWorld: '" +arg+"'"); 
		// When this command is received client will define the main game window (and its initial sub windows AKA div)			
		if (this.hmegDb!=null)
		{
			this.defineAndDrawPage();
		}
	}
	/*else if (cmd=="hmegShow") // Deprecated, use showWorld
	{			
		console.log("onMessageArg: hmegShow: '" +arg+"'"); 
		if (this.hmegDb!=null)
		{
			//this.hmegDb.debugDump(" ");
			
			this.defineAndDrawPage();
		}
	}*/
	/*else if (cmd=="avatarId")
	{
		console.log("onMessageArg: avatarId: '" +arg+"'"); 
		// This tells the client which object it shall view the world from.
		this.setAvatarId(parseInt(arg[1]));
		console.log("avatarId: ~"+this.avatarId);
		console.log("avatarPlayerReferencesId: ~"+this.avatarPlayerReferencesId);
	}*/
	else if (cmd=="setAprId")
	{
		console.log("onMessageArg: setAprId: '" +arg+"'"); 
		// This tells the client which object it shall view the world from.
		this.setAvatarPlayerReferencesId(parseInt(arg[1]));
		console.log("avatarPlayerReferencesId: ~"+this.avatarPlayerReferencesId);
		console.log("avatarId: ~"+this.avatarId);
	}
	else
	{
		console.log("onMessageArg: unknown command: '" + cmd+"'"); 
		console.log("onMessageArg: '" +arg+"'"); 
		//empireClose();
	}
}

HmegWin.prototype.getAvatar=function()
{
	var avatar=this.hmegDb.getById(this.avatarId);
	

	// This is just for debugging
	if (avatar==null)
	{
		console.log("no avatar ~"+this.avatarId);
	}
	else
	{
		if (this.avatarPlayerReferencesId != avatar.avatarPlayerReferencesId)
		{
			throw new Error("this.avatarPlayerReferencesId != avatar.avatarPlayerReferencesId, ~"+ this.avatarPlayerReferencesId +", ~"+avatar.avatarPlayerReferencesId+ ", ~"+this.avatarId);				
		}
	}
	
	
	return avatar;
}


HmegWin.prototype.setAvatarId=function(id)
{
    if (this.avatarId!=id)
    {
		console.log("setAvatarId, ~"+this.avatarId +", ~"+id);
	    this.avatarId=id;
	}
	

	// This is just for debugging
	var avatar = this.getAvatar();
	if (avatar==null)
	{
		throw new Error("Preferably server should send avatar data before sending avatarId, ~"+id);
	}
	else
	{
		if (this.avatarPlayerReferencesId != avatar.avatarPlayerReferencesId)
		{
			throw new Error("this.avatarPlayerReferencesId != avatar.avatarPlayerReferencesId, ~"+ this.avatarPlayerReferencesId +", ~"+avatar.avatarPlayerReferencesId+ ", ~"+this.avatarId);				
		}
	}
	
}

HmegWin.prototype.getAvatarPlayerReferences=function()  // AvatarPlayerReferences alias FlameUser
{
	return this.hmegDb.getById(this.avatarPlayerReferencesId);
}

HmegWin.prototype.setAvatarPlayerReferencesId=function(id) 
{
    if (this.avatarPlayerReferencesId!=id)
    {
		console.log("setAvatarPlayerReferencesId, ~"+this.avatarPlayerReferencesId +", ~"+id);

		this.avatarPlayerReferencesId=id;
	}
	
	var avatarPlayerReferences=this.getAvatarPlayerReferences();
  
	if (avatarPlayerReferences==null)
	{
    	throw new Error("Did not find avatarPlayerReferences ~"+this.avatarPlayerReferencesId);
	} 
	else
	{
		this.setAvatarId(avatarPlayerReferences.avatarId);	
	
    	/*if (this.avatarId != avatarPlayerReferences.avatarId)
		{
			console.log("this.avatarId != avatarPlayerReferences.avatarId, ~"+this.avatarId +", ~"+ avatarPlayerReferences.avatarId+ ", ~"+id);
			if (typeof avatarPlayerReferences.avatarId !== "undefined")
			{
				this.setAvatarId(avatarPlayerReferences.avatarId);
			}
			else
			{
				throw new Error("avatarPlayerReferences.avatarId is undefined for apr ~"+id);				
			}
		}*/
	}
}


HmegWin.prototype.moveAvatar=function()
{
	var avatar = this.getAvatar();

	if (avatar != null)
	{
		var t_ms = (new Date()).getTime();

		if (avatar.serverChangedPositionCounter!=this.changedPositionCounter)
		{
	       	console.log("teleport avatar ~"+avatar.id+" "+avatar.serverChangedPositionCounter+" "+this.changedPositionCounter);
	
			// Server has moved the avatar to a new place (or adjusted the position).
			// Client must use the new position and move it from there.
	
			this.wantedRotation = avatar.rotation; 
			this.wantedX = avatar.x;
			this.wantedY = avatar.y;
			
			this.changedPositionCounter = avatar.serverChangedPositionCounter;
		}
		else
		{
			// How much time has passed since last move.
			
			var dt = t_ms - this.updatedTimeMs;

	       	//console.log("tick "+t_ms+" "+dt+"  "+this.wantedRotationSpeed+" "+this.wantedForwardSpeed+"  "+this.wantedRotation+" "+this.wantedX+" "+this.wantedY);
			
			this.wantedRotation += this.wantedRotationSpeed * dt;
			
			var r = Math.PI*this.wantedRotation/180.0;
					
			var dx = this.wantedForwardSpeed * Math.cos(r) * dt;
			var dy = this.wantedForwardSpeed * Math.sin(r) * dt;	
			
			this.wantedX += dx;
			this.wantedY += dy;
			
		}	
	
		this.updatedTimeMs = t_ms;
		avatar.timeOfLastUpdateMs = t_ms;
	}
	else
	{
		console.log("no avatar to move");
	}
}


HmegWin.prototype.backToPreviousSubWin=function()
{
	// We could keep a stack but in this game it is never deeper than 1 anyway.
	this.subWin = new HmegMapWin(this);

	// Redefine the window (top div and its sub divs.)
	this.defineAndDrawPage();
	
	// And draw it
	this.drawDiv();

}

HmegWin.prototype.onInputText=function(srcId)
{
	var theSrcTextBox = document.getElementById(srcId);
	var str=theSrcTextBox.value;
	doSend('consoleInput "'+str+'"');
	theSrcTextBox.value="";
}


HmegWin.prototype.mapCanvasCancel=function() 
{
	this.backToPreviousSubWin();
	this.mapMemoryClear();
	this.mapClearSelections();

	$("p").empty();
	window.clearInterval(this.mapIntervalVariable)
        document.title = this.mapHeadingText;
	doSend('cancel');
	empireClose();
}


