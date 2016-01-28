/*
hello_world_sdl2.cpp

The plan is to make a demo with some OpenGl/WebGl rendering with Emscripten using shaders.
Originally I had a version with direct rendering. This program will show how to upgrade from that to using shaders.



www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

The code here is made using tutorials, they have made much effort and so have I. So you can only use this code if you keep this text, all references (comments) and update the history section if you modify it. Or actually you mus go to the sources and take it from there. I have marked sources as much as possible




History
Created by Henrik Bjorkman 2014-11-09 using code and tutorials from internet kripken, lazyfoo and others. NOTE! Their copyright may apply.
2015-03-07 Trying again.


*/

// In OpenGL ES 2.0, to render any geometry to the scene, you have to create two tiny little programs called shaders.
// Shaders are written in a C-like language called GLSL. More reading:
//
// http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Table-of-Contents.html
// http://www.arcsynthesis.org/gltut/
// http://www.opengl-tutorial.org/
// https://www.opengl.org/wiki/Getting_Started
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
// http://nehe.gamedev.net/
// https://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_%28C_/_SDL%29
// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
// http://www.raywenderlich.com/3664/opengl-tutorial-for-ios-opengl-es-2-0
// http://en.wikibooks.org/wiki/OpenGL_Programming
// http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
// http://www.lighthouse3d.com/tutorials/glsl-core-tutorial/color-example/

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
#include "MirrorDb.h"
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
#include "AvatarPlayerReferences.h"
#include "YukigassenWorld.h"
#include "YukigassenInventory.h"
#include "YukigassenAvatar.h"
#include "YukigassenRoom.h"
#include "YukigassenChunkRoom.h"
#include "InputBuffer.h"
#include "SymMap.h"
#include "main_sdl2.h"



// JavaScript shall not be multithreaded but there is some sort of problem in this aspect. This determines if we do assert or just log the problem. It shall be assert but...
#define SOFT_ERROR






// 3D coordinates
// X is right
// Y is up (along the plane of the screen)
// Z is out of the screen (towards the user looking at the screen)

// 3D axis colors
// X : read
// Y : green
// Z : blue



// These numbers are also used in hello.js and/or on.js if changing here remember to change there also.
enum {
	NumericEventKeyDown = 1,
	NumericEventKeyUp = 2,
	NumericEventMovementX = 3,
	NumericEventMovementY = 4,
	NumericEventKeyPress = 5
};

