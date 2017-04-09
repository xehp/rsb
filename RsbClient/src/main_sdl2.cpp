/*
hello_world_sdl2.cpp

The plan is to make a demo with some OpenGl/WebGl rendering with Emscripten using shaders.
Originally I had a version with direct rendering. This program will show how to upgrade from that to using shaders.



www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

The code here is made using tutorials, they have made much effort and so have I. So you can only use this code if you keep this text, all references (comments) and update the history section if you modify it. Or actually you mus go to the sources and take it from there. I have marked sources as much as possible


In OpenGL ES 2.0, to render any geometry to the scene, you have to create two tiny little programs called shaders.
Shaders are written in a C-like language called GLSL. More reading:

http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Table-of-Contents.html
http://www.arcsynthesis.org/gltut/
http://www.opengl-tutorial.org/
https://www.opengl.org/wiki/Getting_Started
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
http://nehe.gamedev.net/
https://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_%28C_/_SDL%29
http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
http://www.raywenderlich.com/3664/opengl-tutorial-for-ios-opengl-es-2-0
http://en.wikibooks.org/wiki/OpenGL_Programming
http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
http://www.lighthouse3d.com/tutorials/glsl-core-tutorial/color-example/


History
Created by Henrik Bjorkman 2014-11-09 using code and tutorials from internet (see list). NOTE! Their copyright may apply somewhat in some way.
2015-03-07 Trying again.


*/

#ifndef __EMSCRIPTEN__
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#endif

#include <assert.h>
#include <math.h>

#include "includeGlEtc.h"

#include "CreateProgram.h"
#include "Pyramid3dProgram.h"
#include "Box2DProgram.h"
#include "Cube3dProgram.h"
#include "Quad2dProgram.h"
#include "Texture2d5Program.h"

// #include <SOIL/SOIL.h> // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06

#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif

#include "HybridLib.h"
#include "WindowObject.h"
#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "DrawImage.h"
#include "WordReader.h"
#include "WordWriter.h"
#include "TextPage.h"
#include "TextPageScroll.h"
#include "MirrorBase.h"
#include "Pyramid3dProgram.h"
#include "ColorPyramid.h"
#include "Color2DBox.h"
#include "CrossHair.h"
#include "TexCube.h"
#include "Text2DBox.h"
#include "MirrorFactory.h"
#include "RsbRoundBuffer.h"
#include "RsbString.h"
#include "RsbInvItem.h"
#include "GameUser.h"
#include "YukigassenWorld.h"
#include "YukigassenInventory.h"
#include "YukigassenAvatar.h"
#include "YukigassenRoom.h"
#include "YukigassenChunkRoom.h"
#include "InputBuffer.h"
#include "SoundLoaderPlayer.h"
#include "SymMap.h"
#include "Shaders.h"
#include "version.h"
#include "main_sdl2.h"

///////////////////////// TODO: This is for testing ///////////////////////////////
#include "ModelObjLoader.h"
ModelObjLoader* modelLoaded;


// JavaScript shall not be multithreaded but there is some sort of problem in this aspect. This determines if we do assert or just log the problem. It shall be assert but...
//#define SOFT_ERROR






// 3D coordinates
// X is right
// Y is up (along the plane of the screen)
// Z is out of the screen (towards the user looking at the screen)

// 3D axis colors
// X : read
// Y : green
// Z : blue


enum GameType
{
	unknownGame=0,
	yukigassenGame=1,
	empireGame=2
};


enum
{
	JS_ARROW_LEFT=37,
	JS_ARROW_UP=38,
	JS_ARROW_RIGHT=39,
	JS_ARROW_DOWN=40,
};


int openConnection(const char *hostname, int port);
int closeConnection();






const D3Vector mainPos(2.0f, 0.5f, 10.0f); // We start with observer/camera 10 units above screen
const D3Vector mainNose(0.0f, 0.0f,-1.0f); // We start with observer/camera looking from above the screen into it, nose points into screen, as you are probably sitting now reading this.
const D3Vector mainHead(0.0f, 1.0f, 0.0f); // We start with observer/camera head upwards, as you are probably sitting now reading this.

D3PosAndOrientation mainCameraPao(mainPos, mainNose, mainHead);
D3PosAndOrientation mainSentPao(mainPos, mainNose, mainHead);
bool mainFreeMoveMode=false;
D3Vector aVelocity(0.0f, 0.0f, 0.0f);

int mainCounter = 0x7fffffff; // Set this to zero to exit/quit
float mainSpeedForward = 0.0f;
float mainSpeedRight = 0.0f;
float mainSpeedUp = 0.0f;
bool mainCrouch = false;
int mainTime = 0; // milliseconds







// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06







//TextPageScroll *textPage=NULL; // This is the regular text page, menu options, messages etc are shown here.
//TextPageScroll *tabPage=NULL; // This is the alternative text page, it is shown when user hold down the tab key.
InputBuffer *textInputBuffer=NULL;



// Text and 2d images
Text2DBox *hudScreenBox=NULL; // This is used to indicate if player was hit.
Text2DBox *amoTextBox=NULL; // This is used to show how many snow balls player have



ColorPyramid* colorPyramid=NULL;




//char strBuf[256]={0};


MirrorBase *rootDb=NULL;
MirrorIdList *globalDb=NULL; // All objects sent from server shall be stored here.
MirrorIdList *localDb=NULL; // Local objects (not from server) to be rendered are stored here.
//MirrorBase *renderBase=NULL;


int mirrorClientsSeqnr=0; // This is the latest client sequence number that we have used in a message to server.
int mirrorAckedClientsSeqnr=0; // This is for the latest client sequence number that the server have acknowledged.
int timeLastSending=0; // system time when last message sent to server, we must not send messages too often.
bool hitIndicationIsOn=false;
//int autoReplyPw=0;


// messages to be shown to the player are found here. Can we move this to TextPage?
int ourMessagesIndex=0;
int ourMessagesIndexWaitCount=0;

bool showPlayerStats=false;



// In localRenderMb things that are shown all time (regardless of ourAvatarId and localAvatar) are placed.
MirrorBase *localRenderMb=NULL;

MirrorBase * localAvatar=0; // Where in the local rendering DB we are looking. There is also the global avatar "ourAvatarId" which is where in the global DB the player is. Don't mix these two, the IDs are in different scopes.

// Here player preferences (not part of a game) are placed
// TODO Perhaps move this to a DB of its own and send the data over using mirror commands instead of playerPreference command. This would require some way of telling DBs apart when sent using mirror commands (don't want to increase overhead).
MirrorBase *playerPreferenceMb=NULL;

// Just a little box shown at login, was used to test things in the early beginning of the development of this client.
Color2DBox *colorBox;


int debugDetectMultiThreading=0; // This is for debugging only, it can be removed later when everything is runs perfectly.

bool groundContact=false; // Used to tell how (in what mode) a 3d avatar shall move.
int serverChangedPositionCounter=0; // Used to know if server has teleported avatar.

// These are used when changing connection to a game server from a login server
int playerIndex=-1;
int playerCode=-1;

// Used to translate key codes from SDL to our own codes.
SymMap symMap;

// Used to know if amoTextBox needs to be updated. See also amoTextBox.
int prevAvatarAmmo=0x7FFFFFF;

// Variables for status of keyboard
// Perhaps we should have a map for all keys (instead of just these below) so we can check if they are down or up.
static bool altGr=false;
//static bool shift=false;
//static bool ctrl=false;
static bool leftShift=false;
//static bool rightShift=false;
static bool leftCtrl=false;
//static bool rightCtrl=false;
static bool activateQuickAndDirtyTildeFix=false;

const char* fontFileBig="font16x32.png";
const char* fontFileSmall="font8x16.png";
GameType gameType = unknownGame;


void mainRender()
{

	// Clear The Screen And The Depth Buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );




	// Read more at: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/



	// TODO The avatar can be in a sub room in which case its pao must be translated to coordinates of this room first.
    // See also void D3ObjWithPos::renderAll()


	assert(rootDb!=NULL);

	// Here we render the 3D objects.
	// If avatar is not in the topmost room we need to translate its position into that.
	if (ourAvatarId<0)
	{
		// This is just used to render the colored pyramid background during login etc, we don't have an avatar.

		//const D3PosAndOrientation cameraPao(mainPos, mainNose, mainHead);
		const D3PosAndOrientation cameraPao(mainCameraPao);

		// Camera matrix
		View       = glm::lookAt(
									glm::vec3(cameraPao.pos.x,cameraPao.pos.y,cameraPao.pos.z),
									glm::vec3(cameraPao.pos.x+cameraPao.nose.x,cameraPao.pos.y+cameraPao.nose.y,cameraPao.pos.z+cameraPao.nose.z), // This is a point in the room that we look at.
									glm::vec3(cameraPao.head.x,cameraPao.head.y,cameraPao.head.z)  // Head is up (set to 0,-1,0 to look upside-down)
		);



		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model      = glm::mat4(1.0f); // This will not change anything so we can remove the "* Model" in the line below.



		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP        = Projection * View * Model;





		// What is GL_DEPTH_BUFFER_BIT in glClear? See: http://learnopengl.com/#!Advanced-OpenGL/Depth-testing
		// This is how it is calculated: https://www.opengl.org/sdk/docs/man/html/gl_FragDepth.xhtml
		// Now enable it:
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		//rootDb->renderAll();

	}
	else
	{
		// As soon as we are in a game and we have an avatar, this is used.

		assert(globalDb!=NULL);
		const MirrorBase *avatarMb=globalDb->getObjectById(ourAvatarId);
		D3PosAndOrientation renderPos(mainCameraPao);  // mainCameraPao is the avatars position given in the coordinates of the room avatar is in.
		MirrorBase *renderMb=avatarMb->getParentObj(); // This is the room for which we the renderPos is relevant.

		assert(avatarMb!=NULL);

		// If the parent of current room is a room then we can render from there, but need to translate the coordinates so they match.
		if (dynamic_cast<const YukigassenChunkRoom *>(renderMb->getParentObj())!=NULL)
		{
			const YukigassenRoom *renderYr=dynamic_cast<const YukigassenRoom *>(renderMb);
			renderYr->pao.translatePaoToSuper(&renderPos);
			renderMb = renderMb->getParentObj();
		}

		// We may want to step up even one more room. We could also have used a while loop here instead of doing it twice. But two steps up is enough.
		if (dynamic_cast<const YukigassenChunkRoom *>(renderMb->getParentObj())!=NULL)
		{
			const YukigassenRoom *renderYr=dynamic_cast<const YukigassenRoom *>(renderMb);
			renderYr->pao.translatePaoToSuper(&renderPos);
			renderMb = renderMb->getParentObj();
		}


		// Camera matrix
		View       = glm::lookAt(
									glm::vec3(renderPos.pos.x,renderPos.pos.y,renderPos.pos.z),
									glm::vec3(renderPos.pos.x+renderPos.nose.x,renderPos.pos.y+renderPos.nose.y,renderPos.pos.z+renderPos.nose.z), // This is a point in the room that we look at.
									glm::vec3(renderPos.head.x,renderPos.head.y,renderPos.head.z)  // Head is up (set to 0,-1,0 to look upside-down)
		);



		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model      = glm::mat4(1.0f); // This will not change anything so we can remove the "* Model" in the line below.



		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP        = Projection * View * Model;





		// What is GL_DEPTH_BUFFER_BIT in glClear? See: http://learnopengl.com/#!Advanced-OpenGL/Depth-testing
		// This is how it is calculated: https://www.opengl.org/sdk/docs/man/html/gl_FragDepth.xhtml

		// This is a feature to avoid spending rendering resources on faces that are not visible anyway.
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		YukigassenChunkRoom *renderYcr=dynamic_cast<YukigassenChunkRoom *>(renderMb);
		if (renderYcr!=NULL)
		{
			renderYcr->renderChunkRoom(&renderPos);
		}
		else if (renderMb!=NULL)
		{
			renderMb->renderAll();
		}

	}


	// In addition to the above 3D stuff we may also have some 2D to show in in the foreground.
	// This is used for messages, statistics and some other indications such as when a player was hit.
	// Hopefully there is no 3D objects inside this since if it is it may end up in strange places (to show 3d we must have camera position set).

	assert(localAvatar!=NULL);
	localAvatar->renderAll();

	assert(localRenderMb!=NULL);
	localRenderMb->renderAll();





	// Perhaps not needed but resetting the program used.
	glUseProgram(0);


	WindowObject *windowObject=rootDb->getWindowObject();
	windowObject->renderIt();
}













