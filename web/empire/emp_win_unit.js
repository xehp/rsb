// emp_win_unit.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb

EmpWinUnit.prototype = Object.create(EmpWinList.prototype);
EmpWinUnit.prototype.constructor = EmpWinUnit;





function EmpWinUnit(parentWin)
{
	//console.log("EmpWinUnit:");

	EmpWinList.call(this, parentWin); // call super constructor

	this.offsetX=2;
	this.sectorSizeX=72;
	this.unitOffsetY=55;
	this.stepY=10;
	this.font='8pt Calibri';
	this.sizeX=64;
	this.sizeY=64;

	this.orderList=[];
}


EmpWinUnit.prototype.showPossibleOrder=function(context, i, order)
{
	//console.log("EmpWinUnit.showPossibleOrder: "+i+" "+order);

	var imageObjMove = new Image();
	var x=this.offsetX+i*this.orderSizeX;
	var y=0;
	var w=this.orderSizeX;
	var h=this.orderSizeY;
	var fileName=order+".png"
	imageObjMove.onload = function() {
		context.drawImage(imageObjMove, x, y, w, h);

	};
	imageObjMove.onerror = function() {
		context.font = '8pt Calibri';
		context.fillStyle = 'black';
		context.fillText(order, x, y+32/2);
	};
	imageObjMove.src = fileName;
}

EmpWinUnit.prototype.getPossibleOrders=function()
{
	//console.log("EmpWinUnit.getPossibleOrders:");

	var i=rootDiv.mapSelection.unitType;
	if (i in rootDiv.empDb.unitTypesList.children)
	{
		var t=rootDiv.empDb.unitTypesList.children[i];
		var o=t.possibleOrders;
		var arg=hlibSplitString(o);
		return arg;
	}
	return [];
}

EmpWinUnit.prototype.getOrder=function(x)
{
	console.log("EmpWinUnit.getOrder:");

	var m = (x-this.offsetX) / this.orderSizeX;
	var n = Math.floor(m);
	console.log("getOrder "+n+" "+m);
	var order = null;
	if ((n>=0) && (n<this.orderList.length))
	{
		order=this.orderList[n];
	}
	return order;
}


EmpWinUnit.prototype.defineDiv=function(divSize)
{
	console.log("EmpWinUnit.defineDiv:");
	
	this.sectorSizeX=rootDiv.mapSectorWidth;
	this.sectorSizeY=rootDiv.mapSectorHeight;
	

	this.orderSizeX=rootDiv.mapSectorWidth*2;
	this.orderSizeY=rootDiv.mapSectorWidth*2;
	
	var arg=this.getPossibleOrders();
	var orderTotX = this.orderSizeX* arg.length;
	if (orderTotX > divSize.x)
	{
		// All orders do not fit on page. Scale down with some factor.
		var f=divSize.x/orderTotX;
		this.orderSizeX*=f;
		this.orderSizeY*=f;
	}


	var curHeight=this.sectorSizeY+this.orderSizeY;

	var newPage='';
	newPage+='<div style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll;">';

	newPage+='<div>';
	newPage+='<canvas id="myUnitCanvas" width="'+(divSize.x-32)+'" height="'+this.sectorSizeY+'"></canvas>';
	newPage+='<canvas id="myOrderCanvas" width="'+(divSize.x-32)+'" height="'+this.orderSizeY+'"></canvas>';
	newPage+='</div>';

	newPage+='<div style="text-align:left">';
	newPage+='<input class=empbutton id="upButton" type="button" value="up" onclick="rootDiv.mapSelection=rootDiv.mapSelection.parent; rootDiv.mapSetShowState(2)">';
	newPage+='<input class=empbutton id="moveButton" type="button" value="move" onclick="rootDiv.mapMoveOrder()">';
	newPage+='<input class=empbutton id="boardButton" type="button" value="go aboard   " onclick="rootDiv.mapGoAboard()">';
	newPage+='<input class=empbutton id="unselectButton" type="button" value="unselect" onclick="rootDiv.clearSelectionList(); rootDiv.mapSetShowState(2)"></br>';
	newPage+='<p>';
	newPage+='</div>';

	newPage+='<div>';
	newPage+=this.defineDivList({x:divSize.x-32, y:divSize.y-curHeight-20-32});
	newPage+='</div>';

	newPage+='</div>';

	return newPage;
}

