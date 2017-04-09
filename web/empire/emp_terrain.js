// emp_terrain.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb



// subclass extends superclass
EmpTerrain.prototype = Object.create(EmpBase.prototype);
EmpTerrain.prototype.constructor = EmpTerrain;

function EmpTerrain(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor
	
	parent.eTerrain=this;
}

EmpTerrain.prototype.readSelf=function(wr)
{
	EmpBase.prototype.readSelf.call(this, wr);

	this.mirrorDb.terrain=this;
	this.sizeX=parseInt(wr.readNext());
	this.sizeY=parseInt(wr.readNext());
}

// deprecated, to be removed when no longer used.
/*
EmpTerrain.prototype.showSelfContext=function(context)
{
	this.showSelfTerrainContext(context);
}
*/

// deprecated, to be removed when no longer used.
EmpTerrain.prototype.showSelfTerrainContext=function(context)
{
	var len = this.children.length
	for (var i=0; i<len; ++i)
	{
		if (i in this.children)
		{
			var c=this.translateSectorIndexToColumn(i);
			var r=this.translateSectorIndexToRow(i);

			var offset=rootDiv.mapCalcOffSet(r);
			this.children[i].showSelfAndSubUnitContext(context, offset+c*rootDiv.mapSectorWidth, r*rootDiv.mapSectorHeight, rootDiv.mapSectorWidth, rootDiv.mapSectorHeight);
		}
	}
}


EmpTerrain.prototype.getSectorToRight=function(idx)
{
	var x=idx%this.sizeX;
	var y=Math.floor(idx/this.sizeX);
	if (x>=this.sizeX-1)
	{
		x=0;
	}
	else
	{
		x++;
	}
	return x+y*this.sizeX;
}

EmpTerrain.prototype.getSectorToLeft=function(idx)
{
	var x=idx%this.sizeX;
	var y=Math.floor(idx/this.sizeX);
	if (x<=0)
	{
		x=this.sizeX-1;
	}
	else
	{
		x--;
	}
	return x+y*this.sizeX;
}

EmpTerrain.prototype.getSectorToUp=function(idx)
{
	var x=idx%this.sizeX;
	var y=Math.floor(idx/this.sizeX);
	if (y==0)
	{
		y=this.sizeY-1;
	}
	else
	{
		y--;
	}
	return x+y*this.sizeX;
}

EmpTerrain.prototype.getSectorToDown=function(idx)
{
	var x=idx%this.sizeX;
	var y=Math.floor(idx/this.sizeX);

	if (y==this.sizeY-1)
	{
		y=0;
	}
	else
	{
		y++;
	}	
	return x+y*this.sizeX;
}


EmpTerrain.prototype.getSectorToUpRight=function(idx)
{
	var y=Math.floor(idx/this.sizeX);

	// If we are at even row then move just up.
	if ((y%2)==0)
	{
		return this.getSectorToUp(idx);
	}
	
	// Otherwise move left and up		
	return this.getSectorToRight(this.getSectorToUp(idx));
}

EmpTerrain.prototype.getSectorToUpLeft=function(idx)
{
	var y=Math.floor(idx/this.sizeX);

	// If we are at uneven row then move just up.
	if ((y%2)!=0)
	{
		return this.getSectorToUp(idx);
	}
	
	// Otherwise move left and up		
	return this.getSectorToLeft(this.getSectorToUp(idx));
}

EmpTerrain.prototype.getSectorToLowLeft=function(idx)
{
	var y=Math.floor(idx/this.sizeX);

	// If we are at uneven row then move just down.
	if ((y%2)!=0)
	{
		return this.getSectorToDown(idx);
	}
	
	// Otherwise move left and up		
	return this.getSectorToLeft(this.getSectorToDown(idx));
}

EmpTerrain.prototype.getSectorToLowRight=function(idx)
{
	var y=Math.floor(idx/this.sizeX);

	// If we are at uneven row then move just down.
	if ((y%2)==0)
	{
		return this.getSectorToDown(idx);
	}
	
	// Otherwise move left and up		
	return this.getSectorToRight(this.getSectorToDown(idx));
}

