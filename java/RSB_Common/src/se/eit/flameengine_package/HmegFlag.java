package se.eit.flameengine_package;

import se.eit.d1_pkg.D1SubRoot;
import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

public class HmegFlag extends FlameEntity {

	
	@Override
	public void doMoveAnimation(D1SubRoot ro)
	{
		
	}
	
	@Override
	public void readSelf (WordReader wr)
	{
		super.readSelf(wr);
		teamIndex = wr.readInt();
		
	}
	
	@Override
	public void writeSelf (WordWriter ww)
	{
		super.writeSelf(ww);
		ww.writeInt(teamIndex);
	}

	@Override
	public boolean OnWall (int x, int y)
	{
		return true;
	}
	
	
	@Override
	public boolean OnCollision (FlameEntity collider)
	{
		if (collider instanceof FlameAvatar)
		{
			FlameAvatar coll = (FlameAvatar)collider;
			
			// Check if flag return
			if (coll.getTeam() == this.teamIndex)
			{	
				// Check if score point
				if (coll.hasFlag == true)
				{
					coll.dropFlag();
							
					coll.postMessageToAllPlayers("The flag of team "+teamIndex+" was captured");
					System.out.println("Team score!");
					
					
				}
			}
			else
			{
				if ((coll.health > 0) && (coll.freezeTime<=0) && (coll.getTeam() != this.teamIndex))
				{
					// Get root
					D1SubRoot ro = this.getDbSubRoot();			
					try {
						
						// Lock before moving.
						ro.lockWrite();
						this.relinkSelf(coll);
						coll.hasFlag = true;
						coll.postMessageToAllPlayers("Alert, the flag of team "+teamIndex + " was taken by "+coll.getName());
						this.setUpdateCounter();
					}
					finally
					{
						
						// Unlock!
						ro.unlockWrite();
					}
				}
			}
			
		}
		return false;
	}
	
	public HmegFlag ()
	{
		super();
		this.setName("Flag");
	}
	
}
