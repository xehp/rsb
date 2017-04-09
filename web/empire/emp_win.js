// emp_win.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-03-14 by Henrik Bjorkman www.eit.se/hb










function EmpWin()
{
	//this.subInputState=null;
	this.empDb=new EmpDb();

	this.mobileMode=null;
	this.mapZoom=1;

	this.mapSectorWidth=16*2;
	this.mapSectorHeight=14*2;
	
	
	this.mapIntervalVariable;
	this.mapMouseDownPos;
	this.mapHeadingText;
	this.mapOrder=null;
	
	
	this.mapSelection=null;
	this.selectionList=[];
	this.nSelections=0;

	this.empWinUnit = null;
	this.empWinBuild = null;
	this.empWinNations = null;
	this.empWinSectors = null;
	this.empWinTerrain = null;
	this.empWinTerminal = null;
	this.empWinMenu = null;
	this.empWinTextinput = null;
	this.empWinHdr = null;
	this.empWinAssemble = null;

	this.subWin = null; // In the biggest "div" different kind of info can be shown, this keep track of which info is being shown.
	
	this.mapNation=-1; // an integer index, to tell which state this player control. 
	
	this.headingText="Empire";
	


	//this.buttonMode=1; // 0 = don't auto change window, 1 = change window directly
	
}


EmpWin.prototype.init=function()
{
	this.sectorTypesList=new EmpSectorTypeList();
	this.mapInitData();

	this.empWinUnit = new EmpWinUnit(this);
	this.empWinBuild = new EmpWinBuild(this);
	this.empWinNations = new EmpWinStates(this);
	this.empWinSectors = new EmpWinSectors(this);
	this.empWinTerrain = new EmpWinTerrain(this);
	this.empWinTerminal = new EmpWinTerminal(this);
	this.empWinMenu = new EmpWinMenu(this);
	this.empWinTextinput = new EmpWinTextinput(this);
	this.empWinMsg = new EmpWinMsg(this);
	this.subWin=this.empWinTerrain;
	this.empWinHdr=new EmpWinHdr(this);
	this.empWinAssemble = new EmpWinAssemble(this);
}


	
	


EmpWin.prototype.mapClearSelections=function()
{
	this.mapOrder=null;
	this.mapSelection=null;
	this.empWinMenu.drawDiv();
	this.clearSelectionList();
}	


EmpWin.prototype.mapInitData=function()
{
	//terrainMap=new terrainClass2(32,32,5, 0);
	//unitTypeNames=new Array();
	//knownUnits=new Array();

	this.mapIntervalVariable=null;
	this.mapMouseDownPos=null;
	this.mapHeadingText=null;
	this.mapOrder=null;
	this.mapSelection=null;
	this.clearSelectionList();	
}



