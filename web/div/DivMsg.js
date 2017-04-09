// hemg_win_msg.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-19 by Henrik Bjorkman www.eit.se/hb

DivMsg.prototype = Object.create(DivBase.prototype);
DivMsg.prototype.constructor = DivMsg;





function DivMsg(parentWin)
{
	DivBase.call(this, parentWin); // call super constructor

	// constants
	this.offsetY=36;

	this.textOffsetY=(this.sectorSizeY*3)/4;

	this.msgIndex = -1;
	this.tail = -1;
	this.prevText = "";
}




DivMsg.prototype.defineDiv = function(divSize)
{ 
	var newPage='';

	// The messages are shown here in the messages text area
	newPage+='<div style="width:'+divSize.x+'px; height:'+divSize.y+'px;float:right">';
	if (this.parentWin.mobileMode)
	{
		newPage+='<textarea id="msgTextArea" class=emptext readOnly="yes" onmouseup="rootDiv.backToPreviousSubWin()" style="width:'+divSize.x+'px; height:'+divSize.y+'px;"></textarea><br/>';
	}
	else
	{
		newPage+='<textarea id="msgTextArea" class=emptext readOnly="yes" style="width:'+divSize.x+'px; height:'+divSize.y+'px;"></textarea><br/>';
	}
	newPage+='</div>';

	return newPage;
}

DivMsg.prototype.addEventListenersDiv=function()
{
	console.log("addEventListenersDiv");
	this.element = document.getElementById("msgTextArea");
	this.textBoxClear();
}

DivMsg.prototype.drawDiv = function()
{
	var p = this.parentWin;

	// Find the round buffer used to send text lines from server
	var apr = p.getAvatarPlayerReferences();

	if (apr!=null)
	{
		var r = apr.getRoundBuffer();
		if (r!=null)
		{
			if (r.tail != this.tail)
			{
				// show messages from the round buffer
				this.tail = r.tail
				this.msgIndex = r.showRoundBufferOnTextArea(this, this.msgIndex);
			}
		}
		else
		{
			console.log("did not find RoundBuffer for ~"+p.avatarId);
		}
	}
	else
	{
		console.log('no avatarPlayerReferences');
	}
}




DivMsg.prototype.addText=function(msg)
{
	console.log(msg);

	var e = this.element;
	e.value+=msg;
	e.scrollTop = e.scrollHeight;
	this.prevText += msg;
}




// not just clear, we add some old text also if any
DivMsg.prototype.textBoxClear=function()
{
	console.log("textBoxClear");

	var e = this.element;
	e.value=this.prevText;
	e.scrollTop = e.scrollHeight;
}


