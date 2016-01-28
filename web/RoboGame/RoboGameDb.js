// RoboGameDb.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-27 by Henrik Bjorkman www.eit.se/hb


// subclass extends superclass
RoboGameDb.prototype = Object.create(MirrorDb.prototype);
RoboGameDb.prototype.constructor = RoboGameDb;




function RoboGameDb()
{
	MirrorDb.call(this); // call super constructor
}




RoboGameDb.prototype.showRecursive = function()
{
	this.rootObj.showRecursive();	
}


RoboGameDb.prototype.getSector=function(pos)
{
	return this.terrain.children[pos];
}




RoboGameDb.prototype.showById=function()
{
	var len = this.byId.length
	console.log("RoboGameDb.showById "+len);
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






RoboGameDb.prototype.selfToString=function()
{
	return 'RoboGameDb';
}

