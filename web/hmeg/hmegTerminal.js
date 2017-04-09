// emp_win_terminal.js
// Copyright (C) 2016 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-03 by Henrik

HmegTerminal.prototype = Object.create(DivTerminal.prototype);
HmegTerminal.prototype.constructor = HmegTerminal;



function HmegTerminal(parentWin)
{
	DivTerminal.call(this, parentWin); // call super constructor
	//this.text="";
}


HmegTerminal.prototype.onkeypress=function(event)
{
	//console.log('keypress: ' + event.which);		
}




