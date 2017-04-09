package se.eit.flameengine_package;

import java.io.IOException;
import java.util.Random;

import se.eit.d1_pkg.D1SubRoot;

public class HmegBullet extends FlameEntity {

	int msTime = 0; // age of the bullet
	final int msLife = 3 * 1000; // time to live for bullet
	
	public FlameEntity owner;
	
	
	public HmegBullet ()
	{
		super();
		entitySprite = FlameTile.bullet;
		this.setUpdateCounter();
	}
	
	@Override
	public void doMoveAnimation(D1SubRoot ro)
	{
		
	}
	
	@Override
	public boolean OnWall (int x, int y)
	{
		System.out.println("Collide: "+x + " "+y);
		//error("OnWall not implemented");
		if (this.getParent() instanceof FlameRoom)
		{
			FlameRoom cpr =(FlameRoom)this.getParent();
			if (cpr.getTile(x, y) == FlameTile.cracked)
			{
				cpr.changeTile(x, y, 0);
			}
			if (cpr.getTile(x, y) == 1)
			{
				cpr.changeTile(x, y, FlameTile.cracked);
			}
		}
		this.unlinkSelf();
		return true;
	}
	
	
	public void tickEntityMs(long deltaMs)
	{
		
		// Check if maximum life time.
		if (msTime >= msLife)
		{
			// Kill self.
			D1SubRoot ro = this.getDbSubRoot();
			try
			{
				ro.lockWrite();
				this.unlinkSelf();
				return;
			}
			finally
			{
				ro.unlockWrite();
			}
			
		}
		msTime += deltaMs;

		super.tickEntityMs(deltaMs);
	}
	
	@Override
	public boolean OnCollision (FlameEntity collider)
	{
		
		// Maybe kill the other bullet?
		if (collider instanceof HmegBullet)
		{
			HmegBullet coll = (HmegBullet) collider;
			// Kill each other... only if not on same team
			if (this.owner instanceof FlameAvatar && coll.owner instanceof FlameAvatar)
			{
				if (((FlameAvatar) this.owner).getTeam() != ((FlameAvatar)coll.owner).getTeam())
				{
					collider.unlinkSelf();
					this.unlinkSelf();
				}
			}
			
			return false;
		}
		
		// Don't kill your owner.
		if (collider == owner)
		{
			debug("Don't kill owner");
			return false;
		}
		if (collider instanceof FlameAvatar && owner instanceof FlameAvatar)
		{
			FlameAvatar own = (FlameAvatar)owner; 
			FlameAvatar coll = (FlameAvatar)collider;
			
			// Move that tank TODO: This moving is done all over the program, make this into an function.
			if (getParent() instanceof FlameRoom)
			{
				FlameRoom parent = (FlameRoom)getParent();
				Random rand = new Random();
				float xf = rand.nextInt(parent.xSectors / 4);
				float yf = rand.nextInt(parent.ySectors / 4);
				while (parent.getTile(xf * 4 - 1.5f, yf * 4 - 1.5f) != 0)
				{
					xf = rand.nextInt(parent.xSectors / 4);
					yf = rand.nextInt(parent.ySectors) / 4;
				}
				coll.position = new FlamePao(xf, yf, 0);
				coll.wantedChangedPositionCounter++;
				coll.setUpdateCounter();
				
			}
			
			if (coll.getTeam() == own.getTeam())
			{
				this.unlinkSelf();
				return false;
			}
			if (coll.hasFlag == true)
			{
				coll.health = 0;
				coll.freezeTime = FlameAvatar.FREEZE_TIME; 
				coll.dropFlag();
				this.unlinkSelf();
				return false;
			}
		}
		
		
		System.out.println("Target hit!");
		// Kill that tank!
		collider.health = 0;
		
				
		
		//collider.postMessageToThis("You were hit by "+owner.getName());
		try {
			if (collider instanceof FlameAvatar)
			{
				FlameAvatar coll = (FlameAvatar)collider;
				coll.stc.writeLine("consoleMessage \"You were hit by "+owner.getName()+"\n\"");
				coll.postMessageToAllPlayers(""+coll.getName()+" was hit by "+owner.getName());
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		this.unlinkSelf();
		return true;
	}
	
}
