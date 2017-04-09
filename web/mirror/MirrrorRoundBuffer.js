// MirrorRoundBuffer.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb



// subclass extends superclass
extend(MirrorRoundBuffer, MirrorBase);


function MirrorRoundBuffer(world, parent, emType, arg)
{	
	MirrorBase.call(this, world, parent, emType, arg); // call super constructor
	
	parent.eRoundBuffer=this;
}

MirrorRoundBuffer.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);

	this.head = parseInt(wr.readNext());
	this.tail = parseInt(wr.readNext());
	this.msgCount = parseInt(wr.readNext());
	this.maxObjects = parseInt(wr.readNext()); // max number of messages stored in the round buffer, max index is: maxObjects*2-1

    //console.log("MirrorRoundBuffer.prototype.readSelf "+arg);

	//this.info=arg.slice(3);


	return wr.getArgLen(); // TODO remove this, return value is not used anyway
}

MirrorRoundBuffer.prototype.selfToString=function()
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


MirrorRoundBuffer.prototype.showRoundBufferOnTextArea=function(textDiv, msgIndex)
{
	//console.log('showRoundBufferOnTextArea');


	if (msgIndex<0)
	{
		msgIndex=this.head;
	}

	for (var i=0; i<=this.maxObjects; ++i)
	{
		if (msgIndex in this.children)
		{
			var c = this.children[msgIndex];
			var str= c.objName+": "+c.order+"\n";
			textDiv.addText(str);
		}

		msgIndex++;
		if (msgIndex>=this.maxObjects*2)
		{
			msgIndex=0;
		}
	}
	return this.tail;;
}






