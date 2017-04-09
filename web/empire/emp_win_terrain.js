// emp_win_terrain.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-28 by Henrik Bjorkman www.eit.se/hb




EmpWinTerrain.prototype = Object.create(DivMouse.prototype);
EmpWinTerrain.prototype.constructor = EmpWinTerrain;

// This class handles a div that can show the world map in a "div". A div is a part of a html page. 
function EmpWinTerrain(parentWin)
{	
	DivMouse.call(this, parentWin); // call super constructor

	this.scrollOffsetX=null; // in pixels
	this.scrollOffsetY=null;

	this.mapSizeX=null; // in pixels
	this.mapSizeY=null;
	
	this.scrollTopRemembered=null; // in pixels
	this.scrollLeftRemembered=null;
}

// This is called when this div shall be added to the page.
// NOTE this only sets up the page. When it is time to draw the div drawDiv is called.
EmpWinTerrain.prototype.defineDiv=function(divSize)
{
    // http://stackoverflow.com/questions/9798331/zoom-a-browser-window-view-with-javascript
	/*$(document).ready(function(){
	  $('body').css('zoom','100%'); // Webkit browsers
	  $('body').css('zoom','1.0'); // Other non-webkit browsers
	  $('body').css('-moz-transform',scale(1.0, 1.0)); // Moz-browsers
	});*/

	this.setScrollToHome();

	var x=this.mapSizeX;
	if (x>divSize.x)
	{
		x=divSize.x;
		//console.log("EmpWinTerrain.defineDiv: x="+x); 
	}
	

	var y=this.mapSizeY;
	if (y>divSize.y)
	{
		y=divSize.y;
		//console.log("EmpWinTerrain.defineDiv: y="+y); 
	}

	var canvasXSize=Math.round(this.mapSizeX+rootDiv.mapSectorWidth/2);

	// Set scroll to so that the canvas is centered. By default it would be in upper left.
	if ((this.mapSizeY!=null) && (this.scrollTopRemembered==null))
	{
		this.scrollTopRemembered=Math.round(this.mapSizeY/2-divSize.y/2);
		this.scrollLeftRemembered=Math.round(canvasXSize/2-divSize.x/2);

		//console.log("canvasXSize "+canvasXSize+", scrollLeftRemembered "+this.scrollLeftRemembered+ ", divSize.x " +divSize.x);
		//console.log("this.mapSizeY "+this.mapSizeY+", scrollTopRemembered "+this.scrollTopRemembered+ ", divSize.y " +divSize.y);

		if (this.scrollTopRemembered<0)
		{
			this.scrollTopRemembered=0;
		}
		if (this.scrollLeftRemembered<0)
		{
			this.scrollLeftRemembered=0;
		}
	}


	// The central area of the page	
	var newPage='';
	//newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px;">';
	newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll; float:right;">';
	newPage+='<canvas id="myCanvas" width="'+canvasXSize+'px" height="'+this.mapSizeY+'px"></canvas>';
	newPage+='</div>';

	return newPage;		
}

// After all "div" have been setup (after defineDiv but before drawDiv) this is called so that each div can register and remember its elements etc.
EmpWinTerrain.prototype.addEventListenersDiv=function()
{
	if (this.scrollTopRemembered!=null)
	{
		$('#terrainDiv').scrollTop(this.scrollTopRemembered);
		$('#terrainDiv').scrollLeft(this.scrollLeftRemembered);
	}

	this.canvasElement=document.getElementById("myCanvas");
	this.context=this.canvasElement.getContext("2d");

	DivMouse.prototype.addEventListenersDiv.call(this, "myCanvas");

}


