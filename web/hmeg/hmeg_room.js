// hmeg_room.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-21 by Henrik Bjorkman www.eit.se/hb


// subclass extends superclass
HmegRoom.prototype = Object.create(HmegEntity.prototype);
HmegRoom.prototype.constructor = HmegRoom;




function HmegRoom(world, parent, arg)
{
	HmegEntity.call(this, world, parent, arg); // call super constructor
	
	this.imageDefaultSize = 32;
	
	this.canvas = null; // We put all blocks of the room in on localy stored image.
	
	this.initSelf();

	this.needRefresh = true;
}




HmegRoom.prototype.readSelf=function(wr)
{
	HmegEntity.prototype.readSelf.call(this, wr);


	this.outerX=parseInt(wr.readNext());
	this.outerY=parseInt(wr.readNext());
	this.xSectors=parseInt(wr.readNext());
	this.ySectors=parseInt(wr.readNext());

	this.map=[];
		
	for(var x=0;x<this.xSectors;x++)
	{
		this.map[x]=[];
		for(var y=0;y<this.ySectors;y++)
		{
			this.map[x][y] = parseInt(wr.readNext());
		}
	}

	this.sectorHeight = 32;
	this.sectorWidth = 32;

	this.needRefresh = true;
}


HmegRoom.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", x="+this.x;
	str+=", y="+this.y;


	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}


HmegRoom.prototype.isControlPanel=function(x, y)
{
	var CityPvpBlock_controlPanel=11;
	
	if (this.map[x][y]==CityPvpBlock_controlPanel)
	{
		return true;
	}
	return false;
}


HmegRoom.prototype.refreshSelf=function()
{			
	if ((this.needRefresh != false) || (this.changedCounter != DivImagesBeingLoaded))
	{
		this.changedCounter = DivImagesBeingLoaded;
	
		console.log("refreshing");
		var canvas = document.createElement('canvas');
		var context = canvas.getContext('2d');
	
		canvas.width = this.xSectors * this.sectorWidth;
		canvas.height = this.ySectors * this.sectorHeight;
		
		// This is for debugging, remove later
		//context.fillStyle="#C0C0E0";
		//context.fillRect(0, 0, canvas.width, canvas.height);
	
		this.canvas=canvas;
		var t=this;
			
	
		for (var x=0; x<this.xSectors; x++)
		{
			for (var y=0; y<this.ySectors; y++)
			{
				var  r= this.map[x][y]; 
				if (r!=0)
				{			
					
					// This is for debugging, remove later
					//context.fillStyle="#0080F0";				
					//context.fillRect(xy.x, xy.y, this.sectorWidth, this.sectorHeight);
	
					var argu = 0;
					var block = this.hmegImgList.img[r+"_"+argu];
					if (block === undefined)
					{
						context.fillStyle="#4040A0";				
						context.fillRect(x*this.sectorWidth, y*this.sectorHeight, (this.sectorWidth-1), (this.sectorHeight-1));
					}
					else
					{
						block.showSelfContextXY(context ,x*this.sectorWidth, y*this.sectorHeight, this.sectorWidth, this.sectorHeight);
					}
					
					
				}
				else
				{
						// Make some other background color where there is nothing (helps debugging much)
						context.fillStyle="#e0c0c0";	
						context.fillRect(x*this.sectorWidth, y*this.sectorHeight, (this.sectorWidth-1), (this.sectorHeight-1));
				}	
				
			}			
		}
		this.needRefresh = false;
	}
}