// This sends an avatarPos message to server.
// This is called from mainTick (and also from evaluateMessageFromServer).
static void updateAvatarToServer()
{
	WordWriter ww(512);
	ww.writeWord("avatarPos");
	mainCameraPao.writeSelf(&ww);
	aVelocity.writeSelf(&ww);
	ww.writeInt(serverChangedPositionCounter);

	ww.writeInt(mirrorClientsSeqnr);


	const char* str = ww.getString();
	mainSend(str);

	++mirrorClientsSeqnr;
	mainSentPao = mainCameraPao;


	// Server must reply to the avatarPos message with an avatarAck message. Or it might be with an mirrorUpdated message?
}




const int maxDeltaTime = 100; // max stepping of time is 100 ms


// The avatar position is updated locally to avoid the lag that updating via server would give
// TODO server side check that client does not make illegal moves.
static void localAvatarMovement(YukigassenAvatar *ya, const float deltaTime)
{
	const float margin = 0.2f;

	YukigassenRoom *ybr = dynamic_cast<YukigassenRoom*>(ya->getParentObj());
	assert(ybr!=NULL);

	D3Vector currentPos(mainCameraPao.pos); // Remember current position if move is not possible
	D3PosAndOrientation zPao(mainCameraPao); // Will later be set to the direction player is looking locked in the Z plane


	float bodyLength=YukigassenAvatar::noseToFeetLength;
	if (mainCrouch)
	{
		bodyLength*=0.5;
	}

	// To determine if avatar is on ground or in air we need to know how far it is to ground..
	const float distToGround = ybr->calcDistToGround(&currentPos, bodyLength*1.1f);

	// The server has not changed avatar position, it is OK for client to move it.
	if (groundContact || ((distToGround<bodyLength) && (aVelocity.z>=0.0f)))
	{
		// This avatar is in contact with the ground

		// TODO: We assume positive Z axis is down here. We should use the roomAcceleration vector in avatar parent room.


		// Get desired direction in the Z plain
		/*if ((zPao.nose.z>=0.999999f) || (zPao.nose.z<=-0.999999f))
		{
			zPao.nose.set(1.0f, 0.0f, 0.0f);
		}*/

		D3Vector zRight(zPao.nose, zPao.head);
		if (zPao.head.z<0)
		{
			zPao.head.set(0.0f, 0.0f, -1.0f);
			zRight.cross(zPao.nose, zPao.head);
			zPao.nose.cross(zPao.head, zRight);
		}

		// ground is within reach, reset velocity to zero and start over
		aVelocity.set(0.0f,0.0f,0.0f);

		float groundSpeed=(ya->avatarStateMs)?50.0f:25.0f;

		if (mainCrouch)
		{
			groundSpeed*=0.666667;
		}



		const float a=distToGround-bodyLength;

		if (a<0)
		{
			// Feet is under ground, climb up
			float m = -(deltaTime*groundSpeed*0.2f);
			if (a<m)
			{
				//printf("a %f, m %f\n", a, m);
				mainCameraPao.pos.z += m;
			}
			else
			{
				mainCameraPao.pos.z += a;
			}

			// Check that it is possible to climb up (nothing in the way)
			if (ybr!=NULL)
			{
				if (ybr->isInsideBlockMargin(&(mainCameraPao.pos), margin))
				{
					const bool currentIsInsideBlock=ybr->isInsideBlockMargin(&currentPos, margin);

					//printf("mainTick: rise inside\n");
					if (currentIsInsideBlock==false)
					{
						// rising up was not possible
						//printf("mainTick: rise was reverted\n");
						mainCameraPao.pos.set(currentPos);
						aVelocity.set(0.0f,0.0f,0.0f);
						if (mainSpeedUp>0)
						{
							aVelocity.add(mainSpeedUp*20.0f, zPao.head);
						}

					}
				}
			}
		}







		if (mainSpeedUp>0)
		{
			// jump
			aVelocity.add(mainSpeedUp*20.0f, zPao.head);
		}



		if ((mainSpeedForward!=0) && (mainSpeedRight!=0))
		{
			// forward and left/right
			aVelocity.add(mainSpeedForward*groundSpeed*0.7f, zPao.nose);
			aVelocity.add(mainSpeedRight*groundSpeed*0.7f, zRight);

		}
		else if (mainSpeedForward!=0)
		{
			// forward
			aVelocity.add(mainSpeedForward*groundSpeed, zPao.nose);

		}
		else if (mainSpeedRight!=0)
		{
			// move right
			aVelocity.add(mainSpeedRight*groundSpeed, zRight);

		}

		// When avatar is in contact with ground it shall be standing up, feet down head up.
		mainCameraPao.nose.cross(mainCameraPao.head, zRight);
		mainCameraPao.head.cross(zRight, mainCameraPao.nose);
		mainCameraPao.nose.normalize();
		mainCameraPao.head.normalize();

		//pao.head=zPao.head;
		//pao.nose=zPao.nose;

		mainCameraPao.pos.add(deltaTime, aVelocity);

		//ya->limitPos(pao.pos);


		/*if (mainSpeedUp==0)
		{
			aVelocity.set(0.0f,0.0f,0.0f);
		}*/

		groundContact=false;
	}
	else
	{
		// In air, no contact with ground, avatar is ballistic.

		const YukigassenRoom *pyr=ybr;

		if (pyr->roomAcceleration.absLength()<=0.01)
		{
			// micro gravity
			D3Vector zRight(zPao.nose, zPao.head);

			if  (mainSpeedUp!=0)
			{
				aVelocity.add(mainSpeedUp*1.0f, zPao.nose);
			}

			if (mainSpeedForward!=0)
			{
				// forward
				aVelocity.add(mainSpeedForward*1.0f, zPao.nose);

			}
			if (mainSpeedRight!=0)
			{
				// move right
				aVelocity.add(mainSpeedRight*1.0f, zRight);
			}

			// TODO Normalize aVelocity so that it is not faster to go diagonally

		}
		else
		{
			aVelocity.add(deltaTime, pyr->roomAcceleration);
		}

		mainCameraPao.pos.add(deltaTime, aVelocity);
	}

	// Check that it is possible to move (nothing in the way)
	if (ybr!=NULL)
	{
		// Tells if the avatar would collide with blocks in the room that it is in.
		if (ybr->isInsideBlockMargin(&(mainCameraPao.pos), margin))
		{
			// Check that current position is not already blocked since then its no use going back to there.
			const bool currentIsInsideBlock=ybr->isInsideBlockMargin(&currentPos, margin);

			//printf("mainTick: inside\n");
			if (currentIsInsideBlock==false)
			{
				// Move was not possible
				//printf("mainTick: move was reverted\n");
				mainCameraPao.pos.set(currentPos);
				aVelocity.set(0.0f,0.0f,0.0f);
				groundContact=true;
			}
		}
	}
}

