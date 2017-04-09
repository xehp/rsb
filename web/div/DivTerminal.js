// emp_win_terminal.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb

DivTerminal.prototype = Object.create(DivBase.prototype);
DivTerminal.prototype.constructor = DivTerminal;




function DivTerminal(parentWin)
{
	DivBase.call(this, parentWin); // call super constructor
	this.count=null;
	this.prevText='';
	this.element=null;
	
	this.changeConter=0;
	this.previousChangeConter=0;
}

DivTerminal.prototype.defineDiv=function(divSize)
{
	var newPage='';

	// This div is intended for the central area of the page	
	newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px; overflow-x: scroll; overflow-y: scroll; float:right;">';

	
	// The console text output area (multiple lines where resulting text is shown).
	newPage+='<textarea id="consoleArea" class=emptext rows=' + (divSize.y/16 - 2) + ' readOnly="yes"></textarea><br/>'; // This creates the console text area
	
	if (!this.parentWin.mobileMode)
	{
		// The console text input area (the single line where text is entered).
		newPage+='<input type="text" id="inputText" size="88" onchange="rootDiv.onInputText(\'inputText\')"><br/>'; // This is where text can be written by user
		newPage+='<input type="button" value=enter onclick="rootDiv.onInputText(\'inputText\')">';
		newPage+='<input type="button" value=cancel onclick="rootDiv.mapCanvasCancel()">';
		newPage+='<input type="button" value=back onclick="rootDiv.subWin.back();"><br>';
	}


	
	newPage+='</div>';



	return newPage;
}


DivTerminal.prototype.addEventListenersDiv=function()
{
	this.element = document.getElementById("consoleArea");
	this.textBoxClear();
}

DivTerminal.prototype.drawDiv=function()
{
	this.element = document.getElementById("consoleArea");

	if (this.previousChangeConter!=this.changeConter)
	{
		var e = this.element;
		if (e == null)
		{
			// Did we forget to call addEventListenersDiv? Will make a quick and dirty here instead of debugging.
			this.element = document.getElementById("consoleArea");
			e = this.element;
			if (e == null)
			{
				return;
			}
		}

		e.scrollTop = e.scrollHeight;
		this.previousChangeConter=this.changeConter;
	}
}


DivTerminal.prototype.click=function(mouseUpPos)
{
	console.log("not implemented yet");
}

DivTerminal.prototype.back=function()
{
	console.log("back");

	this.prevText=this.element.value+"---\n";
	this.element=null;
	rootDiv.backToPreviousSubWin();
}

DivTerminal.prototype.addText=function(msg)
{
	//this.element = document.getElementById("consoleArea");

	//var str=hlibRemoveQuotes(msg)+"\n";

	if (this.prevText!=null)
	{
		this.prevText += msg;
	}

	var e = this.element;
	if (e!=null)
	{
		e.value+=msg;
		e.scrollTop = e.scrollHeight;
		//console.log("scrollHeight: "+e.scrollHeight);
	}

	this.changeConter++;
}


// not just clear, we add some old text also if any
DivTerminal.prototype.textBoxClear=function()
{
	var e = this.element;

	e.value=this.prevText; 
	e.scrollTop = e.scrollHeight;
}
