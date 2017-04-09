// hemg_win_msg.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-19 by Henrik Bjorkman www.eit.se/hb

HmegWinMsg.prototype = Object.create(DivMsg.prototype);
HmegWinMsg.prototype.constructor = HmegWinMsg;





function HmegWinMsg(parentWin)
{
	DivMsg.call(this, parentWin); // call super constructor
}



HmegWinMsg.prototype.addEventListenersDiv=function()
{
	DivMsg.prototype.addEventListenersDiv.call(this);

	if (this.prevText == "")
	{
		this.addText("Welcome to Flame\n");
	}
}

HmegWinMsg.prototype.drawDiv = function()
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