// This shall be called periodically (such as 60 times per second or so) from mainPollAndTick or from main.
void mainTick()
{

	// The following lines are debug only, can be removed or commented out later.
	#ifdef SOFT_ERROR
	if (debugDetectMultiThreading!=0)
	{
		printf("debugDetectMultiThreading %d\n",debugDetectMultiThreading);
	}
	#else
	assert(debugDetectMultiThreading==0);
	#endif
	debugDetectMultiThreading=1;


	const int currTime = SDL_GetTicks(); // milli seconds
	int deltaTimeMs = currTime - mainTime;
	if (deltaTimeMs>maxDeltaTime)
	{
		deltaTimeMs = maxDeltaTime;
	}
	mainTime += deltaTimeMs;

	const float deltaTime = deltaTimeMs*0.001f;


	if ((mainFreeMoveMode) || (ourAvatarId<0))
	{
		// Move camera without avatar
		mainCameraPao.forward(mainSpeedForward*deltaTimeMs*0.01);
		mainCameraPao.right(mainSpeedRight*deltaTimeMs*0.01);
		mainCameraPao.up(mainSpeedUp*deltaTimeMs*0.01);
	}
	else
	{
		// Calculate avatar movement

		MirrorBase *avatarMb=globalDb->getObjectById(ourAvatarId);
		YukigassenAvatar *ya=dynamic_cast<YukigassenAvatar *>(avatarMb);
		//ya->limitPos(pao.pos);


		// pao can be looking up, but running is along the plane and jump is perpendicular to the plane. The plane is perpendicular to gravity/acceleration.






		// Did server change avatar position?
		if (serverChangedPositionCounter!=ya->serverChangedPositionCounter)
		{
			// This means that the server have changed to position in a way that we the client must follow. It can be teleporting or moving from one room to another.
			// We shall take the position the server say. We could continue moving after that but for now we just take the position and stay there.
			printf("serverChangedPositionCounter %d\n", serverChangedPositionCounter);
			mainCameraPao.set(ya->pao);
			//aVelocity.set(0.0f,0.0f,0.0f); // TODO we shall use velocity from server also.
			aVelocity.set(ya->objectVelocity);
			serverChangedPositionCounter=ya->serverChangedPositionCounter;
		}
		else
		{
			// Server did not change the avatar position, we can calculate it locally.
			localAvatarMovement(ya, deltaTime);
		}


		// This checks if we shall show the player hit "snow screen". An overlay 2D texture.
		if (ya->avatarStateMs)
		{
			// player has been hit.
			if (hitIndicationIsOn==false)
			{
				// hit indication is off so it needs to be turned on.
				hudScreenBox->setImage(0,0, 256, 256);
				textMessagesPage->addText("You where hit!");
				// TODO: I want a sound also but program crashes when I use this.
				/*
				if (beepSound!=NULL)
				{
					beepSound->load_Sound("boom.ogg");
					beepSound->play_Sound();

				}
				*/
				//SoundLoader::playSoundFile("boom.ogg");

				hitIndicationIsOn=true;
			}
		}
		else
		{
			// player is not hit
			if (hitIndicationIsOn==true)
			{
				// hit indication is on so it shall be turned off.
				hudScreenBox->clearText();
				textMessagesPage->addText("go");
				hitIndicationIsOn=false;
			}
		}

		// We have some 2D ammo indicator. This calculates its value.
		const int a=ya->avatarAmmo/1000;
		if (a!=prevAvatarAmmo)
		{
			// Remake the text only if value is changed.
			if (a>0)
			{
				char tmpStr[80];
				sprintf(tmpStr, "%d", a);
				amoTextBox->setText(tmpStr);
			}
			else
			{
				amoTextBox->clearText();
			}
			prevAvatarAmmo=a;
		}


		// Check Messages to the player
		// Messages are stored in a round buffer. Check here if there are any new messages.
		GameUser *apr = dynamic_cast<GameUser *>(globalDb->getObjectById(ya->avatarPlayerReferencesId));
		if (apr!=NULL)
		{
			RsbRoundBuffer *rsbRoundBuffer=apr->rsbRoundBuffer;
			if (rsbRoundBuffer!=NULL)
			{
				if (ourMessagesIndex!=rsbRoundBuffer->tail)
				{
					// There are more messages.
					MirrorBase *smb = rsbRoundBuffer->getChildByIndex(ourMessagesIndex);
					if (smb != NULL)
					{
						// Message was found in DB.

						RsbString *rs =dynamic_cast<RsbString *>(smb);
						if(rs!=NULL)
						{
							// Add this message to the text window
							const wchar_t *str=rs->getValueW();
							textMessagesPage->addTextW(str,0);
							ourMessagesIndex=rsbRoundBuffer->incAndWrap(ourMessagesIndex);
							ourMessagesIndexWaitCount=0;
						}
						else
						{
							// error, the message was not an RSB string object
							textMessagesPage->addText("not string",0);
							ourMessagesIndex=rsbRoundBuffer->incAndWrap(ourMessagesIndex);
						}
					}
					else
					{
						// Message was not found in DB.
						if (ourMessagesIndexWaitCount>1000)
						{
							textMessagesPage->addText("not loaded yet", "rrb");
							ourMessagesIndex=rsbRoundBuffer->incAndWrap(ourMessagesIndex);
						}
						else
						{
							++ourMessagesIndexWaitCount;
						}
					}
				}
				else
				{
					// No more messages.
				}

			}
		}
		else
		{
			printf("error: apr is null\n");
		}

		// "showPlayers" this is the statistics window. It is shown when the player holds down the tab key.
		if (showPlayerStats!=false)
		{
			if (localAvatar != textStatisticsPage)
			{
				localAvatar = textStatisticsPage;
			}

			textStatisticsPage->clearText();

			MirrorBase *m = globalDb->findChildByName("_listOfUsers", 2);
			if (m!=NULL)
			{
				//                01234567890123456789012345678901
				textStatisticsPage->addText("player                  team  score",0);

				MirrorContainer *pl=dynamic_cast<MirrorContainer*>(m);

				for(int i=0; i<pl->childVector.size(); ++i)
				{
					MirrorBase *mb=pl->childVector[i];
					GameUser* apr=dynamic_cast<GameUser*>(mb);
					if (apr!=NULL)
					{
						if (apr->isActive)
						{
							char str[256];
							str[sizeof(str)-1]=0;
							//MirrorBase *a = globalDb->getObjectById(apr->avatarId);


							sprintf(str, "%-23s %d     %d/%d", apr->getName(), apr->teamIndex, apr->fragScore, apr->lossScore);

							textStatisticsPage->addText(str,0);

							assert(str[sizeof(str)-1]==0);
						}
					}
				}
			}
			else
			{
				textStatisticsPage->addText("_listOfUsers not found",0);
				// Just to help debugging, show what was found instead
				for(int i=0; i<globalDb->childVector.size(); ++i)
				{
					MirrorBase *mb=globalDb->childVector[i];
					if (mb!=NULL)
					{
						textStatisticsPage->addText(mb->getName(),0);
					}
				}
			}

			//showPlayers=false;
		}
		else
		{
			// The statistics page is not wanted
			if (localAvatar == textStatisticsPage)
			{
				// We had an existing page, it can be deleted since its not wanted now.
				textStatisticsPage->clearText();
				localAvatar=textMessagesPage;
			}
		}



		// Send position update to server.
		// But not to often. Not more often than once per 100 ms.
		const int timeDiffLastSending=currTime-timeLastSending;
		if (timeDiffLastSending>=100)
		{
			// we may not send an infinite number of messages to server without hearing something from server.
			const int seqnr = mirrorClientsSeqnr - mirrorAckedClientsSeqnr;
			if (seqnr<4)
			{
				// no need to send unless position has changed
				if (!mainCameraPao.equals(mainSentPao))
				{
					updateAvatarToServer();
					timeLastSending = currTime;
				}
			}
			else
			{
				// We must hold back sending avatar position updates for a while, server is not ready.
				//printf("mirrorClientsSeqnr %d %d\n", mirrorClientsSeqnr, mirrorAckedClientsSeqnr);
			}
		}


	} // if ((mainFreeMoveMode) || (ourAvatarId<0))

	mainRender();

	// This is just for debugging. Can be removed later when everything works perfectly.
	#ifdef SOFT_ERROR
	if (debugDetectMultiThreading!=1)
	{
		printf("debugDetectMultiThreading %d\n",debugDetectMultiThreading);
	}
	#else
	assert(debugDetectMultiThreading==1);
	#endif
	debugDetectMultiThreading=0;
}


// Cleanup
static void cleanupGame()
{
	//printf("cleanupGame\n");

	if (globalDb!=NULL)
	{
		globalDb->unlinkSelf();
		delete globalDb;
		globalDb = NULL;
	}
	ourAvatarId=-1;
	ourMessagesIndex=0;

	//mainPos.set(2.0f, 0.5f, 10.0f); // We start with observer/camera 10 units above screen
	//mainNose.set(0.0f, 0.0f,-1.0f); // We start with observer/camera looking from above the screen into it, nose points into screen, as you are probably sitting now reading this.
	//mainHead.set(0.0f, 1.0f, 0.0f); // We start with observer/camera head upwards, as you are probably sitting now reading this.

	mainCameraPao.set(mainPos, mainNose, mainHead);
	//sentPao.set(mainPos, mainNose, mainHead);
	mainFreeMoveMode=false;
	aVelocity.set(0.0f, 0.0f, 0.0f);

	//mainCounter = 0x7fffffff; // Set this to zero to exit/quit
	mainSpeedForward = 0.0f;
	mainSpeedRight = 0.0f;
	mainSpeedUp = 0.0f;
	//mainTime = 0; // milliseconds



	if (colorPyramid!=NULL)
	{
		//colorPyramid->unlinkSelf();
		delete(colorPyramid);
		colorPyramid=NULL;
	}

	if (colorBox!=NULL)
	{
		//colorBox->unlinkSelf();
		delete(colorBox);
		colorBox=NULL;
	}

	if (modelLoaded!=NULL)
	{
		delete(modelLoaded);
		modelLoaded=NULL;
	}
}







// This is not used, the server will make the help messages instead.
#if 0
static void keyHelp()
{
	for (int key=0; key<'a';key++)
	{
		int m=symMap.getSymMap(key);
		if (m!=0)
		{
			const char *n=SymMap::getNameFromSym(m);
			if (n!=NULL)
			{
				const char *o=SymMap::getKeyName(key);
				char str[256];
				if (o!=NULL)
				{
					sprintf(str, "%15s %-15s", o, n);
				}
				else if ((key>' ' && key<='~'))
				{
					sprintf(str, "%15c %-15s", key, n);
				}
				textPage->addText(str);
			}
		}
	}
}
#endif

//static int debugKeyCounter=100;
static void processKeysPressed(const char *str);


static void processSymDownInStringInputMode(int key)
{
	// entering text, button or single key
	//printf("InputModeEnteringString, key down %d\n", key);

	if (leftCtrl)
	{
		switch(key)
		{
		case 'C':
			{
				const wchar_t* wstr= textInputBuffer->getText();


				const int s=4096;
				char *tmp = (char *)malloc(s+4); // free is done just some lines down.
				assert(tmp!=NULL);
				const int n=WordWriter::encodeStringW(tmp, wstr, s); // TODO it shall be utf8 not backslash-hex
				assert(n<=s);
				tmp[n]=0;

				SDL_SetClipboardText(tmp); // https://wiki.libsdl.org/CategoryClipboard
				free(tmp); // don't forget this
				break;
			}
		case 'V':
			{
				char* str=SDL_GetClipboardText(); // https://wiki.libsdl.org/CategoryClipboard
				processKeysPressed(str);
				SDL_free(str);
				break;
			}
		default:
			break;
		}
	}
	else
	{
		switch(key)
		{
			case SymMap::specUp:
				mainSpeedForward = 0.1f;
				textInputBuffer->getPrev();
				break;
			case SymMap::specDown:
				mainSpeedForward = -0.1f;
				textMessagesPage->clearText();
				break;
			case SymMap::specPageUp:
				printf("pg up event\n");
				localAvatar->scrollUp();
				break;
			case SymMap::specPageDown:
				printf("key: pg down event\n");
				localAvatar->scrollDown();
				break;
			case SymMap::specLeft:
				printf("key: left\n");
				mainSpeedRight = -0.1f;
				//textInputBuffer->DelChar();
				textInputBuffer->MoveCursorBack();
				break;
			case SymMap::specRight:
				printf("key: right\n");
				mainSpeedRight = 0.1f;
				textInputBuffer->MoveCursorForward();
				break;
				// specDel interfered with '.' so removed it for now.
			/*case SymMap::specDel:
				printf("key: del\n");
				textInputBuffer->MoveCursorForward();
				textInputBuffer->DelChar();
				break;*/
			case SymMap::specBackspace:
				printf("key: backspace\n");
				textInputBuffer->DelChar();
				break;
			case SymMap::specEsc:
				printf("key: esc event\n");
				mouseRelease();
				break;
			case SymMap::specEnd:
				textInputBuffer->MoveCursorToEnd();
				break;
			case SymMap::specHome:
				//mouseRelease();
				textInputBuffer->inputBufferClear();
				textInputBuffer->inputEnterKeyEntryMode();
				break;
			case SymMap::specEnter:
				printf("SDLK_RETURN/SDLK_KP_ENTER\n");
				if (textInputBuffer->inputBufferLen>0)
				{
					textInputBuffer->DoneNowSend();
					textInputBuffer->inputEnterStringEntryMode();
					textInputBuffer->textInputBox->setText("<enter a text or enter an empty line to leave text entry mode>");
					textInputBuffer->SetPrefix("consoleInput");
				}
				else
				{
					textInputBuffer->inputBufferClear();
					textInputBuffer->inputEnterKeyEntryMode();
				}
				break;
			case SymMap::specAltGr:
				{
					altGr=true;
					break;
				}
			case SymMap::specLeftShift:
				{
					leftShift=true;
					break;
				}
			case SymMap::specLeftCtrl:
				{
					leftCtrl=true;
					break;
				}
			default:
				if ((key<' ') || (key>'~'))
				{
					printf("key: unknown key down event %d\n", key);

					// It seams SDL mapping of HW keys does not work the same way as in native.
					// Did not get '~' to work in web version, as a temporary workaround
					// all characters that are not known are used as '~'.
					#ifdef __EMSCRIPTEN__
					if (key==0)
					{
						//textInputBuffer->AddChar('~');
						activateQuickAndDirtyTildeFix=true;
					}
					#endif
				}
				break;
		}
	}
}