// This creates (make HTML layout) for the main window page of this game
// The main window contains 4 sub windows (AKA DIVs)
// * header on the to of the window
// * button panel to the left
// * middle right, different things can be shown here such as the world map.
// * messages text area in the lower part
EmpWin.prototype.defineDiv=function()
{
	console.log("defineDiv");

	var msgAreaHeight=120;
	var scrollbarSize=64;
	var buttonListWidth=0;
	
	if (this.mobileMode==null)
	{
		if (window.innerWidth>window.innerHeight)
		{
			this.mobileMode=0;
			//this.mapZoom=1;
		}
		else
		{
			this.mobileMode=1;			
			//this.mapZoom=2;
		}
	}
	
	
	if (msgAreaHeight*4>window.innerHeight)
	{
		msgAreaHeight=Math.floor(window.innerHeight/4);
	}
	
	if (!this.mobileMode)
	{
		//buttonListWidth=Math.floor(window.innerWidth*0.08);
		buttonListWidth=100;
	}

	this.mapSectorWidth=(32*this.mapZoom);
	this.mapSectorHeight=(28*this.mapZoom);

	var buttonWinSize={x: buttonListWidth, y: window.innerHeight-msgAreaHeight-scrollbarSize};
	var msgWinSize={x: window.innerWidth-buttonListWidth-scrollbarSize, y: msgAreaHeight};
	var divSize={x: window.innerWidth-buttonWinSize.x-scrollbarSize, y: window.innerHeight-msgWinSize.y-scrollbarSize};

	// TODO we need a callback if window size is changed. 
	/*$( window ).resize(
		function() {rootDiv.mapSetShowState(0);
	});*/


	// It would be possible to set new CSS from here but that is not needed now. 
	/*
	$('html, body').css({
	    'overflow': 'hidden',
	    'height': '100%'
	});
	*/

	var newPage='';


	document.title = rootDiv.headingText + ' - ' + clientVersion;

	

	//console.log('mapOpenCreateCanvas');

	
	if (buttonListWidth>0)
	{
		// Desktop mode (AKA landscape)
	
		// header on the to of the window
		// The upper text area, shows info about selected unit and hints to user
		newPage+=rootDiv.empWinHdr.defineDiv(buttonWinSize)

		// button panel to the left
		// Buttons to the left and map etc to the right here
		newPage+=rootDiv.empWinMenu.defineDiv(buttonWinSize);


		// * middle right, different things can be shown here such as the world map.
		// Main section, the map etc can be shown here.
		// mapGetStateHandler is used to get which sub DIV to be shown here
		newPage+=rootDiv.mapGetStateHandler().defineDiv(divSize);

	    // messages text area in the lower part
		// The message section, text messages are shown here.
		newPage+=rootDiv.empWinMsg.defineDiv(msgWinSize);
				
	}
	else
	{
		// Mobile mode (AKA portrait)

		newPage+=rootDiv.mapGetStateHandler().defineDiv(divSize);
	
	    // messages text area in the lower part
		newPage+=rootDiv.empWinMsg.defineDiv(msgWinSize);
	
	}


	$("body").empty();
	$("body").append(newPage);


		
		
	// Remember our main window	
	rootDiv.element = document.getElementById('myCanvas');

}


EmpWin.prototype.addEventListenersDiv=function()
{
	rootDiv.mapGetStateHandler().addEventListenersDiv();

	this.empWinMsg.addEventListenersDiv();

	//this.empWinMenu.addEventListenersDiv();	

	// If game need direct input from keyboard this is a good place to setup the callback.
	// This game don't need direct input from keyboard
}


EmpWin.prototype.drawDiv=function()
{
	//console.log("drawDiv");
	
	this.empWinHdr.drawDiv();

	this.mapGetStateHandler().drawDiv();

	this.empWinMsg.drawDiv();

	// show selected unit/sector etc
	this.empWinMenu.drawDiv();	
}


EmpWin.prototype.defineAndDrawPage=function()
{
	this.defineDiv();
	this.addEventListenersDiv();
	this.drawDiv();
}


EmpWin.prototype.onInputText=function(srcId)
{
	var theSrcTextBox = document.getElementById(srcId);
	var str=theSrcTextBox.value;
	doSend('consoleInput "'+str+'"'); // was textMsg
	theSrcTextBox.value="";
}





// This handles all input from server when this game is running in server.	
EmpWin.prototype.onMessageArg=function(arg)
{
	var i=0;
	var cmd=arg[0];
	var reply='';


	//console.log("onMessageArg: '" +arg+"'"); 

	// Here we check what server want client program to do.
	// Commands sent by ConnectionThread need to be interpreted here.


	if (this.empDb.onMirrorMessage(arg)!=0)
	{
		// Message was handled by mirror
	}
	else if (cmd=="return")
	{
		empireClose();
	}
	else if (cmd=="consoleMessage")
	{
		//var msg=hlibRemoveQuotes(arg[1]);
		var msg=arg[1];
		
		console.log("consoleMessage: '" +msg+"'");

		this.empWinTerminal.addText(hlibRemoveQuotes(msg)+"\n");
			
		// We can send it to message window also if terminal is not open.
		// This did not work so well since it is overwritten almost immediately
		if (this.subWin != this.empWinTerminal)
		{
			this.empWinMsg.addText(hlibRemoveQuotes(msg));
		}
	}
	else if (cmd=="mirrorUpdated")
	{
		// The server sends this when it has finished sending a batch of updates.
		// When this is received it it time to redraw the map.			
		if (this.empDb!=null)
		{
			this.drawDiv();
		}
		doSend('mirrorAck ' + arg[1]);
	}
	else if (cmd=="showWorld")
	{
		// When this command is received client will define the main game window (and its initial sub windows AKA div)			
		if (this.empDb!=null)
		{
			this.defineAndDrawPage();
		}
	}
	else if (cmd=="joinEmpire")
	{
		// This tells the client which state the player is. This is the analog to avatarId in yukigassen.
		this.mapNation = parseInt(arg[1]);
		console.log("this.inputState0: joinEmpire: " + this.mapNation); 
	}
	else
	{
		console.log("onMessageArg: unknown command: '" + cmd+"'"); 
		//empireClose();
	}	

	return this;
}


