//////////////////////////////////////////////////////////////////////////
//					GameOverEffect.h
//
//	Handles the entire sequence when the user dies.
//////////////////////////////////////////////////////////////////////////
#ifndef __GAME_OVER_EFFECT_H__
#define __GAME_OVER_EFFECT_H__

#include "SexyAppFramework/Common.h"

namespace Sexy
{

	class Graphics;

	//////////////////////////////////////////////////////////////////////////
	//	Represents an exploding planet at a given XY, drawing the given
	//	frame number.
	//////////////////////////////////////////////////////////////////////////	
	struct Explosion
	{
		int		mX, mY;
		int		mFrame;
	};

	//////////////////////////////////////////////////////////////////////////
	//	Used to spell out "GAME OVER". Each letter, beginning with G,
	//	slowly goes from black to full red. Once all letters are spelled,
	//	the process reverses, making it fade back to black.
	//////////////////////////////////////////////////////////////////////////	
	struct Letter
	{
		int		mRed;
		SexyString mChar;

		Letter(SexyString c) {mChar = c; mRed = 0;}
	};

	//////////////////////////////////////////////////////////////////////////
	//	Stats that summarize the user's entire game.
	//////////////////////////////////////////////////////////////////////////	
	struct EndGameStats
	{
		SexyString		mFavoritePlanet;		// We keep track of the planet
		SexyString		mFavoriteExport;		// and export eaten most.
		int				mScore;
		int				mLevel;
		int				mNumPlanetsEaten;
		int				mPopulationConsumed;
	};

	//////////////////////////////////////////////////////////////////////////
	//	The transition to the start of the new game is done by these 1 pixel
	//	wide weird lines that drip from the top of the screen. Each line, as it
	//	drips, draws a black line but the bottommost pixel of the line blinks
	//	using our over-used HSL color effect (see LevelUpEffect for more HSL details).
	//	Once it hits the bottom, it drips upward, this time drawing the whole line
	//	with the HSL color, until the entire screen is flashing and pulsing crazily.
	//	The lines are delayed by a random time interval to make it look more ... drippy.
	//////////////////////////////////////////////////////////////////////////	
	struct DrippyLine
	{	
		int				mSpeed;		// Pixles per update the lines moves, < 0 for upward.
		int				mX;
		int				mDelay;		// How long to wait before dripping down
		int				mHeight;	// How far it has dripped so far
		bool			mDripUp;	// Dripping upward or downward?

		DrippyLine(float s, int x, int d) 
		{mSpeed = s; mX = x; mDelay = d; mHeight = 0; mDripUp = false;}
	};

class GameOverEffect
{

	private:

		// The states used for this effect
		enum
		{
			RED_FADE_IN,		// The pulsating, red fade in that obscures the whole screen
			RED_HOLD,			// We hold the fully opaque, red screen for a short interval for dramatic effect
			SHOWING_LETTERS,	// Displaying the GAME OVER letters in dramatic style
			FADING_LETTERS,		// Making them fade out
			SHOWING_STATS,		// Displaying the user's game performance statistics
			DRIP_OUT			// Doing the weird drippy effect that takes the user to a new game.
		};

		// The whole time this effect is going on, we make these planet explosion animations trigger
		// randomly around the screen, again for dramatic but fun uselessness.
		std::vector<Explosion>	mExplosion;		
		std::vector<Letter>		mText;			// The letters "GAME OVER"
		std::vector<DrippyLine> mLines;			// Each of the drippy lines
		EndGameStats			mStats;
		int						mState;
		int						mAlpha;			// Used for fading the screen in/out
		int						mRed;			// The red value during the RED_ states pulses between 200 and 255.
		int						mRedChange;		// How fast (and which direction) the red pulsing is going
		int						mUpdateCnt;		// Since this isn't a widget, we'll manually track the number of times the update method is called
		int						mHue;			// For hue/saturation/luminence crazy weird flashing effect
		int						mRedHoldCount;	// Counter for the RED_HOLD state, when it drops to 0 we switch to SHOWING_LETTERS
		bool					mFadeOut;		// Indicates if the lines are done dripping and we're doing the final fade out
		bool					mActive;		// Indicates if the game over sequence is running or not
		bool					mCanInitFirstLevel;	// When true, indicates that the board should set up the first level so that it'll be there when we fade out

		//////////////////////////////////////////////////////////////////////////
		//	Function: Init
		//
		//	Purpose: Sets up and initializes/resets all variables.
		//////////////////////////////////////////////////////////////////////////
		void Init();

		//////////////////////////////////////////////////////////////////////////
		//	Function: PulseRed
		//	
		//	Purpose: Convenience function that handles the pulsing of the red
		//	value between 200 and 255.
		//////////////////////////////////////////////////////////////////////////		
		void PulseRed();

	public:

		//////////////////////////////////////////////////////////////////////////
		//	GameOverEffect
		//////////////////////////////////////////////////////////////////////////		
		GameOverEffect();
		
		//////////////////////////////////////////////////////////////////////////
		//	Draw
		//////////////////////////////////////////////////////////////////////////
		void Draw(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Update
		//////////////////////////////////////////////////////////////////////////
		void Update(void);

		//////////////////////////////////////////////////////////////////////////
		//	Function: CanInitFirstLevel
		//	Returns: true or false, indicating if the first level can be setup for
		//	restarting.
		//
		//	Purpose: Called by the Board's Update method, this returns true
		//	when the board should initialize the first level, so that when the
		//	game over effect fades out, the level will appear underneath it.
		//	When the function returns true, it automatically sets internal variables
		//	so that the next time the function is called, it will return false, 
		//	preventing the Board from accidentally initializing the first level 
		//	multiple times. After the next call to Activate, it is allowed to
		//	return true again. 
		//
		//	Once the screen is totally filled after closing the stats display,
		//	the Board is allowed to init the first level.
		//////////////////////////////////////////////////////////////////////////	
		bool CanInitFirstLevel(void);

		//////////////////////////////////////////////////////////////////////////
		//	Function: CanStartNewGame
		//
		//	Purpose: Returns true or false indicating if the user is allowed
		//	to click to start a new game. This can only happen when the user
		//	is viewing the stats and all other states have completed.
		//	This will trigger the effects that transition to the start of a 
		//	new game.
		//////////////////////////////////////////////////////////////////////////	
		bool CanStartNewGame()			{return mState == SHOWING_STATS;}

		//////////////////////////////////////////////////////////////////////////
		//	Function: HideBoard
		//	Returns: true or false indicating whether or not the board should
		//	hide all of its display stuff, except for the starfield which always
		//	displays.
		//////////////////////////////////////////////////////////////////////////	
		bool HideBoard();

		//////////////////////////////////////////////////////////////////////////
		//	Function: IsActive
		//	Returns: true or false indicating if the sequence is running or not
		//////////////////////////////////////////////////////////////////////////	
		bool IsActive()					{return mActive;}

		//////////////////////////////////////////////////////////////////////////
		//	Function: Activate
		//	Parameters:
		//		ls	-	A stat structure containing the info for the past level
		//
		//	Purpose: Begins the game over sequence
		//////////////////////////////////////////////////////////////////////////
		void Activate(EndGameStats es)	{Init(); mActive = true; mStats = es;}

		//////////////////////////////////////////////////////////////////////////
		//	Function: DoneViewingStats
		//
		//	Purpose: Called by Board when the user clicks the mouse button, 
		//	indicating that they want the stats screen to go away and have the
		//	game restart.
		//////////////////////////////////////////////////////////////////////////
		void DoneViewingStats();			
};


}

#endif // __GAME_OVER_EFFECT_H__