/*
SymMap.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created July 2015 by Henrik Bjorkman www.eit.se

*/

class SymMap
{
public:


    // internal key codes
    // codes used for keys outside the range ((i>' ') && (i<='~'))
	// The server does not know SDL so we remap all key codes to an internal format that our server knows.
    // TODO remove these and use the SDL codes. Server must be updated also when doing that.
	enum
	{
		specUnknown=0,
		specBackspace=8, // this is nr 8 in ascii table
		specTab=9, // 9
		specRightShift=11,
		specEnter=13, // 13
		specRightCtrl=15,
		specLeftShift=16,
		specLeftCtrl=17,
		specEsc=27, // 27
		specSpace=32, // 32
		specPageUp=33,
		specPageDown=34,
		specEnd=35,
		specHome=36,
		specRight=37, // should be 39?
		specUp=38,
		specLeft=39, // should be 37?
		specDown=40,


		spec0=48,  // 48 in ascii table
		spec1=49,
		spec2=50,
		spec3=51,
		spec4=52,
		spec5=53,
		spec6=54,
		spec7=55,
		spec8=56,
		spec9=57,

		specA=65, // 65 in ascii
		specB=66,
		specC=67,
		specD=68,
		specE=69,
		specF=70,
		specG=71,
		specH=72,
		specI=73,
		specJ=74,
		specK=75,
		specL=76,
		specM=77,
		specN=78,
		specO=79,
		specP=80,
		specQ=81,
		specR=82,
		specS=83,
		specT=84,
		specU=85,
		specV=86,
		specW=87,
		specX=88,
		specY=89,
		specZ=90,

		specNumPad0=96,
		specNumPad1=97,
		specNumPad2=98,
		specNumPad3=99,
		specNumPad4=100,
		specNumPad5=101,
		specNumPad6=102,
		specNumPad7=103,
		specNumPad8=104,
		specNumPad9=105,
		specNumPadPlus=107,
		specNumPadDel=108,
		specNumPadMinus=109,
		specNumPadDiv=111,

		specF1=112,
		specF2=113,
		specF3=114,
		specF4=115,
		specF5=116,
		specF6=117,
		specF7=118,
		specF8=119,
		specF9=110,
		specF10=121,
		specF11=122,
		specF12=123,
		specPlus=171,
		specStar=222,
		specMinus=173,
		specAltGr=225

	};


	// These enums will hopefully match the keyboardMappings in PlayerConnectionThread.java or PlayerData.java
	enum
	{
		undefinedKey=0,
		actionKey=1,
		backOrCancel=2,
		enterText=5,
		jumpUp=6,
		moveForward=7,
		moveBackwards=8,
		moveRight=9,
		moveLeft=10,
		showScore=12,
		//plusCode_deprecated=13,
		//minusCode_deprecated=14,
		mouseCapture=15,
		endCode=16,
		rollRight_deprecated=17,
		rollLeft_deprecated=18,
		yawRight_deprecated=19,
		yawLeft_deprecated=20,
		//swapTeam=21,
		pitchUp_deprecated=22,
		pitchDown_deprecated=23,
		testBeep=24,
		clearText=26,
		upOne=27,
		downOne=28,
		leftOne=29,
		rightOne=30,
		pageUp=31,
		pageDown=32,
		returnCode=33,
		mouseRelease=34,
		crouchOrDuck=35,
		invScreen=36
	} actionCodes;


	SymMap()
	{
		clearSymMap();
	}



	// See also symMapgetSymFromName in PlayerData.java
	static int getActionCodeFromActionName(const char * cmd)
	{
		const int ch = cmd[0];
		switch(ch)
		{
			case 'a':
				if (strcmp(cmd, "actionKey")==0)
				{
					// Server must send avatarAck after it receives an avatarPos
					return actionKey;
				}
				break;
			case 'b':
				if (strcmp(cmd, "backOrCancel")==0)
				{
					return backOrCancel;
				}
				break;
			case 'c':
				if (strcmp(cmd, "clearText")==0)
				{
					return clearText;
				}
				else if (strcmp(cmd, "crouchOrDuck")==0)
				{
					return crouchOrDuck;
				}
				break;
			case 'd':
				if (strcmp(cmd, "downOne")==0)
				{
					return downOne;
				}
				break;

			case 'e':
				if (strcmp(cmd, "enterText")==0)
				{
					return enterText;
				}
				else if (strcmp(cmd, "endCode")==0)
				{
					return endCode;
				}
				break;

			case 'i':
				if (strcmp(cmd, "invScreen")==0)
				{
					// Server must send avatarAck after it receives an avatarPos
					return invScreen;
				}
				break;

			case 'j':
				if (strcmp(cmd, "jumpUp")==0)
				{
					// Server must send avatarAck after it receives an avatarPos
					return jumpUp;
				}
				break;

			case 'l':
				if (strcmp(cmd, "leftOne")==0)
				{
					// Server must send avatarAck after it receives an avatarPos
					return leftOne;
				}
				break;

			case 'm':
				if (strcmp(cmd, "moveForward")==0)
				{
					// Server must send avatarAck after it receives an avatarPos
					return moveForward;
				}
				else if (strcmp(cmd, "moveBackwards")==0)
				{
					return moveBackwards;
				}
				else if (strcmp(cmd, "moveRight")==0)
				{
					return moveRight;
				}
				else if (strcmp(cmd, "moveLeft")==0)
				{
					return moveLeft;
				}
				else if (strcmp(cmd, "mouseCapture")==0)
				{
					return mouseCapture;
				}
				/*else if (strcmp(cmd, "minusCode")==0)
				{
					return minusCode_deprecated;
				}*/
				else if (strcmp(cmd, "mouseRelease")==0)
				{
					return mouseRelease;
				}
				break;

			case 'p':
				/*if (strcmp(cmd, "plusCode")==0)
				{
					return plusCode_deprecated;
				}*/
				if (strcmp(cmd, "pitchUp")==0)
				{
					return pitchUp_deprecated;
				}
				if (strcmp(cmd, "pitchDown")==0)
				{
					return pitchDown_deprecated;
				}
				if (strcmp(cmd, "pageUp")==0)
				{
					return pageUp;
				}

				if (strcmp(cmd, "pageDown")==0)
				{
					return pageDown;
				}
				break;
			case 'r':
				if (strcmp(cmd, "rollRight")==0)
				{
					return rollRight_deprecated;
				}
				if (strcmp(cmd, "rollLeft")==0)
				{
					return rollLeft_deprecated;
				}
				if (strcmp(cmd, "rightOne")==0)
				{
					return rightOne;
				}
				if (strcmp(cmd, "returnCode")==0)
				{
					return returnCode;
				}
				break;

			case 's':
				if (strcmp(cmd, "showScore")==0)
				{
					return showScore;
				}
				/*else if (strcmp(cmd, "swapTeam")==0)
				{
					return swapTeam;
				}*/
				break;
			case 't':
				if (strcmp(cmd, "testBeep")==0)
				{
					return testBeep;
				}
				break;
			case 'u':
				if (strcmp(cmd, "upOne")==0)
				{
					return upOne;
				}
				break;

			case 'y':
				if (strcmp(cmd, "yawRight")==0)
				{
					return yawRight_deprecated;
				}
				if (strcmp(cmd, "yawLeft")==0)
				{
					return yawLeft_deprecated;
				}
				break;
			default:
				break;
		}
		printf("did not find %s\n", cmd);
		return 0;
	}

