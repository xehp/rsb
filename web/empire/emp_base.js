// emp_base.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb, All rights reserved
// Created 2014-12-26 by Henrik Bjorkman

// subclass extends superclass
EmpBase.prototype = Object.create(MirrorBase.prototype);
EmpBase.prototype.constructor = EmpBase;





function EmpBase(empDb, parent, emType, arg)
{	
	MirrorBase.call(this, empDb, parent, arg); // call super constructor

	this.empType=emType;

	//this.readSelf(arg);

	this.setQuickLinks(arg);	
}



EmpBase.prototype.setQuickLinks=function(arg)
{
}




EmpBase.prototype.getNameOrIndex=function()
{
	if ((this.objName!=='undefined') && (this.objName.length>0))
	{
		return this.objName;
	}
	return ""+this.index;
}


EmpBase.prototype.getPathName=function()
{
	if (this.parent!=null)
	{
		return this.parent.getPathName()+"."+this.getNameOrIndex();
	}
	return this.getNameOrIndex();
}


EmpBase.prototype.getSectorIndexFromXY=function(x,y)
{
	return x+this.sizeX*y;
}


EmpBase.prototype.translateSectorIndexToColumn=function(index)
{
	return index % this.sizeX
}


EmpBase.prototype.translateSectorIndexToRow=function(index)
{
	return  Math.floor(index/this.sizeX);
}


EmpBase.prototype.fromXYToRowCol=function(xyPos)
{
	var ny = Math.floor(xyPos.y/rootDiv.mapSectorHeight);
	var nx = xyPos.x/rootDiv.mapSectorWidth;

	if (ny<0)
	{
		console.log("EmpBase.fromXYToRowCol nx="+nx);
	}

	if ((ny%2)!=0)
	{
		nx -= 0.5;
	}

	if (nx<0)
	{
		var w=rootDiv.empDb.getEmpireWorld();
		var t=w.getEmpireTerrain();
		console.log("EmpBase.fromXYToRowCol nx="+nx+", sizeX="+t.sizeX);
		nx+=t.sizeX;
	}

	nx = Math.floor(nx);

	return {
		col: nx,
		row: ny
	};
}


EmpBase.prototype.fromXYToPos=function(xyPos)
{
	var rc=this.fromXYToRowCol(xyPos);
	return rc.col + rc.row * this.sizeX;
}



EmpBase.prototype.orderIsRelevant=function(order)
{
	if ((order=='unload') || (order=='cancelOrder'))
	{
		if (this.getNChildObjects()==0)
		{
			return false;
		}
	}
	return true;
}


EmpBase.prototype.getSectorPosRecursive=function()
{
	if (this.empType=="EmpireSector")
	{
		return this.index;
	}
	else if (this.parent!=null)
	{
		return this.parent.getSectorPosRecursive();
	}
	return -1;
}

EmpBase.prototype.selfToString=function()
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


EmpBase.prototype.selfToDebugString=function()
{
	var str=selfToString();
	var pn=this.getPathName();
	str+=", id="+this.id;
	str+=", pn="+pn;
	str+=", idx="+this.index;
	str+=", empType="+this.empType;
	if (this.empType=="EmpireSector")
	{
		str+=", terrain="+this.sectorTerrain;
	}
	else if (this.empType=="EmpireUnit")
	{
		str+=", ut="+this.unitType;
		str+=", pos="+this.getSectorPosRecursive();
	}
	return str;
}

// Gives the object that describes the type of unit for this unit.
EmpBase.prototype.getUnitType=function()
{
	if (this.unitType in this.mirrorDb.unitTypesList.children)
	{
		return this.mirrorDb.unitTypesList.children[this.unitType];
	}
	return null;
}

// Gives the type name for this object (not its own name) 
EmpBase.prototype.getUnitTypeName=function()
{
	var t=this.getUnitType();
	if (t!=null)
	{
		return t.objName;
	}
	return 'unknown';
}




// Returns:
// >=0 : If only on nation have units in the sector
// -1  : If nobody have units in the sector
// -2  : If more than one nation have units in the sector
EmpBase.prototype.getSectorOwner=function()
{
	var owner=-1;
	// If there are sub units (aka children) then loop all and check for owners
	if (this.children!=null)
	{
		var len = this.children.length
		for (var i=0; i<len; ++i)
		{
			if (i in this.children)
			{
				c=this.children[i];
				if (typeof c.unitOwner !== 'undefined')
				{
					var o=c.unitOwner;
					if (owner != o)
					{
						if (owner==-1)
						{
							owner=o;
						}
						else if (owner>=0)
						{
							owner=-2;
						}
					}
				}
			}
		}
	}
	return owner;
}

// Returns:
//  0  : If nobody own the sector
//  1  : State 'state' own this sector (alone) 
//  2  : State 'state' have no units in the sector but other states do.
//  3  : State 'state' have units in the sector and other states too.
EmpBase.prototype.getSectorOwnerState=function(state)
{
	var ownerState=0;
	// If there are sub units (AKA children) then loop all and check for owners
	if (this.children!=null)
	{
		for (var i in this.children)
		{
			var c=this.children[i];
			if (typeof c.unitOwner !== 'undefined')
			{
				if (c.unitOwner==state)
				{
					ownerState|=1;
				}
				else if (c.unitOwner>=0)
				{
					ownerState|=2;
				}
			}
		}
	}
	return ownerState;
}







// this is only for debugging, can be removed later
EmpBase.prototype.showSelf=function()
{
	var str=this.selfToString();
	//chatRoomTextBoxAppend(str);
	console.log(str);
}


// this is only for debugging, can be removed later
EmpBase.prototype.showRecursive=function()
{
	this.showSelf();

	if (this.children!=null)
	{
		// http://stackoverflow.com/questions/3010840/loop-through-array-in-javascript
		var len = this.children.length
		for (var i=0; i<len; ++i) 
		{
			if (i in this.children) 
			{
				var c = this.children[i];
				if (c!=null)
				{
					if (c.index!=i)
					{
						console.log("showRecursive: inconsistent index "+c.index+" "+i+" "+c.selfToString());
					}

					//console.log("empWorldInfo: child '"+c.getPathName()+"'");				
					c.showRecursive();
				}
			}
		}
	}
}



// is this still used?
EmpBase.prototype.getNext=function(current)
{
	if (this.children!=null)
	{
		var i=0;
		if (current!=null)
		{
			i=current.index+1;
		}
		var len = this.children.length;
		while (i<len)
		{
			if (i in this.children) 
			{
				var c = this.children[i];
				return c;
			}
			i++;
		}
	}
	return null;
}


EmpBase.prototype.readSelf=function(arg)
{
	return MirrorBase.prototype.readSelf.call(this, arg);
}