// These numbers are also used in hello.js and/or on.js if changing here remember to change there also.
enum {
	StringEventOnOpen = 1,
	StringEventOnError = 2,
	StringEventOnMessage = 3,
	StringEventOnClose = 4,
	StringEventMouseButtonDown = 5,
	StringEventMouseButtonUp = 6,
	StringEventMouseWheel = 7,
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


SDL_Window* mainWindow;
SDL_Renderer* mainRenderer;
SDL_GLContext mainGlContext;







// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06







//TextPageScroll *textPage=NULL; // This is the regular text page, menu options, messages etc are shown here.
//TextPageScroll *tabPage=NULL; // This is the alternative text page, it is shown when user hold down the tab key.
InputBuffer *textInputBuffer=NULL;



// Text and 2d images
Text2DBox *hudScreenBox=NULL;
Text2DBox *amoTextBox=NULL; // Wrong name, not used for ammo but for count down after being hit.


/*
class InputButton
{
	public:
	int upper;
	int lower;
	int ch;
};

InputButton inputButtons[10];
*/

ColorPyramid* colorPyramid=NULL;




//char strBuf[256]={0};



MirrorDb *globalDb=NULL; // All objects sent from server shall be stored here.
MirrorDb *localDb=NULL; // Local objects (not from server) to be rendered are stored here.
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


MirrorBase *localRenderMb=NULL;
MirrorBase *localHiddenMb=NULL;
MirrorBase *playerPreferenceMb=NULL;

Color2DBox *colorBox;


int debugDetectMultiThreading=0; // This is for debugging only, it can be removed later when everything is runs perfectly.
bool groundContact=false;
int serverChangedPositionCounter=0;

int playerIndex=-1;
int playerCode=-1;

SymMap symMap;

int prevAvatarAmmo=0x7FFFFFF;


void mainRender()
{

	// Clear The Screen And The Depth Buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );




	// Read more at: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/



	// TODO The avatar can be in a sub room in which case its pao must be translated to coordinates of this room first.
    // See also void D3ObjWithPos::renderAll()


	assert(globalDb!=NULL);


	// Here we rended the 3D objects.
	// If avatar is not in the topmost room we need to translate its position into that.
	const MirrorBase *avatarMb=globalDb->getObjectById(ourAvatarId);
	if (avatarMb==NULL)
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

		globalDb->renderAll();

	}
	else
	{
		// As soon as we are in a game, and we have an avatar, this is used.

		D3PosAndOrientation renderPos(mainCameraPao);  // mainCameraPao is the avatars position given in the coordinates of the room avatar is in.
		MirrorBase *renderMb=avatarMb->getParentObj(); // This is the room for which we the renderPos is relevant.


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

	assert(textFocusPage!=NULL);
	textFocusPage->renderAll();

	assert(localRenderMb!=NULL);
	localRenderMb->renderAll();





	// Perhaps not needed but resetting the program used.
	glUseProgram(0);


    // This command tells GL to show the newly rendered buffer (replacing the previous).
	SDL_RenderPresent(mainRenderer);  // It is also possible to use glutSwapBuffers() here
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

		const float margin = 0.2f;
		MirrorBase *avatarMb=globalDb->getObjectById(ourAvatarId);
		YukigassenAvatar *ya=dynamic_cast<YukigassenAvatar *>(avatarMb);
		//ya->limitPos(pao.pos);


		// pao can be looking up, but running is along the plane and jump is perpendicular to the plane. The plane is perpendicular to gravity/acceleration.

		D3Vector currentPos(mainCameraPao.pos); // Remember current position if move is not possible
		D3PosAndOrientation zPao(mainCameraPao); // Will later be set to the direction player is looking locked in the Z plane


		YukigassenRoom *ybr = dynamic_cast<YukigassenRoom*>(ya->getParentObj());
		assert(ybr!=NULL);

		const float distToGround = ybr->calcDistToGround(&currentPos);

		float bodyLength=YukigassenAvatar::noseToFeetLength;
		if (mainCrouch)
		{
			bodyLength*=0.5;
		}

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
			// The server has not changed avatar position, it is OK for client to move it.
			if (groundContact || ((distToGround<bodyLength) && (aVelocity.z>=0.0f)))
			{
				// This avatar is on contact with the ground

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
					// forward
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
				// In air, no contact with ground

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


		// This checks if we shall show the player hit "snow screen". An overlay 2D texture.
		if (ya->avatarStateMs)
		{
			// player has been hit.
			if (hitIndicationIsOn==false)
			{
				// hit indication is off so it needs to be turned on.
				hudScreenBox->setImage(0,0, 256, 256);
				textMessagesPage->addText("You where hit!");
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
		AvatarPlayerReferences *apr = dynamic_cast<AvatarPlayerReferences *>(globalDb->getObjectById(ya->avatarPlayerReferencesId));
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
							const char *str=rs->getValue();
							textMessagesPage->addText(str,0);
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
			if (textFocusPage != textStatisticsPage)
			{
				textFocusPage = textStatisticsPage;
			}

			textStatisticsPage->clearText();

			MirrorBase *m = globalDb->findChildByName("playerList", 2);
			if (m!=NULL)
			{
				//                01234567890123456789012345678901
				textStatisticsPage->addText("player                  team  score",0);

				MirrorContainer *pl=dynamic_cast<MirrorContainer*>(m);

				for(int i=0; i<pl->childVector.size(); ++i)
				{
					MirrorBase *mb=pl->childVector[i];
					AvatarPlayerReferences* apr=dynamic_cast<AvatarPlayerReferences*>(mb);
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
			textStatisticsPage->addText("",0);

			//showPlayers=false;
		}
		else
		{
			// The statistics page is not wanted
			if (textFocusPage == textStatisticsPage)
			{
				// We had an existing page, it can be deleted since its not wanted now.
				textStatisticsPage->clearText();
				textFocusPage=textMessagesPage;
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

	// TODO: We shall not only uppdate avatar pos but all objects in database that have speed. For now we let server do those calculations. See TODO comment in YukigassenRoom.tickRoom.
	// In the long run all moving objects should be registered in a list and the positions for those extrapolated here



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
	printf("cleanupGame\n");

	globalDb->clear();
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

// This shall be called when a key is pressed down.
static int processKeyDownEvent(int key)
{

	if (textInputBuffer->GetMode() == InputModeIdle)
	{
		// not entering text, so it is direct key input.

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
				printf("backOrCancel\n");
				mainSend("backOrCancel");
				cleanupGame();
				break;
			case SymMap::endCode:
				printf("end event\n");
				mainCounter = 0;
				return 1;
			case SymMap::showScore:
				showPlayerStats=true;
				break;
			case SymMap::returnCode:
				printf("returnCode\n");
				textInputBuffer->DoneNowSend();
				break;
			case SymMap::upOne:
				textFocusPage->scrollUp();
				break;
			case SymMap::downOne:
				textFocusPage->scrollDown();
				break;
			case SymMap::leftOne:
				mainCameraPao.yaw_right(-0.01f);
				break;
			case SymMap::rightOne:
				mainCameraPao.yaw_right(0.01f);
				break;
			case SymMap::pageUp:
				printf("pg up event\n");
				textFocusPage->scrollUp();
				textFocusPage->scrollUp();
				textFocusPage->scrollUp();
				textFocusPage->scrollUp();
				break;
			case SymMap::pageDown:
				printf("pg down event\n");
				textFocusPage->scrollDown();
				textFocusPage->scrollDown();
				textFocusPage->scrollDown();
				textFocusPage->scrollDown();
				break;
			case SymMap::jumpUp:
				mainSpeedUp = 0.1f;
				break;
			case SymMap::mouseCapture:
				// home
				printf("mouseCapture\n");
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
				printf("testBeep\n");
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
									char tStr[160]={0};
									sprintf(tStr, "%03lld %.120s", rs->getNumberOf(), rs->getValue());
									assert(tStr[sizeof(tStr)-1]==0);
									textInventoryPage->addText(tStr, indexStr);
								}
								else
								{
									char tStr[160]={0};
									sprintf(tStr, "%03lld %.120s", 1LL, mb->getName());
									assert(tStr[sizeof(tStr)-1]==0);
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
				textFocusPage=textInventoryPage;
				mouseRelease();

				break;
			}
			case SymMap::enterText:
				printf("enterText\n");
				break;
			case SymMap::crouchOrDuck:
				/*if (mainCrouch==false)
				{
					textPage->addText("crouchOrDuck is not implemented yet\n");
				}*/
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
	}
	else if (textInputBuffer->GetMode() == InputModeEnteringKey)
	{
		// entering single key
		const int spec=symMap.getSpec(key);
		printf("key %d %d\n", key, spec);
		{
			char tmp[40];
			sprintf(tmp, "%d",spec);
			mainSend(tmp);
			textInputBuffer->inputBufferClear();
		}

	}
	else
	{
		// entering text, button or single key
		switch(key)
		{
			case SDLK_UP:
				textInputBuffer->getPrev();
				break;
			case SDLK_DOWN:
				textMessagesPage->clearText();
				break;
			case SDLK_PAGEUP:
				printf("pg up event\n");
				textFocusPage->scrollUp();
				break;
			case SDLK_PAGEDOWN:
				printf("pg down event\n");
				textFocusPage->scrollDown();
				break;
			case SDLK_LEFT:
				printf("left\n");
				textInputBuffer->DelChar();
				break;
			case SDLK_RIGHT:
				printf("right\n");
				break;
			case SDLK_BACKSPACE:
				printf("backspace\n");
				textInputBuffer->DelChar();
				break;
			case SDLK_ESCAPE:
				printf("esc event\n");
				mouseRelease();
				return 1;
			case SDLK_END:
				// end
				mainCounter = 0;
				break;
			default:
				if ((key<' ') || (key>'~'))
				{
					printf("unknown key down event %d\n", key);
				}
				break;
		}

	}
	return 0;
}


// This shall be called when a key is released.
static int processKeyUpEvent(int key)
{
	if (textInputBuffer->GetMode() == InputModeIdle)
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
				textFocusPage=textMessagesPage;
				break;
			case SymMap::enterText:
				printf("enterText\n");
				textInputBuffer->inputEnterStringEntryMode();
				textInputBuffer->SetPrefix("consoleInput");
				break;
			default:
				break;
		}
	}
	else
	{
		// entering text
		switch(key)
		{
			case SDLK_RETURN:
				textInputBuffer->DoneNowSend();
				break;
			default:
				break;
		}
	}
	return 0;
}

// The processKeyDownEvent/processKeyUpEvent are for when a specific hardware key (on the keyboard) is pressed down or released up.
// This function is called when a character is generated from the keyboard. That is not always the same,
// more than one hardware key can be be needed to generate one character. Holding down one key (or a combination of keys) can generate more than one character.
static int processKeyPress(int sym)
{
	switch (textInputBuffer->GetMode())
	{
		case InputModeEnteringButton:
			textInputBuffer->AddChar(sym);
			textInputBuffer->DoneNowSend();
			break;

		case InputModeIdle:
			break;

		default:
			if ((sym>=' ') && (sym<='~')) // TODO: Allow all unicode characters, for now only 7bit ascii
			{
				textInputBuffer->AddChar(sym);
			}
			else
			{
				printf("processKeyPress: %d is not supported yet\n", sym);
				#ifdef __EMSCRIPTEN__
				// Did not get ~to work in web version, this is a temporary workaround.
				if (sym==0)
				{
					textInputBuffer->AddChar('~');
				}
				#endif
			}
	}

	return 0;
}

static void processKeysPressed(const char *str)
{
	while(*str)
	{
		processKeyPress(*str);
		++str;
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

extern "C" int mainHandleNumericEvent(int eventId, int eventData)
{
	/*if (debugDetectMultiThreading!=0)
	{
		printf("debugDetectMultiThreading %d\n",debugDetectMultiThreading);
	}*/
	assert(debugDetectMultiThreading==0);
	debugDetectMultiThreading=3;


	switch(eventId)
	{
	case 0:
		// tick
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

	//mainTick();

	/*if (debugDetectMultiThreading!=3)
	{
		printf("debugDetectMultiThreading %d\n",debugDetectMultiThreading);
	}*/
	assert(debugDetectMultiThreading==3);
	debugDetectMultiThreading=0;

	return 0;
}

void mainUnknownMessageFromServer(const char *str)
{
	printf("mainUnknownMessageFromServer: '%s'\n",str);
}




static unsigned short audio_chunk[1*22050];
static unsigned int audio_len;
static unsigned char *audio_pos;

  /* The audio function callback takes the following parameters:
     stream:  A pointer to the audio buffer to be filled
     len:     The length (in bytes) of the audio buffer
  */
void fill_audio(void *udata, unsigned char *stream, int len)
{
    /* Only play if we have data left */
    if ( audio_len <= 0 )
    {
    	SDL_PauseAudio(1);
        return;
    }

    /* Mix as much data as possible */
    len = ( len > audio_len ? audio_len : len );
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}


// Returns true if and auto answer was made.
// false if not.
static bool autoAnswer(const char *styleTag)
{
	int tch=styleTag[0];

	switch(tch)
	{
		case '2':
			if (strcmp(styleTag, "2d_or_3d_support")==0)
			{
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
		case 's':
			if (strcmp(styleTag, "selectType")==0)
			{
				mainSend("player");
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


// This will evaluated messages from the server
static void evaluateMessageFromServer(WordReader *wr)
{
	const char *cmd=wr->readWord();

	mainDebug("evaluateMessageFromServer: ",cmd);


	const int ch = cmd[0];
	switch(ch)
	{
		case 'a':
			if (strcmp(cmd, "avatarAck")==0)
			{
				// Server must send avatarAck after it receives an avatarPos
				mirrorAckedClientsSeqnr=wr->readInt();
			}
			else if (strcmp(cmd, "avatarId")==0)
			{
				// Server send this to tell us the ID of this players avatar
				const char *avatarIdStr=wr->readWord();
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

				/*#ifdef __EMSCRIPTEN__
				mouseCapture();
				#endif*/
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;

		case 'b':
			if (strcmp(cmd, "beepOne")==0)
			{
				// This is used to test audio, it works somewhat but does not work perfectly yet.
				SDL_PauseAudio(0);
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 'c':
			if (strcmp(cmd, "consoleMessage")==0)
			{
				while(!wr->endOfStr())
				{

					// Server must send avatarAck after it receives an avatarPos
					const char* msg=wr->readString();

					if (msg!=NULL)
					{
						printf("consoleMessage '%s'\n", msg);
						// The message can be more than one line. So it needs to be split into lines before sent to mainTextMessagesPage.
						WordReader wr(msg);
						for(;;)
						{
							const char* str=wr.readLine();
							if (str==NULL)
							{
								break;
							}
							textMessagesPage->addText(str, 0);
						}
					}
				}


			}
			else if (strcmp(cmd, "clientCode")==0)
			{
				// Before server sends a reconnectServer it can send this message to give login codes for the new server.
				playerIndex=wr->readInt();
				playerCode=wr->readInt();
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;

		case 'l':
			if (strcmp(cmd, "listClear")==0)
			{
				// TODO: Perhaps instead of implementing this here we shall remove it from server?
				printf("listClear not implemented yet\n");
			}
			else if (strcmp(cmd, "listAdd")==0)
			{
				const char *listEntry=wr->readString();
				// TODO: Perhaps instead of implementing this here we shall remove it from server?
				printf("listAdd not implemented yet %s\n", listEntry);
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 'm':
			if (strcmp(cmd, "mirrorUpdate")==0)
			{
				// Server sends this when an object has been changed.
				const int objectId = wr->readInt();
				const int newParentId = wr->readInt(); // new parent ID
				const char *objectTypeStr=wr->readWord();

				MirrorBase *mb = globalDb->getObjectById(objectId);
				if (mb != NULL)
				{
					// check that object type is same as before, if not it is a new object and the old one has been deleted.
					if (strcmp(objectTypeStr, mb->getTypeName())!=0)
					{
						printf("mirrorUpdate: Object ~%d, type is not supposed to be changed: oldType=%s, newType=%s\n", objectId, mb->getTypeName(), objectTypeStr);
#if 1
						// This code is not tested. It might work or not.
						mb->unlinkSelf();
						delete(mb);

						MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

						if (mb!=NULL)
						{
							printf("mirrorAdd ~%d %s\n", objectId, objectTypeStr);

							mb->id = objectId;
							mb->parentId = newParentId;
							mb->readSelf(wr);

							mb->linkSelfIntoDb(globalDb);

							mb->setupInternalRenderingData();
						}
						else
						{
							printf("mirrorFactoryCreate failed on %s\n", objectTypeStr);
						}
#endif
					}
					else
					{
						const int prevParentId = mb->parentId;
						if (newParentId != prevParentId)
						{
							//printf("moving object between rooms ~%d ~%d ~%d %d\n",objectId, prevParentId, newParentId, mb->index);

							MirrorContainer *prevParent=(MirrorContainer *)(globalDb->getObjectById(prevParentId));
							MirrorContainer *newParent=(MirrorContainer *)(globalDb->getObjectById(newParentId));

							prevParent->removeChildObject(mb);

							mb->readSelf(wr);

							newParent->addChildObject(mb);

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
								MirrorContainer *parent=(MirrorContainer *)(globalDb->getObjectById(prevParentId));
								//printf("changing index within room is not supported ~%d %d %d\n",objectId, oldIndex, mb->index);
								// For now will set index to old value. But we should also remove it from its parent room, set new index and put it back.
								// TODO: This happens a lot, perhaps it is a server error that makes it happen so often.
								mb->index=oldIndex;
								parent->removeChildObject(mb);
								mb->index=newIndex;
								parent->addChildObject(mb);

							}
						}
						mb->dataChanged();
					}
				}
				else
				{
					printf("mirrorUpdate: unknown object ~%d\n",objectId);

					// The following code should not be needed. But if server sends mirrorUpdate we are supposed to have the object in globalDb. Aha! If a parent object is deleted, server sent mirrorRemove then we delete all objects stored in it, but server does not delete all of those. Until we figured out how to solve that we just create the object again.
					// The workaround below will only restore objects that are updated, e.g. avatar body is lost and will not return using this since (some) body parts don't need to updated very often. So avatar becomes invisible if it was in a room that was deleted. Hopefully the invisible avatar problem is now solved by a change in iterateUpdatesToClient so that updates are sent first and removals after.

					MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

					if (mb!=NULL)
					{
						printf("mirrorAdd ~%d %s\n", objectId, objectTypeStr);

						mb->id = objectId;
						mb->parentId = newParentId;
						mb->readSelf(wr);

						mb->linkSelfIntoDb(globalDb);

						mb->setupInternalRenderingData();
					}
					else
					{
						printf("mirrorFactoryCreate failed on %s\n", objectTypeStr);
					}




				}
			}
			else if (strcmp(cmd, "mirrorAdd")==0)
			{
				// Server sends this when an object shall be added to our mirror database.

				const int objectId = wr->readInt();
				const int parentId = wr->readInt();
				const char *objectTypeStr=wr->readWord();

				MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

				if (mb!=NULL)
				{
					printf("mirrorAdd ~%d %s\n", objectId, objectTypeStr);

					mb->id = objectId;
					mb->parentId = parentId;
					mb->readSelf(wr);

					mb->linkSelfIntoDb(globalDb);

					mb->setupInternalRenderingData();

				}
				else
				{
					printf("mirrorAdd failed on %s\n", objectTypeStr);
				}

			}
			else if (strcmp(cmd, "mirrorUpdated")==0)
			{
				char str[256];
				const int mirrorServersSeqnr=wr->readInt();
				mirrorAckedClientsSeqnr=wr->readInt();
				sprintf(str,"mirrorAck %d\n", mirrorServersSeqnr);
				mainSend(str);
			}
			else if (strcmp(cmd, "mirrorRemove")==0)
			{
				// Server sends this when an object shall be removed from our mirror database.

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
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 'o':
			if (strcmp(cmd, "openYukigassen")==0)
			{
				// remove demo objects
				const char *wMsg=wr->readWord();
				printf("openYukigassen %s\n",wMsg);
				cleanupGame();
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 'p':
			if (strcmp(cmd, "playerPreference")==0)
			{
				// player preferences are things like which key to use for what
				const char *prefName=wr->readWord();
				const char *prefValue=wr->readString();
				printf("playerPreference %s '%s'\n",prefName, prefValue);

				if (playerPreferenceMb==NULL)
				{
					playerPreferenceMb=new MirrorContainer();
					playerPreferenceMb->setName("playerPreferenceMb");
					playerPreferenceMb->linkSelfIntoDb(localDb);
				}

				/*RsbString *rs=new RsbString();
				rs->setName(prefName);
				rs->setValue(prefValue);
				rs->parentId = playerPreferenceMb->id;
				rs->linkSelfIntoDb(localDb);*/

				symMap.set(prefName, prefValue);

			}
			/*else if (strcmp(cmd, "playerPreferenceDefault")==0) // is this still used? seems not.
			{
				symMap.clearSymMap();

			}*/
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 'q':
			if (strcmp(cmd, "query")==0)
			{
				const char* styleTag = wr->readWord();
				const char* textMsg = wr->readString();
				const char* expectedReplyType = wr->readWord();

				printf("query: '%s', '%s', '%s', '%s'\n", cmd, styleTag, textMsg, expectedReplyType);

				if (strcmp(styleTag, "in_charge")==0)
				{
					printf("wrong game\n");
					mainCounter = 0;
				}

				textInputBuffer->clearText();
				textMessagesPage->clearText();
				textMessagesPage->addText(textMsg, "0");

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
						textMessagesPage->addText(str, buttonText);
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
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 's':
			if (strcmp(cmd, "showWorld")==0)
			{
				// Do nothing, world is shown by main loop
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 'w':
			if (strcmp(cmd, "www.eit.se/rsb/0.9/server")==0)
			{
				// Server sends the line above to identify itself, client must reply with its version string.
				// TODO: no other commands from server should be accepted before this.
				mainSend("www.eit.se/rsb/0.9/client");
			}
			else
			{
				mainUnknownMessageFromServer(cmd);
			}
			break;
		case 0:
			// empty string
			break;
		default:

			mainUnknownMessageFromServer(cmd);
			break;
	}
}


extern "C" const char * mainHandleStringEvent(int eventId, const char* eventData)
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
	case 0:
		// tick
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
		//printf("StringEventOnMessage: '%.200s'\n", eventData);
		mainDebug("StringEventOnMessage: ",eventData);

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
		const int x = wr.readInt();
		const int y = wr.readInt();
		const float f = translateScreenToNormatedY(y);
		const InputMode i = textInputBuffer->GetMode();
		switch(i)
		{
			case InputModeEnteringButton:
			{
				printf("StringEventMouseButtonUp InputModeEnteringButton %d %d\n", x, y);

				const char * cmd = textFocusPage->isButton(f);
				if (cmd!=NULL)
				{
					printf("cmd: b%.160s\n",cmd);
					mainSend(cmd);
					//inputBufferClear();
					textInputBuffer->clearText();
					textFocusPage->clearText();
				}

				break;
			}
/*#ifdef __EMSCRIPTEN__
			case InputModeIdle:
				if ((!mouseIsPointerLock()) && (textFocusPage==textMessagesPage))

				{
					mouseCapture();
				}
				break;
#endif*/
			default:
				if (!mouseIsPointerLock())
				{
					printf("StringEventMouseButtonUp default %d %d\n", x, y);

					const char * cmd = textFocusPage->isButton(f);
					if (cmd!=NULL)
					{
						printf("cmd: %.160s\n",cmd);
						char tmpStr[160];
						sprintf(tmpStr, "invSelect %.120s", cmd);
						mainSend(tmpStr);
						textInputBuffer->clearText();
						textFocusPage->clearText();
						textFocusPage=textMessagesPage;

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
			textFocusPage->scrollUp();
		}
		else if (w>0)
		{
			textFocusPage->scrollDown();
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
			sprintf(str, "Could not connect %.160s:%d", hostname, port);
			mainHandleStringEvent(StringEventOnError, str);
			mainCounter=0;
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
					textFocusPage->scrollUp();
				}
				else if (y<0)
				{
					printf("scroll minus event %d\n", y);
					textFocusPage->scrollDown();
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
	printf("  %s <hostname> <portnr>\n", pgmName);
}

#endif


// Initialize OpenGL shader programs
// returns:
//   >=0 if OK
//   <0 if something failed
static int mainInitGl()
{
	// Set the background almost black
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	if (logGlError()) return -1;

	// Create the shader programs
	if (quad2dInitGl()<0) {return -2;}
	if (pyramid3dInitGl()<0) {return -3;}
	if (cube3dProgramInitGl()<0) {return -4;}
	if (mainInitGlBox2DTex()<0) {return -5;}
	if (texture2d5InitGl()<0) {return -6;}

	// Projection matrix : 45° Field of View, ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, mainAspectRatio, 0.1f, 100.0f);

	// Now add some objects so we can test if our shader programs work.

	globalDb = new MirrorDb(); // Objects controlled from server are stored here. Client shall not manipulate these objects.
	globalDb->setName("globalDb");
	localDb = new MirrorDb(); // Objects not controlled from server are stored here. Objects generated locally are stored here.
	localDb->setName("localDb");

	localRenderMb=new MirrorContainer(); // Local content that shall be shown shall be stored here (this is a folder in localDb).
	localRenderMb->setName("localRenderMb");
	localRenderMb->linkSelfIntoDb(localDb);

	localHiddenMb=new MirrorContainer(); // Local content that shall not be shown (well not shown as part localRenderMb at least, some of the objects here may still be rendered by someone else) of shall be stored here (this is a folder in localDb). Client can move object between localRenderMb and localHiddenMb to get then shown or not shown.
	localHiddenMb->setName("localHiddenMb");
	localHiddenMb->linkSelfIntoDb(localDb);


	// This is the little pyramid, shown at login.
	{
		colorPyramid = new ColorPyramid(5.0,1.0,2.0);
		colorPyramid->setName("colorPyramid");
		colorPyramid->parentId=localRenderMb->id;
		colorPyramid->linkSelfIntoDb(localDb);
	}


	/*
	{
		TexCube *texCube = new TexCube(2.0, 1.0, 2.0);
		texCube->parentId=localRenderMb->id;
		texCube->linkSelfIntoDb(localDb);
	}*/

	// This is the crosshair, helps player to see where they aim.
	{
		CrossHair *crossHair = new CrossHair(0.1f, 0.1f);
		crossHair->setName("crossHair");
		crossHair->parentId=localRenderMb->id;
		crossHair->linkSelfIntoDb(localDb);
		printf("created %s ~%d\n", crossHair->getName(), crossHair->getId());
	}


	// This is just a small decoration, not useful for anything.
	{
		colorBox = new Color2DBox(0.9, -0.9, 0.1f, 0.1f);
		colorBox->setName("colorBox");
		colorBox->parentId=localRenderMb->id;
		colorBox->linkSelfIntoDb(localDb);
	}

	// This is used to show how many snow balls a player have.
	{
		amoTextBox = new Text2DBox(0.9f, 0.9f, 0.3f, 0.15f);
		amoTextBox->setName("amoTextBox");
		amoTextBox->parentId=localRenderMb->id;
		amoTextBox->linkSelfIntoDb(localDb);
	}


	// This is used for messages and menu options etc-
	textMessagesPage=new TextPageScroll();
	textMessagesPage->setName("textMessagesPage");
	textMessagesPage->parentId=localHiddenMb->id;
	textMessagesPage->linkSelfIntoDb(localDb);
	textMessagesPage->setUp();
	printf("created %s ~%d\n", textMessagesPage->getName(), textMessagesPage->getId());

	textFocusPage=textMessagesPage;

	textStatisticsPage=new TextPageScroll();
	textStatisticsPage->setName("textStatisticsPage");
	textStatisticsPage->parentId=localHiddenMb->id;
	textStatisticsPage->linkSelfIntoDb(localDb);
	textStatisticsPage->setUp();
	printf("created %s ~%d\n", textStatisticsPage->getName(), textStatisticsPage->getId());

	textInventoryPage=new TextPageScroll();
	textInventoryPage->setName("textInventoryPage");
	textInventoryPage->parentId=localHiddenMb->id;
	textInventoryPage->linkSelfIntoDb(localDb);
	textInventoryPage->setUp();
	printf("created %s ~%d\n", textStatisticsPage->getName(), textStatisticsPage->getId());


	textInputBuffer=new InputBuffer();
	textInputBuffer->setName("textInputBuffer");
	textInputBuffer->parentId=localRenderMb->id;
	textInputBuffer->linkSelfIntoDb(localDb);
	textInputBuffer->setUp();



	// This is used to show a mask over the entire screen, used when a player is hit by a snow ball.
	hudScreenBox = new Text2DBox(0.0f, 0.0f, 1.98f, 1.98f);
	hudScreenBox->setName("hudScreenBox");
	hudScreenBox->parentId=localRenderMb->id;
	hudScreenBox->linkSelfIntoDb(localDb);

	return 0;
}

//extern void fill_audio(void *udata, unsigned char *stream, int len);


// Returns 0 if OK
int initAudio()
{
    SDL_AudioSpec wanted;

    /* Set the audio format */
    wanted.freq = 22050;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;    /* 1 = mono, 2 = stereo */
    wanted.samples = 1024;  /* Good low-latency value for callback */
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    /* Open the audio device, forcing the desired format */
    if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return(-1);
    }


    for (int i = 0; i < SIZEOF_ARRAY(audio_chunk);i++) {
    	double a=((double)i)*340.0*(2*3.1415)/wanted.freq;
    	double d=sin(a);
    	unsigned short s=0x7fff+10.0*d;
    	audio_chunk[i]=s;
    }
	// Shall audio_len be number of samples or number of bytes?
    //audio_len=SIZEOF_ARRAY(audio_chunk);
    audio_len=sizeof(audio_chunk);

    // Spela en "trudelut"

    audio_pos = (unsigned char *)audio_chunk;
    //SDL_PauseAudio(0); // This repeats the audio until paused.

    return 0;
}



// This is used if compiling for native or emscripten to html target (for other targets mainInit is used).
extern "C" int main(int argc, char** argv) {

	printf("main: command line arguments:\n");
	for(int i=0; i<argc; i++)
	{
		printf("  %d %s\n",i, argv[i]);
	}


	// Initialize SDL's Video subsystem. TODO: We may need to init more here, like SDL_INIT_AUDIO or SDL_INIT_EVERYTHING.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf( "SDL could not initialize, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}

	printf("ticks %d\n", SDL_GetTicks());

	// OpenGL version to use
#ifdef USE_OPENGL_2_1
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
#else
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#endif

	//SDL_CreateWindowAndRenderer(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN, &mainWindow, &mainRenderer);
	mainWindow = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (mainWindow == NULL)
	{
		printf( "SDL could not create window, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}



	mainGlContext = SDL_GL_CreateContext( mainWindow );

	if (mainGlContext == NULL)
	{
		printf( "SDL could not create GL context, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}


	//Initialize GLEW

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
	}



	// Use Vsync
	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	{
		printf( "Set swap interval failed, SDL Error: %s\n", SDL_GetError() );
		return -1;
	}

	// SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, Uint32 flags)
	mainRenderer = SDL_CreateRenderer(mainWindow, 0, SDL_RENDERER_ACCELERATED);
	if (mainRenderer == NULL)
	{
		printf( "SDL could not create renderer, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}

	// Check that we have accelerated OpenGL and target texture.
	SDL_RendererInfo displayRendererInfo;
	SDL_GetRendererInfo(mainRenderer, &displayRendererInfo);
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0) {
		printf("renderer is not accelerated\n");
		return -1;
	}
	if ((displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		printf("no render target texture\n");
		return -1;
	}

	imageLoaderInit();
	drawTextInit();
	drawImageInit();



	// So now we have a window to draw/render in

	// Initialize OpenGL
	const int r = mainInitGl();

	if (r<0)
	{
		// Something failed
		printf("mainInitGl failed, r=%d\n", r);
	}
	else
	{

		printf("hello, world!\n");

		mainTime = SDL_GetTicks();


		// Init Audio
		initAudio();





		#ifdef __EMSCRIPTEN__

			// http://kripken.github.io/emscripten-site/docs/getting_started/FAQ.html
			// How can I tell when the page is fully loaded and it is safe to call compiled functions?
			EM_ASM( allReady() );

			// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
			// http://kripken.github.io/emscripten-site/docs/porting/emscripten-runtime-environment.html#emscripten-runtime-environment
			// http://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html
			// Not using mainPollAndTick here, we register all event handlers in handwritten JavaScript (allReady) since when this was written (Mars 2015) SDL_SetRelativeMouseMode would not work otherwise. And perhaps websockets need that also.
			emscripten_set_main_loop(mainTick, 0, 1);

			EM_ASM( allOver() );

		#else
			// Capture mouse. Perhaps we should do this when user clicks the window instead. For debugging purposes mouse capture is done manually by pressing home button.
			//SDL_SetRelativeMouseMode(SDL_TRUE);



			// Open tcpip connection to server here.
			if (argc>=3)
			{
				const char *hostname = argv[1];
				const int port = atoi(argv[2]);

				openConnection(hostname, port);

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
			else
			{
				help(argv[0]);
			}



			printf("mainCounter <= 0\n");
			closeConnection();

			SDL_SetRelativeMouseMode(SDL_FALSE);

		#endif

	}

	/*
	printf("0 {\n");
	globalDb->debugDump(1);
	printf("0 }\n");
	printf("0 {\n");
	localDb->debugDump(1);
	printf("0 }\n");
    */

	cleanupGame();

	if (globalDb!=NULL)
	{
		printf("deleting globalDb\n");
		delete(globalDb);
		globalDb = NULL;
	}

	textFocusPage=NULL;

	if (localDb!=NULL)
	{
		printf("deleting localDb\n");
		localDb->unlinkAllChildObjects();
		delete(localDb);
		localDb = NULL;
	}

	drawImageQuit();
	drawTextQuit();
	imageLoaderQuit();

	// Perhaps we shall not delete these since they are deleted when localDb is deleted.
	//delete(textInputBuffer);
	//delete(textMessagesPage);
	//delete(textStatisticsPage);
	//delete(hudScreenBox); hudScreenBox=NULL;
	//delete(colorPyramid); colorPyramid=NULL;


	YukigassenTexturedRoomCleanup();
	//TexCubeCleanup();

	glDeleteProgram(quad2dProgramId);

	cube3dCleanup();

	glDeleteProgram(pyramid3dProgramId);

	glDeleteProgram(box2DProgramId);


	SDL_CloseAudio();

	SDL_DestroyWindow(mainWindow);

	printf("quit %d\n", SDL_GetTicks());


	SDL_Quit();

	assert(debugCounter==0);

	printf("game over\n");

	return 0;
}


