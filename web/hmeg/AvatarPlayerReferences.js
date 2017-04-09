// AvatarPlayerReferences.js
// Copyright (C) 2014 Henrik Bjorkman www.eit.se/hb
// Created 2014-12-26 by Henrik Bjorkman www.eit.se/hb


		



// subclass extends superclass
AvatarPlayerReferences.prototype = Object.create(MirrorBase.prototype);
AvatarPlayerReferences.prototype.constructor = AvatarPlayerReferences;

function AvatarPlayerReferences(world, parent, emType, arg)
{	
	MirrorBase.call(this, world, parent, emType, arg); // call super constructor
	this.roundBuffer=null;	
}

AvatarPlayerReferences.prototype.readSelf=function(wr)
{
	MirrorBase.prototype.readSelf.call(this, wr);


	this.avatarId=wr.readNext();
	this.isActive=wr.readNext();
	this.teamIndex=wr.readNext();
	this.fragScore=wr.readNext();
	this.lossScore=wr.readNext();
	this.FORWARD_SPEED=wr.readNext();
	this.BACKWARD_SPEED=wr.readNext();

}


AvatarPlayerReferences.prototype.selfToString=function()
{
	var str="name="+this.objName;

	str+=", avatarId="+this.order;
	str+=", isActive="+this.order;
	str+=", teamIndex="+this.order;
	str+=", fragScore="+this.order;
	str+=", lossScore="+this.order;
	
	var n=this.getNChildObjects();
	if (n>0)
	{
		str+=", n="+n;
	}

	return str;
}


AvatarPlayerReferences.prototype.getRoundBuffer=function()
{
	if (this.roundBuffer==null)
	{
		var cl=this.children;
		var i;
		for(i in cl)
		{
			// Only object of type HmegEntity need to be shown (others ignored)
			if (cl[i] instanceof MirrorRoundBuffer)
			{
				this.roundBuffer = cl[i];
			}
		}
	}
		
	return this.roundBuffer;
}
