// emp_win_base.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-31 by Henrik Bjorkman www.eit.se/hb


var rootDiv=null;




function DivBase(parentWin)
{	
	this.parentWin=parentWin;
}

// Returns the html code needed to show the div.
DivBase.prototype.defineDiv=function(divSize)
{
	console.log('defineDiv not implemented '+divSize.x+" "+divSize.y);
}


DivBase.prototype.drawDiv=function()
{
	console.log('drawDiv not implemented');
}

DivBase.prototype.click=function(mouseUpPos)
{
	console.log('click not implemented');
}


DivBase.prototype.drag=function(mouseDownPos, mouseUpPos)
{
	console.log('drag not implemented');
}


// This registers the code that will handle various events.
// TODO: Perhaps startDiv would be a better name.
DivBase.prototype.addEventListenersDiv=function()
{
	console.log('addEventListenersDiv not implemented');
}



DivBase.prototype.getRootDiv=function()
{
	/*
	if (this.parentWin!=null)
	{
		return this.getRootDiv();
	}
	return this;
	*/
	return rootDiv;
}


