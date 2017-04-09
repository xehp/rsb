package se.eit.flameengine_package;

import java.util.Random;

import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

public class FlamePao {

	/*---------------------------------------------*\
	|* Created: 28 Feb 2016 By: Alexander Björkman *|
	|* Flame Position and orientation              *|
	|* also used for velocity and rotation speed.  *|
	\*---------------------------------------------*/
	
	public float x;
	public float y;
	public float rotation;
	
	FlamePao (final float x, final float y, final float rotation)
	{
		this.x = x;
		this.y = y;
		this.rotation = rotation;
	}
	
	FlamePao ()
	{
		this.x = 0;
		this.y = 0;
		this.rotation = 0;
	}
	
	// Get x and y from a distance and rotation, 
	public static FlamePao getFromRotation (final float rotation, final float distance)
	{
		float radians = (float) Math.toRadians(rotation);
		
		return (new FlamePao((float)Math.cos(radians) * distance, (float)Math.sin(radians) * distance, 0));
	}
	
	public FlamePao (final FlamePao original)
	{
		this.x = original.x;
		this.y = original.y;
		this.rotation = original.rotation;
	}
	
	public void writeSelf(WordWriter ww)
	{
		ww.writeFloat(x);
		ww.writeFloat(y);
		ww.writeFloat(rotation);
	}
	
	public FlamePao (WordReader ww)
	{
		this.x = ww.readFloat();
		this.y = ww.readFloat();
		this.rotation = ww.readFloat();

		
	}
	
	public static FlamePao getRandomPos (FlameRoom fr)
	{
		Random rand = new Random();
		int xf = rand.nextInt(fr.xSectors / 4);
		int yf = rand.nextInt(fr.ySectors / 4);
		while (fr.getTile(xf * 4 - 1.5f, yf * 4 - 1.5f) != 0)
		{
			xf = rand.nextInt(fr.xSectors / 4);
			yf = rand.nextInt(fr.ySectors) / 4;
		}
		
		float x = xf * 4 - 1.5f;
		float y = yf * 4 - 1.5f;
		return new FlamePao(x,y,0);
	}
	
	// Used for iterating positions.
	public FlamePao multiply (final float a)
	{
		// multiply all vectors. excluding rotation.
		return new FlamePao (this.x * a, this.y * a, this.rotation);
	}
	
}
