/*
MirrorFactory.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "includeGlEtc.h"
#include "WordReader.h"

#ifndef RSB_SERVER
#include "ActivePlayerList.h"
#include "GameUser.h"
#endif
#include "RsbLong.h"
#include "RsbString.h"
#include "RsbInvItem.h"
#include "RsbRoundBuffer.h"
#ifdef EMPIRE_CLIENT
#include "EmpireOrder.h"
#include "EmpireMoney.h"
#include "EmpireRoundBuffer.h"
#include "EmpireSector.h"
#include "EmpireState.h"
#include "EmpireStatesList.h"
#include "EmpireTerrain.h"
#include "EmpireUnit.h"
#include "EmpireUnitOrSector.h"
#include "EmpireUnitType.h"
#include "EmpireUnitTypeList.h"
#include "EmpireWorld.h"
#endif
#ifndef RSB_SERVER
/*
#include "YukigassenAvatar.h"
#include "YukigassenVoxelMap.h"
#include "YukigassenBlockRoom.h"
#include "YukigassenChunkRoom.h"
#include "YukigassenEntity.h"
#include "YukigassenInventory.h"
#include "YukigassenBillboard.h"
#include "YukigassenRoom.h"
#include "YukigassenTexturedRoom.h"
#include "YukigassenWorld.h"
#include "YukigassenTopRoom.h"
*/
#endif
#include "MirrorFactory.h"



static void unknownType(const char *typeName)
{
	printf("mirrorFactoryCreate: unknown type %s\n", typeName);
	assert(0);
}


// This method is a factory, it is the responsibility of the caller to do delete.
MirrorBase *mirrorFactoryCreate(const char* typeName)
{
	//const char* typeName = ws->getNextString();

	const int ch = *typeName;

	WordReader t(typeName);

	MirrorBase *bo = NULL;

	switch(ch)
	{
#ifndef RSB_SERVER
		case 'A':
		{
			if (t.equalsIgnoreCase("ActivePlayerList"))
			{
				bo = new ActivePlayerList();
			}
			else if (t.equalsIgnoreCase("AvatarPlayerReferences"))
			{
				bo = new GameUser();
			}
			break;
		}
#endif
#ifdef EMPIRE_CLIENT
		case 'E':
		{
			const char ch2=typeName[6];
			switch(ch2)
			{
				case 'M':
					if (t.equalsIgnoreCase("EmpireMoney"))
					{
						bo = new EmpireMoney();
					}
					break;
				case 'O':
					if (t.equalsIgnoreCase("EmpireOrder"))
					{
						bo = new EmpireOrder();
					}
					break;
				case 'R':
					if (t.equalsIgnoreCase("EmpireRoundBuffer"))
					{
						bo = new EmpireRoundBuffer();
					}
					break;
				case 'S':
					if (t.equalsIgnoreCase("EmpireSector"))
					{
						bo = new EmpireSector();
					}
					else if (t.equalsIgnoreCase("EmpireState"))
					{
						bo = new EmpireState();
					}
					else if (t.equalsIgnoreCase("EmpireStatesList"))
					{
						bo = new EmpireStatesList();
					}
					break;
				case 'T':
					if (t.equalsIgnoreCase("EmpireTerrain"))
					{
						bo = new EmpireTerrain();
					}
					break;

				case 'U':
					if (t.equalsIgnoreCase("EmpireUnit"))
					{
						bo = new EmpireUnit();
					}
					else if (t.equalsIgnoreCase("EmpireUnitType"))
					{
						bo = new EmpireUnitType();
					}
					else if (t.equalsIgnoreCase("EmpireUnitTypeList"))
					{
						bo = new EmpireUnitTypeList();
					}
					else if (t.equalsIgnoreCase("EmpireUnitOrSector"))
					{
						bo = new EmpireUnitOrSector();
					}
					break;
				case 'W':
					if (t.equalsIgnoreCase("EmpireWorld"))
					{
						bo = new EmpireWorld();
					}
					break;
				default:
					break;
			}
			break;
		}
#endif
#ifndef RSB_SERVER
		case 'G':
		{
			if (t.equalsIgnoreCase("GameUser"))
			{
				bo = new GameUser();
			}
			break;
		}
#endif
		case 'R':
		{
			char ch2=typeName[3];
			switch(ch2)
			{
				case 'I':
					if (t.equalsIgnoreCase("RsbInvItem"))
					{
						bo = new RsbInvItem();
					}
					break;
				case 'L':
					if (t.equalsIgnoreCase("RsbLong"))
					{
						bo = new RsbLong();
					}
					break;
				case 'R':
					if (t.equalsIgnoreCase("RsbRoundBuffer"))
					{
						bo = new RsbRoundBuffer();
					}
					break;
				case 'S':
					if (t.equalsIgnoreCase("RsbString"))
					{
						bo = new RsbString();
					}
					break;
				default:
					break;
			}
			break;
		}
#ifndef RSB_SERVER
		/*
		case 'Y':
		{
			const char ch2=typeName[10];
			switch(ch2)
			{
				case 'A':
					if (t.equalsIgnoreCase("YukigassenAvatar"))
					{
						bo = new YukigassenAvatar();
					}
					break;
				case 'B':
					if (t.equalsIgnoreCase("YukigassenBlockRoom"))
					{
						bo = new YukigassenBlockRoom();
					}
					else if (t.equalsIgnoreCase("YukigassenBillboard"))
					{
						bo = new YukigassenBillboard();
					}
					break;
				case 'C':
					if (t.equalsIgnoreCase("YukigassenChunkRoom"))
					{
						bo = new YukigassenChunkRoom();
					}
					break;
				case 'E':
					if (t.equalsIgnoreCase("YukigassenEntity"))
					{
						bo = new YukigassenEntity();
					}
					break;
				case 'I':
					if (t.equalsIgnoreCase("YukigassenInventory"))
					{
						bo = new YukigassenInventory();
					}
					break;
				case 'R':
					if (t.equalsIgnoreCase("YukigassenRoom"))
					{
						bo = new YukigassenRoom();
					}
					break;
				case 'T':
					if (t.equalsIgnoreCase("YukigassenTexturedRoom"))
					{
						bo = new YukigassenTexturedRoom();
					}
					else if (t.equalsIgnoreCase("YukigassenTopRoom"))
					{
						bo = new YukigassenTopRoom();
					}
					break;
				case 'V':
					if (t.equalsIgnoreCase("YukigassenVoxelMap"))
					{
						bo = new YukigassenVoxelMap();
					}
					break;
				case 'W':
					if (t.equalsIgnoreCase("YukigassenWorld"))
					{
						bo = new YukigassenWorld();
					}
					break;
				default:
					break;
			}
			break;
		}
		*/
#endif
		default:
		{
			break;
		}
	}

	if (bo==NULL)
	{
		unknownType(typeName);
	}
	else
	{
		// This is to help debugging. make and error if the name of the object type that was created does not match the desired name, that is the class did not have "virtual const char* getTypeName()"
		const char* n = bo->getTypeName();
		if (strcmp(typeName, n)!=0)
		{
			printf("getTypeName failed for %s, %s\n", typeName, n);
			assert(strcmp(typeName, n)==0);
		}
	}

	return bo;
}
