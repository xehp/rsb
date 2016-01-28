// emp_unit_type.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb


// subclass extends superclass
EmpUnitType.prototype = Object.create(EmpBase.prototype);
EmpUnitType.prototype.constructor = EmpUnitType;

function EmpUnitType(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
}

EmpUnitType.prototype.readSelf=function(arg)
{
	var n = EmpBase.prototype.readSelf.call(this, arg);

	

	this.unitMass=parseInt(arg[n+6]);
	this.possibleOrders=hlibRemoveQuotes(arg[n+19]);
	this.possibleBuilds=hlibRemoveQuotes(arg[n+20]);

	//this.info=arg.slice(3);


	this.empImg=new EmpImg(this.objName);


	return arg.length;
}


EmpUnitType.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", possibleOrders="+this.possibleOrders;
	str+=", possibleBuilds="+this.possibleBuilds;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}


EmpUnitType.prototype.showSelfContextXY=function(context, x, y, width, height)
{
	this.empImg.showSelfContextXY(context, x, y, width, height);
}