EmpWin.prototype.mapSetShowState=function(newState)
{
	if (newState==0)
	{
		// state 0 is deprecated, set this.mapSelection to null and use newState 2 instead.
		this.mapSelection=null;
		newState=2;
	}
	
	if (newState==3) 
	{
		console.log('state 3 is deprecated, use 2 instead');
		newState=2;
	}

	this.subWin=this.mapGetStateHandlerObj(newState);
	
	// Redefine the window (top div and its sub divs.)
	this.defineAndDrawPage();
	
	// 
	this.drawDiv();
}


EmpWin.prototype.mapMoveOrder = function()
{
	console.log("mapMoveOrder");
	if (this.nSelected()==0)
	{
		this.mapSetUpperTextArea1("select a unit first'");
	}
	else
	{
		//var element=document.getElementById("myCanvas");

		this.mapOrder='moveTo';
		this.mapSelection=null;			
		this.empWinHdr.mapSetUpperTextArea1("click on the destination to move to (on the map)");
		this.mapSetShowState(0);
		//this.mapGetStateHandler().drawDiv();
		this.empWinMenu.drawDiv();
	}
}





EmpWin.prototype.mapMemoryClear = function()
{
	console.log("mapMemoryClear");
	this.empWinTerrain.center();
	this.mapClearSelections();
}



// TODO is this still used?
EmpWin.prototype.mapGoAboard=function()
{
	if ((this.mapSelection!=null) && (this.nSelected()>0) && (this.mapOrder!=null))
	{
		for(i in this.selectionList)
		{						
			console.log('unitOrder '+ i + ' "'+this.mapOrder+' '+ this.mapSelection.id +'"');
			doSend('unitOrder '+ i + ' "'+this.mapOrder+' '+ this.mapSelection.id +'"');
		}
		//this.mapClearSelections();
		this.clearSelectionList();
		this.mapOrder=null;
	}
	else if ((this.mapSelection!=null) && (this.nSelected()>0))
	{
		for(i in this.selectionList)
		{		
			console.log('unitOrder '+ i + ' "goTo '+this.mapSelection.id+'"');
			doSend('unitOrder '+ i + ' "goTo '+this.mapSelection.id+'"');
		}
		//this.mapClearSelections();
		this.clearSelectionList();
	}
	else if (this.nSelected()>0)
	{
		//this.mapRemember();
		this.mapOrder='goTo';
		this.mapSetUpperTextArea1("now select the unit to board, then press 'go aboard' again");
	}
	else
	{
		this.mapSetUpperTextArea1("select a unit first'");
	}
}

/*
EmpWin.prototype.mergeUnits=function()
{
	console.log('mergeUnits '+this.mapSelection.id+'  '+this.selectionList);

	for(i in this.selectionList)
	{		
		console.log('the list '+ i);
	}

	var nSelections=this.selectionList.length;
	if (nSelections>=2)
	{
	    //var firstSelection=this.selectionList[0];
	    var firstSelection=this.mapSelection.id;
		var str='';
		//for(i=1;i<nSelections;i++)
		//{		
		//	str+=' '+this.selectionList[i];
		//}
		for(i in this.selectionList)
		{		
			str+=' '+ i;
			firstSelection=i;
		}
		console.log('unitOrder '+ firstSelection + ' "mergeGroup '+str+'"');
		doSend('unitOrder '+ firstSelection + ' "mergeGroup '+str+'"');
		this.clearSelectionList();		
	}
}
*/
EmpWin.prototype.mergeUnits=function()
{
	console.log('groupUnits '+this.mapSelection.id+'  '+this.selectionList);
	for(i in this.selectionList)
	{		
		console.log('the list '+ i);
	}

	var nSelections=this.selectionList.length;
	if (nSelections>=2)
	{
		var str='';
		for(i in this.selectionList)
		{		
			str+=' '+ i;
			firstSelection=i;
		}
		//console.log('groupUnits "' + str + '"');
		//doSend('groupUnits "' + str + '"');
		doSend('groupUnits' + str);
		console.log('groupUnits' + str);
		this.clearSelectionList();		
	}
	else
	{
		console.log('at least two units must be selected to group them');	
	}
}


