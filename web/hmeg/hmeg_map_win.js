// hmeg_map_win.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-28 by Henrik Bjorkman www.eit.se/hb




HmegMapWin.prototype = Object.create(DivMouse.prototype);
HmegMapWin.prototype.constructor = HmegMapWin;

function HmegMapWin(parentWin)
{	
	DivMouse.call(this, parentWin); // call super constructor

	this.scrollOffsetX=null; // in pixels
	this.scrollOffsetY=null;

	this.mapSizeX=null; // in pixels
	this.mapSizeY=null;
	
	this.scrollTopRemembered=null; // in pixels
	this.scrollLeftRemembered=null;
	
	this.parentWin = parentWin;
	this.currentRoom = null;
	
	this.wantedX = null;
	this.wantedY = null;
	this.wantedRotation = null;
	this.changedCounter = null;
	
	
}


// This is called when this div shall be added to the page.
// NOTE this only sets up the page. When it is time to draw the div drawDiv is called.
HmegMapWin.prototype.defineDiv=function(divSize)
{
	console.log("defineDiv: x="+divSize.x+", y="+divSize.y);
	this.initRoom();

	var canvasXSize=divSize.x-16;
	var canvasYSize=divSize.y-16;

	// Set scroll to so that the canvas is centered. By default it would be in upper left.
	if ((this.mapSizeY!=null) && (this.scrollTopRemembered==null))
	{
		this.scrollTopRemembered=Math.round(this.mapSizeY/2-divSize.y/2);
		this.scrollLeftRemembered=Math.round(canvasXSize/2-divSize.x/2);

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
	newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll;">';
	newPage+='<canvas id="mapCanvas" width="'+canvasXSize+'px" height="'+canvasYSize+'px"></canvas>';
	newPage+='</div>';

	console.log("****************");

	return newPage;		
}


// After all "div" have been setup (after defineDiv but before drawDiv) this is called so that each div can register and remember its elements etc.
HmegMapWin.prototype.addEventListenersDiv=function()
{
	console.log("addEventListenersDiv");
	
	if (this.scrollTopRemembered!=null)
	{
		$('#terrainDiv').scrollTop(this.scrollTopRemembered);
		$('#terrainDiv').scrollLeft(this.scrollLeftRemembered);
	}

	this.element=document.getElementById("mapCanvas");
	this.context=this.element.getContext("2d");

	DivMouse.prototype.addEventListenersDiv.call(this, "mapCanvas");

}




HmegMapWin.prototype.showWorldMapSectors=function(context, hmegRoom)
{
	//console.log("showWorldMapSectors "+hmegRoom.xSectors+ " "+hmegRoom.ySectors+ "  "+this.sectorWidth+" "+this.sectorHeight);


	var p = this.parentWin; 
	var a = p.getAvatar();
	var d= p.hmegDb;

	// Current room shall be the room in which the avatar is.
	var r = a.parent;
	if (r != hmegRoom)
	{
		console.log("r != hmegRoom");
	}
	

	if ((p.wantedX!=this.wantedX) || (p.wantedY!=this.wantedY) || (p.wantedRotation!=this.wantedRotation) || (this.changedCounter != d.changedCounter) || (hmegRoom.changedCounter != DivImagesBeingLoaded))
	{
		//console.log('HmegMapWin: showWorldMapSectors');
	
		var element=this.element;
		this.context.fillStyle="#e0e0e0";				
		this.context.fillRect(0, 0, element.width, element.height);
	
		this.wantedX = p.wantedX;
		this.wantedY = p.wantedY;
		this.wantedRotation = p.wantedRotation;
		this.changedCounter = d.changedCounter;
		

		var px=Math.floor(p.wantedX);
		var py=Math.floor(p.wantedY);
		
		
		// This is for regular blocks of the current room
		var xy=this.translateBlockToCanvas(0, 0);
		hmegRoom.refreshSelf();
		context.drawImage(hmegRoom.canvas, xy.x, xy.y, hmegRoom.canvas.width, hmegRoom.canvas.height);
	
	
	
	
		// This is for entities residing in the current room
		// if any loop all children
		var cl = hmegRoom.children;
		if (cl!=null)
		{
			var i;
			for(i in cl)
			{
				// Only object of type HmegEntity need to be shown (others ignored)
				if (cl[i] instanceof HmegEntity)
				{
					var s=cl[i];
					//console.log("HmegMapWin.prototype.drawDiv "+s.index+" "+s.selfToString());
					
					s.extrapolatePosition();

					var xy=this.translateBlockToCanvas(s.x, s.y);	
	
					// If it is the avatar we need special handling
	
	
					// TODO perhaps the if room below can be moved into method showSelfContextXY?
	
					if (s == a)
					{
						// Its our avatar, Special handling for the avatar
						
						// We may need a copy of avatar that is located at the wanted location.
						// But using an ugly fix instead, setting rotation to wantedRotation (it will be set back again when server sends an update)
						a.rotation=p.wantedRotation;
						
						s.showSelfContextXY(context ,this.element.width/2, this.element.height/2, this.sectorWidth, this.sectorHeight);					
					}
					else
					{
						// It is not our avatar
						
						// is it a room?
						
						if (s instanceof HmegRoom)
						{
							// Rooms will have one image that is to be drawn to represent the room (even if the room contains other objects)
							// An alternative to this would be to draw child objects recursively. 
		
							context.fillStyle="#c0c0c0";				
							context.fillRect(xy.x, xy.y, this.sectorWidth*s.outerX, this.sectorHeight*s.outerY);				
		
							var canvas2 = document.createElement('canvas');
							var context2 = canvas2.getContext('2d');
		
							canvas2.width = this.sectorWidth*s.xSectors;
							canvas2.height = this.sectorHeight*s.ySectors;
		
							this.showWorldMapSectors(context2, s);
		
							
							context.drawImage(canvas2, xy.x, xy.y, this.sectorWidth*s.outerX, this.sectorHeight*s.outerY);
		
						}
						
						
						// This is for debugging, remove later				
						//context.fillStyle="#80F000";				
						//context.fillRect(xy.x, xy.y, this.sectorWidth, this.sectorHeight);

						s.showSelfContextXY(context ,xy.x, xy.y, this.sectorWidth, this.sectorHeight);
					}
				}
			}
		}
	}	
}

HmegMapWin.prototype.initRoom=function()
{
	//console.log("init begin");

	var p = this.parentWin;
	var d = p.hmegDb;

	this.sectorWidth=p.sectorWidth;
	this.sectorHeight=p.sectorHeight;

	//console.log("sector w="+this.sectorWidth+", h="+this.sectorHeight);

	if (d!=null)
	{	
		var w = d.rootObj;
		if (w!=null)
		{
			//this.currentRoom = w.findSubObjectByName("spawnRoom");  // TODO: This shall be the room that is parent of avatar.
			var playerAvatar = d.getById(p.avatarId);
			if (playerAvatar!=null)
			{		
				this.currentRoom = playerAvatar.parent;
				if (this.currentRoom !=null)
				{
					this.mapSizeX=this.sectorWidth * this.currentRoom.xSectors;
					this.mapSizeY=this.sectorHeight * this.currentRoom.ySectors;
				}
				else
				{
					//console.log("no current room");
					throw new Error("no current room for avatar ~"+p.avatarId);
				}		
			}
			else
			{
				// server must send avatarId and/or setAprId
				//console.log("no avatar"); 
				throw new Error("no avatar ~"+p.avatarId);
			}		
		}
		else
		{
			console.log("no world");
			//throw new Error("no world");
		}		
	}
	else
	{
		//console.log("no hmegDb");
		throw new Error("no hmegDb");
	}		
	//console.log("init done");

}



// This is called when this div shall be drawn (or redrawn)
HmegMapWin.prototype.drawDiv=function()
{
	//console.log("drawDiv begin, current room id=~"+this.currentRoom.id+", size: x="+this.currentRoom.xSectors+", y="+this.currentRoom.ySectors);
	
	this.initRoom();

	if (this.currentRoom!=null)
	{
		this.mapSizeX=this.sectorWidth * this.currentRoom.xSectors;
		this.mapSizeY=this.sectorHeight * this.currentRoom.ySectors;
		//var canvasXSize=Math.round(this.mapSizeX);
		//var canvasYSize=Math.round(this.mapSizeY);


		// show world map sectors
			
		var showRoom=this.currentRoom;
		
		/*
		var a=this.playerAvatar;
		if (a!=null)
		{
			var p = showRoom.getParent(); 
			// if in control room then 
			if (p instanceof HmegRoom)
			{
				if (showRoom.isControlPanel(a.x,a.y))
				{
					console.log("control panel");
					a=showRoom;
					showRoom=p;
				}
			}
		}
		*/
		
		this.showWorldMapSectors(this.context, showRoom);
		
	}
	else
	{
		console.log("no current room");
	}		

	//console.log("drawDiv done");
}



// TODO: translating mouse position to canvas does not work on android-chrome. Perhaps this is a hint:  http://stackoverflow.com/questions/5189968/zoom-canvas-to-mouse-cursor
HmegMapWin.prototype.click=function(mouseUpPos)
{
	mapPos = this.translateCanvasToBlock(mouseUpPos);


	console.log("click " + mapPos.x +" "+mapPos.y);
	
		
    websocket.send('mapClick '+mapPos.x+' '+mapPos.y);
	
}


// We need a drag mouse function to scroll around if world is bigger than what we can show.
// http://www.w3schools.com/tags/canvas_clip.asp


HmegMapWin.prototype.drag=function(mouseDownPos, mouseUpPos)
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

/*
HmegMapWin.prototype.translateBlockToCanvas=function(x,y)
{
	var a = this.parentWin.getAvatar();

	var px=(x-a.x)*this.sectorWidth+this.element.width/2;
	var py=(y-a.y)*this.sectorHeight+this.element.height/2;

	return {x: px, y: py};
}

HmegMapWin.prototype.translateCanvasToBlock=function(xy)
{
	var a = this.parentWin.getAvatar();

	var bx = ( (xy.x - this.element.width/2) / (this.sectorWidth)) + a.x;
	var by = ( (xy.y - this.element.height/2) / (this.sectorHeight)) + a.y;

	//var i = bx + this.ScreenSizeX*by ; 
	//return i;

	console.log("translateCanvasToBlock: "+xy.x+" "+xy.y+"  "+bx+" "+by);

	return {x: bx, y: by};
}
*/

HmegMapWin.prototype.translateBlockToCanvas=function(x,y)
{
	var pw = this.parentWin;

	var px=(x-pw.wantedX)*this.sectorWidth+this.element.width/2;
	var py=(y-pw.wantedY)*this.sectorHeight+this.element.height/2;

	return {x: px, y: py};
}

HmegMapWin.prototype.translateCanvasToBlock=function(xy)
{
	var pw = this.parentWin;

	var bx = ( (xy.x - this.element.width/2) / (this.sectorWidth)) + pw.wantedX;
	var by = ( (xy.y - this.element.height/2) / (this.sectorHeight)) + pw.wantedY;

	//var i = bx + this.ScreenSizeX*by ; 
	//return i;

	console.log("translateCanvasToBlock: "+xy.x+" "+xy.y+"  "+bx+" "+by);

	return {x: bx, y: by};
}




HmegMapWin.prototype.center=function()
{
	this.scrollOffsetX=null;
	this.scrollOffsetY=null;
	this.setScrollToHome();
	this.drawDiv();	
}


HmegMapWin.prototype.setScrollToHome=function()
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


HmegMapWin.prototype.onkeypress=function(event)
{
		//console.log('keypress: ' + event.which);		
		switch(event.which)
		{
			case 101:
				console.log('keypress: inventory');
				this.parentWin.subWin = new HmegWinInventory(this.parentWin);
				this.parentWin.defineAndDrawPage();
				break;
			case 102:
				console.log('keypress: terminal');
				
				this.parentWin.subWin = new HmegTerminal(this.parentWin);
				this.parentWin.defineAndDrawPage();
				break;
			default:
				if (event.which<32)
				{
					console.log('keypress: return');
				
					this.parentWin.subWin = new HmegMapWin(this.parentWin);
					this.parentWin.defineAndDrawPage();

				}
				else
				{
				        //websocket.send('keypress "'+event.which+'"');
				}
		        break;
		}
}
