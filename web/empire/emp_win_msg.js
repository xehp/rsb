// emp_win_msg.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb

EmpWinMsg.prototype = Object.create(DivMsg.prototype);
EmpWinMsg.prototype.constructor = EmpWinMsg;





function EmpWinMsg(parentWin)
{
	DivMsg.call(this, parentWin); // call super constructor
}




EmpWinMsg.prototype.defineDiv = function(divSize)
{ 
	var newPage='';

	// The messages are shown here in the messages text area
	newPage+='<div style="width:'+divSize.x+'px; height:'+divSize.y+'px;float:right">';
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


EmpWinMsg.prototype.addEventListenersDiv=function()
{
	DivMsg.prototype.addEventListenersDiv.call(this);
	
	if (this.prevText == "")
	{
		this.addText("Welcome to empire\n");
	}
}


EmpWinMsg.prototype.drawDiv = function()
{

	// Find the round buffer used to send text lines from server
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
						// show messages from the round buffer
						this.msgIndex = r.showSelfOnTextArea(this, this.msgIndex);
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

