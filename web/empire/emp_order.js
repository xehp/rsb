// emp_order.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb


		



// subclass extends superclass
EmpOrder.prototype = Object.create(EmpBase.prototype);
EmpOrder.prototype.constructor = EmpOrder;

function EmpOrder(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
	
}

EmpOrder.prototype.readSelf=function(arg)
{
	var n = EmpBase.prototype.readSelf.call(this, arg);

	this.order=hlibRemoveQuotes(arg[n]);
	
	return arg.length;
}


EmpOrder.prototype.selfToString=function()
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


