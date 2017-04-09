// emp_unit.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb, All rights reserved
// Created 2014-03-29 by Henrik Bjorkman


		



// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/create


// subclass extends superclass
EmpUnit.prototype = Object.create(EmpBase.prototype);
EmpUnit.prototype.constructor = EmpUnit;



function EmpUnit(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
}

EmpUnit.prototype.readSelf=function(wr)
{
	EmpBase.prototype.readSelf.call(this, wr);
	

	this.unitOwner=parseInt(wr.readNext());
	this.unitTime=parseInt(wr.readNext());
	this.unitHealth=parseInt(wr.readNext());
	this.unitType=parseInt(wr.readNext());
	wr.skip(1); //this.attackPoints=parseInt(wr.readNext());
	this.movePoints=parseInt(wr.readNext());
}

EmpUnit.prototype.getMovePointsUpdated=function()
{
	var empireWorld=this.mirrorDb.empireWorld;
	
	var deltaTime = empireWorld.getGameTime() - this.unitTime;

	var mp = this.movePoints + deltaTime;
	
	if (mp>100)
	{
		mp=100;
	}

	return mp;
}

EmpUnit.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", id="+this.id;
	str+=", type="+this.mirrorDb.unitTypesList.children[this.unitType].objName;
	str+=", owner="+this.unitOwner;
	str+=", unitHealth="+this.unitHealth;
	str+=", movePoints="+this.getMovePointsUpdated();

	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}


EmpUnit.prototype.showSelfUnitContextXY=function(context, x, y, width, height)
{
	var t=this.getUnitType();
	if (t!=null)
	{
		t.showSelfContextXY(context, x, y, width, height);
	}
	else
	{
		context.font = '8pt Calibri';
		context.fillStyle = 'black';
		context.fillText(this.unitType, x, y+8);
	}
}



EmpUnit.prototype.getMass=function()
{
	var t=this.getUnitType();
	if (t!=null)
	{
		return t.unitMass;
	}
    else
    {
        console.log("did not find mass for '" + t+"'");
    }
	return 0;
}









