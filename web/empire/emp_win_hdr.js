// emp_win_menu.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-01-11 by Henrik Bjorkman www.eit.se/hb

EmpWinHdr.prototype = Object.create(DivBase.prototype);
EmpWinHdr.prototype.constructor = EmpWinHdr;





function EmpWinHdr(parentWin)
{
	DivBase.call(this, parentWin); // call super constructor

	this.parentWin=parentWin;
}


// This updates some text fields in the menu/button area.
// Right text area is not to the right any longer.
EmpWinHdr.prototype.mapSetUpperTextArea1=function(msg)
{
	var e = document.getElementById("upperTextArea1");
	if (e != null)
	{
		e.value=hlibRemoveQuotes(msg);
		//e.scrollTop = e.scrollHeight;
	}

}




EmpWinHdr.prototype.defineDiv=function(divSize)
{
	var newPage='';

	// The upper area of the page	

	newPage+='<header>';	
	newPage+='<input type="text" id="upperTextArea1" class=emptext size="90" readOnly="yes"><br/>';
	newPage+='</header>';

	newPage+='<p>'
	
	


	
	return newPage;
}

EmpWinHdr.prototype.addEventListenersDiv=function()
{
	//this.addEventListenersDiv("myCanvas");
}

EmpWinHdr.prototype.drawDiv=function()
{
}

