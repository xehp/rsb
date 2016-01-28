// RoboGameWin.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-19 by Henrik Bjorkman www.eit.se/hb



RoboGameWin.prototype = Object.create(DivBase.prototype);
RoboGameWin.prototype.constructor = RoboGameWin;









function RoboGameWin(headingText)
{
	DivBase.call(this, null); // call super constructor

	this.mirrorDb=new RoboGameDb();

	this.sectorWidth=32;
	this.sectorHeight=32;
	
	this.mouseDownPos;

	this.headingText=headingText;


	this.subWin = null;
	//this.msgWin = null;

	// Observer (AKA camera) location, from where we want to look at the 3d objects in the world.
	// TODO: Needs top be three vectors so that we can look in different directions. See "emscripten-sdl2-gl".
	this.x=0;
	this.y=0;
	this.z=0;

}	
	

RoboGameWin.prototype.init=function()
{
	this.subWin = new RoboGameGlDiv(this);
	//this.msgWin = new RoboGameWinMsg(this);
}	

// This creates the main web page for empire game
RoboGameWin.prototype.defineDiv=function()
{
	var msgAreaHeight=120;
	var scrollbarSize=16;

	
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

	//newPage+=this.msgWin.defineArea(msgWinSize);
	
	newPage+='</section>';


	$("body").empty();
	$("body").append(newPage);



}

RoboGameWin.prototype.defineAndDrawPage=function()
{
	this.defineDiv();
	this.addEventListenersDiv();
	this.drawDiv();
}	


RoboGameWin.prototype.addEventListenersDiv=function()
{
	this.subWin.addEventListenersDiv();
	//this.msgWin.addEventListenersDiv();
	var t=this;
	document.onkeypress=function(event)
	{
		console.log('keypress: ' + event.which);
		
		switch(event.which)
		{
			case 97: // a
				t.x--;
			    break;
			case 100: // d
				t.x++;
				console.log('keypress: x '+t.x);
			    break;
			case 120: // x
				t.y--;
			    break;
			case 119: // w
				t.y++;
			    break;
			case 32: // space
				t.z--;
			    break;
			case 60: // <
				t.z++;
			    break;
			case 119: // w
				t.y++;
			    break;
			case 101: // e
				console.log('keypress: inventory');
				
				//t.subWin = new RoboGameWinInventory(t);
				//t.defineAndDrawPage();
			        break;
			default:
				if (event.which<32)
				{
					console.log('keypress: less than space "'+event.which+'"');
				
					//t.subWin = new RoboGameMapWin(t);
					//t.defineAndDrawPage();
			
				}
				else
				{
				        websocket.send('keypress "'+event.which+'"');
				}
			    break;
		       		        
		}
		
		t.drawDiv();
		
	}
}

RoboGameWin.prototype.drawDiv=function()
{
	this.subWin.drawDiv();
	//this.msgWin.drawWin();
}

	
// Is this used?
RoboGameWin.prototype.onInputText=function(srcId)
{
	var theSrcTextBox = document.getElementById(srcId);
	var str=theSrcTextBox.value;
	doSend('textMsg "'+str+'"');
	theSrcTextBox.value="";
}
	
	
// This receives messages from server (via internet web sockets).
RoboGameWin.prototype.onMessageArg=function(arg)
{
	var i=0;
	var cmd=arg[0];
	var reply='';


	//console.log("onMessageArg: '" +arg+"'"); 

	// Here we check what server want client program to do.
	// Commands sent by ConnectionThread need to be interpreted here.


	if (this.mirrorDb.onMirrorMessage(arg)!=0)
	{
		// Message was handled by mirror
	}
	else if (cmd=="mirrorUpdated")
	{			
		if (this.mirrorDb!=null)
		{
			this.drawDiv();
		}
		doSend('mirrorAck ' + arg[1]);
	}
	/*else if (cmd=="mirrorWorld") // TODO: this command should be moved to mirror. But shall it be in MirrorDb or a new MirrorWin.js? It creates a new EmpDb which is a problem in moving it.
	{
		this.mirrorDb=new EmpDb();
		this.subInputState = this.mirrorDb;
	}*/
	else if (cmd=="return")
	{
		empireClose();
	}
	else if (cmd=="consoleMessage")
	{
		if (this.msgWin)
		{
			this.msgWin.textBoxAppend(arg[1]);
		}
		else
		{
			console.log("textBoxAppend " + arg[1]);
		}
	}
	else if (cmd=="showWorld")
	{			
		if (this.mirrorDb!=null)
		{
			//this.mirrorDb.debugDump(" ");
			
			this.defineAndDrawPage();
		}
	}
	else if (cmd=="empWorldClose")
	{			
		console.log("onMessageArg: this.empWorldClose:"); 
		empireClose();
	}
	else if (cmd=="avatarId")
	{			
		this.avatarId = parseInt(arg[1]);	
	}
	else
	{
		console.log("onMessageArg: unknown command: '" + cmd+"'"); 
		//empireClose();
	}	
}




