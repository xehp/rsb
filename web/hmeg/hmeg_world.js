// hmeg_world.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-21 by Henrik Bjorkman www.eit.se/hb



// subclass extends superclass
HmegWorld.prototype = Object.create(MirrorBase.prototype);
HmegWorld.prototype.constructor = HmegWorld;

function HmegWorld(world, parent, arg)
{	
	MirrorBase.call(this, world, parent, arg); // call super constructor
	console.log("HmegWorld: '"+arg+"'");	

	//this.eUnitTypeList=null;
	//this.eStatesList=null;
	//this.eTerrain=null;
	
	//this.playerAvatar=null;
}


HmegWorld.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);

	this.gameTime=wr.getAbsArg(6);
	this.gameSpeed=wr.getAbsArg(10);
	
	this.mirrorDb.empireWorld=this;

	//this.info=arg.slice(3);
	return wr.getArgLen();  // TODO remove this, return value is not used anyway
}

HmegWorld.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", empType="+this.empType;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}






