package se.eit.flameengine_package;


import java.util.Random;
import se.eit.d1_pkg.*;
import se.eit.rsb_package.AvatarInterface;
import se.eit.web_package.*;

public class FlameEntity extends D1ThreadSafe {
	
	FlamePao position = new FlamePao ();
	FlamePao oldPosition = new FlamePao ();
	
	// TODO: Include in read and write.
	FlamePao velocity = new FlamePao (); // Unit is blocks per millisecond
	
	public float bodyRadius = 0.25f;
	
	int teamIndex = 0;
	
	int a = 0;
	

	
	// Entity health.
	public int health = 20;
	
	// entity i entity (item) comer att display'a. TODO: Move to seperate class.
	public int state = 0;
	public int stack = 0;
	
	// Resources
	public int entitySprite = 0;
	public int fill_wood = 0;
	public int fill_stone = 0;
	public int fill_mineral = 0;
	
	Random rand = new Random (); // TODO rand should reside in the world object

	
	public FlameEntity(D1Container parent, String name) 
	{
		super();
		linkSelf(parent);
		this.regName(name);
	}
	
	
	public FlameEntity() 
	{
		super();	
	}
	

	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);
		position = new FlamePao(wr);
		velocity = new FlamePao(wr);
		
		health = wr.readInt();
		entitySprite = wr.readInt();
		stack = wr.readInt();
		state = wr.readInt();
		
		// Only save or read team index if we are not a avatarinterface.
		if (this instanceof AvatarInterface)
		{
			wr.readInt();
		}
		else
		{
			
			teamIndex = wr.readInt();
		}

	}
	
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("setSprite"))
		{
			entitySprite = wr.readInt();
			this.setUpdateCounter();
		}
		return super.setInfo(wr, infoName);
	}
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		position.writeSelf(ww);
		velocity.writeSelf(ww);
		ww.writeInt(health);
		ww.writeInt(entitySprite);
		ww.writeInt(stack);
		ww.writeInt(state);
		// Only save or read team index if we are not a avatarinterface.
		if (this instanceof AvatarInterface)
		{
			ww.writeInt(0);
		}
		else
		{
			ww.writeInt(teamIndex);
		}
		//ww.writeInt(teamIndex);
	}
	
	// Denna metod anropas av game engine periodiskt
	public void tickEntityMs(long deltaMs)
	{
		// Itirate velocity.
		if ((this.velocity.rotation!=0) || (this.velocity.x!=0) || (this.velocity.y!=0))
		{
			this.position.rotation += this.velocity.rotation * (deltaMs) ;
			move (this.velocity.multiply(deltaMs));
			this.setUpdateCounter();
		}
	}
	
	// Will return true if there was a collision
	final public boolean doCompleteCollisionCheck (FlamePao positionCheck) 
	{
		float newX = positionCheck.x;
		float newY = positionCheck.y;
		
		// Check if parent is FlameRoom. else no point in continuing.
		if (!(this.getParent() instanceof FlameRoom))
		{
			error("Parent was not FlameRoom");
			return false;
		}
		FlameRoom cpr = (FlameRoom) this.getParent();
		
		// Check if it is not okay to walk on the tile.
		if (FlameTile.isWalkable(cpr.getTile(newX, newY))!=true)
		{
			
			// It isn't. Handle
			if (this.OnWall((int)Math.floor(newX), (int)Math.floor(newY)) == true)
			{
				return true;
			}
		}
			
		
		
		// Check if we walk into a door. TODO: Move elsewhere.
		if (cpr.getTile(newX, newY) == FlameTile.doorOut)
		{
			D1Container pp=this.getParent().getParent();
			debug("move from "+getParent()+ "to "+pp.getId());
			this.relinkSelf(pp);
			return false;
		}
		
		// Here we check if we bump into other entities
		D1Base list[] = cpr.getListOfSubObjectsThreadSafe();
		for(int i=0;i<list.length;i++)
		{
			if (list[i] instanceof FlameEntity)
			{
				FlameEntity cpe = (FlameEntity)list[i];
				
				// Check if collision is flame entity.
				if (cpe!=this)
				{
					
					// TODO: Move to HmegPao distance to
					float deltaRelativeX = newX - cpe.position.x;
					float deltaRelativeY = newY - cpe.position.y;
					
					float distanceRelative =  (float)Math.sqrt(deltaRelativeX * deltaRelativeX + deltaRelativeY * deltaRelativeY);

					if (distanceRelative < this.bodyRadius + cpe.bodyRadius) {
						
					    // TODO: Move collision to other function and add return statement as class.
						boolean otherWantsToCollide = this.OnCollision(cpe);
						boolean thisWantsToCollide = cpe.OnCollision (this);
						
						// Both can interfere.
						boolean shouldNotContinue = otherWantsToCollide || thisWantsToCollide;
						if (shouldNotContinue)
							return true;
					}
				}
			}
			
		}
		
		// Nothing interfered.
		return false;
	}

	public void doMoveAnimation(D1SubRoot ro)
	{	
		ro.lockWrite();
		try {
		if (state == 0) {state=2;}
		else if (state == 1) {state=3;}
		// Reverse
		else if (state == 2) {state=0;}
		else if (state == 3) {state=1;}	
		}
		finally
		{
			ro.unlockWrite();
		}
	}
	
	
	public void move(FlamePao deltaPao)
	{
		
		// Get root.
		D1SubRoot ro=this.getDbSubRoot();

		// Lock root
		ro.lockWrite();
		
		// Then try the code.
		try{
			D1Base p = this.getParent();
			
			// There is only point in continuing if parent is a Room.
			if (p instanceof FlameRoom)
			{
	
				// It was, for easier use we create a pointer 
				FlameRoom cpr=(FlameRoom)p;
				
				
				
				
				// Create the new position.
				final float newX=position.x+deltaPao.x;
				final float newY=position.y+deltaPao.y;
				
				if (doCompleteCollisionCheck (new FlamePao (newX, newY, position.rotation)) == true)
					return;
				
		        // Here we check that we will not move outside the current room.
				if ( (newX<0) || (newX>=cpr.xSectors) || (newY<0) || (newY>=cpr.ySectors) )
				{
					return;
				}
				
				
					
					// Update last position to new position.
					oldPosition.x=position.x;
					oldPosition.y=position.y;	
					
					// Update the animation state.
					if (newX >position.x )
					{
					 if (state==2)
					 {
						state = 1; 
					 }
					
						
					}
					if (newX <position.x )
					{	
						if (state==1)
						 {
							state = 2; 
						 }
					}
					
					// Move the entity.
					position.x=newX;
					position.y=newY;
					
					doMoveAnimation(ro);
				}
			
		}
		finally
		{
			
			// Unlock root.
			ro.unlockWrite();
		}
		this.setUpdateCounter();			

	}

	public FlameEntity findItem(int id)
	{
		if (this.listOfStoredObjects!=null)
		{
			for(D1Storable ds: this.listOfStoredObjects)
			{
				if (ds instanceof FlameEntity)
				{
					FlameEntity cpe=(FlameEntity)ds;
					if (cpe.entitySprite==id)
					{
						return cpe;
					}
				}
			}
		}
		return null;
	}

	
	public boolean OnWall (int x, int y)
	{
		error("OnWall not implemented");
		return true;
	}
	
	public int haveItem(int id)
	{
		int count=0;
		for(D1Storable ds: this.listOfStoredObjects)
		{
			if (ds instanceof FlameEntity)
			{
				FlameEntity cpe=(FlameEntity)ds;
				if (cpe.entitySprite==id)
				{
					count += cpe.stack ;
				}
			}
		}
		return count;
	}
	
	// Return true if collision is blocking move. TODO: Take in more parameters!
	public boolean OnCollision (FlameEntity collider)
	{
		error("OnCollision not implemented");
		return true;
	}
	
	

	public void giveItem(int entitySprite , int count)
	{
		debugWriteLock();
		
		
		FlameEntity cpe = this.findItem(entitySprite);
		
		if (cpe!=null)
		{
			cpe.stack+=1;
			cpe.setUpdateCounter();
		}
		else
		{	
			// Create a new object
			cpe = new FlameEntity ();
			cpe.linkSelf(this);
			cpe.regName("i"+entitySprite); // All objects shall have some name in RSB
			cpe.entitySprite = entitySprite;
			cpe.stack = count;
			//cpe.state = ite; // temporary test for new JS client.
	
			cpe.setUpdateCounter(); // should not be needed here.

			// assign coordinates to the new object.
			// Objects in the inventory are placed from upper left according to index
			if (this instanceof FlameRoom)
			{
				FlameRoom cpr=(FlameRoom)this;
				cpr.position.x = cpe.getIndex()%cpr.xSectors;
				cpr.position.y = cpe.getIndex()/cpr.xSectors;
			}
		}
	}

	
	public void moveAllChildObjectsToParent()
	{
		if (this.listOfStoredObjects!=null)
		{
			for (D1Storable ds : this.listOfStoredObjects)
			{
				if (ds instanceof D1Container)
				{
					// move to parent object
					D1Container dc=(D1Container)ds;
					dc.relinkSelf(this.getParent());
					
				}
			}
		}
	}
	
	public int getTeam ()
	{
		return teamIndex;
	}
	
	public void setTeam (int teamIndex)
	{
		this.teamIndex = teamIndex;
	}

}
