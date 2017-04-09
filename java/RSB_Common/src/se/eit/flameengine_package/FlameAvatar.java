
package se.eit.flameengine_package;
import se.eit.d1_pkg.*;
import se.eit.rsb_package.AvatarInterface;
import se.eit.rsb_package.GameUser;
import se.eit.rsb_package.RsbRoundBuffer;
import se.eit.rsb_server_pkg.ServerTcpConnection;
import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

public class FlameAvatar extends FlameEntity implements AvatarInterface {

	final static public int FREEZE_TIME = 10000;
	final static public float BULLET_VELOCITY = 0.01f;
	public ServerTcpConnection stc;
	
	public boolean hasFlag = false;

	
	public void dropFlag () {
				
		if (hasFlag != false)
		{
			HmegFlag flag = (HmegFlag) this.getChildFromIndexOrName("Flag");
			if (flag == null)
			{
				return;
			}
			if (this.getParent() instanceof FlameRoom)
			{
				flag.position = FlamePao.getRandomPos((FlameRoom)this.getParent());
			}
			
			moveAllChildObjectsToParent();
			hasFlag = false;
			this.postMessageToAllPlayers(this.getName()+" dropped the flag");
			this.setUpdateCounter();
		}
	}
	
	public float wantedForwardMotion = 0f;
	public float wantedRotationSpeed = 0f;  // unit is degrees per second
	
	public float wantedX = 0f;
	public float wantedY = 0f;
	public float wantedRotation = 0f;  // unit is degrees per second
	public int wantedChangedPositionCounter = -1;

	public int freezeTime = 0;
	
	int startAmmo = 5;
	
	public int ammo = startAmmo;
	

	public int refillTime = 0;
	
	RsbRoundBuffer rsbRoundBuffer=null;
	
	private int avatarPlayerReferencesId=-1; // The avatar shall be connected to a player. This is used to tell which player this avatar is associated with.
	private GameUser avatarPlayerReferences = null;
	

	// Usually the client calculates and sets the position of its avatar. By incrementing this we tell client to use the position from server. 
    private int serverChangedPositionCounter = 0;


    private boolean debugLagReported = false;
	
	public boolean OnWall (int x, int y)
	{
		//error("OnWall not implemented");
		return true;
	}
	
