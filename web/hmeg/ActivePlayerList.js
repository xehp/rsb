// ActivePlayerList.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb


		



// subclass extends superclass
ActivePlayerList.prototype = Object.create(MirrorBase.prototype);
ActivePlayerList.prototype.constructor = ActivePlayerList;

function ActivePlayerList(world, parent, emType, arg)
{	
	MirrorBase.call(this, world, parent, emType, arg); // call super constructor
	
}

ActivePlayerList.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);

	//this.order=hlibRemoveQuotes(wr.readNext());
}


ActivePlayerList.prototype.selfToString=function()
{
	var str="name="+this.objName;

	//str+=", order="+this.order;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}



