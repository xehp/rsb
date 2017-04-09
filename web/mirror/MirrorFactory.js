// MirrorFactory.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb, all rights reserved.
// Created 2015-01-30 by Henrik Bjorkman


function MirrorFactory(world, parent, cType, arg)
{
	/*if (parent==null)
	{
		console.log("parent is null, cType " + cType+" "+arg);
	}
	else
	{
		console.log("parent.id "+parent.id +", cType " + cType+" "+arg);
	}*/

	if (cType.length>0)
	{
		var ch = cType.charAt(0);
		switch(ch)
		{

			case 'A':
			{
				if (cType == 'ActivePlayerList')
				{
					return new ActivePlayerList(world, parent, arg);		
				}
				else if (cType == 'AvatarPlayerReferences')
				{
					return new AvatarPlayerReferences(world, parent, arg);		
				}
				break;
			}
			case 'E':
			{
				if (cType == 'EmpireUnit')
				{
					return new EmpUnit(world, parent, cType, arg);
				}
				else if (cType == 'EmpireSector')
				{
					return new EmpSector(world, parent, cType, arg);
				}
				else if (cType == 'EmpireOrder')
				{
					return new EmpOrder(world, parent, cType, arg);
				}
				else if (cType == 'EmpireTerrain')
				{
					return new EmpTerrain(world, parent, cType, arg);
				}
				else if ((cType == 'EmpireState') || (cType == 'EmpireNation'))
				{
					return new EmpState(world, parent, cType, arg);
				}
				else if (cType == 'EmpireRoundBuffer')
				{
					return new EmpRoundBuffer(world, parent, cType, arg);
				}
				else if ((cType == 'EmpireStatesList') || (cType == 'EmpireNationsList'))
				{
					return new EmpStatesList(world, parent, cType, arg);
				}
				else if (cType == 'EmpireUnitType')
				{
					return new EmpUnitType(world, parent, cType, arg);
				}
				else if (cType == 'EmpireUnitTypeList')
				{
					return new EmpUnitTypeList(world, parent, cType, arg);
				}
				else if (cType == 'EmpireWorld')
				{
					return new EmpWorld(world, parent, cType, arg);
				}
				else if (cType == 'EmpireMoney')
				{
					return new EmpMoney(world, parent, cType, arg);
				}

				break;
			}
				
			case 'F':
			{
				// For the Flame Engine (based on Drekkar Engine).
				if (cType == 'FlameWorld')
				{
					return new HmegWorld(world, parent, arg);
				}
				else if (cType == 'FlameRoom')
				{
					return new HmegRoom(world, parent, arg);
				}
				else if (cType == 'FlameEntity')
				{
					return new HmegEntity(world, parent, arg);
				}
				else if (cType == 'FlameAvatar')
				{
					return new HmegAvatar(world, parent, arg);
				}
				else if (cType == 'FlameUser')
				{
					return new AvatarPlayerReferences(world, parent, arg);		
				}
				else if (cType == "FlameScripted")
				{
					return new HmegEntity (world, parent, arg);
				}

				break;
			}

			case 'H':
			{
				if (cType == 'HmegCrate')
				{
					return new HmegEntity (world, parent, arg);
				}
				// For tankerstrike TODO: Auto detect
				else if (cType == 'HmegBullet')
				{
					return new HmegEntity (world, parent, arg);	
				}
				else if (cType == 'HmegFlag')
				{
					return new HmegEntity (world, parent, arg);	
				}

				break;
			}

			case 'R':
			{
				if ((cType == 'RsbLong') || (cType == 'EmpireMoney'))
				{
					return new MirrorLong(world, parent, arg);
				}
				else if (cType == 'RsbString')
				{
					return new MirrorString(world, parent, arg);
				}
				else if (cType == 'RsbRoundBuffer')
				{
					return new MirrorRoundBuffer(world, parent, arg);
				}
				/*else if (cType == 'RsbBigBitMap')
				{
					return new MirrorBigBitMap(world, parent, arg);
				}*/

				break;
			}
			default:
				break;
	

		}


	}

	
	//console.log("unknown cType " + cType+" "+arg);
	console.log("unknown cType " + cType);


	return null;
}