	void set(int c, int sym)
	{
		if ((sym>='A') && (sym<='Z'))
		{
			symMap[sym-'A'+'a']=c;
		}
		if ((sym>='a') && (sym<='z'))
		{
			symMap[sym-'a'+'A']=c;
		}

		symMap[sym]=c;
	}

	void set(const char* prefName, int sym)
	{
		if ((sym>=0) && (sym<SIZEOF_ARRAY(symMap)))
		{
			const int c=SymMap::getActionCodeFromActionName(prefName);

			set(c, sym);
		}
		else
		{
			printf("sym out of range for %s\n", prefName);
		}
	}


	void set(const char* prefName, const char* prefValue)
	{
		int sym=atoi(prefValue);
		set(prefName, sym);
	}


	void clearSymMap()
	{
		for(int i=0; i<SIZEOF_ARRAY(symMap); ++i)
		{
			symMap[i]=0;
		}

	}

	// Translates SDL key codes to those used by our java server (it does not know about SDL unfortunately)
	int getSpec(int key)
	{
		switch(key)
		{
			case SDLK_UP:
				return specUp;
			case SDLK_DOWN:
				return specDown;
			case SDLK_LEFT:
				return specLeft;
			case SDLK_RIGHT:
				return specRight;
			case SDLK_PAGEUP:
				return specPageUp;
			case SDLK_PAGEDOWN:
				return specPageDown;
			case SDLK_HOME:
				return specHome;
			case SDLK_END:
				return specEnd;
			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				return specEnter;
			case SDLK_SPACE:
				return specSpace;
			case SDLK_ESCAPE:
				return specEsc;
			case SDLK_BACKSPACE:
				return specBackspace;
			case SDLK_TAB:
				return specTab;
			case SDLK_LSHIFT:
				return specLeftShift;
			case SDLK_RSHIFT:
				return specRightShift;
			case SDLK_LCTRL:
				return specLeftCtrl;
			case SDLK_RCTRL:
				return specRightCtrl;
			case SDLK_F1:
				return specF1;
			case SDLK_F2:
				return specF2;
			case SDLK_F3:
				return specF3;
			case SDLK_F4:
				return specF4;
			case SDLK_F5:
				return specF5;
			case SDLK_F6:
				return specF6;
			case SDLK_F7:
				return specF7;
			case SDLK_F8:
				return specF8;
			case SDLK_F9:
				return specF9;
			case SDLK_F10:
				return specF10;
			case SDLK_F11:
				return specF11;
			case SDLK_F12:
				return specF12;
			case SDLK_KP_PERIOD:
				return specNumPadDel;
			case SDLK_KP_DIVIDE:
				return specNumPadDiv;
			case SDLK_KP_MULTIPLY:
				return specStar;
			case SDLK_KP_MINUS:
				return specNumPadMinus;
			case SDLK_KP_PLUS:
				return specNumPadPlus;

			default:
				if ((key>=0) && (key<SIZEOF_ARRAY(symMap)))
				{
					// For compatibility web <-> native translate all letters to upper case if it is a native build.
					#ifndef __EMSCRIPTEN__
					if ((key>='a') && (key<='z'))
					{
						return key-('a'-'A');
					}
					#endif

					return key;
				}
				break;
		}
		return 0;
	}


	// This will give the action code from the internal key code
	int remap(int specKey)
	{
		assert((specKey>=0) && (specKey<SIZEOF_ARRAY(symMap)));

		return symMap[specKey];
	}

	int getSymMap(unsigned int i)
	{
		if (i<SIZEOF_ARRAY(symMap))
		{
			return symMap[i];
		}
		return -1;
	}

private:
	int symMap[256];

	//int reverseMap[128];

};
