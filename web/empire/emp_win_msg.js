// emp_win_msg.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb

EmpWinMsg.prototype = Object.create(DivBase.prototype);
EmpWinMsg.prototype.constructor = EmpWinMsg;





function EmpWinMsg(parentWin)
{
	DivBase.call(this, parentWin); // call super constructor

	// constants
	this.offsetY=36;

	this.sectorSizeX=rootDiv.mapSectorWidth;
	this.sectorSizeY=rootDiv.mapSectorHeight;
	this.textOffsetY=(this.sectorSizeY*3)/4;
	
	this.parentWin=parentWin;

	this.orderList=[]; // is this used
}




EmpWinMsg.prototype.defineArea = function(divSize)
{ 
	var newPage='';

	// The messages are shown here in the messages text area
	newPage+='<div style="width:'+divSize.x+'px; height:'+divSize.y+'px;">';
	if (this.parentWin.mobileMode)
	{
		newPage+='<textarea id="msgTextArea" class=emptext readOnly="yes" onmouseup="rootDiv.mapSetShowState(6)" style="width:'+divSize.x+'px; height:'+divSize.y+'px;"></textarea><br/>';
	}
	else
	{
		newPage+='<textarea id="msgTextArea" class=emptext readOnly="yes" style="width:'+divSize.x+'px; height:'+divSize.y+'px;"></textarea><br/>';
	}
	newPage+='</div>';

	return newPage;
}

EmpWinMsg.prototype.drawWin = function()
{
	// show messages from the round buffer
	var w=rootDiv.empDb.getEmpireWorld();
	if (w!=null)
	{		
		var list=w.getEmpireStatesList();
		if (list!=null)
		{
			var cl=list.children;
			if (cl!=null)
			{
				var n=cl[rootDiv.mapNation];
				if (n!=null)
				{
					var r=n.eRoundBuffer;
					if (r!=null)
					{
						r.showSelfOnTextArea("msgTextArea");
					}
				}
				else
				{
					console.log('no nation '+rootDiv.mapNation);
				}
			}
		}	
	}
}

/*
EmpRoundBuffer.prototype.textBoxAppend=function(textAreaName, msg)
{
	var e = document.getElementById(textAreaName);

	e.value+=hlibRemoveQuotes(msg)+"\n";
	e.scrollTop = e.scrollHeight;
}
*/
