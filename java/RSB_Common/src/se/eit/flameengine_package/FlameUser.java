package se.eit.flameengine_package;

import se.eit.rsb_package.GameUser;
import se.eit.web_package.*;

public class FlameUser extends GameUser {

	public float forward_speed = 0.002f;
	public float backward_speed = 0.001f;
	public float speed_reset = 0;
	
	@Override
	public void readSelf (WordReader wr)
	{
		super.readSelf(wr);
		forward_speed = wr.readFloat();
		backward_speed = wr.readFloat();
	}
	
	@Override
	public void writeSelf (WordWriter ww)
	{
		super.writeSelf(ww);
		ww.writeFloat(forward_speed);
		ww.writeFloat(backward_speed);
		
	}
	
}
