// emp_win_textinput.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb

EmpWinTextinput.prototype = Object.create(DivBase.prototype);
EmpWinTextinput.prototype.constructor = EmpWinTextinput;




function EmpWinTextinput(parentWin)
{
	DivBase.call(this, parentWin); // call super constructor
	this.count=0;
}

EmpWinTextinput.prototype.defineDiv=function(divSize)
{
	var newPage='';

	// The central area of the page	
	newPage+='<div style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll;">';

	if (!rootDiv.mobileMode)
	{	
		newPage+='<input type="text" id="inputText" onchange="rootDiv.onInputText(\'inputText\')"><br/>'; // This is where text can be written by user
		newPage+='<input type="button" value=enter onclick="rootDiv.onInputText(\'inputText\')">';
		newPage+='<input type="button" value=cancel onclick="rootDiv.mapCanvasCancel()">';
		newPage+='<input type="button" value=back onclick="rootDiv.mapSetShowState(0)"><br>';
	}
	else
	{
		newPage+='<label for="info">'+'Enter text'+'</label>';
		newPage+='<textarea name="textArea" id="inputText" onchange="rootDiv.onInputText(\'inputText\')"></textarea><br/>';
		newPage+='<input type="button" value=enter onclick="rootDiv.onInputText(\'inputText\')">';
		newPage+='<input type="button" value=cancel onclick="rootDiv.mapCanvasCancel()">';
		newPage+='<input type="button" value=back onclick="rootDiv.mapSetShowState(0)"><br>';
	}
	
	newPage+='</div>';



	return newPage;
}

EmpWinTextinput.prototype.addEventListenersDiv=function()
{
        document.getElementById("inputText").focus();
}

EmpWinTextinput.prototype.drawDiv=function()
{
}


EmpWinTextinput.prototype.click=function(mouseUpPos)
{
	console.log("not implemented yet");
}

/*
EmpWinTextinput.prototype.empConsoleBoxAppend=function(msg)
{
	var e = document.getElementById("consoleArea");
	if (e!=null)
	{
		e.value+=hlibRemoveQuotes(msg)+"\n";
		e.scrollTop = e.scrollHeight;
	}
	else
	{
		console.log(msg);
	}
}
*/

