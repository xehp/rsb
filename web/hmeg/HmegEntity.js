// hmeg_room.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-02-21 by Henrik Bjorkman www.eit.se/hb


// subclass extends superclass
HmegEntity.prototype = Object.create(MirrorBase.prototype);
HmegEntity.prototype.constructor = HmegEntity;




function HmegEntity(mirrorDb, parent, arg)
{
	MirrorBase.call(this, mirrorDb, parent, arg); // call super constructor
	
	//this.unknown= new DivImg("unknown");
	//this.img = [];
	this.initSelf();
	var hmegWin = this.mirrorDb.hmegWin;
	this.hmegImgList = hmegWin.hmegImgList; 
	
	this.timeOfLastUpdateMs=(new Date()).getTime();
}

HmegEntity.prototype.initSelf=function()
{

/*
    	// Entities & Blocks
	this.img["0_0"] = new DivImg("air");
	//this.img["1_0"] = new DivImg("dirt");
	this.img["1_0"] = new DivImg("block1");
	this.img["2_0"] = new DivImg("ladder");
	this.img["3_0"] = new DivImg("grass");
	this.img["4_0"] = new DivImg("door");
	this.img["8_0"] = new DivImg("wood");
	this.img["9_0"] = new DivImg("woodstairright");
	this.img["13_0"] = new DivImg("avatar_0");
	this.img["13_1"] = new DivImg("avatar_1");
	this.img["13_2"] = new DivImg("avatar_2");
	this.img["13_3"] = new DivImg("avatar_3");

	this.img["14_0"] = new DivImg("bullet");
	this.img["15_0"] = new DivImg("flag_0");
	this.img["15_1"] = new DivImg("flag_1");

	this.img["16_0"] = new DivImg("tank_0");
	this.img["16_1"] = new DivImg("tank_1");
	this.img["16_2"] = new DivImg("tank_0");
	this.img["16_3"] = new DivImg("tank_1");

	this.img["17_0"] = new DivImg("block2");
	this.img["18_0"] = new DivImg("cactus");
	this.img["19_0"] = new DivImg("tankstone");
*/
}


HmegEntity.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);

	this.x = parseFloat(wr.readNext());
	this.y = parseFloat(wr.readNext());
	this.rotation = parseInt(wr.readNext());

	this.velocity = {};
	this.velocity.x =parseFloat(wr.readNext());
	this.velocity.y =parseFloat(wr.readNext());
	this.velocity.rotation =parseFloat(wr.readNext());

	this.health = parseInt(wr.readNext());
	this.itemtype = parseInt(wr.readNext());
	this.stack = parseInt(wr.readNext());
	this.state = parseInt(wr.readNext());
	this.teamIndex = parseInt(wr.readNext());

	this.timeOfLastUpdateMs=(new Date()).getTime();


}


HmegEntity.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", id="+this.id;

	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}


// In HMEG x,y shall point at the center of the object, not upper left corner as before.
HmegEntity.prototype.showSelfContextXY=function(context, x, y, width, height)
{
	var n=""+this.itemtype+"_"+this.state;

	if (this instanceof HmegAvatar)
	{
		// taken care of in HmegAvatar.prototype.showSelfContextXY
	}
	else
	{
		context.fillStyle = 'black';
		context.strokeStyle="#00FF00";
	}

	var r=Math.PI*this.rotation/180.0;


	context.translate(x, y);

	context.font = '8pt Calibri';
	context.fillText(""+this.objName ,-10 , -20);


	// rotate the image
	context.rotate(r);
	
	if (n in this.hmegImgList.img)
	{
		//context.setTransform(1,0,1,0,0,0);
		this.hmegImgList.img[n].showSelfContextXY(context, 0 - width/2, 0 - height/2, width, height);
	}
	else
	{
		this.hmegImgList.unknown.showSelfContextXY(context, 0 - width/2, 0 - height/2, width, height);
		context.font = '8pt Calibri';
		context.fillText(""+n ,-10 , +30);
	}

	context.beginPath();
	context.moveTo(-10, 0);
	context.lineTo(30, 0);
	context.moveTo(0, -10);
	context.lineTo(0, 10);
	context.stroke();
	
	context.rotate(-r);


	// This is for entities (in this case it is flags) residing in the object, if any loop all children
	var cl = this.children;
	if (cl!=null)
	{
		var i;
		for(i in cl)
		{
			// Only object of type HmegEntity need to be shown (others ignored)
			if (cl[i] instanceof HmegEntity)
			{
				var s=cl[i];
				//s.extrapolatePosition();
				s.showSelfContextXY(context, 0, -height/2, width, height)
			}
		}
	}	

	context.translate(-x, -y);
	
	
}

// Extrapolate position
HmegEntity.prototype.extrapolatePosition=function()
{
    // Extrapolate the position using delta time and velocity
	var t_ms = (new Date).getTime();
	var dt = t_ms - this.timeOfLastUpdateMs;
	this.x += (dt * this.velocity.x);
	this.y += (dt * this.velocity.y);
	this.timeOfLastUpdateMs = t_ms;
}
