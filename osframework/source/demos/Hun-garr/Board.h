#ifndef __BOARD_H__
#define __BOARD_H__

//////////////////////////////////////////////////////////////////////////
//						Board.h
//
//	This is the third class to look at in this particular demo
//	(after main.cpp and GameApp.h/.cpp). The Board class is where most of
//	your actual game programming will go. It is here that we will do
//	all our game drawing, updating, and input processing. Of course, in
//	a larger application, you would probably do drawing and updating in
//	multiple files, but you would still most likely use something similar
//	to a Board class as the master game logic class. 
//
//	The reason that the Board class is a widget is because when a widget
//	is added to the GameApp's WidgetManager, it will automatically have its
//	Update and Draw methods called, and it will automatically receive input
//	at the appropriate times. Furthermore, by making it a widget and adding
//	it to the WidgetManager, the game logic loop, Update(), will be guaranteed
//	to run at a standard 100FPS on all machines. This is extremely important
//	as you always want your logic code to run at the same speed, but want
//	the drawing code to run as fast as possible. That way on faster machines
//	your program doesn't run its logic faster than on a slower machine.
//
//	You can think of the Board as a canvas upon which we do all our
//	drawing, and a central hub where if we need to, we instruct other
//	classes where and when to draw to.
//////////////////////////////////////////////////////////////////////////

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"
#include "SexyAppFramework/Rect.h"

namespace Sexy
{


class Graphics;
class GameApp;
class ButtonWidget;
class WidgetManager;
class LevelupEffect;
class GameOverEffect;
class SoundInstance;

// Each grid piece is this many pixels wide/tall
const int GRID_PIX_SIZE		=	16;

// Starting/ending Y coordinate of the grid (above it is the status display)
const int GRID_START_Y		=	128; 
const int GRID_END_Y		=	480 - GRID_PIX_SIZE;

// Starting/ending X coordinate of the grid
const int GRID_START_X		=	GRID_PIX_SIZE;
const int GRID_END_X		=	640 - GRID_PIX_SIZE;

// Total number of grid tiles wide/tall
const int GRID_WIDTH		=	(GRID_END_X - GRID_START_X) / GRID_PIX_SIZE;
const int GRID_HEIGHT		=	(GRID_END_Y - GRID_START_Y) / GRID_PIX_SIZE;

// >= this percent begins the next level
const int LEVELUP_PERCENT	=	80;

// >= this percent gives a bonus
const int COMPLETION_BONUS_PCT = 85;

// Bonus for >= COMPLETION_BONUS_PCT
const int COMPLETION_BONUS = 20000;

//////////////////////////////////////////////////////////////////////////
//	Represents one segment of the two expanding lines that emit
//	from the cursor when the left mouse button is pressed on a valid
//	grid region.
//////////////////////////////////////////////////////////////////////////
struct ExpandingLine
{
	bool	mIsVertical;		// Orientation of the line

	// When the line reaches its target, that particular segment is done and planets can bounce off it.
	// If a planet hits it before its done, it will break and you will lose a life
	bool	mDone;				
	bool	mBroken;			// Indicates if the line broke and thus the player lost a life.
	float	mX;
	float	mY;
	float	mWidth;
	float	mHeight;
	float	mTargetX;			// XY coordinate the line is trying to reach
	float	mTargetY;

	ExpandingLine() {mIsVertical = mDone = true; mBroken = false; mX = mY = mWidth = mHeight = mTargetX = mTargetY = 0;}
};

//////////////////////////////////////////////////////////////////////////
//	Represents one of the little grid squares on the board.
//	Each square can have 1 of 3 states. See the Board class for the states.
//////////////////////////////////////////////////////////////////////////
struct GridTile
{
	int		mFillState;

	// A rectangle, used when the grid space is being filled in. This indicates
	// the actual region that is currently filled. The unfilled region is drawn
	// normally. Regions expand over time, and don't instantly fill up.
	FRect	mFillRect;		

