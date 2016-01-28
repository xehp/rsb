// emp_round_buffer.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb



// subclass extends superclass
EmpRoundBuffer.prototype = Object.create(EmpBase.prototype);
EmpRoundBuffer.prototype.constructor = EmpRoundBuffer;

function EmpRoundBuffer(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
	
	parent.eRoundBuffer=this;
}

EmpRoundBuffer.prototype.readSelf=function(arg)
{
	var n = EmpBase.prototype.readSelf.call(this, arg);

	this.head = parseInt(arg[n]);
	this.tail = parseInt(arg[n+1]);
	this.msgCount = parseInt(arg[n+2]);
	this.maxObjects = parseInt(arg[n+3]); // max number of messages stored in the round buffer, max index is: maxObjects*2-1

    //console.log("EmpRoundBuffer.prototype.readSelf "+arg);

	//this.info=arg.slice(3);
	
	return arg.length;
}

EmpRoundBuffer.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", head="+this.head;
	str+=", tail="+this.tail;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}




EmpRoundBuffer.prototype.showSelfOnTextArea=function(textAreaName)
{
	var nation=this.parent;
	var nationList=nation.parent;
	var nationNr=nationList.children.indexOf(nation);
	//console.log("nr "+rootDiv.mapNation+" "+nationNr);
	if (rootDiv.mapNation==nationNr)
	{

		this.textBoxClear(textAreaName);

		//var len = this.children.length;
		var j=this.head;
		for (var i=0; i<this.maxObjects; ++i)
		{
			if (j in this.children)
			{
				var c = this.children[j];
				//var str = c.selfToString();
				var str= c.objName+": "+c.order;
				this.textBoxAppend(textAreaName, str);
			}

			j++;
			if (j>=this.maxObjects*2)
			{
				j=0;
			}
		}
	}
}


EmpRoundBuffer.prototype.textBoxAppend=function(textAreaName, msg)
{
	var e = document.getElementById(textAreaName);

	e.value+=hlibRemoveQuotes(msg)+"\n";
	e.scrollTop = e.scrollHeight;
}


EmpRoundBuffer.prototype.textBoxClear=function(textAreaName)
{
	var e = document.getElementById(textAreaName);

	e.value="";
	e.scrollTop = e.scrollHeight;
}

