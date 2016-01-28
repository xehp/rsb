// emp_sector.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-28 by Henrik Bjorkman www.eit.se/hb




// subclass extends superclass
EmpSector.prototype = Object.create(EmpBase.prototype);
EmpSector.prototype.constructor = EmpSector;

function EmpSector(world, parent, emType, arg)
{	
	EmpBase.call(this, world, parent, emType, arg); // call super constructor

	// TODO The base class MirrorBase will do readSelf which should make the following 2 lines superflous, not just superflous but they create problems if the number of parameters are changed.
	this.unitOwner=parseInt(arg[4]);
	this.sectorTerrain=parseInt(arg[5]);

	this.seaColor="#CFCFDF";
}

EmpSector.prototype.readSelf=function(arg)
{
	var n = EmpBase.prototype.readSelf.call(this, arg);

	this.unitOwner=parseInt(arg[n]);
	this.sectorTerrain=parseInt(arg[n+1]);

	return n+2;	
}


EmpSector.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", id="+this.id;
	str+=", owner="+this.unitOwner;

    str+=", sectorTerrain="+this.sectorTerrain;

	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}

EmpSector.prototype.getSectorTypeName=function(sectorTerrain)
{
	var typeName;
	
	if ((sectorTerrain&4)!=0)
	{
		typeName='city';
	}
	else if ((sectorTerrain&16)!=0)
	{
		typeName='mineralDeposit';
	}
	else if ((sectorTerrain&8)!=0)
	{
		typeName='field';
	}
	else if ((sectorTerrain&3)==3)
	{
		typeName='beach';
	}
	else if ((sectorTerrain&2)!=0)
	{
		typeName='land';
	}
	else if ((sectorTerrain&1)!=0)
	{
		typeName='sea';
	}
	else
	{
		typeName='unknown';		
	}

	return typeName;
}

EmpSector.prototype.getHeaviestUnit=function()
{
	var mass=0;
	var unit=null
	for(i in this.children)
	{
		var u = this.children[i];
		if (u instanceof EmpUnit)
		{
			var m = u.getMass(); 
			if (m>=mass)
			{
				unit = u;
				mass = m;
			}
		}
		else
		{
			console.log("not an EmpUnit " + typeof u);
		}
	}
	return unit;
}


EmpSector.prototype.showSectorSubUnitContext=function(context, x, y, width, height)
{
	var n=this.getNChildObjects();
	if (n==1)
	{
		for(i in this.children)
		{
			this.children[i].showSelfUnitContextXY(context, x, y, width, height);
		}
	}
	else if (n>0)
	{
		// Show the heaviest unit
		var u = this.getHeaviestUnit();
		
		if (u!=null)
		{
			u.showSelfUnitContextXY(context, x, y, width, height);
	
			// Show as number of units
			context.font = '12pt Calibri';
			context.fillStyle = 'black';
			context.fillText(""+n, x+width/4, y+(height*2)/3);
		}
		else
		{
			console.log("did not find heaviest unit, id=~"+ this.id +", index="+this.index+", objName="+this.objName+", n="+n);
		}
	}
	else
	{
		// no child units, do nothing
	}

}

EmpSector.prototype.getLandColor=function()
{
/*
	switch(this.sectorTerrain & 0xFC)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			// only land (or land+sea)
			return "#90E090";
		case 4:
		case 5:
		case 6:
		case 7:
			// city/village
			return "#A0D060";
		case 8:
		case 9:
		case 10:
		case 11:
			// grass plain
			return "#80F080";
		case 12:
		case 13:
		case 14:
		case 15:
			// city/village and grass plain
			return "#A0F080";
		case 16:
		case 17:
		case 18:
		case 19:
			// minerals
			return "#80B040";
		default:
			return "#90E090";
	}*/
	return "#90E090";
}


