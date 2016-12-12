//////////////////////////////////////////////////////////////////////////
//				LevelupEffect.h
//
//	Does the level up effect, from the bouncing "LEVEL UP!" text to
//	the stats display and weird transition effects.
//////////////////////////////////////////////////////////////////////////


#ifndef __LEVELUP_EFFECT_H__
#define __LEVELUP_EFFECT_H__

#include "SexyAppFramework/Common.h"
#include "SexyAppFramework/Rect.h"

namespace Sexy
{

	class Graphics;


//////////////////////////////////////////////////////////////////////////
//	Represents a letter that bounces, used for the "LEVEL UP!" text.
//////////////////////////////////////////////////////////////////////////
struct BouncyChar
{
	SexyString		mChar;				// The character bouncing
	bool			mDone;				// Completed bouncing yet?
	float			mX, mY;				
	float			mBounceSpeed;		// How fast up or down it's bouncing, affected by "gravity" 
	float			mOldBounceSpeed;	// The base value that mBounceSpeed started at. Gets reduced over time.

	BouncyChar(SexyString t, float x, float y, float s)
	{mChar = t; mX = x; mY = y; mBounceSpeed = mOldBounceSpeed = s; mDone = false;}
};

//////////////////////////////////////////////////////////////////////////
//	Contains stat info to display after completing a level.
//////////////////////////////////////////////////////////////////////////
struct LevelupStats
{
	int		mPopulationEaten;
	int		mPercentComplete;
	int		mLevelCompleted;

	// Always 3 strings per planet:
	// Planet name, exports, population (comma delimited)
	std::vector<SexyString> mPlanetsEaten;
	
};

class LevelupEffect
{

	private:

		// States that the level up effect goes through
		enum
		{
			LEVELUP_TEXT,			// Displaying the bouncy LEVEL UP! text
			CURTAIN_IN,				// Moving the black "curtain" inward to cover up the level
			CURTAIN_OUT,			// Moving the red "curtain" out after above to reveal the stats
			SHOWING_STATS,			// Displaying level stats
			COVERING_STATS,			// Doing the weird transitionary cover-up screen after clicking to continue
			FADE_OUT_STATS			// Fading out the above screen
		};
		
		std::vector<BouncyChar>	mText;			// The letters that spell LEVEL UP!, as BouncyChar structures
		LevelupStats	mStats;					// Stat info to display
		bool			mActive;				// If true, means we're doing the level up sequence
		bool			mDone;					// If done, means all finished and the next level can actually begin
		bool			mStartNextLevel;		// When true, indicates that the board should set up the next level so that it'll be there when we fade out

		int				mHue;					// For hue/saturation/luminence crazy weird flashing effect
		int				mCurtainX;				// X location of the black/red curtain edge (for the left curtain, the right is the same size)
		int				mState;					// One of the above nums

		// When doing the COVERING_STATS phase, this is how wide 1/2 of the filled in part of the screen is. The other half
		// is the same size.
		int				mCoverWidth;			
		int				mStripHeight;			// For the above, we make strips quickly appear, alternating up/down filling 

		// Indicates whether the strips are increasing Y (so strip comes from top) or decreasing
		// (so strip comes from bottom)
		int				mStripSizeChange;
		int				mFadeOutAlpha;			// Alpha amount for the final fade out effect

		//////////////////////////////////////////////////////////////////////////
		//	Function: Init
		//
		//	Purpose: Sets up and initializes/resets all variables.
		//////////////////////////////////////////////////////////////////////////		
		void	Init();

	public:

		LevelupEffect();

		//////////////////////////////////////////////////////////////////////////
		//	Function: Update
		//	Parameters:
		//		theFrac	-	Value from Board::UpdateF, used for smooth motion
		//
		//////////////////////////////////////////////////////////////////////////		
		void	Update(float theFrac);

		//////////////////////////////////////////////////////////////////////////
		//	Draw
		//////////////////////////////////////////////////////////////////////////		
		void	Draw(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: Activate
		//	Parameters:
		//		ls	-	A stat structure containing the info for the past level
		//
		//	Purpose: Begins the level up sequence
		//////////////////////////////////////////////////////////////////////////		
		void	Activate(LevelupStats ls);

		//////////////////////////////////////////////////////////////////////////
		//	Function: DoneViewingStats
		//
		//	Purpose: Called by Board when the user clicks the mouse button, 
		//	indicating that they want the stats screen to go away and have the
		//	next level begin.
		//////////////////////////////////////////////////////////////////////////		
		void	DoneViewingStats();

		//////////////////////////////////////////////////////////////////////////
		//	Function: StartNextLevel
		//	Returns: true or false, indicating if the next level can be started.
		//
		//	Purpose: Called by the Board's Update method, this returns true
		//	when the board should initialize the next level, so that when the
		//	level up effect fades out, the next level will appear underneath it.
		//	When the function returns true, it automatically sets internal variables
		//	so that the next time the function is called, it will return false, 
		//	preventing the Board from accidentally initializing the same level 
		//	multiple times. After the next call to Activate, it is allowed to
		//	return true again. 
		//
		//	Once the screen is totally filled after closing the stats display,
		//	the Board is allowed to init the next level.
		//////////////////////////////////////////////////////////////////////////		
		bool	StartNextLevel();

		//////////////////////////////////////////////////////////////////////////
		//	Function: IsDone
		//	Returns: true or false indicating if the entire sequence is done
		//
		//	Purpose: Used to let the board know when playing of the next level
		//	can begin.
		//////////////////////////////////////////////////////////////////////////		
		bool	IsDone()		{return mDone;}

		//////////////////////////////////////////////////////////////////////////
		//	Function: IsActive
		//	Returns: true or false indicating if the sequence is running or not
		//////////////////////////////////////////////////////////////////////////		
		bool	IsActive()		{return mActive;}

		//////////////////////////////////////////////////////////////////////////
		//	Function: ShowingStats
		//	Returns: true or false indicating if the stats display is visible
		//////////////////////////////////////////////////////////////////////////
		bool	ShowingStats()	{return mState == SHOWING_STATS;}		

		//////////////////////////////////////////////////////////////////////////
		//	Function: HideBoard
		//	Returns: true or false indicating whether or not the board should
		//	hide all of its display stuff, except for the starfield which always
		//	displays.
		//////////////////////////////////////////////////////////////////////////		
		bool	HideBoard()		{return (mState == SHOWING_STATS) || (mState == CURTAIN_OUT) || (mState == COVERING_STATS);}

		//////////////////////////////////////////////////////////////////////////
		//	Function: HidePlanets
		//	Returns: true or false indicating if just the planets should be
		//	hidden.
		//
		//	Purpose: Used to hide the planets but still allow the rest of the
		//	game board to display. Used during the transtion to the stats
		//	display screen.
		//////////////////////////////////////////////////////////////////////////		
		bool	HidePlanets()	{return IsActive() && (HideBoard() || (mState == LEVELUP_TEXT) || (mState == CURTAIN_IN));}
};


}

#endif //__LEVELUP_EFFECT_H__