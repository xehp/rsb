package se.eit.flameengine_package;

public class HmegCrate extends FlameEntity {
	
	
	@Override
	public boolean OnCollision (FlameEntity collider)
	{
		if (collider instanceof FlameAvatar)
		{
			FlameAvatar coll = (FlameAvatar) collider;
			FlameUser fu =(FlameUser)coll.getAvatarPlayerReferences();
			fu.forward_speed = 10;
		}
		return false;
	}
	

}