EmpWinUnit.prototype.addEventListenersDiv=function()
{
	console.log("EmpWinUnit.addEventListenersDiv:");


	DivBase.prototype.addEventListenersDiv.call(this, "myOrderCanvas");

	this.addEventListenersDivList();
}


EmpWinUnit.prototype.drawDiv=function()
{
	//console.log("EmpWinUnit.drawDiv:");


	this.drawUnitList(rootDiv.mapSelection);

	{
		var element=document.getElementById("myUnitCanvas");
		var context=element.getContext("2d");
	
		context.fillStyle="#E0E0E0";	
		context.fillRect(0, 0, element.width, element.height);
	
		this.orderList=[];
	
		var u=rootDiv.mapSelection;
	
		if (u!=null)
		{
	
			var str=u.selfToString();
	
	
			var str=u.selfToString();
			u.showSelfUnitContextXY(context, 0, 0, rootDiv.mapSectorWidth, rootDiv.mapSectorHeight);			
							
			context.font = '10pt Calibri';
			context.fillStyle = 'black';
			context.fillText(str, this.sectorSizeX+8, this.textOffsetY);
		}
	}
	
	if (u!=null)
	{
		// possible orders for the unit
		var element=document.getElementById("myOrderCanvas");
		var context=element.getContext("2d");

		// Needed to comment this out since it cause much flickering. TODO Fix this flickering in a better way.
		//context.fillStyle="#E0E0E0";	
		//context.fillRect(0, 0, element.width, element.height);

		var arg=this.getPossibleOrders();
		var c=0;
		var n=arg.length;
		//element.width=n*this.sectorSizeX+8;
		
		
		for (var i=0; i<n;i++)
		{
			var a=arg[i];
			//console.log("showPossibleOrder "+i+" "+a);
			if (rootDiv.mapSelection.orderIsRelevant(a))
			{
				this.orderList[c]=a;
				this.showPossibleOrder(context, c, a);
				c++;
			}
		}



	}
	else
	{
		var str="no unit selected";
		context.font = this.font;
		context.fillStyle = 'black';
		context.fillText(str, this.offsetX, this.stepY);

	}



	// show selected unit/sector etc
	this.parentWin.empWinMenu.mapUpdateUpperTextAreas();
}



EmpWinUnit.prototype.click=function(mouseUpPos)
{
	var x=mouseUpPos.x;
	var y=mouseUpPos.y;

	console.log("EmpWinUnit.click: "+x+" "+y);

	if (rootDiv.mapSelection!=null)
	{


		// This was a click in the order area

		var order= this.getOrder(x);
		console.log("order "+order);

		if (order == null)
		{
			console.log("did not find an order at "+x+" "+y);		
			// For now will use this as an "up" button since one is missing.
			rootDiv.mapSelection=rootDiv.mapSelection.parent;
			rootDiv.mapSetShowState(2);				
		}
		else if (order == "build")
		{
			rootDiv.mapSetShowState(4);
		}
		else if (order == "moveTo")
		{
			rootDiv.clearSelectionList();
			rootDiv.addSelection(rootDiv.mapSelection.id);
			rootDiv.mapMoveOrder();
			rootDiv.mapSetShowState(0);
		}
		else if (order == "cancelOrder")
		{
			rootDiv.mapCancelOrder();
			rootDiv.mapSetShowState(2);
		}
		else if (order == "goTo")
		{
			rootDiv.clearSelectionList();
			rootDiv.addSelection(rootDiv.mapSelection.id);
			rootDiv.mapGoAboard();
			rootDiv.mapSetShowState(0);
		}
		else
		{
			console.log("sending order "+order);
			rootDiv.mapSendOrderOne(order);
			rootDiv.mapSetShowState(0);
		}
	}
	else
	{
		console.log("no unit selected");
	}
}


