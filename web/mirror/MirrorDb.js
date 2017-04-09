// MirrorDb.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb, All rights reserved
// Created 2015-01-28 by Henrik Bjorkman





function MirrorDb()
{
	this.rootObj=null; // Reference to the top most EmpBase object stored
	this.latestObj=null; // used only during initial download
	this.stateDepth=0; // used only during initial download
	this.byId=[];
	this.changedCounter=0;
}



MirrorDb.prototype.getById=function(id)
{
	var len = this.byId.length
	//console.log("MirrorDb.getById "+id+" "+len);
	if (id in this.byId) 
	{
		var u = this.byId[id];
		
		// This is just for debugging
		if (u.id!=id)
		{
			console.log("showById: inconsistent id "+u.id+" != "+id);
		}
				
		return u;		
	}
	console.log("MirrorDb.getById did not find ~"+id+", byId.length="+len);
	return null;
}



// This shall be called when a mirror message was received
// Returns non zero if message was handled here, 0 otherwise.
MirrorDb.prototype.onMirrorMessage=function(arg)
{
	//console.log("onMirrorMessage ", arg);


	var cmd=arg[0];
	if ((cmd=="ma") || (cmd=="mu") || (cmd=="mirrorUpdate") || (cmd=="mirrorAdd"))
	{
		if (cmd=="ma")
		{
			console.log("onMirrorMessage ", arg);
		}
	
		var id=parseInt(arg[1]);
		var parentId=parseInt(arg[2]);
		var objType=arg[3];

		var wr = new WordReader(arg.slice(4)); // TODO probably more efficient like this: var wr = new WordReader(arg); wr.skip(4);  But getAbsArg is still used in some places, that must be fixed first.

		if (id in this.byId)
		{
			// update an existing object
			var u = this.byId[id];

			u.updateSelf(parentId, objType, wr);
			//this.needRedraw(); // on second thought, will let the server send a special message to trigger redrawing.
		}
		else
		{
			// add a new object

			// does it have a parent?
			if (parentId in this.byId)
			{
				// yes, get the parent object
				var p = this.byId[parentId];

				//console.log("this.mirrorUpdate: add new object, id="+id+", parentId="+parentId+", objType="+objType);

				p.addChild(objType, wr);
				
				// This is for debugging
				if (wr.getNRemaining()>0)
				{
					console.log("Too many arguments given to '"+cmd+"', '"+objType+"' "+wr.getNRemaining());
					//throw new Error("Too many arguments given to '"+cmd+"', '"+objType+"' "+wr.getNRemaining());
				}

				//this.needRedraw();
			}
			else
			{
				// There is no parent

				if (parentId>=0)
				{				
					// This shall not happen, server shall send parent before child.
					throw new Error("unknown parent "+parentId+" for object "+id);
				}

				// No parent object. But perhaps it is the top most object?
				if (this.rootObj==null)
				{
				
					// This is the top most object in our database, that is why it has no parent, OK.
					this.rootObj=MirrorFactory(this, null, objType, wr);
				}
				else
				{	
					// This shall not happen, we already have a top most object in our database.
					throw new Error("can't add "+id+" "+" because "+parentId+ " is not known");
				}
			}
		}
		this.changedCounter++;
		return 1;				
	}
	else if ((cmd=="mr") || (cmd=="mirrorRemove"))
	{
		if (arg.length>=2)
		{
			var id=parseInt(arg[1]);
			if (id in this.byId)
			{
				var u = this.byId[id];

				console.log("mirrorRemove: "+id+" "+u.selfToString());

				u.unlinkSelfFromWorld();

				//this.needRedraw();			
			}
			else
			{
				console.log("mirrorRemove: did not find unit ~"+id);
			}					
		}
		else
		{
			doError("to few arguments for removeUnit");
		}		
		this.changedCounter++;
		return 1;				
	}

	return 0;

}

MirrorDb.prototype.debugDump=function(prefix)
{
	console.log("debugDump");
	if (this.rootObj!=null)
	{
		this.rootObj.debugDump(prefix+" ");
	}
	else
	{
		console.log("rootObj is null");
	}
}
