// emp_win_terminal.js
// Copyright (C) 2016 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-03 by Henrik

EmpWinTerminal.prototype = Object.create(DivTerminal.prototype);
EmpWinTerminal.prototype.constructor = EmpWinTerminal;





function EmpWinTerminal(parentWin)
{
	DivTerminal.call(this, parentWin); // call super constructor
}


EmpWinTerminal.prototype.addEventListenersDiv=function()
{
	this.element = document.getElementById("consoleArea");

	this.element.value+=this.text;


	if (rootDiv.empDb!=null)
	{
		var cl=rootDiv.empDb.getEmpireWorld().getEmpireStatesList().children;
		var n=cl[rootDiv.mapNation];	

		if (n!=null) 
		{
			var r=n.eRoundBuffer;
	
			if (r!=null)
			{
				if (this.count==null)
				{
					this.count=r.head;
				}
	
				// Add latest messages also to console text area
				// Is this usefull? Perhaps we should keep messages only in the message text area
				var i=0; // this variable is just to avoid eternal loop if something is wrong 
				while((this.count!=r.tail) && (i<r.maxObjects)) 
				{
					if (this.count in r.children)
					{
						var c = r.children[this.count];
						var str= c.objName+": "+c.order;
						this.textBoxAppend(str);
					}
			
					this.count++;
					if (this.count>=r.maxObjects*2)
					{
						this.count=0;
					}
					++i;
				}
			}
		}
	}
}

