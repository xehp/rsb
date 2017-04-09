// emp_state.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb





// subclass extends superclass
EmpState.prototype = Object.create(EmpBase.prototype);
EmpState.prototype.constructor = EmpState;

function EmpState(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
	
	this.eRoundBuffer=null;
	this.money=null;
}

EmpState.prototype.readSelf=function(wr)
{
	EmpBase.prototype.readSelf.call(this, wr);

	this.sectorOwner=parseInt(wr.readNext());
	this.unitTime=parseInt(wr.readNext());

	this.moneyEarned = parseInt(wr.readNext());
	this.moneySpent = parseInt(wr.readNext());
	this.headOfState = wr.readNext();
	this.allyList = wr.readNext();
	this.homeSectorId = parseInt(wr.readNext());
	this.stateMode = parseInt(wr.readNext());
	this.coRuler = wr.readNext();
	this.hostileToState = wr.readNext();
	this.stateMotto = wr.readNext();
}

EmpState.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", money="+this.savedMoney;

	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}


EmpState.prototype.getMoneyObj=function()
{
	if (this.money==null)
	{
		this.money=this.findSubObjectByName("_empireMoney");
		if (this.money!=null)
		{
			console.log("getMoneyObj "+this.money.objName+ " "+this.money.value);
		}
	}

	return this.money;
}

EmpState.prototype.getSavedMoneyUpdated=function()
{
	var o=this.getMoneyObj();
	var m=0;
	
	if (o!=null)
	{
		m = o.value;
	}

	/*
	var empireWorld=this.mirrorDb.empireWorld;
	
	var deltaTime = empireWorld.getGameTime() - this.unitTime;

	var m = this.savedMoney + (deltaTime*this.moneyChange);
	*/

	//console.log("state "+ this.objName+ " money "+m);

	return m;
}





