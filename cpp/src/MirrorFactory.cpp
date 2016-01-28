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

#include "includeGlEtc.h"
#include "WordReader.h"

#include "ActivePlayerList.h"
#include "AvatarPlayerReferences.h"
#include "RsbLong.h"
#include "RsbString.h"
#include "RsbInvItem.h"
#include "RsbRoundBuffer.h"
#include "RoboGameWorldRoot.h"
#include "RoboGameWorld.h"
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
		case 'A':
		{
			if (t.equalsIgnoreCase("ActivePlayerList"))
			{
				bo = new ActivePlayerList();
			}
			else if (t.equalsIgnoreCase("AvatarPlayerReferences"))
			{
				bo = new AvatarPlayerReferences();
			}
			break;
		}
		case 'R':
		{
			char ch2=typeName[3];
			switch(ch2)
			{
				case 'o':
					if (t.equalsIgnoreCase("RoboGameWorldRoot"))
					{
						bo = new RoboGameWorldRoot();
					}
					else if (t.equalsIgnoreCase("RoboGameWorld"))
					{
						bo = new RoboGameWorld();
					}
					break;

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
		const char* n = bo->getTypeName();
		if (strcmp(typeName, n)!=0)
		{
			printf("getTypeName failed for %s, %s\n", typeName, n);
			assert(strcmp(typeName, n)==0);
		}
	}

	return bo;
}