	@Override
	public void readSelf(WordReader wr)	
	{
		super.readSelf(wr);

		//allowedMove.readSelf(wr);
		avatarPlayerReferencesId = wr.readInt();
		serverChangedPositionCounter = wr.readInt();
		avatarPlayerReferences = null;
	}

	
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(avatarPlayerReferencesId);
		ww.writeInt(serverChangedPositionCounter);
	}

	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);					
		pw.println("avatarPlayerReferencesId "+avatarPlayerReferencesId);		
		pw.println("serverChangedPositionCounter "+serverChangedPositionCounter);		
	}
	
	
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("avatarPlayerReferencesId"))
		{
			avatarPlayerReferencesId = wr.readInt();
			this.setUpdateCounter();
			avatarPlayerReferences = null;
			return 1;
		}
		return super.setInfo(wr, infoName);
	}
		
	
	@Override
	public int getInfo(WordWriter ww, String infoName, WordReader wr)
	{
		if (infoName.equals("avatarPlayerReferencesId"))
		{
			ww.writeInt(avatarPlayerReferencesId);
			return 1;
		}
		if (infoName.equals("serverChangedPositionCounter"))
		{
			ww.writeInt(serverChangedPositionCounter);
			return 1;
		}
		return super.getInfo(ww, infoName, wr);
	}


	
	public void shoot ()
	{
	
		// Check if allowed to shoot.
		if (freezeTime <= 0 && ammo > 0)
		{
			debug("shoot");
			
			HmegBullet bullet = new HmegBullet ();
			bullet.owner = this;
			bullet.position = new FlamePao (this.position);
			bullet.velocity = FlamePao.getFromRotation(this.position.rotation, BULLET_VELOCITY);
			bullet.linkSelf(this.getParent());
			ammo -= 1;
		}
	}

	public FlameAvatar() 
	{
		super();	
		entitySprite = FlameTile.tank;
	}

	public boolean OnCollision (FlameEntity collider)
	{
		// Don't kill other bullets.
		if (collider instanceof HmegBullet)
		{
			return false;
		}
		
		if (collider instanceof HmegFlag)
		{
			return false;
		}

		return true;
	}
	
	// Find or create the message buffer
	public RsbRoundBuffer findRsbRoundBuffer()
	{
		if (rsbRoundBuffer!=null)
		{
			return rsbRoundBuffer;
		}
		
		int n=this.getListCapacity();
		for(int i=0;i<n;i++)
		{
			D1Base b=this.getObjFromIndex(i);
			if (b instanceof RsbRoundBuffer)
			{
				rsbRoundBuffer=(RsbRoundBuffer)b;
				return rsbRoundBuffer;
			}
		}
		rsbRoundBuffer = new RsbRoundBuffer(this, "erb"+this.getIndex());
		//this.addObject(erb);
		//erb.setUpdateCounter(); // There is a problem with setting update counter, it does not work when addObject is done. When that is fixed this line can be removed.
		return rsbRoundBuffer;
	}
	
	@Override	
	public void postMessageToThis(String str)
	{
		debugWriteLock();
		
		D1SubRoot r = this.getDbSubRoot();
		
		D1Base b = r.getDbIdObj(avatarPlayerReferencesId);
		
		if (b==null)
		{
			debug("did not find avatarPlayerReferences ~"+avatarPlayerReferencesId);
			return;
		}
		
		b.postMessageToThis(str);
	}

	
	public void postMessageToAllPlayers(String str)
	{
		GameUser ownApr = getAvatarPlayerReferences();
		ownApr.getParent().postMessageToThis(str);
	}
	
	
	@Override
	public void tickEntityMs(long deltaMs)
	{
		
		this.velocity.rotation = wantedRotationSpeed;

		float radians = (float) Math.toRadians(this.position.rotation);
		this.velocity.x = (float)Math.cos(radians) * wantedForwardMotion;
		this.velocity.y = (float)Math.sin(radians) * wantedForwardMotion;

		//super.tickEntityMs(deltaMs);

		// TODO Here we could try do detect if client is cheating with speed hacks.
		
		if (serverChangedPositionCounter==wantedChangedPositionCounter)
		{
			// Client is up to date with servers latest teleport or position adjustment

			//D1Base p = this.getParent();
			//FlameRoom cpr=(FlameRoom)p;

	        // Here we check that we will not move outside the current room.
			/*if ( (this.wantedX<0) || (this.wantedX>=cpr.xSectors) || (this.wantedY<0) || (this.wantedY>=cpr.ySectors) )
			{
				// Move not possible, teleport back by incrementing serverChangedPositionCounter, that will tell client to use current position from server.
				serverChangedPositionCounter++;
				this.setUpdateCounter();
			}
			// Check if it is okay to walk on it anyway.
			else if (!(FlameTile.isWalkable(((FlameRoom) p).getTile(this.wantedX, this.wantedY))==true))
			{
				// Move not possible, teleport back by incrementing serverChangedPositionCounter, that will tell client to use current position from server.
				serverChangedPositionCounter++;
				this.setUpdateCounter();
			}*/
			if (this.doCompleteCollisionCheck(new FlamePao (wantedX, wantedY, 0)) == true)
			{
				serverChangedPositionCounter++;
				this.setUpdateCounter();
			}
			else
			{
				if ((this.position.x!=this.wantedX) || (this.position.y!=this.wantedY) || (this.position.rotation!=this.wantedRotation))
				{
					this.position.x=this.wantedX;
					this.position.y=this.wantedY;
					this.position.rotation=this.wantedRotation;
					this.setUpdateCounter();
				}
			}
			debugLagReported = false;
		}
		else
		{
			// This means that we the server have changed the position and the client has not yet reacted to that. We ignore all position updates from client until it has.
			if (!debugLagReported)
			{
				debug("wantedChangedPositionCounter "+wantedChangedPositionCounter+" != "+serverChangedPositionCounter);
				debugLagReported = true;
			}
		}

		
		//System.out.println("Velocity: "+position.x+" "+position.y);
		
		
		// Decrease freeze time.
		if (freezeTime > 0)
		{
			freezeTime -= deltaMs;
		}
		
		if (refillTime <= 0 && ammo < startAmmo)
		{
			refillTime = 1 * 1000;
			ammo +=1;
			this.setUpdateCounter();
		}
		else if (ammo < startAmmo)
		{
			refillTime -= deltaMs;
		}
		
		// Check if dead
		if (health < 1)
		{
			
			// Respawn.
			health = 20;
			
			// Punish for respawn.
			this.freezeTime = FREEZE_TIME;
		}
	}

	// Find or create the message buffer
	public RsbRoundBuffer findMsgRoundBuffer()
	{
		if (rsbRoundBuffer!=null)
		{
			return rsbRoundBuffer;
		}
		
		int n=this.getListCapacity();
		for(int i=0;i<n;i++)
		{
			D1Base b=this.getObjFromIndex(i);
			if (b instanceof RsbRoundBuffer)
			{
				rsbRoundBuffer=(RsbRoundBuffer)b;
				return rsbRoundBuffer;
			}
		}
		rsbRoundBuffer = new RsbRoundBuffer(this, "roundBuffer");
		//this.addObject(erb);
		//erb.setUpdateCounter(); // There is a problem with setting update counter, it does not work when addObject is done. When that is fixed this line can be removed.
		return rsbRoundBuffer;
	}
	
	public GameUser getAvatarPlayerReferences()
	{
		if (avatarPlayerReferences==null)
		{
			D1SubRoot r = this.getDbSubRoot();
			D1Base b = r.getDbIdObj(avatarPlayerReferencesId);
			try
			{
				avatarPlayerReferences = (GameUser)b;
			}
			catch (ClassCastException e)
			{
				error("not a AvatarPlayerReferences "+e);
			}
		}
		return avatarPlayerReferences;
	}

	
	@Override
	public int getTeam()
	{
		GameUser apr = getAvatarPlayerReferences();
		return apr.teamIndex;
	}

	
	// This is to be called when client has sent the avatarPos command
	public void readWantedPao(WordReader wr)
	{
		this.wantedX = wr.readFloat();
		this.wantedY = wr.readFloat();
		this.wantedRotation = wr.readFloat();
		this.wantedChangedPositionCounter = wr.readInt();
		
		// We don't set update counter here since wanted variables are only temporary (at least for now)
	}
	
	public void setAvatarPlayerReferences(GameUser apr)
	{
		avatarPlayerReferencesId = apr.getId();
		avatarPlayerReferences = null;
		this.setUpdateCounter();
	}

	@Override
	public void setTeam(int team) {
		avatarPlayerReferences.setTeam(team);
	}
	
	
	@Override
	public void unlinkSelf()
	{
		dropFlag();
		super.unlinkSelf();
	}
	
}
