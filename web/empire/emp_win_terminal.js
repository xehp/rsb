// emp_win_terminal.js
// Copyright (C) 2016 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-03 by Henrik

EmpWinTerminal.prototype = Object.create(DivTerminal.prototype);
EmpWinTerminal.prototype.constructor = EmpWinTerminal;





function EmpWinTerminal(parentWin)
{
	DivTerminal.call(this, parentWin); // call super constructor
}


