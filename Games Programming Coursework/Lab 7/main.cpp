/*
=================
main.cpp
Main entry point for the Game
=================
*/

#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cBalloon.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"

using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

//Detect if game should begin.
static bool gameShouldPlay = false;

//Detect if game should end.
static bool gameShouldEnd = false;

//Detect if how To should be Dislayed.
static bool showHowTo = false;

D3DXVECTOR2 balloonTrans = D3DXVECTOR2(300,300);

vector<cBalloon*> aBalloon;
vector<cBalloon*>::iterator iter;
vector<cBalloon*>::iterator index;

RECT clientBounds;

TCHAR szTempOutput[30];

bool gHit = false;
int gShapesClicked = 0;
char gShapesClickedStr[50];

D3DXVECTOR3 expPos;
list<cExplosion*> gExplode;

cXAudio gExplodeSound;
cXAudio gMainSoundtrack; //This holds the main soundtrack.

cD3DXTexture* balloonTextures[4];
char* balloonTxtres[] = {"Images\\RedCircle.png","Images\\RedCircle.png","Images\\RedCircle.png","Images\\explosion.png"};

bool insideTheRect(RECT thisSprite, POINT aPoint) 						// Return true if a point is within the rectangle
{
	if ((aPoint.x >= thisSprite.left) &&
		(aPoint.x <= thisSprite.right) &&
		(aPoint.y >= thisSprite.top ) &&
		(aPoint.y <= thisSprite.bottom))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{	
		case WM_LBUTTONDOWN:
			{
				if(gameShouldPlay==true){ // If the Main Game has begun.
				POINT mouseXY;
				mouseXY.x = LOWORD(lParam);
				mouseXY.y = HIWORD(lParam);
				
				expPos = D3DXVECTOR3((float)mouseXY.x,(float)mouseXY.y, 0.0f);

				iter = aBalloon.begin();
				while (iter != aBalloon.end() && !gHit)
				{
					if ( (*iter)->insideRect((*iter)->getBoundingRect(),mouseXY))
					{
						OutputDebugString("Hit!\n");
						gHit = true;
						expPos = (*iter)->getSpritePos();
						gExplode.push_back(new cExplosion(expPos,balloonTextures[3]));
						
						gExplodeSound.playSound(L"Sounds\\pop.wav",false);
						iter = aBalloon.erase(iter);
						gShapesClicked++;
						sprintf_s( gShapesClickedStr, 50, "Shapes Clicked : %d", gShapesClicked);
					}
					else
					{
						++iter;
					}
					return 0;
				}

				gHit = false;
				return 0;
				}
				//Below are where the conditions for our main menu buttons are held.
				//==================================================================

				if((gameShouldPlay == false)&&(showHowTo == false)&&(gameShouldEnd == false)) { 
					gMainSoundtrack.playSound(L"Sounds\\Popcorn.mp3",true);
					POINT mouseXY;
					mouseXY.x = LOWORD(lParam);
					mouseXY.y = HIWORD(lParam);
					
						RECT playPos; 
						SetRect(&playPos, 288,134,494,255); //This sets the clickable position for the Play Button.
					
					if(	insideTheRect(playPos,mouseXY)) // This detects if the area is hit.
					{		
					
					gameShouldPlay = true; // Starts the game.
					return 0;

					}

					RECT howToPos;
						SetRect(&howToPos, 300,273,478,347); //This sets the clickable position for the How To Button.
					
					if(	insideTheRect(howToPos,mouseXY)) // This detects if the area is hit.
					{		
					
					showHowTo = true; // This sets the trigger to display the How to Play Screen.
					return 0;
					}

						RECT quitPos;
						SetRect(&quitPos, 310,360,460,435); //This sets the clickable position for the Quit Button.
					
					if(	insideTheRect(quitPos,mouseXY)) // This detects if the area is hit.
					{		
					
					PostQuitMessage(0); // This quits the game.
					return 0;
					}
					if((gameShouldPlay == false)&&(gameShouldEnd == true)&&(showHowTo == false)){
					
						RECT quitPos2;
						SetRect(&quitPos2, 298,473,503,558); //This sets the clickable position for the Quit Button on the End Game Screen.
					
					if(	insideTheRect(quitPos2,mouseXY)) // This detects if the area is hit.
					{		
					
					PostQuitMessage(0); // This quits the game.
					return 0;
					}
						RECT replayPos;
						SetRect(&replayPos, 285,333,515,458); //This sets the clickable position for the Quit Button on the End Game Screen.
					
					if(	insideTheRect(replayPos,mouseXY)) // This detects if the area is hit.
					{		
					
					gameShouldEnd = false; // This replays the game.
					gameShouldPlay = true;
					return 0;
					
					}
				}
				return 0;
			}
			

	
		case WM_CLOSE:
			{
			// Exit the Game
				PostQuitMessage(0);
				 return 0;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
	}
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}
}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/

bool initWindow( HINSTANCE hInstance )
{	
	//if(gameShouldPlay == false){

	WNDCLASSEX wcexm;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcexm.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcexm.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcexm.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcexm.cbClsExtra = 0; // extra bytes to allocate for this class
	wcexm.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcexm.hInstance = hInstance; // handle to the application instance
	wcexm.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcexm.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
	wcexm.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcexm.lpszMenuName = NULL; // the resource name for the menu
	wcexm.lpszClassName = "Main Menu"; // the class name being created
	wcexm.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon


	RegisterClassEx(&wcexm);
	//Creating the Main Menu
		wndHandle = CreateWindow("Main Menu",			// the window class to use
							 "Brain Bash",			// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT, // the starting x coordinate
							CW_USEDEFAULT, // the starting y coordinate
							800, // the pixel width of the window
							600, // the pixel height of the window
							NULL, // the parent window; NULL for desktop
							NULL, // the menu for the application; NULL for none
							hInstance, // the handle to the application instance
							NULL); // no values passed to the window
	
		// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
	}

	

//	if(gameShouldPlay == true) Made the mistake here of thinking you need seperate windows to run the one application.. Oops.
//	{	
//		WNDCLASSEX wcex;
//	// Fill in the WNDCLASSEX structure. This describes how the window
//	// will look to the system
//	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
//	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
//	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
//	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
//	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
//	wcex.hInstance = hInstance; // handle to the application instance
//	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
//	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
//	wcex.lpszMenuName = NULL; // the resource name for the menu
//	wcex.lpszClassName = "Shapes"; // the class name being created
//	wcex.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon
//
//	// Create the window
//	wndHandle = CreateWindow("Shapes",			// the window class to use
//							 "Games Programming Coursework",			// the title bar text
//							WS_OVERLAPPEDWINDOW,	// the window style
//							CW_USEDEFAULT, // the starting x coordinate
//							CW_USEDEFAULT, // the starting y coordinate
//							800, // the pixel width of the window
//							600, // the pixel height of the window
//							NULL, // the parent window; NULL for desktop
//							NULL, // the menu for the application; NULL for none
//							hInstance, // the handle to the application instance
//							NULL); // no values passed to the window
//	
//	// Make sure that the window handle that is created is valid
//	if (!wndHandle)
//		return false;
//	// Display the window on the screen
//	ShowWindow(wndHandle, SW_SHOW);
//	UpdateWindow(wndHandle);
//	return true;
//}
//}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time
	float timeInGame = 0.0f;				// Time Passed in the Main Game is held here.
	int gameScore = 0;					//gameScore held here. Should not be a float for final hand in. 

	GetClientRect(wndHandle,&clientBounds);

	float fpsRate = 1.0f/25.0f;

	D3DXVECTOR3 aballoonPos;
	//if(gameShouldPlay == true){
	sprintf_s( gShapesClickedStr, 50, "Shapes Clicked : %d", gShapesClicked);
	//}

	//====================================================================
	// Load four textures for the shapes; yellow, green, red & explosion
	//====================================================================
	
		for (int txture = 0; txture < 4; txture++)
	{
		balloonTextures[txture] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), balloonTxtres[txture]);
	
		}

	// Here we can set where the shapes spawn.
	D3DXVECTOR3 balloonPos;

	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );

	/* Here I can set the exact number of shapes I want to set per level. */
	int numShapes = (5);

	for(int loop = 0; loop < numShapes; loop++)
	{
		aBalloon.push_back(new cBalloon());
		balloonPos = D3DXVECTOR3((float)clientBounds.right/(loop+2),(float)clientBounds.bottom/(loop+2),0);		
		aBalloon[loop]->setSpritePos(balloonPos);
		aBalloon[loop]->setTranslation(D3DXVECTOR2(200.0f,0.0f));
		aBalloon[loop]->setTexture(balloonTextures[(loop % 3)]);
	}
	

	
	//This is for the Main Menu
	LPDIRECT3DSURFACE9 aSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer

	//This is for the Main Game
	LPDIRECT3DSURFACE9 bSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer2 = NULL;  // This will hold the back buffer

	//This is for the End Scene
	LPDIRECT3DSURFACE9 cSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer3 = NULL;  // This will hold the back buffer

	//This for the How To Scene.
	LPDIRECT3DSURFACE9 dSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer4 = NULL;  // This will hold the back buffer

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Create the background surface(s)
	aSurface = d3dMgr->getD3DSurfaceFromFile("Images\\Main Menu.png");
	bSurface = d3dMgr->getD3DSurfaceFromFile("Images\\Clouds Background.png");
	cSurface = d3dMgr->getD3DSurfaceFromFile("Images\\endGame.png");
	dSurface = d3dMgr->getD3DSurfaceFromFile("Images\\howToPlay.png");

	// load custom font
	cD3DXFont* balloonFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "JI Balloon Caps");

	RECT textPos;
	SetRect(&textPos, 50, 10, 550, 100);

	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);

	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			//Code for Menu Screen
			if((gameShouldPlay == false)&&(gameShouldEnd == false))
			{
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(aSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dMgr->endRender();

			}

			//Code for How To Screen
			if((gameShouldPlay == false)&&(showHowTo == true))
			{
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(dSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dMgr->endRender();

			}

			
			//=======================================================
			//Main Game Found Here
			//=======================================================

			if(gameShouldPlay == true)
			{
			// Game code goes here
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			float dt = (currentTime - previousTime)*sPC;

			// Accumulate how much time has passed.
			timeElapsed += dt;
			
			// This is to be used as a fail condition and/or a score multiplier. 
			timeInGame += dt;
			

			//if(timeElapsed >= 10) { Will test timer here.
				//OutputDebugString("Game should End");
				
			//}

				if((gShapesClicked == numShapes)||(timeInGame>=10)){ //Game End Conditions
					gameShouldEnd = true;
					gameShouldPlay = false;
					numShapes++; // Here the number of shapes increments for the next play of the main level. 
					gameScore = gShapesClicked + gameScore; //Here we take the winning clicks and add them to our current game score.
					d3dMgr->beginRender();
					theBackbuffer = d3dMgr->getTheBackBuffer();
					d3dMgr->updateTheSurface(cSurface, theBackbuffer);
					d3dMgr->releaseTheBackbuffer(theBackbuffer);
					d3dMgr->endRender();
					
				}

				//Code for Game Over Screen
			if(gameShouldEnd == true)
			{
				
				//Empty For Now.
			}
			/*
			==============================================================
			| Update the postion of the balloons and check for collisions
			==============================================================
			*/
			if(timeElapsed > fpsRate)
			{
				for(iter = aBalloon.begin(); iter != aBalloon.end(); ++iter)
				{
					(*iter)->update(timeElapsed);			// update balloon
					aballoonPos = (*iter)->getSpritePos();  // get the position of the current balloon
					/*
					==============================================================
					| Check for boundry collision and change balloon direction
					==============================================================
					*/
					if (aballoonPos.x>(clientBounds.right - 60) || aballoonPos.x < 2 )
					{
						(*iter)->setTranslation((*iter)->getTranslation()*(-1));
					}
					/*
					==============================================================
					| Check each balloon against each other for collisions
					==============================================================
					*/				
					for(index = aBalloon.begin(); index != aBalloon.end(); ++index)
					{
						if ((*iter)->collidedWith((*iter)->getBoundingRect(),(*index)->getBoundingRect()))
						{
							// if a collision occurs change the direction of each balloon that has collided
							OutputDebugString("Collision!!");
							(*iter)->setTranslation((*iter)->getTranslation()*(-1));
							(*index)->setTranslation((*index)->getTranslation()*(-1));
						}
					}
				}
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(bSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				
				d3dxSRMgr->beginDraw();
				vector<cBalloon*>::iterator iterB = aBalloon.begin();
				for(iterB = aBalloon.begin(); iterB != aBalloon.end(); ++iterB)
				{
					d3dxSRMgr->setTheTransform((*iterB)->getSpriteTransformMatrix());  
					d3dxSRMgr->drawSprite((*iterB)->getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
				
				}
				list<cExplosion*>::iterator iter = gExplode.begin();
				while(iter != gExplode.end())
				{
					if((*iter)->isActive() == false)
					{
						iter = gExplode.erase(iter);
					}
					else
					{
						(*iter)->update(timeElapsed);
						d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*iter)->getTexture(),&((*iter)->getSourceRect()),NULL,NULL,0xFFFFFFFF);
						++iter;
					}
				}
				d3dxSRMgr->endDraw();
				balloonFont->printText(gShapesClickedStr,textPos);
				d3dMgr->endRender();
				timeElapsed = 0.0f;
				//timeInGame = 0.0f;
			}

			previousTime = currentTime;
		}
		}
	}
	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int) msg.wParam;
}