static void processSymUpInStringInputMode(int key)
{
	// entering text, button or single key
	//printf("InputModeEnteringString, key up %d\n", key);
	switch(key)
	{
		case SymMap::specUp:
			mainSpeedForward = 0.f;
			break;
		case SymMap::specDown:
			mainSpeedForward = -0.0f;
			break;
		case SymMap::specPageUp:
			break;
		case SymMap::specPageDown:
			break;
		case SymMap::specLeft:
			mainSpeedRight = -0.0f;
			break;
		case SymMap::specRight:
			mainSpeedRight = 0.1f;
			break;
		case SymMap::specDel:
			break;
		case SymMap::specBackspace:
			break;
		case SymMap::specEsc:
			break;
		case SymMap::specEnd:
			break;
		case SymMap::specHome:
			break;
		case SymMap::specEnter:
		case SymMap::specAltGr:
			altGr=false;
			break;
		case SymMap::specLeftShift:
			{
				leftShift=false;
				break;
			}
		case SymMap::specLeftCtrl:
			{
				leftCtrl=false;
				break;
			}
		default:
			if ((key<' ') || (key>'~'))
			{
				printf("key: unknown key up event %d\n", key);
				if ((key==0) && (activateQuickAndDirtyTildeFix))
				{
					textInputBuffer->AddChar('~');
				}

			}
			break;
	}

	activateQuickAndDirtyTildeFix=false;
}



// This shall be called when a key is pressed down.
// key is the SDL name for physical keys.
static int processKeyDownEvent(int key)
{
	switch(textInputBuffer->GetMode())
	{
		case InputModeFirstPerson:
		{
			// not entering text, so it is direct key input, usually controlling the movement of the players avatar.

			int specKey=symMap.getSpec(key);
			const int sym=symMap.remap(specKey);

			/*if (debugKeyCounter>0)
			{
				printf("processKeyDownEvent %d %d %d\n", key, specKey, sym);
				--debugKeyCounter;
			}*/

			switch(sym)
			{
				case SymMap::backOrCancel: // 8
					printf("SymMap: backOrCancel\n");
					mainSend("backOrCancel");
					cleanupGame();
					break;
				case SymMap::endCode:
					printf("SymMap: end event\n");
					mainCounter = 0;
					return 1;
				case SymMap::showScore:
					showPlayerStats=true;
					break;
				case SymMap::returnCode:
					printf("SymMap: returnCode\n");
					textInputBuffer->DoneNowSend();
					break;
				case SymMap::upOne:
					localAvatar->scrollUp();
					break;
				case SymMap::downOne:
					localAvatar->scrollDown();
					break;
				case SymMap::leftOne:
					mainCameraPao.yaw_right(-0.01f);
					break;
				case SymMap::rightOne:
					mainCameraPao.yaw_right(0.01f);
					break;
				case SymMap::pageUp:
					printf("SymMap: pg up event\n");
					localAvatar->scrollUp();
					localAvatar->scrollUp();
					localAvatar->scrollUp();
					localAvatar->scrollUp();
					break;
				case SymMap::pageDown:
					printf("SymMap: pg down event\n");
					localAvatar->scrollDown();
					localAvatar->scrollDown();
					localAvatar->scrollDown();
					localAvatar->scrollDown();
					break;
				case SymMap::jumpUp:
					mainSpeedUp = 0.1f;
					break;
				case SymMap::mouseCapture:
					// home
					printf("SymMap: mouseCapture\n");
					mouseCapture();
					break;
				case SymMap::mouseRelease:
					// esc
					printf("mouseRelease\n");
					mouseRelease();
					break;
				case SymMap::moveLeft:
					mainSpeedRight = -0.1f;
					break;
				case SymMap::rollRight_deprecated:
					mainCameraPao.roll_right(0.01f);
					break;
				case SymMap::moveRight:
					mainSpeedRight = 0.1f;
					break;
				case SymMap::actionKey:
					{
						char tmp[80];
						sprintf(tmp, "actionKey");
						mainSend(tmp);
						break;
					}
				case SymMap::testBeep:
					printf("SymMap: testBeep\n");
					SDL_PauseAudio(0);
					break;
				case SymMap::moveBackwards:
					mainSpeedForward = -0.1f;
					break;
				case SymMap::rollLeft_deprecated:
					mainCameraPao.roll_right(-0.01f);
					break;
				case SymMap::yawLeft_deprecated:
					mainCameraPao.yaw_right(-0.01f);
					break;
				case SymMap::pitchUp_deprecated:
					mainCameraPao.pitch_up(0.01f);
					break;
				case SymMap::pitchDown_deprecated:
					mainCameraPao.pitch_up(-0.01f);
					break;
				case SymMap::moveForward:
					mainSpeedForward = 0.1f;
					break;
				case SymMap::yawRight_deprecated:
					mainCameraPao.yaw_right(0.01f);
					break;
				case SymMap::clearText:
					textMessagesPage->clearText();
					break;
				case SymMap::invScreen:
				{
					textInventoryPage->addText("inventory", NULL);

					assert(globalDb!=NULL);
					MirrorBase *tmpMb=globalDb->getObjectById(ourAvatarId);
					YukigassenAvatar *ya=dynamic_cast<YukigassenAvatar*>(tmpMb);
					if (ya!=NULL)
					{
						const YukigassenInventory *inv=ya->getInv();
						if (inv!=NULL)
						{
							size_t c=inv->childVector.capacity();
							for (size_t i=0; i<c; ++i)
							{
								MirrorBase *mb = inv->childVector[i];
								if (mb!=NULL)
								{
									char indexStr[20]={0};
									sprintf(indexStr, "%d", mb->getIndex());
									assert(indexStr[sizeof(indexStr)-1]==0);

									RsbInvItem *rs=dynamic_cast<RsbInvItem*>(mb);
									if (rs!=NULL)
									{
										wchar_t tStr[256*sizeof(wchar_t)]={0};
										swprintf(tStr, sizeof(tStr), L"%03lld %.120ls", rs->getNumberOf(), rs->getValueW());

										assert(tStr[SIZEOF_ARRAY(tStr)-1]==0);
										textInventoryPage->addTextW(tStr, indexStr);
									}
									else
									{
										char tStr[160]={0};
										sprintf(tStr, "%03lld %.120s", 1LL, mb->getName());
										assert(tStr[SIZEOF_ARRAY(tStr)-1]==0);
										textInventoryPage->addText(tStr, indexStr);

									}
								}
							}
						}
						else
						{
							textMessagesPage->addText("your inventory is empty", NULL);
						}
					}
					else
					{
						textMessagesPage->addText("avatar not found", NULL);
					}

					// TODO push local avatar location
					localAvatar=textInventoryPage;
					mouseRelease();

					break;
				}
				case SymMap::enterText:
					printf("SymMap: enterText\n");
					break;
				case SymMap::crouchOrDuck:
					mainCrouch=true;
					break;
				default:
				{
					char tmpStr[40];
					sprintf(tmpStr, "keyDown %d", specKey);
					mainSend(tmpStr);
					break;
				}
			}
			break;
		}
		case InputModeEnteringPhysicalKey:
		{
			// entering single key, this was added for the user setting of key mappings. It is controlled by server and probably still only used for that.
			const int spec=symMap.getSpec(key);
			printf("InputModeEnteringKey, key %d %d\n", key, spec);
			{
				char tmp[40];
				sprintf(tmp, "%d",spec);
				mainSend(tmp);
				textInputBuffer->inputBufferClear();
			}
			break;
		}
		case InputModeEnteringTextString:
		{
#ifndef __EMSCRIPTEN__
			processSymDownInStringInputMode(symMap.getSpec(key));
#else
			processSymDownInStringInputMode(key);
#endif
			/*switch(key)
			{
				case 0:altGr=true;break;
				case 16:shift=true;break;
				case 17:ctrl=true;break;
				default:break;
			}*/
			break;
		}
		case InputModeEnteringButtonOrMenu:
		{
			switch(key)
			{
#ifndef __EMSCRIPTEN__
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					// If we are waiting for OK then pressing enter should count as OK. Something like this (if this works):
					mainSend("OK");
					textInputBuffer->clearText();
					localAvatar->clearText();
					// TODO pop local avatar location
					localAvatar=textMessagesPage;
					break;
#endif
				case SDLK_BACKSPACE:
				case SDLK_DELETE:
				case SDLK_END: // Can not use esc since that is used to release mouse, so we use these cancel buttons.
					mainSend("Cancel");
					textInputBuffer->clearText();
					localAvatar->clearText();
					localAvatar=textMessagesPage;
					break;
				case SDLK_ESCAPE:
					printf("key: esc event\n");
					mouseRelease();
					return 1;
				default:
					break;
			}
			break;
		}
		default:
		{
			mouseRelease();
			printf("Unknown input mode %d, key down %d\n", textInputBuffer->GetMode(), key);
			break;
		}
	}
	return 0;
}


// This shall be called when a key is released.
// key is the SDL name for physical keys.
static int processKeyUpEvent(int key)
{
	switch(textInputBuffer->GetMode())
	{
		case InputModeFirstPerson:
		{
			// not entering text
			// some keys like movement are active until released, that is done here.
				const int specKey=symMap.getSpec(key);
			const int sym=symMap.remap(specKey);
			switch(sym)
			{
				case SymMap::showScore:
					showPlayerStats=false;
					break;
				case SymMap::moveForward:
					mainSpeedForward = 0.0f;
					break;
				case SymMap::moveBackwards:
					mainSpeedForward = 0.0f;
					break;
				case SymMap::moveRight:
					mainSpeedRight = 0.0f;
					break;
				case SymMap::moveLeft:
					mainSpeedRight = 0.0f;
					break;
				case SymMap::jumpUp:
					mainSpeedUp = 0.0f;
					break;
				case SymMap::crouchOrDuck:
					mainCrouch=false;
					break;
				case SymMap::clearText:
					textInventoryPage->clearText();
					localAvatar=textMessagesPage;
					break;
				case SymMap::enterText:
					printf("SymMap: enterText\n");
					textInputBuffer->inputEnterStringEntryMode();
					textInputBuffer->SetPrefix("consoleInput");
					break;
				default:
					break;
			}
			break;
		}
		case InputModeEnteringTextString:
		{
			mainSpeedForward = 0.0f;
			mainSpeedRight = 0.0f;

#ifndef __EMSCRIPTEN__
			processSymUpInStringInputMode(symMap.getSpec(key));
#else
			processSymUpInStringInputMode(key);
#endif
			/*switch(key)
			{
				case 0:altGr=false;break;
				case 16:shift=false;break;
				case 17:ctrl=false;break;
				default:break;
			}*/
			break;
		}
		case InputModeEnteringPhysicalKey:
			// Nothing here
			break;


		case InputModeEnteringButtonOrMenu:
		{
			printf("InputModeEnteringButton, key up %d\n", key);
			break;
		}
		default:
			break;
	}
	return 0;
}


// The processKeyDownEvent/processKeyUpEvent are for when a specific hardware key (on the keyboard) is pressed down or released up.
// This function is called when a character is generated from the keyboard. That is not always the same,
// more than one hardware key can be be needed to generate one character. Holding down one key (or a combination of keys) can generate more than one character.
static int processKeyPress(wchar_t code)
{
	activateQuickAndDirtyTildeFix=false;

	//printf("processKeyPress %d\n", code);
	switch (textInputBuffer->GetMode())
	{
		case InputModeEnteringButtonOrMenu:
			textInputBuffer->AddChar(code);
			textInputBuffer->DoneNowSend();
			break;

		case InputModeFirstPerson:
			break;

		default:
			if (isprint(code) || (code>=0xA0))
			{
				textInputBuffer->AddChar(code);
			}
			break;
	}

	return 0;
}

