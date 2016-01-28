// hmeg_win.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-19 by Henrik Bjorkman www.eit.se/hb



TextAdventureWin.prototype = Object.create(DivBase.prototype);
TextAdventureWin.prototype.constructor = TextAdventureWin;









function TextAdventureWin(headingText)
{
	DivBase.call(this, null); // call super constructor


	this.headingText=headingText;

	console.log('TextAdventureWin');
}
	


// This creates the main web page for the game
TextAdventureWin.prototype.defineDiv=function()
{
	var scrollbarSize=12;

	document.title = this.headingText + ' - ' + clientVersion;

	console.log('defineDiv');

	var newPage='';

	newPage+='<section>';


	newPage+='<div>';
	newPage+='<textarea id="textMsgArea" class=emptext readOnly="yes"></textarea><br/>';
	newPage+='</div>';

	newPage+='<div>';
		newPage+='<input type="text" id="inputText" size="88" onchange="rootDiv.onInputText(\'inputText\')"><br/>'; // This is where text can be written by user
		newPage+='<input type="button" value=enter onclick="rootDiv.onInputText(\'inputText\')">';
		newPage+='<input type="button" value=cancel onclick="rootDiv.mapCanvasCancel()">';
	newPage+='</div>';

	
	newPage+='</section>';


	$("body").empty();
	$("body").append(newPage);
}

TextAdventureWin.prototype.onInputText=function(srcId)
{
	var theSrcTextBox = document.getElementById(srcId);
	var str=theSrcTextBox.value;
	doSend('textMsg "'+str+'"');
	theSrcTextBox.value="";
}


TextAdventureWin.prototype.defineAndDrawPage=function()
{
	this.defineDiv();
	this.addEventListenersDiv();
	this.drawDiv();
}	

TextAdventureWin.prototype.mapCanvasCancel=function() 
{
	$("p").empty();
	window.clearInterval(this.mapIntervalVariable)
	doSend('cancel');
	TextAdventureClose();
}


TextAdventureWin.prototype.addEventListenersDiv=function()
{
	this.element = document.getElementById("textMsgArea");

/*
	var t=this;
	document.onkeypress=function(event)
	{
		console.log('keypress: ' + event.which);
		
		switch(event.which)
		{
			case 101:
				console.log('keypress: inventory');
				
				t.subWin = new HmegWinInventory(t);
				t.defineAndDrawPage();
			default:
				if (event.which<32)
				{
					console.log('keypress: return');
				
					t.subWin = new HmegMapWin(t);
					t.defineAndDrawPage();

				}
				else
				{
				        websocket.send('keypress "'+event.which+'"');
				}
		        break;
		}
		
	}
*/
}

TextAdventureWin.prototype.drawDiv=function()
{
	this.addText("hello world");
}

	
TextAdventureWin.prototype.addText=function(str)
{
		// Remove " " from string
		str = str.substring(1);
		str = str.substring(0, str.length - 1);
		if (this.element!=null)
		{
			var e = this.element;
			e.value+=str;
			e.scrollTop = e.scrollHeight;
		}

}
	

TextAdventureWin.prototype.onMessageArg=function(arg)
{
	var i=0;
	var cmd=arg[0];
	var reply='';


	//console.log("onMessageArg: '" +arg+"'"); 

	// Here we check what server want client program to do.
	// Commands sent by ConnectionThread need to be interpreted here.


	if (cmd=="showWin")
	{
		this.defineAndDrawPage();
	}
	else if (cmd=="TextBoxAppend")
	{
		this.addText(arg[1]);
	}
	else if (cmd=="return")
	{
		TextAdventureClose();
	}
	else
	{
		console.log("onMessageArg: unknown command: '" + cmd+"'"); 
	}	
}