// internal help function called from drawDiv
EmpWinTerrain.prototype.showWorldMapSectors=function(context, t)
{
	var cl = t.children;
	if (cl!=null)
	{
		var i;
		for(i in cl)
		{
			if (cl[i] instanceof EmpSector)
			{
				var s=cl[i];
				//console.log("EmpWinTerrain.prototype.drawDiv "+s.index+" "+s.selfToString());
				
				var c=t.translateSectorIndexToColumn(i);
				var r=t.translateSectorIndexToRow(i);			
				var offset=rootDiv.mapCalcOffSet(r);
				var canvasXY=this.fromMapXYToCanvas({x: offset+c*rootDiv.mapSectorWidth, y: r*rootDiv.mapSectorHeight}); 
	
	
				s.showSelfSectorContextBkg(context, canvasXY.x, canvasXY.y, rootDiv.mapSectorWidth, rootDiv.mapSectorHeight);
				s.showSelfSectorContextImg(context, canvasXY.x, canvasXY.y, rootDiv.mapSectorWidth, rootDiv.mapSectorHeight);
				s.showSectorSubUnitContext(context, canvasXY.x, canvasXY.y, rootDiv.mapSectorWidth, rootDiv.mapSectorHeight);
			}
		}
	}
}

// This is called when this div shall be drawn (or redrawn)
EmpWinTerrain.prototype.drawDiv=function()
{
	if (rootDiv.empDb!=null)
	{	
		var w = rootDiv.empDb.getEmpireWorld();
		
		if (w!=null)
		{
			// get the size of our image/map
			var t=w.getEmpireTerrain();
			
			if (t!=null)
			{
				this.mapSizeX=rootDiv.mapSectorWidth * t.sizeX;
				this.mapSizeY=rootDiv.mapSectorHeight * t.sizeY;
				var canvasXSize=Math.round(this.mapSizeX+rootDiv.mapSectorWidth/2);
				var canvasYSize=Math.round(this.mapSizeY+rootDiv.mapSectorWidth/4);

				var canvasElement=this.canvasElement;
				canvasElement.width = canvasXSize;
				canvasElement.height = canvasYSize;
				this.context.fillStyle="#E0E0E0";				
				this.context.fillRect(0, 0, canvasElement.width, canvasElement.height);
		
				// show world map sectors
				this.showWorldMapSectors(this.context, t);
			}
		}		
	}
}


// TODO: translating mouse position to canvas does not work on android-chrome. Perhaps this is a hint:  http://stackoverflow.com/questions/5189968/zoom-canvas-to-mouse-cursor
EmpWinTerrain.prototype.click=function(mouseUpPos)
{
	mapPos = this.fromCanvasToMapXY(mouseUpPos);

	var upSectorPos = rootDiv.empDb.terrain.fromXYToPos(mapPos);
	
	// a click in another sector
	if ((rootDiv.mapOrder!=null) && (rootDiv.nSelected()>0))
	{
		// The user have started to enter a "moveTo" order
		
		// all selected units are to be ordered to move to the sector that was clicked
		for(i in rootDiv.selectionList)
		{						
			var str='unitOrder '+ i + ' "'+ rootDiv.mapOrder+ ' ' + upSectorPos+'"';
			console.log(str);
			doSend(str);
		}
		
		rootDiv.clearSelectionList();
		rootDiv.mapOrder=null;
	}
	else
	{
		// No order is being entered, show the user the content of the sector clicked in.
		
		this.scrollTopRemembered=$('#terrainDiv').scrollTop();
		this.scrollLeftRemembered=$('#terrainDiv').scrollLeft();
		
		rootDiv.mapSelection=rootDiv.empDb.getSector(upSectorPos);
		rootDiv.mapSetShowState(2);
	}
	
		
	// show selected unit/sector etc
	this.parentWin.empWinMenu.drawDiv();
	
}


// We need a drag mouse function to scroll around if world is bigger than what we can show.
// http://www.w3schools.com/tags/canvas_clip.asp


// This is called if mouse drag has been detected. It will reposition the map center.
EmpWinTerrain.prototype.drag=function(mouseDownPos, mouseUpPos)
{
    var mouseDrag=this.mouseDiff(mouseDownPos, mouseUpPos);
	
	this.scrollOffsetX-=mouseDrag.x;
	this.scrollOffsetY-=mouseDrag.y;
	
	/*
	if (this.scrollOffsetX<0)
	{
		this.scrollOffsetX+=this.mapSizeX;
	}
	
	if (this.scrollOffsetY<0)
	{
		this.scrollOffsetY+=this.mapSizeY;
	}

	if (this.scrollOffsetX>=this.mapSizeX)
	{
		this.scrollOffsetX-=this.mapSizeX;
	}

	if (this.scrollOffsetY>=this.mapSizeY)
	{
		this.scrollOffsetY-=this.mapSizeY;
	}
*/


	console.log("drag "+mouseDrag.x+" "+mouseDrag.y+"  "+this.scrollOffsetX+" "+this.scrollOffsetY);
	
	this.drawDiv();
}

