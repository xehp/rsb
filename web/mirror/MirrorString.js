// MirrorString.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb


		



// subclass extends superclass
MirrorString.prototype = Object.create(MirrorBase.prototype);
MirrorString.prototype.constructor = MirrorString;

function MirrorString(world, parent, emType, arg)
{	
	MirrorBase.call(this, world, parent, emType, arg); // call super constructor
	
}

MirrorString.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);

	this.order=hlibRemoveQuotes(wr.readNext());
}


MirrorString.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", order="+this.order;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}



