// emp_world.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-27 by Henrik Bjorkman www.eit.se/hb


// subclass extends superclass
EmpDb.prototype = Object.create(MirrorDb.prototype);
EmpDb.prototype.constructor = EmpDb;




function EmpDb()
{
	MirrorDb.call(this); // call super constructor

	this.terrain=null;       // Todo: replace use of this with getEmpireWorld().getEmpireTerrain()
	this.unitTypesList=null; // Todo: replace use of this with getEmpireWorld().getEmpUnitTypeList()
	this.nationList=null;  	// Todo: replace use of this with getEmpireWorld().getEmpireStatesList()
	this.empireWorld=null; // Reference to the EmpWorld object
}




EmpDb.prototype.showRecursive = function()
{
	this.rootObj.showRecursive();	
}


EmpDb.prototype.getSector=function(pos)
{
	return this.terrain.children[pos];
}




EmpDb.prototype.showById=function()
{
	var len = this.byId.length
	console.log("EmpDb.showById "+len);
	for (var i=0; i<len; ++i) 
	{
		if (i in this.byId) 
		{
			var c = this.byId[i];
			if (c.id!=i)
			{
				console.log("showById: inconsistent id "+c.id+" "+i);
			}				
			c.showSelf();
		}
	}
}







EmpDb.prototype.getEmpireWorld=function()
{
	return this.empireWorld;
}


EmpDb.prototype.selfToString=function()
{
	return 'EmpDb';
}
