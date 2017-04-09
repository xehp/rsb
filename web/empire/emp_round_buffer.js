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

EmpRoundBuffer.prototype.readSelf=function(wr)
{
	EmpBase.prototype.readSelf.call(this, wr);

	this.head = parseInt(wr.readNext());
	this.tail = parseInt(wr.readNext());
	this.msgCount = parseInt(wr.readNext());
	this.maxObjects = parseInt(wr.readNext()); // max number of messages stored in the round buffer, max index is: maxObjects*2-1

    //console.log("EmpRoundBuffer.prototype.readSelf "+arg);

	//this.info=arg.slice(3);
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




EmpRoundBuffer.prototype.showSelfOnTextArea=function(div, msgCount)
{
	var nation=this.parent;
	var nationList=nation.parent;
	var nationNr=nationList.children.indexOf(nation);
	//console.log("nr "+rootDiv.mapNation+" "+nationNr);
	if (rootDiv.mapNation==nationNr)
	{
		//var len = this.children.length;
		if (msgCount<0)
		{
			msgCount=this.head;
		}
		
		for (var i=0; i<=this.maxObjects; ++i)
		{
			if (msgCount in this.children)
			{
				var c = this.children[msgCount];
				//var str = c.selfToString();
				//var str= c.objName+": "+c.order+"\n";
				var str= c.order+"\n";
				div.addText(str);
			}

			msgCount++;
			if (msgCount>=this.maxObjects*2) // indexes can be twice that of max objects
			{
				msgCount=0;
			}
		}
	}
	return this.tail;
}