// This is used to translate xy coordinates from canvas to map.
// On canvas 0,0 is upper left corner.
EmpWinTerrain.prototype.fromCanvasToMapXY=function(canvasPos)
{
	var x = canvasPos.x+this.scrollOffsetX;
	var y = canvasPos.y+this.scrollOffsetY;

	while (x<0)
	{
		x+=this.mapSizeX;
	}
	
	while (y<0)
	{
		y+=this.mapSizeY;
	}

/*
	canvasXSize=Math.round(this.mapSizeX+rootDiv.mapSectorWidth/2);
	while (x>=canvasXSize)
	{
		x-=this.mapSizeX;
	}
*/

	while (x>=this.mapSizeX)
	{
		x-=this.mapSizeX;
	}


	while (y>=this.mapSizeY)
	{
		y-=this.mapSizeY;
	}

	return {x: x, y: y};
}

// This is used to translate xy coordinates from map to canvas.
EmpWinTerrain.prototype.fromMapXYToCanvas=function(mapPos)
{


	var x = mapPos.x-this.scrollOffsetX;
	var y = mapPos.y-this.scrollOffsetY;

	//console.log("EmpWinTerrain.fromMapXYToCanvas "+mapPos.x+" "+this.scrollOffsetX+" "+x+" "+this.mapSizeX);

	while (x<0)
	{
		x+=this.mapSizeX;
	}
	
	while (y<0)
	{
		y+=this.mapSizeY;
	}

	while (x>=this.mapSizeX)
	{
		x-=this.mapSizeX;
	}

	while (y>=this.mapSizeY)
	{
		y-=this.mapSizeY;
	}

	return {x: x, y: y};
}

// Recenters the map (so that the players state is in center of the canvas.
EmpWinTerrain.prototype.center=function()
{
	this.scrollOffsetX=null;
	this.scrollOffsetY=null;
	this.setScrollToHome();
	this.drawDiv();	
}


// internal help function
// It calculates map offsets so that the players state is in center of the canvas.
EmpWinTerrain.prototype.setScrollToHome=function()
{
	if (rootDiv!=null)
	{
		var db=rootDiv.empDb;
		if (db!=null)
		{
			var w = db.getEmpireWorld();
			
			if (w!=null)
			{
				var t=w.getEmpireTerrain();

				if (t!=null)
				{		
					this.mapSizeX=rootDiv.mapSectorWidth * t.sizeX;
					this.mapSizeY=rootDiv.mapSectorHeight * t.sizeY;
	
					// Figure out how to set the offset so that the players home town gets in the middle of the canvas.
					if (this.scrollOffsetX==null)
					{
						var es = w.getState();
						if (es!=null)
						{
							var homeSectorId = es.homeSectorId;
							var homeSector = db.getById(homeSectorId);
							var homeSectorPos = homeSector.index;
							var c=t.translateSectorIndexToColumn(homeSectorPos);
							var r=t.translateSectorIndexToRow(homeSectorPos);
							var playerCenterX=Math.round(c*rootDiv.mapSectorWidth);							
							var playerCenterY=Math.round(r*rootDiv.mapSectorHeight);							
							var mapCenterX=Math.round(this.mapSizeX/2);							
							var mapCenterY=Math.round(this.mapSizeY/2);							
							this.scrollOffsetX=-(mapCenterX-playerCenterX);
							this.scrollOffsetY=-(mapCenterY-playerCenterY);
							console.log("homeSectorId "+homeSectorId+", homeSectorPos " + homeSectorPos+ " c " + c + " r " + r);
							console.log("playerCenterX "+playerCenterX+", playerCenterY " + playerCenterY+ " mapCenterX " + mapCenterX + " mapCenterY " + mapCenterY);
							console.log("scrollOffsetX "+this.scrollOffsetX+ " scrollOffsetY " +this.scrollOffsetY);
						}
					}
				}
			}
		}		
	}	
				
}

