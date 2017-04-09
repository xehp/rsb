/*
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license
*/
package se.eit.rsb_factory_pkg;

import java.util.Hashtable;

import se.eit.LoginLobbyPkg.LoginLobbyAvatar;
import se.eit.LoginLobbyPkg.LoginLobbyWorld;
import se.eit.d1_pkg.*;
//import se.eit.drekkarGamePkg.*; // This is not needed in 2d versions such as flame & empire
import se.eit.empire_package.*;
import se.eit.flameengine_package.*;
import se.eit.rsb_package.*;
//import se.eit.yukigassen_pkg.*; // This is not needed in 2d versions such as flame & empire

public class RsbFactory {
	
	// Where all classes that can be created by factory will be stored. 
	// "? extends D1Base" means anything that extends D1Base recursively or directly.
	private static Hashtable<String, Class<? extends D1Base>> factoryClasses = new Hashtable<String, Class<? extends D1Base>>();
	
	
	public static void static_error(String str)
	{
		System.out.flush();
		System.err.println(className()+": static_error: "+str);
		Thread.dumpStack();
		System.exit(1);
	}
	
	public static void debug(String str)
	{
		System.out.println(className()+": static_error: "+str);
	}
	
	public static String className()
	{	
		// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
		return RsbFactory.class.getSimpleName();	
	}
	
	// TODO: Add prefix. So that names don't collide!
	public static void registerClass (Class<? extends D1Base> classTmp)
	{
		
		String name = classTmp.getSimpleName();
		
		// Check that not already registered
		if (factoryClasses.containsKey(name))
		{
			static_error("Name \""+name+"\" is already used");
			return;
		}
		factoryClasses.put(name, classTmp);
		
		// Debug
		System.out.println("Registering class: "+name);
	}
	
	// Used if you need to alias the class.
	public static void registerClass (Class<? extends D1Base> classTmp, String name)
	{
		factoryClasses.put(name, classTmp);
		
		// Check that not already registered
		if (factoryClasses.containsKey(name))
		{
			static_error("Name \""+name+"\"is already used");
			return;
		}
				
		// Debug
		System.out.println("Registering class: "+name);
	}
	