EmpWin.prototype.mapCanvasCancel=function() 
{
	//empWinConsole=false;
	this.mapSetShowState(0);
	this.mapMemoryClear();
	this.mapClearSelections();

	$("p").empty();
	window.clearInterval(this.mapIntervalVariable)
        document.title = this.mapHeadingText;
	doSend('cancel');
	empireClose();
}


EmpWin.prototype.mapSendOrderOne=function(theOrder)
{
	if (this.mapSelection!=null)
	{
		doSend('unitOrder '+ this.mapSelection.id + ' "'+theOrder+'"');
		this.mapClearSelections();
	}
	else
	{
		this.mapSetUpperTextArea1("select a unit first");
	}
}



EmpWin.prototype.mapCancelOrder = function()
{
	if (this.mapSelection!=null)
	{
		doSend('cancelOrder '+ this.mapSelection.id);
		//this.mapSelection=null;
		this.empWinMenu.drawDiv();
	}
	else
	{
		this.mapSetUpperTextArea1("select a unit first");
	}
}



EmpWin.prototype.mapCalcOffSet=function(y)
{
	var o=0;
	if ((y%2)!=0)
	{
		o=this.mapSectorWidth/2;
	}
	return(o);
}




EmpWin.prototype.mapGetStateHandlerObj=function(state)
{
	switch (state)
	{
		default:
		case 2:
			// The big map
			if (rootDiv.mapSelection == null)
			{
			    console.log("EmpTerrain");
				return this.empWinTerrain;		
			}
			else if (rootDiv.mapSelection instanceof EmpSector)
			{
			    console.log("EmpSector");
				return this.empWinSectors;
			}
			else if (rootDiv.mapSelection instanceof EmpUnit)
			{
			    console.log("EmpUnit");
				return this.empWinUnit;
			}
			else
			{
			    console.log("EmpTerrain");
				return this.empWinTerrain;		
			}
		case 1:
			// Show relations with other states
			return this.empWinNations;
		case 4:
			// The build menu
			return this.empWinBuild;
		case 5:
			// The terminal for messages etc
			return this.empWinTerminal;
		case 6:
			// The menu to be used instead of side panel on mobile devices
			return this.empWinMenu;
		case 7:
			// To enter text on mobile devices.
			return this.empWinTextinput;
		case 8:
			// The build menu
			return this.empWinAssemble;

	}
}
			
EmpWin.prototype.mapGetStateHandler=function()
{
	return this.subWin;
}




EmpWin.prototype.clearSelectionList=function()
{
	this.selectionList=[];
	this.nSelections=0;
}	

EmpWin.prototype.toggleSelection=function(id)
{
	if (this.isSelected(id))
	{
		this.clearSelection(id);
	}
	else
	{
		this.addSelection(id);
	}		
}

EmpWin.prototype.clearSelection=function(id)
{
	if (this.isSelected(id))
	{
		this.selectionList[id]=-1;		
		this.nSelections--;
	}
}

EmpWin.prototype.addSelection=function(id)
{
	if (!this.isSelected(id))
	{
		this.selectionList[id]=id;
		this.nSelections++;
	}		
}

EmpWin.prototype.isSelected=function(id)
{
	if (id in this.selectionList)
	{
		if (this.selectionList[id]>=0)
		{
			return true;
		}
	}
	return false;
}

EmpWin.prototype.nSelected=function()
{
	return this.nSelections;
}

EmpWin.prototype.backToPreviousSubWin=function()
{
	this.mapSelection=null;
	this.mapSetShowState(2);
}

EmpWin.prototype.incReadyTime=function(d)
{
	var w=rootDiv.empDb.empireWorld;

	//readyTime = w.getGameTime()+100; // arg, JavaScript sucks. This is not same as the line below. 
	//readyTime = 100+w.getGameTime(); // But this did not do add either, it also concatenated string
	readyTime = Number(w.getGameTime())+Number(d);

	console.log('readyToTime '+ readyTime);

	doSend('consoleInput "readyToTime '+ readyTime+'"');
}



