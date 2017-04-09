package se.eit.flameengine_package;

public  class FlameTile {

	
static int air = 0;
static int dirt = 1;
static int ladder = 2;
static int grass = 3;
static int doorIn = 6;
static int doorOut = 7;
static int wood = 8;
static int woodstairsleft = 9;
static int woodstairsright = 10;
static int controlPanel = 11;
static int log = 12;
static int avatar = 13;
static int bullet = 14;
static int flag = 15;
static int tank = 16;
static int cracked = 17;
static int cactus = 18;
static int tankstone = 19;
static int crate = 20;
	
	static public String getBlockTexture(int id)
	{
		if (id == air)
		{
			return "air";
		}
		if (id == dirt)
		{
			return "dirt";
		}
		if (id ==ladder)
		{
			return "ladder";
		}
		if (id == grass)
		{
			return "grass";
		}
		if (id == doorOut)
		{
			return "door";
		}
		if (id == wood)	
		{
			return "wood";
		}
		if (id == woodstairsright)	
		{
			return "woodstairright";
		}
		if (id == woodstairsleft)	
		{
			return "woodstairleft";		
		}
		if (id == controlPanel)	
		{
			return "controlpanel";
			
		}
		if (id == controlPanel)	
		{
			return "controlpanel";	
		}
		if (id == log)
		{
			return "log";
		}
		return "unknown";

		
	}
	static public boolean isWalkable(int id)
	{
		
		if (id == ladder || id == air || id == doorOut || id == woodstairsleft || id == woodstairsright || id == controlPanel)
		{
			return true;
		}
		return false;
	}

	
	public static FlamePao inBlockGravity(int id) {
		
		if (id == ladder)
		{
			return new FlamePao (0, 0, 0);
		}
		if (id == woodstairsleft || id == woodstairsright)
		{
			return new FlamePao (0, -1, 0);
		}
		return new FlamePao (0, 1, 0);
	}
	
	static public int loot_wood (int id) { return 1;}
	static public int loot_stone (int id){return 1; }
	static public int loot_mineral (int id){return 1;}
	
	static public int cost_wood (int id) { return 1;}
	static public int cost_stone (int id){return 1; }
	static public int cost_mineral (int id){return 1;}
	
	
	
	
	
}	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