	// All database object classes that can be saved to disk or sent needs to be listed here. 
	// Thats all classes that inherit DbBase shall be listed here.
	public static D1Base createObj(String t) throws NumberFormatException
	{
		
		// Technically only think that is needed.
		Class<? extends D1Base> c = factoryClasses.get(t);
		if (c != null)
		{
			try
			{
				// Instantiate
				return (D1Base) c.newInstance();
			}
			catch (InstantiationException e)
			{
				System.out.println(e);
			}
			catch (IllegalAccessException e)
			{
				System.out.println(e);
			}
		}
		
	
		// The following should be replaced by new method!
		
		// Here all classes that do send something from their listChangedObjects methods needs to be handled.		
		final char ch=t.charAt(0);
		switch(ch)
		{
			case 'A':
			{
				if (t.equalsIgnoreCase("ActivePlayerList"))
				{
					return new ActivePlayerList();
				}
				else if (t.equalsIgnoreCase("AvatarPlayerReferences"))
				{
					return new GameUser();
				}
				break;				
			}
			case 'D':
			{
				final char ch2=t.charAt(1); 
				
				switch(ch2)
				{
					case 'b':
					case '1':
					{
						if ((t.equalsIgnoreCase("D1SubRoot")) || (t.equalsIgnoreCase("DbSubRoot")))
						{
							return new D1SubRoot();
						}
						else if ((t.equalsIgnoreCase("D1NoSaveRoot")) || (t.equalsIgnoreCase("DbNoSaveRoot")))
						{
							return new D1NoSaveRoot();
						}
						break;
					}
					case 'r':
 					{
 						// This is not needed in 2d versions such as flame & empire
						final char ch3=t.charAt(11);
						switch(ch3)
						{
							/*case 'A':
								if (t.equalsIgnoreCase("DrekkarGameAvatar"))
								{
									return new DrekkarGameAvatar();
								}
								break;*/
							
							/*case 'E':
								if (t.equalsIgnoreCase("DrekkarGameEntity"))
								{
									return new DrekkarGameEntity();
								}
								break;*/
								// Server is instead added in serverFactory in class PlayerConnectionThread
							/*case 'S':
								if (t.equalsIgnoreCase("DrekkarGameServer"))
								{
									return new DrekkarGameServer();
								}
								break;*/
							/*case 'W':
								if (t.equalsIgnoreCase("DrekkarGameWorld"))
								{
									return new DrekkarGameWorld();
								}
								break;*/
							default:
							{
								break;
							}
						}					
						break;
					}
					default:
					{
						break;
					}
				}
				
				/*else if (t.equalsIgnoreCase("DrekkarWorld"))
				{
					return new DrekkarWorld(); 
				}*/
				break;				
			}
			case 'E':
			{		
				final char ch2=t.charAt(6);
				switch(ch2)
				{
						
					case 'M':
						if (t.equalsIgnoreCase("EmpireMoney"))
						{
							return new EmpireMoney();
						}
						break;
					case 'N':
						if (t.equalsIgnoreCase("EmpireNation"))
						{
							return new EmpireState();
						}
						if (t.equalsIgnoreCase("EmpireNationsList"))
						{
							return new EmpireStatesList();
						}
						if (t.equalsIgnoreCase("EmpireNation"))
						{
							return new EmpireState();
						}
						break;
					case 'O':
						if (t.equalsIgnoreCase("EmpireOrder"))
						{
							return new EmpireOrder();
						}
						break;
					case 'R':
						if (t.equalsIgnoreCase("EmpireRoundBuffer"))
						{
							return new EmpireRoundBuffer();
						}
						break;
					case 'S':
						if (t.equalsIgnoreCase("EmpireSector"))
						{
							return new EmpireSector();
						}
						if (t.equalsIgnoreCase("EmpireState"))
						{
							return new EmpireState();
						}
						if (t.equalsIgnoreCase("EmpireStatesList"))
						{
							return new EmpireStatesList();
						}
						break;
					case 'T':
						if (t.equalsIgnoreCase("EmpireTerrain"))
						{
							return new EmpireTerrain();
						}			
						break;
					case 'W':
						if (t.equalsIgnoreCase("EmpireWorld"))
						{
							return new EmpireWorld();
						}
						break;
					case 'U':
						if (t.equalsIgnoreCase("EmpireUnit"))
						{
							return new EmpireUnit();
						}
						if (t.equalsIgnoreCase("EmpireUnitTypeList"))
						{
							return new EmpireUnitTypeList();
						}
						if (t.equalsIgnoreCase("EmpireUnitType"))
						{
							return new EmpireUnitType();
						}
						break;
					default:
						break;
				}
				break;	
			}
			
			// For Flame.
			case 'F':
			{
				if (t.equalsIgnoreCase("FlameWorld"))
				{
					return new FlameWorld();
				}
				//else if (t.equalsIgnoreCase("FlameRoom"))
				//{
				//	return new FlameRoom();
				//}
				else if (t.equalsIgnoreCase("FlameEntity"))
				{
					return new FlameEntity();
				}
				else if (t.equalsIgnoreCase("FlameAvatar"))
				{
					return new FlameAvatar();
				}
				else if (t.equalsIgnoreCase("FlameUser"))
				{
					return new FlameUser();
				}
				break;
			}
			
			case 'G':
			{
				if (t.equalsIgnoreCase("GameUser"))
				{
					return new GameUser();
				}
				/*
				if (t.equalsIgnoreCase("GameBase"))
				{
					return new GameBase();
				}
				*/
				break;				
			}
			case 'H':
			{

				if (t.equalsIgnoreCase("HmegBullet"))
				{
					return new HmegBullet ();
				}
				else if (t.equalsIgnoreCase("HmegFlag"))
				{
					return new HmegFlag();
				}
				break;				
			}
			case 'L':
			{				
				/*if (t.equalsIgnoreCase("LoginAvatar"))
				{
					return new LoginAvatar();
				}*/
				if (t.equalsIgnoreCase("LoginLobbyWorld"))
				{
					return new LoginLobbyWorld();
				}
				if (t.equalsIgnoreCase("LoginLobbyAvatar"))
				{
					return new LoginLobbyAvatar();
				}				
				break;				
			}
			case 'P':
			{
				if (t.equalsIgnoreCase("PlayerData"))
				{
					return new UserAccount();
				}
				break;				
			}
			case 'R':
			{
				final char ch2=t.charAt(3);
				switch(ch2)
				{
					case 'I':
						if (t.equalsIgnoreCase("RsbInvItem"))
						{
							return new RsbInvItem();
						}
						break;
					case 'L':
						if (t.equalsIgnoreCase("RsbLong"))
						{
							return new RsbLong();
						}
						break;
					case 'B':
						if (t.equalsIgnoreCase("RsbBigBitMap"))
						{
							return new RsbBigBitMap();
						}
						break;
					case 'R':
						if (t.equalsIgnoreCase("RsbRoundBuffer"))
						{
							return new RsbRoundBuffer();
						}
						break;
					case 'S':
						if (t.equalsIgnoreCase("RsbString"))
						{
							return new RsbString();
						}
						break;
					default: break;
				}
				break;				
			}
			case 'U':
			{
				if (t.equalsIgnoreCase("UserAccount"))
				{
					return new UserAccount();
				}
				break;				
			}
			/*case 'Y':
			{
				// This is not needed in 2d versions such as flame & empire
				final char ch2=t.charAt(10);
				switch(ch2)
				{
				case 'A':
					if (t.equalsIgnoreCase("YukigassenAvatar"))
					{
						return new YukigassenAvatar();
					}
					break;
				case 'B':
					if (t.equalsIgnoreCase("YukigassenBlockRoom"))
					{
						return new YukigassenBlockRoom();
					}
					if (t.equalsIgnoreCase("YukigassenBillboard"))
					{
						return new YukigassenBillboard();
					}
					break;
				case 'C':
					if (t.equalsIgnoreCase("YukigassenChunkRoom"))
					{
						return new YukigassenChunkRoom();
					}
				case 'E':			
					if (t.equalsIgnoreCase("YukigassenEntity"))
					{
						return new YukigassenEntity();
					}
					break;
				case 'I':			
					if (t.equalsIgnoreCase("YukigassenInventory"))
					{
						return new YukigassenInventory();
					}
					break;
				case 'R':
					if (t.equalsIgnoreCase("YukigassenRoom"))
					{
						return new YukigassenRoom();
					}
					break;
				case 'T':
					if (t.equalsIgnoreCase("YukigassenTexturedRoom"))
					{
						return new YukigassenTexturedRoom();
					}
					if (t.equalsIgnoreCase("YukigassenTopRoom"))
					{
						return new YukigassenTopRoom();
					}					
					break;
				case 'V':
					if (t.equalsIgnoreCase("YukigassenVoxelMap"))
					{
						return new YukigassenVoxelMap();
					}
					break;
				case 'W':
					if (t.equalsIgnoreCase("YukigassenWorld"))
					{
						return new YukigassenWorld();
					}
					break;
				}
				break;				
			}*/
			case '}':
			{
				if (t.equalsIgnoreCase("}"))
				{
					// This was an end marker, there are no more sub objects.
					return null;
				}
				break;				
			}
			default:
			{
				 // do nothing, error is reported further down.
			}
		}
		
		static_error("unknown object "+t);
		throw(new NumberFormatException("unknown object '"+t+"'"));			
	}

}
