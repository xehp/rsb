// emp_world.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-03-29 by Henrik Bjorkman www.eit.se/hb



// subclass extends superclass
EmpWorld.prototype = Object.create(EmpBase.prototype);
EmpWorld.prototype.constructor = EmpWorld;

function EmpWorld(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
	console.log("EmpWorld: '"+this.arg+"'");	

	this.eUnitTypeList;
	this.eStatesList=null;
	this.eTerrain=null;
	this.gameTimeObj=null;
	
}


EmpWorld.prototype.readSelf=function(wr)
{
	EmpBase.prototype.readSelf.call(this, wr);


	//this.gameTime=arg[6];
	this.gameSpeed=wr.getAbsArg(10);
	
	this.mirrorDb.empireWorld=this;

	//this.info=arg.slice(3);

}

EmpWorld.prototype.selfToString=function()
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



EmpWorld.prototype.getEmpUnitTypeList=function()
{
	// Following code probably is not needed.
	/*if (this.eUnitTypeList==null)
	{
		if (this.children!=null)
		{
			var i;
			for(i in this.children)
			{
				if (this.children[i] instanceof EmpUnitTypeList)
				{
					this.eUnitTypeList=this.children[i];
				}
			}
		}
	}*/	

	return this.eUnitTypeList;
}


EmpWorld.prototype.getEmpireStatesList=function()
{
	// Following code probably is not needed.
	/*if (this.eStatesList==null)
	{
		if (this.children!=null)
		{
			var i;
			for(i in this.children)
			{
				if (this.children[i] instanceof EmpStatesList)
				{
					this.eStatesList=this.children[i];
				}
			}
		}
	}*/	

	return this.eStatesList;
}


EmpWorld.prototype.getEmpireTerrain=function()
{
	// Following code probably is not needed.
	if (this.eTerrain==null)
	{
		if (this.children!=null)
		{
			var i;
			for(i in this.children)
			{
				if (this.children[i] instanceof EmpTerrain)
				{
					this.eTerrain=this.children[i];
				}
			}
		}
	}	

	return this.eTerrain;
}

EmpWorld.prototype.getState=function()
{
	var list=this.getEmpireStatesList();
	if ((list==null) || (list.children==null))
	{
		return null;
	}
	return list.children[rootDiv.mapNation];
}


EmpWorld.prototype.getGameTimeObj=function()
{
	if (this.gameTimeObj==null)
	{
		this.gameTimeObj=this.findSubObjectByName("_gameTime");
		if (this.gameTimeObj!=null)
		{
			console.log("getGameTime "+this.gameTimeObj.objName+ " "+this.gameTimeObj.value);
		}
	}

	return this.gameTimeObj;
}

EmpWorld.prototype.getGameTime=function()
{
	return this.getGameTimeObj().value;
}