static void processKeysPressed(const char *str)
{
	const int s=WordReader::countUtf8(str);
	if (s>0)
	{
		wchar_t *wstr=(wchar_t*)malloc((s+1)*sizeof(wchar_t)); // free is to be done further down in this function
		wstr[s]=0;

		int n = WordReader::decodeUtf8(wstr, str, s);
		assert(wstr[s]==0);

		if ((n>0) && (n<=s))
		{
			for(int i=0;i<n;++i)
			{
				processKeyPress(wstr[i]);
			}
		}
		else
		{
			printf("failed to decode utf8 %d\n", s);
		}
		free(wstr);
	}
	else
	{
		printf("failed to decode utf8\n");
	}
}


static int processMouseMoveXEvent(int x)
{
	mainCameraPao.yaw_right(x*0.001f);

	return 0;
}

static int processMouseMoveYEvent(int y)
{
	mainCameraPao.pitch_up(y*-0.001f);

	return 0;
}


static char mainReturnString[80]={0};


// This (mainHandleNumericEvent) together with mainHandleStringEvent is the main entry point for information going into the client program.
extern "C" int mainHandleNumericEvent(const NumericEvent eventId, int eventData)
{
	assert(debugDetectMultiThreading==0);
	debugDetectMultiThreading=3;


	switch(eventId)
	{
	case NumericEventTick:
		// tick no longer used
		assert(0);
		break;
	case NumericEventKeyDown:
		processKeyDownEvent(eventData);
		break;
	case NumericEventKeyUp:
		processKeyUpEvent(eventData);
		break;
	case NumericEventMovementX:
		processMouseMoveXEvent(eventData);
		break;
	case NumericEventMovementY: // movementY
		processMouseMoveYEvent(eventData);
		break;
	case NumericEventKeyPress:
		processKeyPress(eventData);
		break;
	default:
		printf("mainHandleNumericEvent: %d %d\n", eventId, eventData);
		break;
	}


	assert(debugDetectMultiThreading==3);
	debugDetectMultiThreading=0;

	return 0;
}

void mainUnknownMessageFromServer(const char *str)
{
	printf("mainUnknownMessageFromServer: '%s'\n",str);
}




// Returns true if and auto answer was made.
// false if not.
static bool autoAnswer(const char *styleTag)
{
	int tch=styleTag[0];

	switch(tch)
	{
		case '2':
			if (strcmp(styleTag, "2d_or_3d_support")==0) // This is deprecated
			{
				assert(0);
				mainSend("3d");
				textInputBuffer->inputBufferClear();
				return true;
			}
			break;
		case 'e':
			if (strcmp(styleTag, "enter_player_index")==0)
			{
				char tmp[256];
				sprintf(tmp, "%d", playerIndex);
				mainSend(tmp);
				textInputBuffer->inputBufferClear();
				return true;
			}
			else if (strcmp(styleTag, "enter_player_code")==0)
			{
				char tmp[256];
				sprintf(tmp, "%d", playerCode);
				mainSend(tmp);
				textInputBuffer->inputBufferClear();
				return true;
			}
			break;
		case 'g':
			if (strcmp(styleTag, "game_support")==0)
			{
				// See method PlayerConnectionThread.serverFactory in server for available games.
				// List here all games that this client can handle (it can be more than one separated by space, inside the quotes)
#ifdef EMPIRE_CLIENT
				mainSend("\"yukigassen lobby empire\"");
#else
				mainSend("\"yukigassen lobby\"");
#endif
				textInputBuffer->inputBufferClear();
				return true;
			}
			break;
		case 's':
			if (strcmp(styleTag, "selectType")==0)
			{
				mainSend("\"player\"");
				textInputBuffer->inputBufferClear();
				return true;
			}
			break;


		default:
			break;
	}


	/*if ((autoReplyPw==0) && (strcmp(styleTag, "enter_player_pw")==0))
	{
		mainSend("\"a\"");
		autoReplyPw++;
		inputBufferClear();
	}
	else if (strcmp(styleTag, "login_ok")==0)
	{
		mainSend("\"0\"");
		inputBufferClear();
	}
	else if (strcmp(styleTag, "join_or_create")==0)
	{
		mainSend("0");
		inputBufferClear();
	}
	else if (strcmp(styleTag, "joining_world")==0)
	{
		mainSend("0");
		inputBufferClear();
	}
	else*/
	/*if (strcmp(styleTag, "list_enter_game_type_name")==0)
	{
		mainSend("yukigassen");
		inputBufferClear();
	}*/


	return false;
}


// Server sends this when an object shall be added to our mirror database.
static void mirrorAdd(WordReader *wr)
{


	const int objectId = wr->readInt();
	const int parentId = wr->readInt();
	const char *objectTypeStr=wr->readToken();

	MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

	if (mb!=NULL)
	{
		printf("mirrorAdd ~%d %s\n", objectId, objectTypeStr);

		mb->id = objectId;
		mb->parentId = parentId;
		mb->readSelf(wr);

		if (globalDb==NULL)
		{
			globalDb=dynamic_cast<MirrorIdList*>(mb);
			assert(globalDb!=NULL); // If this fails then perhaps the game root object is not an IdList? It must be.
			assert(parentId<0);
			assert(mb->parentId==-1);

			mb->index=-1; // TODO Perhaps we shall have a special command to send the top most object in a game since index is server side index and client have its own index for the to most objects. Or server shall not send index to client for this.
			mb->linkSelf(rootDb);
		}
		else
		{
			assert(globalDb->getObjectById(objectId)==NULL);
			MirrorBase *parent=globalDb->getObjectById(mb->parentId);
			mb->linkSelf(parent);
		}

		mb->setupInternalRenderingData();

	}
	else
	{
		// Could not create the object. Probably an unknown type.
		printf("mirrorAdd failed on %s\n", objectTypeStr);
	}
}

// Server sends this when an object has been changed.
static void mirrorUpdate(WordReader *wr)
{
	const int objectId = wr->readInt();
	const int newParentId = wr->readInt(); // new parent ID
	const char *objectTypeStr=wr->readToken();
	MirrorBase *newParent=globalDb->getObjectById(newParentId);
	MirrorBase *mb = globalDb->getObjectById(objectId);
	if (mb != NULL)
	{
		// This was an existing object

		// check that object type is same as before, if not it is a new object and the old one has been deleted.
		if (strcmp(objectTypeStr, mb->getTypeName())!=0)
		{
			// It has changed object type. So it was not same object after all.

			// Remove old object
			mb->unlinkSelf();
			delete(mb);

			// Create the new
			MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

			if (mb!=NULL)
			{
				printf("mirrorAdd ~%d %s\n", objectId, objectTypeStr);

				// Link the new object into our database.
				mb->id = objectId;
				mb->parentId = newParentId;
				mb->readSelf(wr);

				mb->linkSelf(newParent);

				mb->setupInternalRenderingData();
			}
			else
			{
				printf("mirrorFactoryCreate failed on %s\n", objectTypeStr);
			}
		}
		else
		{
			// It has same type so we can assume that it is the same object-

			const int prevParentId = mb->parentId;
			if (newParentId != prevParentId)
			{
				printf("moving object between rooms ~%d ~%d ~%d %d\n",objectId, prevParentId, newParentId, mb->index);
				fflush(stdout);

				MirrorContainer *newParent=(MirrorContainer *)(globalDb->getObjectById(newParentId));

				assert(mb->parentPtr!=NULL);
				assert(mb->parentPtr== globalDb->getObjectById(prevParentId));
				assert(newParent!=NULL);

				mb->parentPtr->removeChildObject(mb);

				mb->readSelf(wr);

				newParent->addChildObject(mb);

				// TODO: Perhaps we could use relinkSelf here? Instead of removeChildObject/addChildObject

				//mb->setupInternalRenderingData(); // call this or not call this? For now not calling it here, but then YukigassenSubRoom::readSelf must do it, not perfect either. The solution is probably to divide avatar into two or more objects, similar to block room, that way avatar can be moved without updating textures.

				//printf("tried anyway ~%d ~%d %d\n",objectId, mb->parentId, mb->index);
			}
			else
			{
				const int oldIndex=mb->index;

				mb->readSelf(wr);
				//mb->setupInternalRenderingData(); // call this or not call this? For now not calling it here, but then YukigassenSubRoom::readSelf must do it, not perfect either. The solution is probably to divide avatar into two or more objects, similar to block room, that way avatar can be moved without updating textures.


				const int newIndex=mb->index;
				if (newIndex!=oldIndex)
				{
					// Object has moved to another slot but in the same room.
					assert(newParent == mb->parentPtr);
					assert(mb->parentPtr->id == prevParentId);
					//printf("changing index within room is not supported ~%d %d %d\n",objectId, oldIndex, mb->index);
					// For now will set index to old value. But we should also remove it from its parent room, set new index and put it back.
					// TODO: This happens a lot, perhaps it is a server error that makes it happen so often.
					// TODO: Perhaps we could use relinkSelf here? Instead of removeChildObject/addChildObject
					mb->index=oldIndex;
					newParent->removeChildObject(mb);
					mb->index=newIndex;
					newParent->addChildObject(mb);
				}
			}
			mb->dataChanged();
		}
	}
	else
	{
		// This was an new object

		printf("mirrorUpdate: update for unknown object ~%d\n",objectId);

		// The following code should not be needed. But if server sends mirrorUpdate we are supposed to have the object in globalDb. Aha! If a parent object is deleted, server sent mirrorRemove then we delete all objects stored in it, but server does not delete all of those. Until we figured out how to solve that we just create the object again.
		// The workaround below will only restore objects that are updated, e.g. avatar body is lost and will not return using this since (some) body parts don't need to updated very often. So avatar becomes invisible if it was in a room that was deleted. Hopefully the invisible avatar problem is now solved by a change in iterateUpdatesToClient so that updates are sent first and removals after.

		MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

		if (mb!=NULL)
		{
			printf("mirrorAdd ~%d %s\n", objectId, objectTypeStr);

			mb->id = objectId;
			mb->parentId = newParentId;
			mb->readSelf(wr);

			mb->linkSelf(newParent);

			mb->setupInternalRenderingData();
		}
		else
		{
			printf("mirrorFactoryCreate failed on %s\n", objectTypeStr);
		}
	}
}

// Note mirrorUpdate and mirrorUpdated are different things. This command shall be renamed to something better.
static void mirrorUpdated(WordReader *wr)
{
	char str[256];
	const int mirrorServersSeqnr=wr->readInt();
	mirrorAckedClientsSeqnr=wr->readInt();
	sprintf(str,"mirrorAck %d\n", mirrorServersSeqnr);
	mainSend(str);
}


// Server sends this when an object shall be removed from our mirror database.
static void mirrorRemove(WordReader *wr)
{
	const int objectId = wr->readInt();
	printf("mirrorRemove ~%d\n", objectId);
	MirrorBase *mb = globalDb->getObjectById(objectId);
	if (mb!=NULL)
	{
		mb->unlinkSelf();
		delete(mb);
	}
	else
	{
		printf("mirrorRemove did not find %d\n", objectId);
	}
}