	GridTile() {mFillState = 0;}
};

//////////////////////////////////////////////////////////////////////////
//	Represents one of the bouncing balls ("Planets" I suppose). For
//	fun, planets get a random name and export, which the player, "Hun-Garr",
//	eats. If a planet is inside a region as it fills, the planet will be
//	destroyed. Each planet destroyed makes the player's beams move faster,
//	up to a point.
//////////////////////////////////////////////////////////////////////////
struct Planet
{
	unsigned int	mPopulation;				// A fun little stat, indicates how many people live on the planet
	int				mImgCol;					// There are 11 planets, this is the column indicating the planet image to draw
	int				mExplodeFrame;				// When it's dying, this keeps track of the current frame of animation
	int				mExplodeSpeed;				// How fast the death animation runs
	int				mNameIdx, mExportIdx;		// Index into the two tables that contain planet names and export lists
	int				mTimer;						// Counter used for animation timing
	float			mX, mY;
	float			mVX, mVY;					// X/Y speed
	float			mRotationAngle;				// In 3D mode, the planets rotate. This is the present angle
	float			mRotateSpeed;				// How fast he planet rotates, in 3D mode
	bool			mExploding;					// If true, the planet is dying and going through its explosion state

	Planet() {mPopulation = mImgCol = mExportIdx = mNameIdx = mTimer = 0; 
				mX = mY = mVX = mVY = mRotationAngle = mRotateSpeed = 0; 
				mExplodeSpeed = 8; mExplodeFrame = 0; mExploding = false;}
};


//////////////////////////////////////////////////////////////////////////
//	Used for the 3 layer star field effect.
//////////////////////////////////////////////////////////////////////////
struct Star
{
	float	mX, mY;
	float	mSpeed;		// How fast the star is moving rightward
	Color	mColor;		// Color, for some depth perception trickery
};

//////////////////////////////////////////////////////////////////////////
//	The flashy bonus text that floats up when you get points
//////////////////////////////////////////////////////////////////////////
struct BonusText
{
	int				mHue;		// See the .CPP file for info on using hue/saturation/luminance values
	int				mAlpha;		// How faded out the text is
	float			mX, mY;
	SexyString		mText;

	BonusText() {mAlpha = 255; mHue = 0; mX = mY = 0;}
};

//////////////////////////////////////////////////////////////////////////
//	Used for the particle effects that fly off of the moving beams
//////////////////////////////////////////////////////////////////////////
struct Particle
{
	float			mX, mY;
	float			mVX, mVY;	// X/Y speed
	int				mFrame;		// Which frame of animation it's on
	int				mTimer;		// Used to control the animation

