// MirrorLong.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb




// subclass extends superclass
extend(MirrorLong, MirrorBase);

function MirrorLong(world, parent, arg)
{	
	this.value=0;
	MirrorBase.call(this, world, parent, arg); // call super constructor
}


MirrorLong.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);

	this.value=hlibRemoveQuotes(wr.readNext());

	//console.log("value "+this.value);

}


MirrorLong.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", value="+this.value;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}

