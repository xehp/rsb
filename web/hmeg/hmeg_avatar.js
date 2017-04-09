
// hmeg_avatar.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-27 by Henrik Bjorkman www.eit.se/hb


// subclass extends superclass
HmegAvatar.prototype = Object.create(HmegEntity.prototype);
HmegAvatar.prototype.constructor = HmegAvatar;




function HmegAvatar(world, parent, arg)
{
	//this.avatarPlayerReferencesId=-1;
	//this.serverChangedPositionCounter=-1;

	HmegEntity.call(this, world, parent, arg); // call super constructor
	
	console.log("HmegAvatar");

}


//HmegAvatar.prototype.initSelf=function()
//{
//	this.img[0] = new DivImg("avatar_0");
//	this.img[1] = new DivImg("avatar_1");
//	this.img[2] = new DivImg("avatar_2");
//	this.img[3] = new DivImg("avatar_3");
//}

HmegAvatar.prototype.readSelf=function(wr)
{
	HmegEntity.prototype.readSelf.call(this, wr);

	this.avatarPlayerReferencesId=parseInt(wr.readNext());
	this.serverChangedPositionCounter=parseInt(wr.readNext());

	this.avatarPlayerReferencesIdDebug = this.avatarPlayerReferencesId; 
	
	// This is only for debugging, remove later
	if (this.avatarPlayerReferencesId == -1)
	{
		console.log("Why is it -1?, this.avatarPlayerReferencesId=" + this.avatarPlayerReferencesId + ",  this.id="+this.id);
	}
	else
	{
		//console.log("OK ~" + this.avatarPlayerReferencesId + " ~" + this.id);
	}
}



HmegAvatar.prototype.getRoundBuffer = function()
{
	// Disabled messages to player for now since the server does not send any.
	// Actually the messages shall go to AvatarPlayerReferences instead anyway
	/*if (this.roundBufferObj==null)
	{
		this.roundBufferObj=this.findSubObjectByName("roundBuffer");
	}
	return this.roundBufferObj;*/
	return null;
}


//HmegAvatar.prototype.showSelfContextXY=function(context, x, y, width, height)
//{
//	if (this.state < this.img.length)
//	{
//		this.img[this.state].showSelfContextXY(context, x, y, width, height);
//	}
//	else
//	{
//		this.unknown.showSelfContextXY(context, x, y, width, height);  
//	}
//}


HmegAvatar.prototype.getTeam = function()
{
	apr = this.mirrorDb.getById(this.avatarPlayerReferencesId);
	if (apr==null)
	{
		console.log("Did not find apr: ~"+this.avatarPlayerReferencesId+", for avatar ~"+this.id);
		return null;
	}
	
	return apr.teamIndex;
}


// In HMEG x,y shall point at the center of the object, not upper left corner as before.
HmegAvatar.prototype.showSelfContextXY=function(context, x, y, width, height)
{

	var team = this.getTeam();
	if (team == 1)
	{
			context.strokeStyle="#0000FF";
			context.fillStyle = 'blue';
	}
	else if (team == 2)
	{
			context.strokeStyle="#FF0000";
			context.fillStyle = 'red';
	}	
	else
	{
			context.strokeStyle='yellow';
			context.fillStyle = 'yellow';
	}	

	HmegEntity.prototype.showSelfContextXY.call(this, context, x, y, width, height);

}

