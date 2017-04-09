// emp_win_sector.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-03-14 by Henrik Bjorkman www.eit.se/hb


EmpWinSectors.prototype = Object.create(EmpWinList.prototype);
EmpWinSectors.prototype.constructor = EmpWinSectors;



function EmpWinSectors(parentWin)
{	
	EmpWinList.call(this, parentWin); // call super constructor

	
}



EmpWinSectors.prototype.defineDiv=function(divSize)
{
	this.sectorSizeX=rootDiv.mapSectorWidth;
	this.sectorSizeY=rootDiv.mapSectorHeight;
	var curHeight=this.sectorSizeY;

	var newPage='';

	// The central area of the page	
	newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll; float:right;">';


	newPage+='<div>';
	newPage+='<canvas id="currentSectorCanvas" width="'+(divSize.x-32)+'" height="'+curHeight+'"></canvas>';
	newPage+='</div>';

	newPage+='<div style="text-align:left">';
	newPage+='<input class=empbutton id="upButton" type="button" value="up" onclick="rootDiv.mapSelection=rootDiv.mapSelection.parent; rootDiv.mapSetShowState(0)">';
	newPage+='<input class=empbutton id="moveButton" type="button" value="move" onclick="rootDiv.mapMoveOrder()">';
	newPage+='<input class=empbutton id="boardButton" type="button" value="go aboard" onclick="rootDiv.mapGoAboard()">';
	newPage+='<input class=empbutton id="unselectButton" type="button" value="unselect" onclick="rootDiv.clearSelectionList(); rootDiv.mapSetShowState(2)">';
	newPage+='<input class=empbutton id="mergeButton" type="button" value="merge" onclick="rootDiv.mergeUnits()">';
	newPage+='<br><p>';
	newPage+='</div>';


	newPage+='<div>';	
	newPage+=this.defineDivList({x:divSize.x-32, y:divSize.y-curHeight-10-32});
	newPage+='</div>';
	
	newPage+='</div>';
	
	return newPage;
}

EmpWinSectors.prototype.addEventListenersDiv=function()
{
	this.currentSectorElement=document.getElementById("currentSectorCanvas");
	this.currentSectorContext=this.currentSectorElement.getContext("2d");

	DivMouse.prototype.addEventListenersDiv.call(this, "currentSectorCanvas");

	this.addEventListenersDivList();

	this.scrollOffsetY=0;	
}



EmpWinSectors.prototype.drawDiv=function()
{
	this.drawCurrentSector();
	this.drawUnitList(this.parentWin.mapSelection);
}


EmpWinSectors.prototype.drawCurrentSector=function()
{
	var element=this.currentSectorElement;
	var context=this.currentSectorContext;

	//context.clearRect(0, 0, element.width, element.height);
	context.fillStyle="#E0E0E0";	
	context.fillRect(0, 0, element.width, element.height);

	if (this.parentWin.mapSelection!=null)
	{
		var str=this.parentWin.mapSelection.selfToString();

		this.parentWin.mapSelection.showSelfSectorContext(context, 0, 0, rootDiv.mapSectorWidth, rootDiv.mapSectorHeight);			
						
		context.font = '10pt Calibri';
		context.fillStyle = 'black';
		context.fillText(str, this.sectorSizeX+8, this.textOffsetY);
	}
}




EmpWinSectors.prototype.click=function(mouseUpPos)
{
	console.log("nothing happens when clicking here, for now");	

	/*
	console.log("For now will use this as an 'up' button since one is missing.");
	rootDiv.mapSelection=rootDiv.mapSelection.parent;
	rootDiv.mapSetShowState(0);
	*/	
}

