// emp_win_sector.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-31 by Henrik Bjorkman www.eit.se/hb


EmpWinStates.prototype = Object.create(DivMouse.prototype);
EmpWinStates.prototype.constructor = EmpWinStates;




function EmpWinStates(parentWin)
{
	DivMouse.call(this, parentWin); // call super constructor

	this.parentWin=parentWin;
}

EmpWinStates.prototype.defineDiv=function(divSize)
{
	var newPage='';

	newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll; float:right;">';

	// The central area of the page	
	newPage+='<div style="width:400px;height:460px;float:left;">';
	newPage+='<canvas id="myCanvas" width="'+(divSize.x-32)+'" height="'+(divSize.y-32)+'"></canvas>';
	newPage+='</div>';

	newPage+='</div>';

	return newPage;
}


EmpWinStates.prototype.addEventListenersDiv=function()
{
	DivMouse.prototype.addEventListenersDiv.call(this, "myCanvas");
}


EmpWinStates.prototype.isBitSet=function(str, bitNo)
{
    var i = Math.floor(bitNo/64);
    var j = Math.floor(bitNo%64);
    
    var h = hlibRemoveQuotes(str);
	var a = hlibSplitString(h);
	
	if (i in a)
	{
		var d=a[i];
		console.log("str="+str+", i="+i+", d="+d);
	
		var l = bigInt(d, 16);
		var m = bigInt(1).shiftLeft(j);
		var b = l.and(m);
	
		return !b.isZero();
	}
	return false;	
}

EmpWinStates.prototype.relation=function(s1, i2)
{
	if (s1.index == i2)
	{
		return " ";	
	}
	else if (this.isBitSet(s1.hostileToState, i2))
	{
		return "hostile";
	}
	else if (this.isBitSet(s1.allyList, i2))
	{
		return "friendly";
	}
	else
	{
		return "neutral";
	}
}



EmpWinStates.prototype.drawDiv=function()
{
	if (rootDiv.empDb!=null)
	{
		var esl=rootDiv.empDb.getEmpireWorld().getEmpireStatesList();
		
		var element=document.getElementById("myCanvas");

		element.height=esl.getNChildObjects()*rootDiv.mapSectorHeight;

		var context=element.getContext("2d");
		//context.clearRect(0, 0, element.width, element.height);
		context.fillStyle="#E0E0E0";	
		context.fillRect(0, 0, element.width, element.height);


		var y=rootDiv.mapSectorHeight*0.8;
		
		var cl=esl.children
		if (cl!=null)
		{
			var s1=cl[rootDiv.mapNation];
			var i;
			for(i in cl)
			{
				//console.log("EmpWinStates: for "+i+" "+cl[i].selfToString());
				context.font = '12pt Calibri';
				context.fillStyle = 'black';

				var s2 = cl[i];
				if (s2 instanceof EmpState)
				{
					context.fillText(s2.index, 2, y);
					context.fillText(this.relation(s1, s2.index), 42, y);
					context.fillText(this.relation(s2, s1.index), 122, y);
					//context.fillText(s2.getSavedMoneyUpdated(), 202, y);
					context.fillText(s2.stateMotto, 212, y);
				}
				else
				{
					context.fillText(s2.selfToString(), 2, y);
				}

				y+=rootDiv.mapSectorHeight;
			}
		}
		
	}
}

EmpWinStates.prototype.click=function(mouseUpPos)
{
	var n = Math.floor(mouseUpPos.y / rootDiv.mapSectorHeight);
	
	var esl=rootDiv.empDb.getEmpireWorld().getEmpireStatesList();
	var s1=esl.children[rootDiv.mapNation];
	
	console.log("EmpWinStates.click, x="+mouseUpPos.x+", y="+mouseUpPos.y+", n="+n+", relation="+ this.relation(s1, n)=="hostile");
	
	// TODO we should open a menu for relations with this state.
	// From that menu it should be possible to change relations and send messages.
	// But for now we just toggle the hostility flag
	//doSend("textMsg 'stateOrder "+n+"'");
	
	
	if (this.relation(s1, n)=="hostile")
	{
		doSend("consoleInput 'rmEnemy "+n+"'");
	}
	else
	{
		doSend("consoleInput 'addEnemy "+n+"'");
	}
}


