// emp_win_menu.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-01-11 by Henrik Bjorkman www.eit.se/hb

EmpWinMenu.prototype = Object.create(DivBase.prototype);
EmpWinMenu.prototype.constructor = EmpWinMenu;





function EmpWinMenu(parentWin)
{
	DivBase.call(this, parentWin); // call super constructor

	this.parentWin=parentWin;
}


// This updates some text fields in the menu/button area.
// Right text area is not to the right any longer.
EmpWinMenu.prototype.mapUpdateRightTextArea=function()
{
    // This shows the number of selected units. 
	var strInfo=(rootDiv.nSelected()>0) ? ""+rootDiv.nSelected() : "";
	this.mapSetUpperTextAreaN("upperTextArea2", strInfo);

	var w=rootDiv.empDb.empireWorld;
	if (w!=null)
	{
		// game time speed in number of game ticks per second.
		// TODO this don't seem to be available any longer.
		//this.mapSetUpperTextAreaN("msPerTickTextArea", w.gameSpeed); 

		// the game time in number of game ticks.
		this.mapSetUpperTextAreaN("gameTimeTextArea", w.getGameTime()); 

		// player money
		var state=w.getState();
		if (state!=null)
		{
			this.mapSetUpperTextAreaN("playerMoneyTextArea", state.getSavedMoneyUpdated());
		}
		else
		{
			this.mapSetUpperTextAreaN("playerMoneyTextArea", "no state");
		}

	}

}


EmpWinMenu.prototype.mapSetUpperTextAreaN=function(areaName, msg)
{
	var e = document.getElementById(areaName);
	if (e != null)
	{
		e.value=hlibRemoveQuotes(msg);
		e.scrollTop = e.scrollHeight;
	}
}




// TODO if this is still used, mode it to emp_win_hdr.js
EmpWinMenu.prototype.mapUpdateUpperTextArea=function()
{
	var strInfo='';

	if (this.mapSelection != null)
	{
		// show info about this unit
		strInfo=this.mapSelection.selfToString();
		/*
	    if (this.mapSelection instanceof EmpSector)
		{
			var n=this.mapSelectionSector.getNChildObjects();
			if (n>0)
			{
				strInfo+='. there are '+n+' units in sector';
			}
			else
			{
				strInfo+='. empty sector';
			}
		}*/
	}
	else
	{
		//strInfo+='';
	}

	if (this.mapOrder != null)
	{
		if (rootDiv.nSelected()>0)
		{
			if (this.mapOrder == "moveTo")
			{
				strInfo+='  select a sector for selected units to move to';				
			}
			else if (this.mapOrder == "goTo")
			{
				strInfo+='  select a unit for selected units to go aboard';
			}
		}
		else
		{
			strInfo+='  select units for '+this.mapOrder;
		}
	}


	this.mapSetUpperTextAreaN("upperTextArea1", strInfo);
}


EmpWinMenu.prototype.mapSetButtonArea=function(d)
{
	if (document.getElementById('moveButton')!=null)
	{
		document.getElementById('moveButton').disabled=d;
		document.getElementById('boardButton').disabled=d;
		document.getElementById('unselectButton').disabled=d;
	}
}
	
	
	
// This updates some text areas currently to the left not up.
EmpWinMenu.prototype.drawDiv=function()
{
	this.mapUpdateUpperTextArea();
	
	//mapUpdateLeftTextAreas();
	
	this.mapUpdateRightTextArea();

	if (document.getElementById('moveButton')!=null)
	{
		if (rootDiv.nSelected()>0)
		{	
			this.mapSetButtonArea(null);
		}
		else
		{
			this.mapSetButtonArea('disabled');
		}
		
		document.getElementById('mergeButton').disabled=(rootDiv.nSelected()>=2)?null:'disabled';
	}

}



EmpWinMenu.prototype.defineDiv=function(divSize)
{
	var newPage='';

	// The central area of the page	
	newPage+='<div id="buildDiv">';

	newPage+='<input type="text" id="upperTextArea2" class=emptext size="10" readOnly="yes"></br>';
	newPage+='<input class=empbutton type="button" value="clear" onclick="rootDiv.mapMemoryClear()"></br>';
	newPage+='<p>'
	newPage+='<input class=empbutton id="mapButton"         type="button" value="map"       onclick="rootDiv.mapSetShowState(0)"></br>';
	newPage+='<input class=empbutton id="moneyButton"       type="button" value="diplomacy"     onclick="rootDiv.mapSetShowState(1)"></br>';
	newPage+='<input class=empbutton id="terminalButton"    type="button" value="terminal"  onclick="rootDiv.mapSetShowState(5)"></br>';

	if (this.parentWin.mobileMode)
	{
		newPage+='<input class=empbutton id="textinputButton"   type="button" value="enterText" onclick="rootDiv.mapSetShowState(7)"></br>';
		newPage+='<input class=empbutton id="desktopButton"  type="button" value="desktopMode" onclick="rootDiv.mobileMode=0; rootDiv.mapSetShowState(6)"></br>';
	}
	else
	{
		newPage+='<input class=empbutton id="menuButton"        type="button" value="menu"      onclick="rootDiv.mapSetShowState(6)"></br>';
	}


	
	newPage+='<input class=empbutton id="zoomInButton"  type="button" value="zoomIn" onclick="rootDiv.mapZoom*=2; rootDiv.mapSetShowState(2)"></br>';		
	newPage+='<input class=empbutton id="zoomOutButton"  type="button" value="zoomOut" onclick="rootDiv.mapZoom/=2; rootDiv.mapSetShowState(2)"></br>';
		

	newPage+='<input class=empbutton id="readyButton"  type="button" value="time+100" onclick="rootDiv.incReadyTime(100)"></br>';


	if (!this.parentWin.mobileMode)
	{
		newPage+='<p>'
		newPage+='<input class=empbutton id="debugDump"  type="button" value="debugDump" onclick="rootDiv.empDb.rootObj.debugDump()"></br>';		
		newPage+='<p>'
		//newPage+='<input type="text" id="msPerTickTextArea" class=emptext size="10" readOnly="yes"></br>'; // This is currently not used 
		newPage+='<input type="text" id="gameTimeTextArea" class=emptext size="10" readOnly="yes"></br>';
		newPage+='<input type="text" id="playerMoneyTextArea" class=emptext size="10" readOnly="yes"></br>';
		newPage+='<p>'

		newPage+='<input class=empbutton id="mobileButton"  type="button" value="mobileMode" onclick="rootDiv.mobileMode=1; rootDiv.mapSetShowState(6)"></br>';		
	}
	
	newPage+='<p>'
	newPage+='<input class=empbutton id="mapQuitButton"     type="button" value="quit"     onclick="rootDiv.mapCanvasCancel()"><br>';
	newPage+='<p>'
	newPage+='<a href="empire/help.html" target="_blank">help</a>';
	
	
	
	
	newPage+='</div>';


	
	return newPage;
}

EmpWinMenu.prototype.addEventListenersDiv=function()
{
	//this.addEventListenersDiv("myCanvas");
}



EmpWinMenu.prototype.click=function(mouseUpPos)
{
}

