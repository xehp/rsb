// emp_money.js
// Copyright (C) 2016 Henrik Bjorkman www.eit.se/hb
// Created 2016-07-23 by Henrik Bjorkman www.eit.se/hb


		



// subclass extends superclass
EmpMoney.prototype = Object.create(EmpBase.prototype);
EmpMoney.prototype.constructor = EmpMoney;

function EmpMoney(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
	
}


EmpMoney.prototype.readSelf=function(wr)
{
	EmpBase.prototype.readSelf.call(this, wr);

	this.value=hlibRemoveQuotes(wr.readNext());
}


EmpMoney.prototype.selfToString=function()
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