	Particle(float x, float y, float vx, float vy)
	{mX = x, mY = y; mVX = vx; mVY = vy; mFrame = 0; mTimer = 0;}
};


#define MAX_STARS 300

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Board :	public Widget, public ButtonListener 
{

	private:

		// The direction that the grid is being filled in
		enum
		{
			FILL_LEFT,
			FILL_RIGHT,
			FILL_UP,
			FILL_DOWN
		};

		// The possible states for each little grid cube
		enum
		{
			GRID_NORMAL,		// Drawn regularly, not filled in
			GRID_FILLING,		// In the process of being filled in, will partially draw filled and normal
			GRID_FILLED			// Totally filled in, piece now acts as a wall and draws differently
		};

		// For the stats at the end of the game that display the planet and export you ate the most,
		// we keep track of them in the maps below. The key is the index from the PLANET_NAME or 
		// PLANET_EXPORTS table, and the value is the number of times it has been consumed. Reset on death
		std::map<int, int>			mPlanetIdxCount;				
		std::map<int, int>			mExportIdxCount;
		std::vector<BonusText>		mBonusText;			
		std::vector<Planet>			mPlanets;					// A list of all the planets bouncing on the level

		// When a planet is eaten, we dump its name, export, and population (comma delimited) in that
		// order into this list, which will be passed to the level up class for a stats summary of
		// the past level.
		std::vector<SexyString>		mPlanetsEaten;				
		std::vector<Particle>		mParticles;					

		GameApp*			mApp;

		// The two beams that the player emits with a left click. Line 1 is ALWAYS either
		// up or left, and line 2 is ALWAYS either down or right.
		ExpandingLine		mMovingLine1, mMovingLine2;			
		GridTile**			mGridState;							// 2D array of all the grid tiles		
		Insets				mFillRegion;						// Indicates the rectangular region being filled.
		Star				mStarField[MAX_STARS];
		LevelupEffect*		mLevelupEffect;
		GameOverEffect*		mGameOverEffect;
		ButtonWidget*		mOptionsBtn;

		// The shorting out, electrical sound of the beams moving. We use a SoundInstance pointer
		// because we want to loop the sound while the beam is moving, and stop it when done.
		// This is easiest done manually.
		SoundInstance*		mShortSound;						

		bool				mHungarrIsVertical;					// Orientation of the player: will the lines come out vertical or horizontal?
		bool				mFilling;							// Is the grid currently being filled?

		int					mFillDirection;						// One of the FILL_ enums
		int					mHungarrX, mHungarrY;				// XY of the IMAGE of the player, not exactly where the beams go though
		int					mLevel;			
		int					mPopulationEaten;					// Number of people eaten this level
		int					mTotalPopulationEaten;				// Total number of people eaten this game
		int					mNumPlanetsEaten;					// Total number of planets eaten this game
		int					mPercentComplete;					// What % filled the grid is

		// Indicates the number of times the game has been asked to pause. When unpausing, we decrement
		// this value. Not really needed in this game, but in general a good idea, as every time a dialog
		// box pops up or the app loses focus or anything like that, you'll want to only fully unpause
		// everything when ALL the elements that requested a pause are complete, and not just one of them.
		int					mPauseLevel;						
		int					mBorderHue;							// HSL value of the border for a flashing effect. See .CPP for more info
		int					mFlashCount;						// How long the border will flash for
		int					mLives;
		int					mScore;
		int					mBeamPulseVal;						// Current intensity for the pulsing effect of the moving beams
		int					mBeamPulseAmt;						// Current amount to inc/dec the intensity by for a pulsing effect

		float				mLine1X, mLine1Y;					// XYs of the two lines that are drawn underneath Hun-garr. These
		float				mLine2X, mLine2Y;					// are always drawn and are not the same as the moving line positions.
		float				mLineSpeed;							// Gets faster with each planet destroyed
		float				mFillSpeed;							// How quickly to fill in the grid

		// Current max rate the planets can move at. Used with the planet's angle of motion to determine separate
		// XY movement values. This increases with each level.
		float				mPlanetSpeed;						

	private:


		//////////////////////////////////////////////////////////////////////////
		//	Function: UpdateHungarrPosition
		//	Parameters:
		//		x, y - Pixel location of the Windows cursor
		//
		//	Purpose: Given a pixel location of the pointer, if it is within the
		//	valid bounds of the grid region, will update the XY to draw the
		//	Hun-garr bitmap at, as well as the little pulsing beams that are attached.
		//////////////////////////////////////////////////////////////////////////		
		void	UpdateHungarrPosition(int x, int y);

		//////////////////////////////////////////////////////////////////////////
		//	Function: UpdatePercentComplete
		//
		//	Purpose: After the lines are done moving, this is called to see how
		//	full the grid is, and potentially take the player to the next level.
		//////////////////////////////////////////////////////////////////////////		
		void	UpdatePercentComplete(void);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawMovingBeams
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Helper function, draws the beams as they move across the grid.
		//////////////////////////////////////////////////////////////////////////		
		void	DrawMovingBeams(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: Beam1DrawHelper
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Convenience function for DrawMovingBeams to avoid copy/pasting
		//////////////////////////////////////////////////////////////////////////		
		void	Beam1DrawHelper(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: Beam2DrawHelper
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Convenience function for DrawMovingBeams to avoid copy/pasting
		//////////////////////////////////////////////////////////////////////////		
		void	Beam2DrawHelper(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawGrid
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Draws all the little rectangles that make up the grid.
		//////////////////////////////////////////////////////////////////////////		
		void	DrawGrid(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawPlanets
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Draws all the planets, convenience function
		//////////////////////////////////////////////////////////////////////////		
		void	DrawPlanets(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawGrid
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Draws the player bitmap and the two pulsing lines that are
		//	attached.
		//////////////////////////////////////////////////////////////////////////		
		void	DrawHungarr(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawHungarrVertBeamsHelper
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Helper function for DrawGrid to avoid copy/pasting
		//////////////////////////////////////////////////////////////////////////		
		void	DrawHungarrVertBeamsHelper(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawHungarrHorizBeamsHelper
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Helper function for DrawGrid to avoid copy/pasting
		//////////////////////////////////////////////////////////////////////////
		void	DrawHungarrHorizBeamsHelper(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DrawUI
		//	Parameters:
		//		g - Graphics object to draw to
		//
		//	Purpose: Draws the interface elements of the game, such as the stats.
		//////////////////////////////////////////////////////////////////////////		
		void	DrawUI(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: FillRectF
		//	Parameters:
		//		g - Graphics object to draw to
		//		fr - FRect to draw
		//
		//	Purpose: Convenience function, since you can't pass an FRect to DrawRect.
		//	Just converts the FRect to a normal Rect. We use FRects though for
		//	more precise motion control.
		//////////////////////////////////////////////////////////////////////////		
		void	FillRectF(Graphics* g, FRect fr);

		//////////////////////////////////////////////////////////////////////////
		//	Function: FillLeft
		//	Parameters:
		//		amt - How many pixels to fill
		//
		//	Purpose: Updates the filling of the grid if it's moving leftward.
		//////////////////////////////////////////////////////////////////////////		
		void	FillLeft(float amt);

		//////////////////////////////////////////////////////////////////////////
		//	Function: FillRight
		//	Parameters:
		//		amt - How many pixels to fill
		//
		//	Purpose: Updates the filling of the grid if it's moving rightward.
		//////////////////////////////////////////////////////////////////////////
		void	FillRight(float amt);

		//////////////////////////////////////////////////////////////////////////
		//	Function: FillUp
		//	Parameters:
		//		amt - How many pixels to fill
		//
		//	Purpose: Updates the filling of the grid if it's moving upward.
		//////////////////////////////////////////////////////////////////////////
		void	FillUp(float amt);

		//////////////////////////////////////////////////////////////////////////
		//	Function: FillDown
		//	Parameters:
		//		amt - How many pixels to fill
		//
		//	Purpose: Updates the filling of the grid if it's moving downward.
		//////////////////////////////////////////////////////////////////////////
		void	FillDown(float amt);

		//////////////////////////////////////////////////////////////////////////
		//	Function: CalculateFillRegions
		//
		//	Purpose: Determines which areas need to be filled, once the grid lines
		//	are done moving.
		//////////////////////////////////////////////////////////////////////////		
		void	CalculateFillRegions(void);

		//////////////////////////////////////////////////////////////////////////
		//	Function: GetVerticalFillValues
		//	Parameters:
		//		startCol - Column in which the beams were released
		//		topRow - The row that the top line reached
		//		botRow - The row the the bottom line reached
		//		dir	- 1 to compute the target fill line in the rightward direction,
		//				-1 to compute it in the leftward direction.
		//		edge - Set to the column that the filling should proceed towards
		//
		//	Purpose: Given a direction to look in, finds a column to fill to.
		//	The column selected is the first column found where EVERY grid piece
		//	in it, from topRow to botRow, is in the normal grid state. If one is
		//	not found, the farthest grid edge is used. Partially filled lines are
		//	bypassed. This is used to get the CLOSEST edge to fill to for both of
		//	the possible directions: the edge that is closest to the player is 
		//	selected and filling will proceed in that direction.
		//////////////////////////////////////////////////////////////////////////
		void	GetVerticalFillValues(int startCol, int topRow, int botRow, int dir, int* edge);

		//////////////////////////////////////////////////////////////////////////
		//	Function: GetHorizontalFillValues
		//	Parameters:
		//		startRow - Row in which the beams were released
		//		leftCol - The column that the left line reached
		//		rightCol - The column the the right line reached
		//		dir	- 1 to compute the target fill line in the downward direction,
		//				-1 to compute it in the upward direction.
		//		edge - Set to the column that the filling should proceed towards
		//
		//	Purpose: Given a direction to look in, finds a row to fill to.
		//	The row selected is the first row found where EVERY grid piece
		//	in it, from leftCol to rightCol, is in the normal grid state. If one is
		//	not found, the farthest grid edge is used. Partially filled lines are
		//	bypassed. This is used to get the CLOSEST edge to fill to for both of
		//	the possible directions: the edge that is closest to the player is 
		//	selected and filling will proceed in that direction.
		//////////////////////////////////////////////////////////////////////////
		void	GetHorizontalFillValues(int startRow, int leftCol, int rightCol, int dir, int* edge);
		
		//////////////////////////////////////////////////////////////////////////
		//	Function: MoveLines
		//	Parameters:
		//		theFrac - Value from UpdateF, used for smooth motion
		//
		//	Purpose: Handles the moving of the lines once released.
		//////////////////////////////////////////////////////////////////////////		
		void	MoveLines(float theFrac);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MovePlanet
		//	Parameters:
		//		theFrac - Value from UpdateF, used for smooth motion
		//		
		//	Returns: true or false indicating if the planet blew up or not.
		//
		//	Purpose: Handles the moving/bouncing of the planets
		//////////////////////////////////////////////////////////////////////////		
		bool	MovePlanet(Planet* p, float theFrac);

		//////////////////////////////////////////////////////////////////////////
		//	Function: CheckPlanetBeamCollision
		//	Parameters:
		//		p - A planet to check for collision
		//
		//	Purpose: Called by MoveLines, checks to see if the given planet
		//	collided with either of the beams. If a beam is still moving,
		//	it will break and the player will lose a life (only 1 life can
		//	be lost though, even if both are broken you don't lose 2 lives).
		//	If the beam is not moving, the planet will bounce off of it.
		//////////////////////////////////////////////////////////////////////////	
		void	CheckPlanetBeamCollision(Planet* p);

		//////////////////////////////////////////////////////////////////////////
		//	Function: InitLevel
		//	Parameters:
		//		level - The level to set up
		//
		//	Purpose: Initializes the next level
		//////////////////////////////////////////////////////////////////////////		
		void	InitLevel(int level);
		
		//////////////////////////////////////////////////////////////////////////
		//	Function: GivePlanetBonus
		//	Parameters:
		//		p - The planet that was destroyed
		//
		//	Purpose: Awards bonuses for destroying a planet and emits bonus text.
		//////////////////////////////////////////////////////////////////////////		
		void	GivePlanetBonus(Planet* p);

		//////////////////////////////////////////////////////////////////////////
		//	Function: AddBonusText
		//	Parameters:
		//		t - The text to add
		//
		//	Purpose: Adds floating bonus text to the center of the screen
		//////////////////////////////////////////////////////////////////////////		
		void	AddBonusText(SexyString t);

		//////////////////////////////////////////////////////////////////////////
		//	Function: AddBonusText
		//	Parameters:
		//		t - The text to add
		//		x, y - Coordinates to add the text at
		//
		//	Purpose: Allows exact placement of the bonus text
		//////////////////////////////////////////////////////////////////////////		
		void	AddBonusText(SexyString t, float x, float y);

		//////////////////////////////////////////////////////////////////////////
		//	Function: LostLife
		//
		//	Purpose: Convenience function, used to decrement the player's
		//	life and if it reaches 0, to start the end game sequence.
		//////////////////////////////////////////////////////////////////////////		
		void	LostLife(void);

		//////////////////////////////////////////////////////////////////////////
		//	Function: EmitSparks
		//
		//	Purpose: Every frame, sends out more sparks from the edges of
		//	the two beams if they are moving.
		//////////////////////////////////////////////////////////////////////////		
		void	EmitSparks(void);

	public:

		//////////////////////////////////////////////////////////////////////////
		//	Function: Board
		//	Parameters:
		//		theApp	- Pointer to the main application class
		//	
		//	Returns: none
		//////////////////////////////////////////////////////////////////////////
		Board(GameApp* theApp);

		virtual ~Board();

		//////////////////////////////////////////////////////////////////////////
		//	Function: KeyChar
		//	Parameters:
		//		theChar - The ASCII character pressed
		//
		//	Purpose: Called by the WidgetManager if a key is pressed and the Board
		//	has focus. Used to see if space was pressed, to toggle pause state.
		//////////////////////////////////////////////////////////////////////////		
		virtual void KeyChar(char theChar);

		//////////////////////////////////////////////////////////////////////////
		//	Function: Draw
		//	Parameters:
		//		g	- Graphics object used to draw all images and fonts to the screen.
		//	
		//	Returns: none
		//
		//	Purpose: Called automatically by GameApp's WidgetManager, this function
		//	is the main method that is responsible for all graphical and textual
		//	displaying.
		//////////////////////////////////////////////////////////////////////////
		virtual void Draw(Graphics* g);		

		//////////////////////////////////////////////////////////////////////////
		//	Function: Update
		//	Parameters: none
		//	Returns: none
		//
		//	Purpose: Called automatically by GameApp's WidgetManager, this method
		//	is GUARANTEED to be called 100 times per second (100FPS) and is where
		//	all main game logic is performed. Of course, if you had a larger more
		//	complex game, you'd most likely divide your logic between several
		//	other files, but this is commonly the central place where all game
		//	logic begins and is executed.
		//////////////////////////////////////////////////////////////////////////
		virtual void Update();

		//////////////////////////////////////////////////////////////////////////
		//	Function: UpdateF
		//	Parameters: 
		//		theFrac	- The number of updates this time slice represents.
		//
		//	Returns: none
		//
		//	Purpose: Like Update, but allows for smooth motion. See previous
		//	demos or the doc for a lengthy explanation.
		//////////////////////////////////////////////////////////////////////////
		virtual void UpdateF(float theFrac);

		//////////////////////////////////////////////////////////////////////////
		//	Function: ButtonDepress
		//	Parameters:
		//		theId	- Integer ID of the button that was clicked
		//
		//	Returns: none
		//
		//	Purpose: This method is called by the WidgetManager when a button widget
		//	is first pressed and THEN released. You can use ButtonPress if you want
		//	to know when the button is first pressed (before it is released).
		//	theId is the integer ID that was assigned to the button when it was
		//	first created. 
		//////////////////////////////////////////////////////////////////////////		
		virtual void	ButtonDepress(int theId);

		//////////////////////////////////////////////////////////////////////////
		//	Function: AddedToManager
		//	Parameters:
		//		theWidgetManager	- Pointer to the main widget manager from
		//								GameApp.
		//
		//	Returns: none
		//
		//	Purpose: This function is automatically called by the widget manager
		//	which also passes a pointer to itself, when the Board class is
		//	added to its list of widgets. Every widget gets this function
		//	called when it is first added. It useful to use this function to
		//	set up any other widgets that the class might contain, such as buttons.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	AddedToManager(WidgetManager* theWidgetManager);

		//////////////////////////////////////////////////////////////////////////
		//	Function: RemovedFromManager
		//	Parameters:
		//		theWidgetManager	- Pointer to the main widget manager from
		//								GameApp.
		//
		//	Returns: none
		//
		//	Purpose: This function is automatically called by the widget manager
		//	which also passes a pointer to itself, when the Board class is
		//	removed from its list of widgets. Every widget gets this function
		//	called when it is finally removed. It useful to use this function to
		//	also remove any widgets that were added and created in AddedToManager.
		//////////////////////////////////////////////////////////////////////////
		virtual void	RemovedFromManager(WidgetManager* theWidgetManager);
			
		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseMove
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime the
		//	mouse moves. The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	MouseMove(int x, int y);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseDown
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//		theClickCount - An integer indicating which mouse button
		//						was pressed. One of the following:
		//			1:  Left button
		//			2:  Double-left-click
		//			3:  Middle button
		//			-1: Right button
		//			-2: Double-right-click
		//
		//		IMPORTANT: Because you can't have a double click with a
		//		single click, you will receive BOTH a left (or right) button as well
		//		as a double left (or right) button message whenever you double-click.
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime 
		//	a mouse button is in the down state.
		//	The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored.
		//////////////////////////////////////////////////////////////////////////
		virtual void	MouseDown(int x, int y, int theClickCount);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseDrag
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime the
		//	mouse moves AND a button is held down. This is the same thing as
		//	MouseMove except that it represents a drag operation. When dragging,
		//	MouseDrag will be called INSTEAD OF MouseMove.
		//	The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored.
		//////////////////////////////////////////////////////////////////////////
		virtual void	MouseDrag(int x, int y);

		//////////////////////////////////////////////////////////////////////////
		//	Function: Pause
		//	Parameters:
		//		p - Boolean indicating pause or unpause
		//
		//	Purpose: Pauses/unpauses the game
		//////////////////////////////////////////////////////////////////////////		
		void			Pause(bool p);

		//////////////////////////////////////////////////////////////////////////
		//	Function: OptionsDialogDone
		//
		//	Purpose: Called by the options dialog box when it closes, so the board
		//	can perform some misc tasks.
		//////////////////////////////////////////////////////////////////////////
		void			OptionsDialogDone();
};


}

#endif // __BOARD_H__