EmpSector.prototype.showSelfSectorContextBkg=function(context, x, y, width, height)
{
	var typeName='unknown';
	
	/*
	if (this.sectorTerrain>1)
	{
	    // land
	    
		// Here we can either look at units in the sector or who owns the sector itself. Both should work but result is a little different.
		var o = this.getSectorOwner();
		// var o = this.unitOwner	
	
		if (o == rootDiv.mapNation)
		{			
			context.fillStyle="#8080F0";
		}
		else if (o >= 0)
		{
			context.fillStyle="#F08080";
		}
		else if (o == -2)
		{
			context.fillStyle="#D080D0";
		}
		else
		{
			context.fillStyle="#B0F0B0"; // for dark green use: "#00FF00"	
		}
	}
	else
	{
	    // sea
		context.fillStyle="#CFCFCF";  // for dark blue use: "#4060FF"	
	}
	context.fillRect(x+4,y+4,width-8,height-8);
	*/
	
	    
	var s = this.getSectorOwnerState(rootDiv.mapNation);

	//  0  : If nobody own the sector
	//  1  : State 'state' own this sector (alone) 
	//  2  : State 'state' have no units in the sector but other states do.
	//  3  : State 'state' have units in the sector and other states too.
	switch(s)
	{
		case 1:		
			context.fillStyle="#8080F0"; // This sector belong to this player, make a blue frame
			context.fillRect(x,y,width,height);
			break;
		case 2:
			context.fillStyle="#F08080"; // Some other player own this sector, make a red fram
			context.fillRect(x,y,width,height);
			break;
		case 3:
			context.fillStyle="#D080D0"; // More than one player in this sector, make a purple frame
			context.fillRect(x,y,width,height);
			break;
		default:				
			break;
	}

	switch(this.sectorTerrain&0x3)
	{
		case 0:
			// nothing
			break;
		case 1:
		    // sea
			context.fillStyle=this.seaColor;	
			context.fillRect(x+1,y+1,width-2,height-2);
			break;
		case 3:
			// land and sea
			context.fillStyle=this.getLandColor();
			context.fillRect(x+1,y+1,width-2,height-2);
			
			context.fillStyle=this.seaColor;	
			context.fillRect(x+width/2-width/8,y+height/2-height/8,width/4,height/4);
			
			var leftIdx=this.parent.getSectorToLeft(this.index);
			var left=this.parent.children[leftIdx];
			if (left!=null)
			{
				if ((left.sectorTerrain&1)!=0)
				{
					context.fillRect(x,y+height/2-height/12,width/2,height/6);
				}
			}
			
			var rightIdx=this.parent.getSectorToRight(this.index);
			var right=this.parent.children[rightIdx];
			if (right!=null)
			{
				if ((right.sectorTerrain&1)!=0)
				{
					context.fillRect(x+width/2,y+height/2-height/12,width/2,height/6);
				}
			}

			var upLeftIdx=this.parent.getSectorToUpLeft(this.index);
			var upLeft=this.parent.children[upLeftIdx];
			if (upLeft!=null)
			{
				if ((upLeft.sectorTerrain&1)!=0)
				{
					context.fillRect(x+width/2-width/8,y,width/8,height/2);
					context.fillRect(x+width/4,y,width/4,height/4);
				}
			}

			var downLeftIdx=this.parent.getSectorToLowLeft(this.index);
			var downLeft=this.parent.children[downLeftIdx];
			if (downLeft!=null)
			{
				if ((downLeft.sectorTerrain&1)!=0)
				{
					context.fillRect(x+width/2-width/8,y+height/2,width/8,height/2);
					context.fillRect(x+width/2-width/4,y+height-height/4,width/4,height/4);
				}
			}


			var upRightIdx=this.parent.getSectorToUpRight(this.index);
			var upRight=this.parent.children[upRightIdx];
			if (upRight!=null)
			{
				if ((upRight.sectorTerrain&1)!=0)
				{
					context.fillRect(x+width/2,y,width/8,height/2);
					context.fillRect(x+width/2,y,width/4,height/4);
				}
			}

			var downRightIdx=this.parent.getSectorToLowRight(this.index);
			var downRight=this.parent.children[downRightIdx];
			if (downRight!=null)
			{
				if ((downRight.sectorTerrain&1)!=0)
				{
					context.fillRect(x+width/2,y+height/2,width/8,height/2);
					context.fillRect(x+width/2,y+height-height/4,width/4,height/4);
				}
			}

			break;
		default:
			// land
			context.fillStyle=this.getLandColor();
			context.fillRect(x+1,y+1,width-2,height-2);
			break;		
	}
}

/*
EmpSector.prototype.showSelfSectorContextImg=function(context, x, y, width, height)
{

	typeName=this.getSectorTypeName(this.sectorTerrain);

	var imageObj = new Image();
	imageObj.onload = function() {
		context.drawImage(imageObj, x, y-Math.round((width-height)/2), width, width); // Images are 64x64 but map on the rows are 56px high, so using width as height. Images shall have some extra transparency in upper and lower instead.
	};
	imageObj.src = typeName+".png";

	//context.stroke();
}
*/

EmpSector.prototype.showSelfSectorContextImg=function(context, x, y, width, height)
{
	if ((this.sectorTerrain & 0xFC)!=0)
	{
		//var t = rootDiv.sectorTypesList.getSectorType(this.sectorTerrain);
		//t.showSelfContextXY(context, x, y, width, height);

		if ((this.sectorTerrain & 0x04)!=0)
		{
			var t = rootDiv.sectorTypesList.getSectorType(0x04);
			t.showSelfContextXY(context, x, y, width, height);
		}

		if ((this.sectorTerrain & 0x08)!=0)
		{
			var t = rootDiv.sectorTypesList.getSectorType(0x08);
			t.showSelfContextXY(context, x, y, width, height);
		}

		if ((this.sectorTerrain & 0x10)!=0)
		{
			var t = rootDiv.sectorTypesList.getSectorType(0x10);
			t.showSelfContextXY(context, x, y, width, height);
		}
				
		if ((this.sectorTerrain & 0x20)!=0)
		{
			var t = rootDiv.sectorTypesList.getSectorType(0x20);
			t.showSelfContextXY(context, x, y, width, height);
		}

		if ((this.sectorTerrain & 0x40)!=0)
		{
			var t = rootDiv.sectorTypesList.getSectorType(0x40);
			t.showSelfContextXY(context, x, y, width, height);
		}

		if ((this.sectorTerrain & 0x80)!=0)
		{
			var t = rootDiv.sectorTypesList.getSectorType(0x80);
			t.showSelfContextXY(context, x, y, width, height);
		}
	}
}


EmpSector.prototype.showSelfSectorContext=function(context, x, y, width, height)
{
	this.showSelfSectorContextBkg(context, x, y, width, height);
	this.showSelfSectorContextImg(context, x, y, width, height);
}


EmpSector.prototype.showSelfAndSubUnitContext=function(context, x, y, width, height)
{
	this.showSelfSectorContextBkg(context, x, y, width, height);
	this.showSelfSectorContextImg(context, x, y, width, height);
	this.showSectorSubUnitContext(context, x, y, width, height);
	
}