// This will evaluated commands from the server
// Returns: true if command found, false if not.
static bool evaluateCommandFromServer(WordReader *wr, const char *cmd)
{
	assert(wr!=NULL);
	assert(cmd!=NULL);
	const int ch = cmd[0];
	switch(ch)
	{
		case 'a':
			if (strcmp(cmd, "avatarAck")==0)
			{
				// Server must send avatarAck after it receives an avatarPos
				mirrorAckedClientsSeqnr=wr->readInt();
				return true;
			}
			else if (strcmp(cmd, "avatarId")==0)
			{
				// Server send this to tell us the ID of this players avatar
				const char *avatarIdStr=wr->readToken();
				printf("our avatar ID: %s\n", avatarIdStr);
				ourAvatarId=atoi(avatarIdStr);

				// Now it should be possible to set our position
				MirrorBase *mb=globalDb->getObjectById(ourAvatarId);
				YukigassenAvatar *ya=dynamic_cast<YukigassenAvatar *>(mb);
				if (ya!=NULL)
				{
					mainCameraPao.set(ya->pao);
				}
				updateAvatarToServer();

				return true;
			}
			break;

		case 'c':
			if (strcmp(cmd, "consoleMessage")==0)
			{
				while(!wr->endOfStr())
				{

					const char* msg=wr->readString();

					if (msg!=NULL)
					{
						printf("consoleMessage '%s'\n", msg);
						// The message can be more than one line. So it needs to be split into lines before sent to mainTextMessagesPage.
						WordReader wr(msg);
						for(;;)
						{
							const wchar_t* str=wr.readLineW();
							if (str==NULL)
							{
								break;
							}

							//printf("consoleMessage line '%s'\n", str);
							textMessagesPage->addTextW(str, 0);
						}
					}
				}
				return true;
			}
			break;

		case 'l':
			if (strcmp(cmd, "listClear")==0)
			{
				// TODO: Perhaps instead of implementing this here we shall remove it from server?
				printf("listClear not implemented yet\n");
				assert(0);
				return true;
			}
			else if (strcmp(cmd, "listAdd")==0)
			{
				const char *listEntry=wr->readString();
				// TODO: Perhaps instead of implementing this here we shall remove it from server?
				printf("listAdd not implemented yet %s\n", listEntry);
				assert(0);
				return true;
			}
			break;
		case 'm':
		{
			switch(cmd[1])
			{
				case 'a':
					if (strcmp(cmd, "ma")==0)
					{
						mirrorAdd(wr);
						return true;
					}
					break;
				case 'r':
					if (strcmp(cmd, "mr")==0)
					{
						mirrorRemove(wr);
						return true;
					}
					break;
				case 'u':
					if (strcmp(cmd, "mu")==0)
					{
						mirrorUpdate(wr);
						return true;
					}
					break;
				case 'i':
				{
					if (strcmp(cmd, "mirrorUpdated")==0)
					{
						mirrorUpdated(wr);
						return true;
					}
					else if (strcmp(cmd, "mirrorUpdate")==0) // deprecated use mu
					{
						mirrorUpdate(wr);
						return true;
					}
					else if (strcmp(cmd, "mirrorAdd")==0) // deprecated use ma
					{
						mirrorAdd(wr);
						return true;
					}
					else if (strcmp(cmd, "mirrorRemove")==0) // deprecated use mr
					{
						mirrorRemove(wr);
						return true;
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case 'o':
			if (strcmp(cmd, "openGame")==0)
			{
				const char *gMsg=wr->readToken();
				const char *wMsg=wr->readToken();
				printf("openGame %s %s\n",gMsg, wMsg);
				if (strcmp(gMsg, "yukigassen")==0)
				{
					gameType=yukigassenGame;
					localAvatar=textMessagesPage;
				}
#ifdef EMPIRE_CLIENT
				else if (strcmp(gMsg, "empire")==0)
				{
					gameType=empireGame;
					// Setup the main menue


					menuPage->clearText();
					menuPage->addText("Empire Main Menu");

					menuPage->addText("Not implemented yet");


					localAvatar=menuPage;
				}
#endif
				else
				{
					// Server started wrong type of game.
					printf("Wrong game, '%s' is not supported by this client\n",gMsg);
					assert(0);
				}
				// remove demo objects, the game will start when server starts filling the mirror database.
				cleanupGame();

				// AFter this the server will send the world,
				// It should also send the command avatarId to tell client which object in the world that
				// belongs to the player at this client.
				// TODO In case this is the empire game the avatar shall be the empire state (not implemented that way yet)
				// TODO If this is the empire game we need a variable to tell in whicg view we are. If it points at the empire terrain object then the world map shall be shown. If it point at a sector than the properties and content of that sector shall be shown. If it shows a unit then properties and content of that unit should be shown. Much to do.
				// Once client has the avatar ID is will render the world and player can take/issue his/hers actions/command
				// So it changes from server driven to client driven here.

				return true;
			}
			break;
		case 'p':
			if (strcmp(cmd, "playerPreference")==0)
			{
				// player preferences are things like which key to use for what
				const char *prefName=wr->readToken();
				const char *prefValue=wr->readString();
				printf("playerPreference %s '%s'\n",prefName, prefValue);

				if (playerPreferenceMb==NULL)
				{
					playerPreferenceMb=new MirrorContainer();
					playerPreferenceMb->setName("playerPreferenceMb");
					playerPreferenceMb->linkSelf(localDb);
				}


				symMap.set(prefName, prefValue);

				return true;
			}
			if (strcmp(cmd, "playSoundFile")==0)
			{
				const char *soundName = wr->readString();
				SoundLoaderPlayer::playSoundFile(soundName);
				return true;
			}
			break;
		case 'q':
			if (strcmp(cmd, "qp")==0)
			{
				const char* expectedReplyType = wr->readToken();
				const char* styleTag = wr->readToken();
				const wchar_t* textMsg = wr->readTokenW();

				//printf("queryPrompt: '%s', '%s', '%s', '%s'\n", cmd, expectedReplyType, styleTag, textMsg);

				if (strcmp(styleTag, "in_charge")==0)
				{
#ifdef EMPIRE_CLIENT
					// TODO do we need to do something here?
#else
					printf("empire is not supported yet by this client\n");
					mainCounter = 0;
#endif
				}

				textInputBuffer->clearText();
				menuPage->clearText();
				menuPage->addTextW(textMsg, "0");

				const bool a = autoAnswer(styleTag);

				if (a)
				{
					// already answered
				}
				else if (strcmp(expectedReplyType, "buttonPrompt")==0)
				{
					const unsigned int nAlternatives = wr->readInt();

					for (unsigned int i=0; i<nAlternatives; ++i)
					{
						char str[256];
						const char* buttonText = wr->readString();
						//sprintf(str, " %c : %.40s\n", getShortKey(i), buttonText);
						sprintf(str, " %.48s\n", buttonText);
						menuPage->addText(str, buttonText);
					}
					textInputBuffer->inputEnterButtonEntryMode();
				}
				else if (strcmp(expectedReplyType, "keyPrompt")==0)
				{
					textInputBuffer->inputEnterKeyEntryMode();
				}
				else
				{
					textInputBuffer->inputEnterStringEntryMode();
				}
				return true;
			}
			break;
		case 'r':
			if (strcmp(cmd, "reconnectServer")==0)
			{
				// How shall we do this? Stay in same main loop or exit the loop and start a new. This try will stay in same loop.

				// Here we shall close current connection and open a new.

				cleanupGame();
				closeConnection();


				const char* newServerAddr = wr->readString(); // The server URL

				// If the address start with / this it is expected to be this format "/<hostname or IP>:<port number>"

				const int f = newServerAddr[0];

				if (f=='/')
				{
					const char *s=newServerAddr+1;
					char a[1024]={0};
					int i=0;

					while((*s) && (*s!=':'))
					{
						a[i]=*s;
						++s;
						++i;
					}

					if((*s) && (*s==':'))
					{
						++s;
						++i;
					}

					int p = atoi(s);

					openConnection(a, p);
				}
				else
				{
					// TODO the code here is probably never used, can be removed?
					printf("reconnectServer: to what?");
					const int newServerPort = wr->readInt(); // The server port
					//const char* serverReference = wr->readString(); // use this to login on the other server-


					openConnection(newServerAddr, newServerPort);
				}
				return true;
			}
			break;
		case 's':
			if (strcmp(cmd, "showWorld")==0)
			{
				// Do nothing, world is shown by main loop
				return true;
			}
			if (strcmp(cmd, "setClientIdxAndCode")==0)
			{
				// Before server sends a reconnectServer it can send this message to give login codes for the new server.
				playerIndex=wr->readInt();
				playerCode=wr->readInt();
				return true;
			}
			break;
		case 'w':
			if (strcmp(cmd, SERVER_VERSION)==0)
			{
				// Server sends the line above to identify itself, client must reply with its version string.
				// TODO: no other commands from server should be accepted before this.
				mainSend(CLIENT_VERSION);
				return true;
			}
			break;
		case 0:
			// empty string
			return true;
			break;
		default:

			break;
	}
	return false;
}


// This will evaluated messages from the server
// TODO Return NULL if command found and an error message if not
static void evaluateMessageFromServer(WordReader *wr)
{
	assert(wr!=NULL);
	const char *cmd=wr->readWord();

	if (cmd==NULL)
	{
		mainDebug("evaluateMessageFromServer","cmd==NULL");
	}
	else
	{
		mainDebug("evaluateMessageFromServer",cmd);

		bool r = evaluateCommandFromServer(wr, cmd);

		if ((r==false) && (strcmp(cmd,"(null)")!=0))  // TODO Why does server send messages '(null)'?
		{
			mainUnknownMessageFromServer(cmd);
		}
	}
}

// This (mainHandleStringEvent) together with mainHandleNumericEvent is the main entry point for information going into the client program.
// Network and Keyboard/mouse events go in here. eventId will tell which event it was.
// In web version this is also called from the Java Script file hello.js.
extern "C" const char * mainHandleStringEvent(const StringEvent eventId, const char* eventData)
{

	// These few lines are for debugging only, can be removed later when all works.
	#ifdef SOFT_ERROR
	if (debugDetectMultiThreading!=0)
	{
		printf("debugDetectMultiThreading %d\n",debugDetectMultiThreading);
	}
	#else
	assert(debugDetectMultiThreading==0);
	#endif
	debugDetectMultiThreading=2;



	switch(eventId)
	{
	case StringEventTick:
		// tick no longer used
		assert(0);
		break;
	case StringEventOnOpen:
		printf("StringEventOnOpen: %s\n", eventData);
		mainSend("rsb_web_game");
		break;
	case StringEventOnError: // onError
		printf("StringEventOnError: %s\n", eventData);
		break;
	case StringEventOnMessage:
	{
		/*if (eventData[0]!='m')
		{
			printf("StringEventOnMessage: '%.200s'\n", eventData);
		}
		else*/
		{
			mainDebug("StringEventOnMessage",eventData);
		}

		WordReader *wr=new WordReader(eventData); // wr shall be deleted further down in this method.
		assert(wr!=NULL);

		/*
		// This code is just some logging for debugging, it can be deleted later.
		WordReader *wr2=new WordReader(eventData);
		int n=0;
		for(;;)
		{
			const char* str=wr2->readString();
			if (str==NULL)
			{
				break;
			}
			printf("mainHandleStringEvent: %d %s\n", n, str);
			++n;
		}
		delete(wr2);
		*/

		evaluateMessageFromServer(wr);

		delete(wr);

		break;
	}
	case StringEventMouseButtonUp:
	{
		WordReader wr(eventData);
		WindowObject *windowObject=rootDb->getWindowObject();
		assert(windowObject!=NULL);
		const int x = wr.readInt();
		const int y = wr.readInt();
		const float f = windowObject->translateScreenToNormatedY(y);
		const InputMode i = textInputBuffer->GetMode();
		switch(i)
		{
			case InputModeEnteringButtonOrMenu:
			{
				printf("StringEventMouseButtonUp InputModeEnteringButton, x=%d, y=%d\n", x, y);

				const char * cmd = localAvatar->isButton(f);
				if (cmd!=NULL)
				{
					printf("button cmd: '%.160s'\n",cmd);
					mainSend(cmd);
					//inputBufferClear();
					textInputBuffer->clearText();
					localAvatar->clearText();
				}

				break;
			}
			default:
				if (!mouseIsPointerLock())
				{
					printf("StringEventMouseButtonUp default, x=%d, y=%d\n", x, y);

					const char * cmd = localAvatar->isButton(f);
					if (cmd!=NULL)
					{
						printf("cmd: %.160s\n",cmd);
						char tmpStr[160];
						sprintf(tmpStr, "invSelect %.120s", cmd);
						mainSend(tmpStr);
						textInputBuffer->clearText();
						localAvatar->clearText();
						localAvatar=textMessagesPage;

					}
					// TODO Only capture mouse if it was locked before entering inventory, a flag would need to be set in "case SymMap::invScreen:".
					mouseCapture();
				}
				break;
		}
		break;
	}
	case StringEventMouseButtonDown:
	{
		//if (SDL_GetRelativeMouseMode())   // Does this work on web? Will use a local flag instead for now.
		if (mouseIsPointerLock())
		{
			mainSend("actionKey");
		}
		else
		{
			//WordReader wr(eventData);
			//const int x = wr.readInt();
			//const int y = wr.readInt();
			//const float f = translateScreenToNormatedY(y);
		}
		break;
	}
	case StringEventMouseWheel:
	{
		WordReader wr(eventData);
		const int w = wr.readInt();
		if (w<0)
		{
			localAvatar->scrollUp();
		}
		else if (w>0)
		{
			localAvatar->scrollDown();
		}
		else
		{
			printf("wheel %d\n",w);
		}

		break;
	}
	case StringEventOnClose:
		printf("StringEventOnClose: %s\n", eventData);
		break;
	default:
		printf("mainHandleStringEvent: %d %s\n", eventId, eventData);
		break;
	}


	//mainTick();

	sprintf(mainReturnString, "no return string");


	#ifdef SOFT_ERROR
	if (debugDetectMultiThreading!=2)
	{
		printf("debugDetectMultiThreading %d\n",debugDetectMultiThreading);
	}
	#else
	assert(debugDetectMultiThreading==2);
	#endif
	debugDetectMultiThreading=0;


	return mainReturnString;
}



// http://kripken.github.io/emscripten-site/docs/api_reference/module.html


// Returns 0 if OK, non zero if not
int openConnection(const char *hostname, int port)
{

	#ifndef __EMSCRIPTEN__

		int r = hcaSocket.connect(hostname, port);
		if (r >= 0)
		{
			mainHandleStringEvent(StringEventOnOpen, "");
		}
		else
		{
			char str[256];
			snprintf(str, sizeof(str), "Could not connect to %.160s:%d", hostname, port);
			//fprintf(stderr,"Fail: %s\n",str);
			//fprintf(stdout,"Fail: %s\n",str);
			mainHandleStringEvent(StringEventOnError, str);
			mainCounter=0;
			return 1;
		}

	#else

		char wsUrl[1024+100]={0};
		sprintf(wsUrl, "ws://%.1024s:%d/", hostname, port);
		assert(wsUrl[sizeof(wsUrl)-1]==0);

		const int r = EM_ASM_INT({
			var s = Pointer_stringify($0);
			onTestWebSocket(s);
			return 0;
		}, wsUrl);

	#endif

	return 0;
}


int closeConnection()
{
	#ifndef __EMSCRIPTEN__

		hcaSocket.close();

	#else

		EM_ASM( websocket.close() );

	#endif

	return 0;
}


#ifndef __EMSCRIPTEN__

void mainPoll()
{
	SDL_Event e;

	//mainCounter--;

	//Handle events on queue
	while ( SDL_PollEvent( &e ) != 0 )
	{
		switch(e.type)
		{
			case SDL_QUIT: //User requests quit
			   printf("quit event\n");
			   mainCounter = 0;
			   break;

			case SDL_KEYDOWN:  //User presses a key
			{
				processKeyDownEvent((int)e.key.keysym.sym);
				break;
			}
			case SDL_KEYUP:  //User releases a key
			{
				processKeyUpEvent((int)e.key.keysym.sym);
				break;
			}
            case SDL_TEXTINPUT:
				processKeysPressed((const char *)e.text.text);
                break;
			case SDL_TEXTEDITING:
			{
				// TODO We should use this instead of writing own code to handle delete, backspace arrows etc in processKeyDownEvent.
				//printf("SDL_TEXTEDITING '%s'\n", e.edit.text);
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				//int x=e.wheel.x;
				int y=e.wheel.y;
				//SDL_GetMouseState( &x, &y );

				if (y>0)
				{
					printf("scroll plus event %d\n", y);
					localAvatar->scrollUp();
				}
				else if (y<0)
				{
					printf("scroll minus event %d\n", y);
					localAvatar->scrollDown();
				}
				else
				{
					printf("SDL_MOUSEWHEEL\n");
				}

				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				//Get mouse position
				int x, y;
				SDL_GetMouseState( &x, &y );
				printf("mouse button down event %d %d\n", x, y);

				// Create a string event
				char str[256];
				sprintf(str, "%d %d", x, y);
				mainHandleStringEvent(StringEventMouseButtonDown, str);

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				//Get mouse position
				int x, y;
				SDL_GetMouseState( &x, &y );
				printf("mouse button up event %d %d\n", x, y);

				// Create a string event
				char str[256];
				sprintf(str, "%d %d", x, y);
				mainHandleStringEvent(StringEventMouseButtonUp, str);

				break;
			}
			case SDL_MOUSEMOTION:
			{
				/*#ifdef __EMSCRIPTEN__
				int x=e.motion.xrel;
				int y=e.motion.yrel;
				// We will ignore mouse events here since our handwritten JavaScript will take care of it.
				//SDL_WarpMouseInWindow(mainWindow, MAIN_SCREEN_WIDTH/2, MAIN_SCREEN_HEIGHT/2);
				#else*/
				int x,y;
				SDL_GetRelativeMouseState(&x, &y);
				//#endif

				if (x!=0)
				{
					processMouseMoveXEvent(x);
				}
				if (y!=0)
				{
					processMouseMoveYEvent(y);
				}
				break;
			}
			default:
				break;
		}

	}
}


static inline void mainPollAndTick()
{
	// Poll input from user
	mainPoll();

	// perform tick/update/draw images
	mainTick();
}



static void help(const char *pgmName)
{
	printf("To few arguments\n");
	printf("usage:\n");
	printf("  %s [options] <hostname> <portnr>\n", pgmName);
	printf("\n");
	printf("options:\n");
	printf("  -n<n>  : Set remote host to <n>\n");
	printf("  -p<n>  : Set IP port to <n> \n");
	printf("  -x<n>  : Set screen width to <n> \n");
	printf("  -y<n>  : Set screen height to <n> \n");
	printf("  -f<n>  : Use image <n> for fonts\n");
	printf("example:\n");
	printf("%s -fcourier8x16.png -x1536 -y1024 localhost 8080\n", pgmName);
	printf("\n");
}

#else
static void help(const char *pgmName)
{
	printf("Sorry, in web version help is not implemented yet.\n");
}

#endif


#ifndef __EMSCRIPTEN__

void debugTest()
{
	WordReader wr("\"\\x41\\101string\\t\\t\\twith esc seq\"  this! is,a  \"test string\"--,    (b+=d+a[i++]--) \"this is an \nembedded string\" 2122,5445.5676 -644 ++   ");
	//WordReader wr("ma 2 0 YukigassenBlockRoom 1 \"spawnRoom\" 2 0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 0.0 -1.0 null 7562750 64.0 32.0 16.0 0.0 0.0 1.6350001 0.0 0.0 0.0 64 32 16 8 4 2");
	//WordReader wr("ma 2 0 YukigassenBlockRoom 1 \"spawnRoom\" 2 0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 0.0 -1.0 null 38750 64.0 32.0 16.0 0.0 0.0 4.905 0.0 0.0 0.0 64 32 16 8 4 2");
	for(;;)
	{
		const char *str = wr.readToken();
		if (str==NULL)
		{
			break;
		}
		printf("str: '%s'\n",str);
		fflush(stdout);
	}
}



void debugDump(const char *fileName)
{
	if (globalDb!=NULL)
	{
		char cwd[8192];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		   fprintf(stdout, "Current working dir: %s\n", cwd);
		else
		   perror("getcwd() error");

		fflush(stdout);

		{
			WordWriter* ww = new WordWriter();
			ww->setSeparatorChar(',');
			ww->writeObjBegin();
			ww->writeString(globalDb->getTypeName());
			globalDb->writeJson(ww);
			ww->writeObjEnd();

			const char *str = ww->getString();
			//const int n = ww->getNBytes();

			{
				FILE *fp = fopen(fileName, "w");
				if (fp != NULL)
				{
					fputs(str, fp);
					fclose(fp);
				}
			}
			delete ww;

			fprintf(stdout, "Saved debug info in: %s/debug.json\n", cwd);
		}
	}
}

void debugLoad(const char* fileName)
{
	WordWriter* ww = new WordWriter();

	{
		FILE *fp = fopen(fileName, "r");
		if (fp != NULL)
		{
			int c;
			while ((c = getc(fp)) != EOF)
			{
				ww->addChar(c);
			}
			fclose(fp);
		}
	}

	if (ww->getNBytes()>0)
	{

		const char *tmpStr=ww->getString();


		WordReader *wr=new WordReader(tmpStr);

		wr->separatorChar=',';

		if (!wr->readObjBegin())
		{
			printf("did not find begin marker\n");
		}

		const char* objectTypeStr=wr->readString();

		MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

		if (mb!=NULL)
		{
			assert(globalDb==NULL);
			globalDb=dynamic_cast<MirrorIdList*>(mb);
			assert(globalDb!=NULL); // If this fails then perhaps the game root object is not an IdList? It must be.
			assert(mb->parentId==-1);

			mb->readJson(wr, rootDb);

			mb->setupInternalRenderingData();

		}
		else
		{
			// Could not create the object. Probably an unknown type.
			printf("mirrorAdd failed on %s\n", objectTypeStr);
		}

		if (!wr->readObjEnd())
		{
			printf("did not find end marker\n");
		}

		delete wr;
	}

	delete ww;
}


#endif


// Initialize OpenGL shader programs etc
// returns:
//   >=0 if OK
//   <0 if something failed
static int mainInit(int screenWidth, int screenHeight)
{

	// Initialize SDL's Video subsystem. TODO: We may need to init more here, like SDL_INIT_AUDIO or SDL_INIT_EVERYTHING.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf( "SDL could not initialize, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}

	printf("ticks %d\n", SDL_GetTicks());

	// OpenGL version to use
#ifdef USE_OPENGL_2_1
	//This is for web GL
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
#else
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#endif


	rootDb = new WindowObject(screenWidth, screenHeight); // Objects controlled from server are stored here. Client shall not manipulate these objects.
	rootDb->setName("root");
	WindowObject *windowObject=rootDb->getWindowObject();
	assert(windowObject!=NULL);
	windowObject->init();



	imageLoaderInit();
	if (DrawText::init(fontFileBig, fontFileSmall)!=0)
	//if (DrawText::init(fontFile, fontFile)!=0)
	{
		printf("Did not find one (or both) font files '%s' '%s'\n", fontFileBig, fontFileSmall);
		return -1;
	}
	drawImageInit();


	// So now we have a window to draw/render in

	// Initialize OpenGL


	// Set the background almost black
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	if (logGlError()) return -1;
	// Create the shader programs
	if (quad2dInitGl()<0) {return -2;}
	if (pyramid3dInitGl()<0) {return -3;}
	if (cube3dProgramInitGl()<0) {return -4;}
	if (mainInitGlBox2DTex()<0) {return -5;}
	if (texture2d5InitGl()<0) {return -6;}
	if (Shaders::loadDefaultShaders()<0) {return -7;}

	// Projection matrix : 45° Field of View, ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, windowObject->mainAspectRatio(), 0.1f, 100.0f);

	// Now add some objects so we can test if our shader programs work.


	localDb = new MirrorIdList(); // Objects not controlled from server are stored here. Objects generated locally are stored here.
	localDb->setName("localDb");
	localDb->linkSelf(rootDb);


	// The login screen
	localRenderMb=new MirrorContainer(); // Local content that shall be shown shall be stored here (this is a folder in localDb).
	localRenderMb->setName("localRenderMb");
	localRenderMb->linkSelf(localDb);

	MirrorContainer * localHiddenMb=new MirrorContainer(); // Local content that shall not be shown (well not shown as part localRenderMb at least, some of the objects here may still be rendered by someone else) of shall be stored here (this is a folder in localDb). Client can move object between localRenderMb and localHiddenMb to get then shown or not shown.
	localHiddenMb->setName("localHiddenMb");
	localHiddenMb->linkSelf(localDb);

	localAvatar=localRenderMb;

	// This is the little pyramid, shown at login.
	{
		colorPyramid = new ColorPyramid(5.0,1.0,2.0);
		colorPyramid->setName("colorPyramid");
		colorPyramid->linkSelf(localRenderMb);
	}

	// This is a little model, tihi
	{
		modelLoaded = new ModelObjLoader(-2.0,0.0,0.0);
		modelLoaded->loadObjModel("cube.obj");
		modelLoaded->setName("modelLoaded");
		modelLoaded->linkSelf(localRenderMb);

	}

	/*
	{
		TexCube *texCube = new TexCube(2.0, 1.0, 2.0);
		texCube->parentId=localRenderMb->id;
		texCube->linkSelfIntoDb(localDb);
	}*/

	// This is the crosshair, helps player to see where they aim.
	{
		//CrossHair *crossHair = new CrossHair(0.1f, 0.1f);
		CrossHair *crossHair = new CrossHair(32, 32);
		crossHair->setName("crossHair");
		crossHair->linkSelf(localRenderMb);
		printf("created %s ~%d\n", crossHair->getName(), crossHair->getId());
	}


	// This is just a small decoration, not useful for anything.
	{
		colorBox = new Color2DBox(0.9, -0.9, 0.1f, 0.1f);
		colorBox->setName("colorBox");
		colorBox->linkSelf(localRenderMb);
	}

	// Get size of the big font since these are needed more than once below.
	const int y=drawTextBig->getSizeY();

	// This is used to show how many snow balls a player has.
	{
		const int x=drawTextBig->getSizeX();
		amoTextBox = new Text2DBox(drawTextBig, 0.9f, 0.9f, windowObject->scale2dPixelToGlFloatX(x), windowObject->scale2dPixelToGlFloatY(y));
		amoTextBox->setName("amoTextBox");
		amoTextBox->linkSelf(localRenderMb);
	}


	// This is used for messages and menu options etc-
	{
		textMessagesPage=new TextPageScroll(drawTextSmall, 0.0f, 1.0f-windowObject->scale2dPixelToGlFloatY(8), 2.0f-windowObject->scale2dPixelToGlFloatX(16), 2.0f-windowObject->scale2dPixelToGlFloatY(8+y+8+8));
		textMessagesPage->setName("textMessagesPage");
		textMessagesPage->linkSelf(localHiddenMb);
		textMessagesPage->setUp();
		printf("created %s ~%d\n", textMessagesPage->getName(), textMessagesPage->getId());
	}

	// This is used for messages and menu options etc-
	{
		menuPage=new TextPageScroll(drawTextBig, 0.0f, 1.0f-windowObject->scale2dPixelToGlFloatY(8), 2.0f-windowObject->scale2dPixelToGlFloatX(16), 2.0f-windowObject->scale2dPixelToGlFloatY(8+y+8+8));
		menuPage->setName("menuPage");
		menuPage->linkSelf(localHiddenMb);
		menuPage->setUp();
		printf("created %s ~%d\n", menuPage->getName(), menuPage->getId());
	}


	localAvatar=menuPage;

	textStatisticsPage=new TextPageScroll(drawTextBig, 0.0f, 1.0f-windowObject->scale2dPixelToGlFloatY(8), 2.0f-windowObject->scale2dPixelToGlFloatX(16), 2.0f-windowObject->scale2dPixelToGlFloatY(8+y+8+8));
	textStatisticsPage->setName("textStatisticsPage");
	textStatisticsPage->linkSelf(localHiddenMb);
	textStatisticsPage->setUp();
	printf("created %s ~%d\n", textStatisticsPage->getName(), textStatisticsPage->getId());

	textInventoryPage=new TextPageScroll(drawTextBig, 0.0f, 1.0f-windowObject->scale2dPixelToGlFloatY(8), 2.0f-windowObject->scale2dPixelToGlFloatX(16), 2.0f-windowObject->scale2dPixelToGlFloatY(8+y+8+8));
	textInventoryPage->setName("textInventoryPage");
	textInventoryPage->linkSelf(localHiddenMb);
	textInventoryPage->setUp();
	printf("created %s ~%d\n", textStatisticsPage->getName(), textStatisticsPage->getId());


	textInputBuffer=new InputBuffer(drawTextBig, 0.0f, -1.0+windowObject->scale2dPixelToGlFloatY(y/2+8), 2.0f-windowObject->scale2dPixelToGlFloatX(16), windowObject->scale2dPixelToGlFloatY(y));
	textInputBuffer->setName("textInputBuffer");
	textInputBuffer->linkSelf(localRenderMb);
	textInputBuffer->setUp();



	// This is used to show a mask over the entire screen, used when a player is hit by a snow ball.
	hudScreenBox = new Text2DBox(drawTextBig, 0.0f, 0.0f, 1.98f, 1.98f);
	hudScreenBox->setName("hudScreenBox");
	hudScreenBox->linkSelf(localRenderMb);



	// Init Audio
	{
		const int r=SoundLoaderInitAudio();
		if (r!=0)
		{
			printf("SoundLoader failed, r=%d\n", r);
			return -1;
		}
		SoundLoaderPlayer::playSoundFile("beep.ogg");
	}


	mainTime = SDL_GetTicks();

	return 0;
}


void mainCleanup();



// This is used if compiling for native or emscripten to html target (for other targets mainInit is used).
extern "C" int main(int argc, char** argv) {
	char *hostname=NULL;
	int port=8080;
	int screenWidth=512+256;
	int screenHeight=512;

	printf("main: command line arguments:\n");
	for(int i=0; i<argc; i++)
	{
		printf("  %d %s\n",i, argv[i]);
	}
	fflush(stdout);


	for(int i=1; i<argc; i++)
	{
		char *str=argv[i];
		int ch0=str[0];
		if (ch0=='-')
		{
			int ch1=str[1];
			switch(ch1)
			{
				case 'n':hostname=str+2;break;
				case 'p':port=atoi(str+2);break;
				case 'x':screenWidth=atoi(str+2);break;
				case 'y':screenHeight=atoi(str+2);break;
				case 'b':fontFileBig=str+2;break;
				case 's':fontFileSmall=str+2;break;
				default:
					printf("unknown switch '%s'",str);
					help(argv[0]);
					break;
			}
		}
		else
		{
			if (hostname==NULL)
			{
				hostname=str;
			}
			else
			{
				port=atoi(str);
			}
		}
	}



	//mainTest();



	#ifdef __EMSCRIPTEN__
		if (mainInit(screenWidth, screenHeight)==0)
		{

			// http://kripken.github.io/emscripten-site/docs/getting_started/FAQ.html
			// How can I tell when the page is fully loaded and it is safe to call compiled functions?
			EM_ASM( allReady() );

			// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
			// http://kripken.github.io/emscripten-site/docs/porting/emscripten-runtime-environment.html#emscripten-runtime-environment
			// http://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html
			// Not using mainPollAndTick here, we register all event handlers in handwritten JavaScript (allReady) since when this was written (Mars 2015) SDL_SetRelativeMouseMode would not work otherwise. And perhaps websockets need that also.
			emscripten_set_main_loop(mainTick, 0, 1);

			EM_ASM( allOver() );

		}

		mainCleanup();
	#else
		if (hostname==NULL)
		{
			return 0;
		}

		printf("remote host name %s\n", hostname);
		printf("port %d\n", port);
		printf("width %d\n", screenWidth);
		printf("height %d\n", screenHeight);

		// Open tcpip connection to server here.
		const int r = openConnection(hostname, port);

		if (r)
		{
			//SoundLoaderPlayer::playSoundFile("boom.ogg");
			//SDL_Delay(5000);

			printf("\n");
			fflush(stdout);
		}
		else if (mainInit(screenWidth, screenHeight)==0)
		{

			// Capture mouse. Perhaps we should do this when user clicks the window instead. For debugging purposes mouse capture is done manually by pressing home button.
			//SDL_SetRelativeMouseMode(SDL_TRUE);



			{
				// The main loop
				while ((mainCounter>0)) {

					// Poll for messages from server
					const char *str = hcaSocket.receive();

					if (str!=NULL)
					{
						if (*str!=0)
						{
							mainHandleStringEvent(StringEventOnMessage, str);
						}
					}
					else
					{
						if (!hcaSocket.isOpen())
						{
							printf("TCP/IP connection lost\n");
							break;
						}
					}

					mainPollAndTick();

					SDL_Delay(1);
				}
			}



			//printf("mainCounter <= 0\n");
			closeConnection();

			SDL_SetRelativeMouseMode(SDL_FALSE);

			mainCleanup();
		}



		#endif

	printf("\n");

	return 0;
}



void mainCleanup()
{
	cleanupGame();

	drawImageQuit();
	DrawText::quit();
	imageLoaderQuit();

	YukigassenTexturedRoomCleanup();
	//TexCubeCleanup();

	glDeleteProgram(quad2dProgramId);

	cube3dCleanup();

	glDeleteProgram(pyramid3dProgramId);

	glDeleteProgram(box2DProgramId);

	SoundLoaderCleanupAudio();

	WindowObject *windowObject=rootDb->getWindowObject();
	assert(windowObject);
	windowObject->cleanup();

	delete rootDb;
	rootDb=NULL;
	localAvatar=NULL;
	windowObject=NULL;

	SDL_CloseAudio();

	Mix_Quit();

	SDL_Quit();
	//printf("quit %d\n", SDL_GetTicks());

	stringCopyCleanup();

	// check for memory leaks
	assert(mirrorBaseDebugCounter==0);
#ifndef __EMSCRIPTEN__
	assert(hcaDebugCount==0);
#endif

	WordReader::cleanup();

	printf("game over\n");
	fflush(stdout);
}
