#pragma warning(disable:4244) 
#pragma warning(disable:4018)
#include "Board.h"
#include "GameApp.h"
#include "Res.h"
#include "LevelupEffect.h"
#include "GameOverEffect.h"
#include "OptionsDialog.h"

#include "SexyAppFramework/Graphics.h"
#include "SexyAppFramework/Color.h"
#include "SexyAppFramework/Rect.h"
#include "SexyAppFramework/ButtonWidget.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ImageFont.h"
#include "SexyAppFramework/SoundManager.h"
#include "SexyAppFramework/SoundInstance.h"
#include "SexyAppFramework/Buffer.h"
#include "SexyAppFramework/MusicInterface.h"


#define _USE_MATH_DEFINES
#include <math.h>

// VC6 workaround
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace Sexy;

// How much faster the beam gets when you eat a planet
const float BEAM_INC_SPEED	=	0.05f;

// Fastest the beam can go
const float MAX_BEAM_SPEED	=	2.5f;

// Table of random planet names
const int NUM_PLANET_NAMES	=	28;
const SexyString PLANET_NAME[] = 
{_S("Deev-z"), _S("SEN-Hen"), _S("Wallach IX"), _S("Salusa Secundus"), _S("Ridiculous Prime"), _S("Architekt V"),
_S("Robot Republica"), _S("Ix"), _S("XOLDOG4000"), _S("Kliprok"), _S("TR-909"), _S("TR-808"), _S("TB-303"), 
_S("DTR011"), _S("dTech"), _S("Rotwang"), _S("Sukhtek"), _S("Romulox"), _S("Dob Reevz"), _S("Skull XII"),
_S("Beefy Prime"), _S("Haas"), _S("Reifenrath"), _S("Gehner Subulon"), _S("ACE-DOGG"), _S("Charolastra"), _S("Nixd"), _S("BASS")};

// Table of random planet exports:
const int NUM_PLANET_EXPORTS =	23;
const SexyString PLANET_EXPORTS[] = 
{_S("Happiness"), _S("Donkeys"), _S("Rabies"), _S("AstroPop"), _S("Idiocy"), _S("Minimal Techno"),
_S("Citizens"), _S("Pain-relieving Pants"), _S("The Quad-Laser"), _S("Septic Systems"), _S("Video Games"),
_S("Robots"), _S("Plaid"), _S("Octagons"), _S("Gingivitis"), _S("Recognizers"), _S("Electro"), _S("Sauce"),
_S("Kindness"), _S("Bison"), _S("Saline"), _S("Cholera"), _S("TyperShark")};


//////////////////////////////////////////////////////////////////////////
//	Inline functions
//////////////////////////////////////////////////////////////////////////

// Given an X coordinate, returns the grid column it maps to
inline int GetCol(float x)
{
	return (int) ((x - GRID_START_X) / GRID_PIX_SIZE);
}

// Given a Y coordinate, returns the grid row it maps to
inline int GetRow(float y)
{
	return (int) ((y - GRID_START_Y) / GRID_PIX_SIZE);
}

// Given a grid column, returns the X pixel of the left edge of it
inline float GetColPix(int col)
{
	return col * GRID_PIX_SIZE + GRID_START_X;
}

// Given a grid row, returns the Y pixel of the top edge of it
inline float GetRowPix(int row)
{
	return row * GRID_PIX_SIZE + GRID_START_Y;
}

// Given an X coordinate, adjusts it so that it is aligned
// with the left edge of the grid square it's over
inline float GetAlignedX(float x)
{
	return (float)GetCol(x) * GRID_PIX_SIZE + GRID_START_X;
}

// Given a Y coordinate, adjusts it so that it is aligned
// with the top edge of the grid square it's over
inline float GetAlignedY(float y)
{
	return (float)GetRow(y) * GRID_PIX_SIZE + GRID_START_Y;
}

// Checks to see if the Y coordinate is in the grid's bounds.
// Needs to know if Hun-garr is oriented vertically or not
inline bool YCoordInBounds(float y, bool vertical)
{
	int h = IMAGE_HUNGARR_HORIZ->GetHeight() / 2;

	if ( (vertical && (y > GRID_START_Y + h) && (y < GRID_END_Y - h)) ||
		(!vertical && (y > GRID_START_Y + 7) && (y < GRID_END_Y - 4)) )
		return true;

	return false;
}

// Checks to see if the X coordinate is in the grid's bounds
inline bool XCoordInBounds(float x)
{
	if ((x > GRID_START_X + 9) && (x < GRID_END_X - 2))
		return true;

	return false;
}

// Checks to see if the column number points to a valid grid column
inline bool ValidCol(int col)
{
	return ((col >= 0) && (col < GRID_WIDTH));
}

// Checks to see if the row number points to a valid grid row
inline bool ValidRow(int row)
{
	return ((row >= 0) && (row < GRID_HEIGHT));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::Board(GameApp* theApp)
{
	mApp = theApp;
	mHungarrIsVertical = true;	
	mLineSpeed = 1.0f;

	mNumPlanetsEaten = 0;
	mPercentComplete = 0;

	mBorderHue = 0;
	mFlashCount = 0;
	mLives = 3;
	mLevel = 1;
	mScore = 0;
	mBeamPulseAmt = 30;
	mBeamPulseVal = 128;

	// Put Hun-garr in a valid spot
	UpdateHungarrPosition(GetColPix(4), GetRowPix(4));

	// Create a 2D array to hold the grid fill state that's of size
	// GRID_WIDTH x GRID_HEIGHT
	mGridState = new GridTile* [GRID_HEIGHT];
	
	//vc6 workaround:
	int i;
	for (i = 0; i < GRID_HEIGHT; i++)
		mGridState[i] = new GridTile[GRID_WIDTH];

	mFillDirection = FILL_LEFT;
	mFillSpeed = 20.0f;
	mPlanetSpeed = 1.5f;
	mPopulationEaten = mTotalPopulationEaten = 0;
	mPauseLevel = 0;

	// Create our starfield
	for (i = 0; i < MAX_STARS; i++)
	{
		Star s;
		s.mX = Rand() % mApp->mWidth;
		s.mY = Rand() % mApp->mHeight;

		int r = Rand() % 3;
		switch (r)
		{
			case 0: s.mSpeed = 0.5f; s.mColor = Color(200, 200, 200, 128); break;
			case 1: s.mSpeed = 0.25f; s.mColor = Color(100, 100, 100, 128); break;
			case 2: s.mSpeed = 0.1f; s.mColor = Color(50, 50, 50, 128); break;
		}

		mStarField[i] = s;
	}

	mLevelupEffect = new LevelupEffect();
	mGameOverEffect = new GameOverEffect();
	mOptionsBtn = NULL;


	// The shorting out, electrical sound of the beams moving. We use a SoundInstance pointer
	// because we want to loop the sound while the beam is moving, and stop it when done.
	// This is easiest done manually.
	mShortSound = mApp->mSoundManager->GetSoundInstance(SOUND_BEAM_MOVING);

	InitLevel(1);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::~Board()
{
	// Frees up the memory allocated to our manual SoundInstance pointer. Required.
	mShortSound->Release();

	for (int i = GRID_HEIGHT; i > 0; --i)
		delete[] mGridState[i - 1];
	delete[] mGridState;

	delete mLevelupEffect;
	delete mGameOverEffect;
	delete mOptionsBtn;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Update()
{
	// If paused, return and don't markdirty, whcih prevents drawing
	// and the stealing of CPU cycles
	if (mPauseLevel > 0)
		return;

	Widget::Update();


	// HSL is an alternative to specifying an RGB color format.
	// Using HSL lets us easily do the hyper blinking crazy weird
	// flashing effect commonly found in old games, such as Robotron.
	// Below, we increment the value by 6 per update. The &0xFF is an 
	// easy way to clamp the value between 0 and 255 instead of having to
	// do a separate if (mHue > 255) mHue -= 255. This lets the value
	// rollover and keep cycling.
	if (--mFlashCount > 0)
		mBorderHue = (mBorderHue + 6) % 0xFF;	

	if (mGameOverEffect->IsActive())
	{
		mGameOverEffect->Update();

		// If the game over effect is in the proper state, we will
		// be able to initialize the first level so that when it fades out,
		// the level will appear underneath it.
		if (mGameOverEffect->CanInitFirstLevel())
		{
			//Reset the critical variables:
			mBorderHue = 0;
			mFlashCount = 0;
			mLives = 3;
			mLevel = 1;
			mScore = 0;
			mFillDirection = FILL_LEFT;
			mFillSpeed = 20.0f;
			mPlanetSpeed = 1.5f;
			mPopulationEaten = mTotalPopulationEaten = 0;
			mPauseLevel = 0;
			mLineSpeed = 1.0f;
			mPlanetIdxCount.clear();
			mExportIdxCount.clear();
			mOptionsBtn->SetVisible(true);
			mApp->mMusicInterface->FadeIn(0);
			InitLevel(1);
		}
	}

	// Make the beams that the player emits pulse with intensity
	mBeamPulseVal += mBeamPulseAmt;
	if (mBeamPulseVal >= 255)
	{
		mBeamPulseVal = 255;
		mBeamPulseAmt = -mBeamPulseAmt;
	}
	else if (mBeamPulseVal <= 0)
	{
		mBeamPulseVal = 0;
		mBeamPulseAmt = -mBeamPulseAmt;
	}
	
	MarkDirty();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::UpdateF(float theFrac)
{
	if (mPauseLevel > 0)
		return;

	if (mFilling)
	{
		// If the beams have been released, update the filling of the grid
		float amt = mFillSpeed * theFrac;

		if (mFillDirection == FILL_RIGHT)
			FillRight(amt);		
		else if (mFillDirection == FILL_LEFT)
			FillLeft(amt);
		else if (mFillDirection == FILL_UP)
			FillUp(amt);
		else if (mFillDirection == FILL_DOWN)
			FillDown(amt);

		// Check what % full the filled regions are if it's done filling
		if (!mFilling)
			UpdatePercentComplete();
		
	}

	// Make the bonus text float upwards and fade it out over time.
	std::vector<BonusText>::iterator it = mBonusText.begin(); 
	while (it != mBonusText.end()) 
	{ 
		BonusText* bt = &*it; 
		bt->mY -= 1.00f * theFrac; 
		bt->mHue = (bt->mHue + 5) % 0xFF; 

		if (--bt->mAlpha <= 0) 
		{ 
			//Totally faded out, remove it 
			it = mBonusText.erase(it); 
		} 
		else
			++it; 
	} 


	// Move the starfield. If a start gets beyond the screen,
	// randomly place it offscreen again
	int i;
	for (i = 0; i < MAX_STARS; i++)
	{
		Star* s = &mStarField[i];
		s->mX += s->mSpeed;
		if (s->mX > mWidth)
		{
			s->mX = -5;
			s->mY = Rand() % mHeight;
		}
	}

	if ((!mMovingLine1.mDone || !mMovingLine2.mDone) && !mGameOverEffect->IsActive())
		MoveLines(theFrac);


	// If we're allowed to show the planets and the game isn't paused and the game
	// over effect isn't playing, then we can move the planets around
	if ((!mLevelupEffect->HidePlanets() || (mPauseLevel > 0)) && !mGameOverEffect->IsActive())
	{
		// Move the planets
		int w = IMAGE_PLANETS->GetCelWidth();
		int h = IMAGE_PLANETS->GetCelHeight();

		// Instead of playing the explosion sound every time a planet gets destroyed, we'll
		// only play it once. That way, if you destroy more than 1 planet in one go, you won't
		// hear the same sound played multipled times at once, which would result in this loud,
		// hideous, flanging sound.
		bool playSound = false;
		for (int i = 0; i < mPlanets.size(); i++)
		{
			Planet* p = &mPlanets[i];

			// Again, the timer is used solely for incrementing the animation frames
			++p->mTimer;

			if (!p->mExploding)
			{
				if (MovePlanet(p, theFrac))
					playSound = true;			// Returns true if the planet is to explode
			}
			else
			{
				if ((p->mTimer % p->mExplodeSpeed) == 0)
				{
					if (++p->mExplodeFrame >= IMAGE_BOMB_RADIAL_DEATH->mNumCols)
					{
						mPlanets.erase(mPlanets.begin() + i);
						--i;
					}
				}
			}
		}

		if (playSound)
			mApp->PlaySample(SOUND_PLANET);
	}

	// update and move the particles. When they have reached
	// their last frame, remove them.
	for (i = 0; i < mParticles.size(); i++)
	{
		Particle* p = &mParticles[i];
		++p->mTimer;

		p->mX += p->mVX * theFrac;
		p->mY += p->mVY * theFrac;
		p->mVY += 0.1f;
		if (p->mTimer % 6 == 0)
		{
			if (++p->mFrame >= IMAGE_PARTICLE_LIGHTNING->mNumCols)
			{
				mParticles.erase(mParticles.begin() + i);
				--i;
			}
		}
	}

	if (mLevelupEffect->IsActive())
	{
		mLevelupEffect->Update(theFrac);

		// If the proper state is reached in the level up effect, then we can begin
		// setting up the next level.
		if (mLevelupEffect->StartNextLevel())
		{
			// Just finished, start the next level
			mOptionsBtn->SetVisible(true);
			InitLevel(mLevel + 1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::FillRight(float amt)
{
	bool change = true;

	// Fill the entire line, moving rightward
	for (int y = mFillRegion.mTop; y <= mFillRegion.mBottom; y++)
	{
		GridTile* gt = &mGridState[y][mFillRegion.mLeft];

		// We only want to fill those pieces that are in the GRID_FILLING state
		if (gt->mFillState == GRID_FILLING)
		{
			// This piece is filling up, expand its width
			gt->mFillRect.mWidth += amt;

			// If the width exceeds that of the grid piece, overflow the result
			// into the tile next to it so that the filling appears continuous.
			if (gt->mFillRect.mWidth >= GRID_PIX_SIZE)
			{
				float overflow = gt->mFillRect.mWidth - GRID_PIX_SIZE;
				gt->mFillState = GRID_FILLED;

				gt->mFillRect.mWidth = GRID_PIX_SIZE;

				//overflow into next column, if the next column is within our
				//fill region and if the piece is in the normal tile state.
				if (mFillRegion.mLeft + 1 <= mFillRegion.mRight)
					if (mGridState[y][mFillRegion.mLeft + 1].mFillState == GRID_NORMAL)
						mGridState[y][mFillRegion.mLeft + 1].mFillRect.mWidth += overflow;
			}
			else
				change = false;
		}		
	}

	//if "change" is true, then move one column right and begin filling in that column next
	//time this function is called. If there are no more columns to fill, we're done.

	if (change && (++mFillRegion.mLeft > mFillRegion.mRight))
		mFilling = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::FillLeft(float amt)
{
	float leftX = GetColPix(mFillRegion.mRight);
	bool change = true;

	//This algorithm works just like FillRight except that it's filling
	//from the right side of the grid piece instead of the left, so there's
	//a couple extra calculations.

	for (int y = mFillRegion.mTop; y <= mFillRegion.mBottom; y++)
	{
		GridTile* gt = &mGridState[y][mFillRegion.mRight];

		if (gt->mFillState == GRID_FILLING)
		{
			gt->mFillRect.mWidth += amt;
			gt->mFillRect.mX -= amt;

			if ((gt->mFillRect.mWidth >= GRID_PIX_SIZE) || (gt->mFillRect.mX < leftX))
			{
				float overflow = gt->mFillRect.mWidth - GRID_PIX_SIZE;
				gt->mFillState = GRID_FILLED;

				gt->mFillRect.mWidth = GRID_PIX_SIZE;
				gt->mFillRect.mX = leftX;

				//overflow into next column
				if (mFillRegion.mRight - 1 >= mFillRegion.mLeft)
				{
					if (mGridState[y][mFillRegion.mRight - 1].mFillState == GRID_NORMAL)
					{
						mGridState[y][mFillRegion.mRight - 1].mFillRect.mWidth += overflow;
						mGridState[y][mFillRegion.mRight - 1].mFillRect.mX -= overflow;
					}
				}
			}
			else
				change = false;
		}
	}

	if (change && (--mFillRegion.mRight < mFillRegion.mLeft))
		mFilling = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::FillUp(float amt)
{
	bool change = true;
	float topY = GetRowPix(mFillRegion.mBottom);

	//This algorithm works just like FillRight except that it's filling
	//from the bottom side of the grid piece instead of the left, so there's
	//a couple extra calculations.

	for (int x = mFillRegion.mLeft; x <= mFillRegion.mRight; x++)
	{
		GridTile* gt = &mGridState[mFillRegion.mBottom][x];

		if (gt->mFillState == GRID_FILLING)
		{
			gt->mFillRect.mHeight += amt;
			gt->mFillRect.mY -= amt;

			if ((gt->mFillRect.mHeight >= GRID_PIX_SIZE) || (gt->mFillRect.mY < topY))
			{
				float overflow = gt->mFillRect.mHeight - GRID_PIX_SIZE;
				gt->mFillState = GRID_FILLED;

				gt->mFillRect.mHeight = GRID_PIX_SIZE;
				gt->mFillRect.mY = topY;

				//overflow into next row
				if (mFillRegion.mBottom - 1 > mFillRegion.mTop)
				{
					if (mGridState[mFillRegion.mBottom - 1][x].mFillState == GRID_NORMAL)
					{
						mGridState[mFillRegion.mBottom - 1][x].mFillRect.mHeight += overflow;
						mGridState[mFillRegion.mBottom - 1][x].mFillRect.mY -= overflow;
					}
				}
			}
			else
				change = false;
		}
	}

	if (change && (--mFillRegion.mBottom < mFillRegion.mTop))
		mFilling = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::FillDown(float amt)
{
	bool change = true;

	//This algorithm works just like FillRight except that it's filling
	//from the top side of the grid piece instead of the left.

	for (int x = mFillRegion.mLeft; x <= mFillRegion.mRight; x++)
	{
		GridTile* gt = &mGridState[mFillRegion.mTop][x];

		if (gt->mFillState == GRID_FILLING)
		{
			gt->mFillRect.mHeight += amt;

			if (gt->mFillRect.mHeight >= GRID_PIX_SIZE)
			{
				float overflow = gt->mFillRect.mHeight - GRID_PIX_SIZE;
				gt->mFillState = GRID_FILLED;

				gt->mFillRect.mHeight = GRID_PIX_SIZE;

				//overflow into next row
				if (mFillRegion.mTop + 1 <= mFillRegion.mBottom)
					if (mGridState[mFillRegion.mTop + 1][x].mFillState == GRID_NORMAL)
						mGridState[mFillRegion.mTop + 1][x].mFillRect.mHeight += overflow;
			}
			else
				change = false;
		}
	}

	if (change && (++mFillRegion.mTop > mFillRegion.mBottom))
		mFilling = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MoveLines(float theFrac)
{

	// Move the lines, differently depending on if they are oriented verticall
	// or horizontally. When a line reaches its target, it is done, and planets
	// can bounce off of it. When both lines are done, we will calculate the region
	// to fill.

	int numDone = 0;
	if (!mMovingLine1.mDone)
	{
		float amt = -mLineSpeed * theFrac;

		if (mMovingLine1.mIsVertical)
		{
			mMovingLine1.mY += amt;
			mMovingLine1.mHeight += fabsf(amt);

			if (mMovingLine1.mY <= mMovingLine1.mTargetY)
			{
				mMovingLine1.mY = mMovingLine1.mTargetY;
				mMovingLine1.mDone = true;
				++numDone;
			}
		}
		else
		{
			mMovingLine1.mX += amt;
			mMovingLine1.mWidth += fabsf(amt);

			if (mMovingLine1.mX <= mMovingLine1.mTargetX)
			{
				mMovingLine1.mX = mMovingLine1.mTargetX;
				mMovingLine1.mDone = true;
				++numDone;
			}
		}
	}

	if (!mMovingLine2.mDone)
	{
		float amt = mLineSpeed * theFrac;

		if (mMovingLine2.mIsVertical)
		{
			mMovingLine2.mHeight += amt;

			if (mMovingLine2.mY + mMovingLine2.mHeight >= mMovingLine2.mTargetY)
			{
				mMovingLine2.mHeight = mMovingLine2.mTargetY - mMovingLine2.mY;
				mMovingLine2.mDone = true;
				++numDone;
			}
		}
		else
		{
			mMovingLine2.mWidth += amt;

			if (mMovingLine2.mX + mMovingLine2.mWidth >= mMovingLine2.mTargetX)
			{
				mMovingLine2.mWidth = mMovingLine2.mTargetX - mMovingLine2.mX;
				mMovingLine2.mDone = true;
				++numDone;
			}
		}
	}	

	// While at least one of the lines is still moving, make a bunch of sparks shower off
	// the edge of them. In non-3d mode, we'll only emit half the sparks to reduce the CPU time consumed.
	if (!mMovingLine2.mDone || !mMovingLine1.mDone)
	{
		int modVal = gSexyAppBase->Is3DAccelerated() ? 2 : 4;
		if (mUpdateCnt % modVal == 0)
			EmitSparks();
	}

	// If both are done at the same time, or both are done but perhaps one completed earlier than the other,
	// then it's time to compute the fill region.
	if ((numDone == 2) || ((numDone == 1) && mMovingLine1.mDone && mMovingLine2.mDone))
	{
		mFilling = true;

		CalculateFillRegions();
	}	

	// Quit playing the electrical shorting out sound when both lines are broken or
	// done or any combination of the two.
	if ((mMovingLine1.mDone && mMovingLine2.mDone) ||
		(mMovingLine1.mBroken && mMovingLine2.mDone) ||
		(mMovingLine2.mBroken && mMovingLine1.mDone))
		mShortSound->Stop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Draw(Graphics* g)
{
	g->SetColor(Color::Black);
	g->FillRect(0, 0, mWidth, mHeight);

	int incAmt = gSexyAppBase->Is3DAccelerated() ? 1 : 2;

	// Draw less starts if not in 3D mode to reduce CPU usage, since they aren't a critical feature
	for (int i = 0; i < MAX_STARS; i += incAmt)
	{
		Star* s = &mStarField[i];
		g->SetColor(s->mColor);
		g->FillRect(s->mX, s->mY, 1, 1);
	}	

	// We don't draw the other game elements under certain conditions, like
	// if the level up and game over effects are in a few particular states.
	if (!mLevelupEffect->HideBoard() && !mGameOverEffect->HideBoard())
	{
		DrawGrid(g);
		DrawUI(g);

		// To prevent cheating, don't draw planets if the game is paused.
		// Also don't show them during certain points of the level up effect
		if (!mLevelupEffect->HidePlanets() || (mPauseLevel > 0))
			DrawPlanets(g);

		if (!mGameOverEffect->IsActive())
			DrawMovingBeams(g);		

		DrawHungarr(g);	
	}	

	if (mLevelupEffect->IsActive())
		mLevelupEffect->Draw(g);

	if (mGameOverEffect->IsActive())
		mGameOverEffect->Draw(g);

	if (mPauseLevel > 0)
	{
		// Paused: draw an overlay
		g->SetColor(Color(0, 0, 0, 128));
		g->FillRect(0, 0, mWidth, mHeight);
		g->SetColor(Color::White);
		g->SetFont(FONT_HUNGARR);
		g->DrawString(_S("PAUSED"), mWidth / 2 - FONT_HUNGARR->StringWidth(_S("PAUSED")) / 2, mHeight / 2);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawGrid(Graphics* g)
{
	// Draw an outline around the whole grid region. See LevelUpEffect.cpp or
	// GameOverEffect.cpp for full details on using HSL instead of RGB for color and
	// why it's useful.
	if (mFlashCount > 0)
		g->SetColor(mApp->HSLToRGB(mBorderHue, 255, 128) & 0xFFFFFFFF);
	else
		g->SetColor(Color(255, 0, 0, 64));

	// grid outline:
	g->FillRect(0, GRID_START_Y, GRID_START_X, mHeight - GRID_START_Y);
	g->FillRect(0, GRID_START_Y - GRID_PIX_SIZE, mWidth, GRID_PIX_SIZE);
	g->FillRect(GRID_END_X, GRID_START_Y, mWidth - GRID_END_X, mHeight - GRID_START_Y);
	g->FillRect(GRID_START_X, GRID_END_Y, GRID_END_X - GRID_START_X, mHeight - GRID_END_Y);


	// To make a weird pattern, a few of the grid pieces will be more brightly
	// colored than the others, if they are in the normal state.
	bool startBright = true;
	for (int y = 0; y < GRID_HEIGHT; y++)
	{
		int drawY = GetRowPix(y);

		for (int x = 0; x < GRID_WIDTH; x++)
		{
			int drawX = GetColPix(x);
			int state = mGridState[y][x].mFillState;
			if (state == GRID_FILLING)
			{
				// The grid piece is in the process of filling up. Draw a different colored rectangle for the
				// filled in part, and then draw the rest normally.
				FRect* fr = &mGridState[y][x].mFillRect;
				Rect normalRect;

				if (mFillDirection == FILL_RIGHT)
					normalRect = Rect(fr->mX + fr->mWidth, drawY, GRID_PIX_SIZE - fr->mWidth + 1, GRID_PIX_SIZE);
				else if (mFillDirection == FILL_LEFT)
					normalRect = Rect(drawX, drawY, GRID_PIX_SIZE - fr->mWidth, GRID_PIX_SIZE);
				else if (mFillDirection == FILL_UP)
					normalRect = Rect(drawX, drawY, GRID_PIX_SIZE, GRID_PIX_SIZE - fr->mHeight);
				else
					normalRect = Rect(drawX, fr->mY + fr->mHeight, GRID_PIX_SIZE, GRID_PIX_SIZE - fr->mHeight + 1);


				if ((normalRect.mWidth > 0) && (normalRect.mHeight > 0))
				{					
					g->SetColor(Color(255, 255, 0, startBright && (x % 2 == 0) ? 128 : 64));
					g->FillRect(normalRect);
					g->SetColor(Color(0, 0, 0));
					g->DrawRect(normalRect);
				}

				g->SetColor(Color(255, 255, 0, 100));
				FillRectF(g, *fr);
			}
			else if (state == GRID_NORMAL)
			{
				// Just draw the grid piece normally, with a black outline around it.
				
				g->SetColor(Color(255, 255, 0, startBright && (x % 3 == 0) ? 64 : 32));	
				g->FillRect(drawX, drawY, GRID_PIX_SIZE, GRID_PIX_SIZE);
				g->SetColor(Color(0, 0, 0));
				g->DrawRect(drawX, drawY, GRID_PIX_SIZE, GRID_PIX_SIZE);
			}
			else
			{
				// The piece is completely filled in, just fill the whole rectangle
				g->SetColor(Color(255, 255, 0, 100));
				FillRectF(g, mGridState[y][x].mFillRect);
			}
		}

		startBright = !startBright;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawUI(Graphics* g)
{
	int ascent = FONT_HUNGARR->GetAscent();
	int height = FONT_HUNGARR->GetHeight();

	g->SetFont(FONT_HUNGARR);
	SexyString s;
	int rightX = FONT_HUNGARR->StringWidth(_S("POPULATION CONSUMED: ")) + 5;
	
	int strWidth;
	s = _S("WORLDS DEVOURED: ");
	strWidth = FONT_HUNGARR->StringWidth(s);
	g->SetColor(Color(255, 255, 255, 128));
	g->DrawString(s, rightX - strWidth, ascent);
	g->SetColor(Color(255, 0, 0, 200));
	g->DrawString(StrFormat(_S("%d"), mNumPlanetsEaten), rightX - 5, ascent);

	s = _S("POPULATION CONSUMED: ");
	g->SetColor(Color(255, 255, 255, 128));
	g->DrawString(s, 5, height * 2);
	g->SetColor(Color(255, 0, 0, 200));
	g->DrawString(CommaSeperate(mPopulationEaten), rightX - 5, height * 2);	

	s = _S("SCORE: ");
	strWidth = FONT_HUNGARR->StringWidth(s);
	g->SetColor(Color(255, 255, 255, 128));
	g->DrawString(s, rightX - strWidth, height * 3);
	g->SetColor(Color(255, 255, 0, 200));
	g->DrawString(StrFormat(_S("%s"), CommaSeperate(mScore).c_str()), rightX - 5, height * 3);



	int x = 380;
	s = _S("SYSTEMS SUBJUGATED: ");
	g->SetColor(Color(255, 255, 255, 128));
	g->DrawString(s, x, ascent);
	g->SetColor(Color(255, 0, 0, 200));
	g->DrawString(StrFormat(_S("%d%%"), mPercentComplete), x + FONT_HUNGARR->StringWidth(s), ascent);

	s = _S("LIVES: ");
	g->SetColor(Color(255, 255, 255, 128));
	g->DrawString(s, x, height * 2);
	strWidth = FONT_HUNGARR->StringWidth(s);
	g->DrawImage(IMAGE_HUNGARR_SMALL, strWidth + x, ascent);
	g->SetColor(Color(255, 0, 0, 200));
	g->DrawString(StrFormat(_S("x%d"), mLives), x + 10 + strWidth + IMAGE_HUNGARR_SMALL->GetWidth(), height * 2);	

	s = _S("LEVEL: ");
	g->SetColor(Color(255, 255, 255, 128));
	g->DrawString(s, x, height * 3);
	g->SetColor(Color(255, 255, 0, 200));
	g->DrawString(StrFormat(_S("%d"), mLevel), x + FONT_HUNGARR->StringWidth(s), height * 3);

	for (int i = 0; i < mBonusText.size(); i++)
	{
		BonusText* bt = &mBonusText[i];
		g->SetColor( (mApp->HSLToRGB(bt->mHue, 255, 128) & 0xFFFFFF) | (bt->mAlpha << 24) );
		g->DrawString(bt->mText, bt->mX, bt->mY);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Beam1DrawHelper(Graphics* g)
{
	// In 3D mode we'll use the DrawImageF versions since they look nicer and
	// perform anti-aliasing, and make floating point movement appear smoother.
	// Since they are more taxing, we'll use the default integer routines if hardware
	// mode is not available.

	// The offsets you see were taken from the actual image itself. You'll notice
	// that the image has a lot of blank space around the actual beam, so we move things
	// around a bit to get a nice aligned look. You'd either just play around with these
	// numbers till it looked right, or your artist would inform you.
	if (mMovingLine1.mIsVertical)
	{
		if (gSexyAppBase->Is3DAccelerated())
		{
			g->DrawImageF(IMAGE_HUNGARR_BEAM_UP, mMovingLine1.mX - 8, mMovingLine1.mY, 
				Rect(0, 0, IMAGE_HUNGARR_BEAM_UP->GetWidth(), mMovingLine1.mHeight));
		}
		else
		{
			g->DrawImage(IMAGE_HUNGARR_BEAM_UP, mMovingLine1.mX - 8, mMovingLine1.mY, 
				Rect(0, 0, IMAGE_HUNGARR_BEAM_UP->GetWidth(), mMovingLine1.mHeight));
		}
	}
	else
	{
		if (gSexyAppBase->Is3DAccelerated())
		{
			g->DrawImageF(IMAGE_HUNGARR_BEAM_LEFT, mMovingLine1.mX, mMovingLine1.mY - 8,
				Rect(0, 0, mMovingLine1.mWidth, IMAGE_HUNGARR_BEAM_LEFT->GetHeight()));
		}
		else
		{
			g->DrawImageF(IMAGE_HUNGARR_BEAM_LEFT, mMovingLine1.mX, mMovingLine1.mY - 8,
				Rect(0, 0, mMovingLine1.mWidth, IMAGE_HUNGARR_BEAM_LEFT->GetHeight()));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Beam2DrawHelper(Graphics* g)
{
	// In 3D mode we'll use the DrawImageF versions since they look nicer and
	// perform anti-aliasing, and make floating point movement appear smoother.
	// Since they are more taxing, we'll use the default integer routines if hardware
	// mode is not available.

	// The offsets you see were taken from the actual image itself. You'll notice
	// that the image has a lot of blank space around the actual beam, so we move things
	// around a bit to get a nice aligned look. You'd either just play around with these
	// numbers till it looked right, or your artist would inform you.

	if (mMovingLine2.mIsVertical)
	{
		if (gSexyAppBase->Is3DAccelerated())
		{
			g->DrawImageF(IMAGE_HUNGARR_BEAM_DOWN, mMovingLine2.mX - 8, mMovingLine2.mY - 1, 
				Rect(0, IMAGE_HUNGARR_BEAM_DOWN->GetHeight() - mMovingLine2.mHeight, 
				IMAGE_HUNGARR_BEAM_DOWN->GetWidth(), mMovingLine2.mHeight));
		}
		else
		{
			g->DrawImage(IMAGE_HUNGARR_BEAM_DOWN, mMovingLine2.mX - 8, mMovingLine2.mY - 1, 
				Rect(0, IMAGE_HUNGARR_BEAM_DOWN->GetHeight() - mMovingLine2.mHeight, 
				IMAGE_HUNGARR_BEAM_DOWN->GetWidth(), mMovingLine2.mHeight));
		}
	}
	else
	{
		if (gSexyAppBase->Is3DAccelerated())
		{
			g->DrawImageF(IMAGE_HUNGARR_BEAM_RIGHT, mMovingLine2.mX - 1, mMovingLine2.mY - 8,
				Rect(IMAGE_HUNGARR_BEAM_RIGHT->GetWidth() - mMovingLine2.mWidth, 0, 
				mMovingLine2.mWidth, IMAGE_HUNGARR_BEAM_RIGHT->GetHeight()));
		}
		else
		{
			g->DrawImage(IMAGE_HUNGARR_BEAM_RIGHT, mMovingLine2.mX - 1, mMovingLine2.mY - 8,
				Rect(IMAGE_HUNGARR_BEAM_RIGHT->GetWidth() - mMovingLine2.mWidth, 0, 
				mMovingLine2.mWidth, IMAGE_HUNGARR_BEAM_RIGHT->GetHeight()));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawMovingBeams(Graphics* g)
{		
	// If the beams are moving, then draw them. Make them pulse too. You make a
	// pulsing effect like we did in the previous demos: draw the image a second time on
	// top of the original additively, and colorize the image, setting the RGB values to
	// a different intensity. The result is an image that gets brighter and dimmer over time.
	if (!mMovingLine1.mBroken && (!mMovingLine1.mDone || mFilling || !mMovingLine2.mDone))
	{		
		Beam1DrawHelper(g);

		if (!mMovingLine1.mDone)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			g->SetColorizeImages(true);
			g->SetColor(Color(mBeamPulseVal, mBeamPulseVal, mBeamPulseVal));			
			Beam1DrawHelper(g);
			g->SetColorizeImages(false);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}

	}

	if (!mMovingLine2.mDone || mFilling || !mMovingLine1.mDone)
	{
		Beam2DrawHelper(g);

		if (!mMovingLine2.mDone)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			g->SetColorizeImages(true);
			g->SetColor(Color(mBeamPulseVal, mBeamPulseVal, mBeamPulseVal));			
			Beam2DrawHelper(g);
			g->SetColorizeImages(false);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}
	}

	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	for (int i = 0; i < mParticles.size(); i++)
	{
		Particle* p = &mParticles[i];		
		g->DrawImageCel(IMAGE_PARTICLE_LIGHTNING, p->mX, p->mY, p->mFrame);		
	}
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawPlanets(Graphics* g)
{
	// If the user has 3d, we'll rotate the planets using the floating point
	// smooth rotation function. If they don't, we'll avoid rotating at all to
	// save on CPU cycles and keep the framerate up.
	int w = IMAGE_PLANETS->GetCelWidth();
	int h = IMAGE_PLANETS->GetCelHeight();
	for (int i = 0; i < mPlanets.size(); i++)
	{
		Planet* p = &mPlanets[i];		
		if (p->mExploding)
		{
			g->DrawImageCel(IMAGE_BOMB_RADIAL_DEATH, 
				p->mX - (IMAGE_BOMB_RADIAL_DEATH->GetCelWidth() / 2 + w / 2),
				p->mY - (IMAGE_BOMB_RADIAL_DEATH->GetCelHeight() / 2 + h / 2),
				p->mExplodeFrame);
		}
		else
		{
			Rect r = Rect(p->mImgCol * w, 0, w, IMAGE_PLANETS->GetCelHeight());

			if (gSexyAppBase->Is3DAccelerated())
				g->DrawImageRotatedF(IMAGE_PLANETS, p->mX, p->mY, p->mRotationAngle, &r);
			else
				g->DrawImage(IMAGE_PLANETS, p->mX, p->mY, r);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawHungarrVertBeamsHelper(Graphics* g)
{
	// This draws the two little static beams that are always attached to
	// Hun-garr's bitmap. If the user has hardware acceleration, we'll
	// draw the beams pulsating. If not, we'll skip it since it's time consuming
	// and doesn't hurt the game any. It'd be worse to drop the framrate by a few FPS
	// for this effect.

	int h = IMAGE_HUNGARR_VERT->GetHeight() / 2;

	if (gSexyAppBase->Is3DAccelerated())
	{
		g->DrawImageF(IMAGE_HUNGARR_BEAM_UP, mLine1X, mLine1Y, 
			Rect(0, 0, IMAGE_HUNGARR_BEAM_UP->GetWidth(), h));

		g->DrawImageF(IMAGE_HUNGARR_BEAM_DOWN, mLine2X, mLine2Y, 
			Rect(0, IMAGE_HUNGARR_BEAM_DOWN->GetHeight() - h, IMAGE_HUNGARR_BEAM_DOWN->GetWidth(), h));

		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);

		g->SetColor(Color(255, 255, 255, mBeamPulseVal));
		g->DrawImageF(IMAGE_HUNGARR_BEAM_UP, mLine1X, mLine1Y, 
			Rect(0, 0, IMAGE_HUNGARR_BEAM_UP->GetWidth(), h));

		g->DrawImageF(IMAGE_HUNGARR_BEAM_DOWN, mLine2X, mLine2Y, 
			Rect(0, IMAGE_HUNGARR_BEAM_DOWN->GetHeight() - h, IMAGE_HUNGARR_BEAM_DOWN->GetWidth(), h));


		g->SetColorizeImages(false);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->DrawImage(IMAGE_HUNGARR_BEAM_UP, mLine1X, mLine1Y, 
			Rect(0, 0, IMAGE_HUNGARR_BEAM_UP->GetWidth(), h));

		g->DrawImage(IMAGE_HUNGARR_BEAM_DOWN, mLine2X, mLine2Y, 
			Rect(0, IMAGE_HUNGARR_BEAM_DOWN->GetHeight() - h, IMAGE_HUNGARR_BEAM_DOWN->GetWidth(), h));
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawHungarrHorizBeamsHelper(Graphics* g)
{
	// This draws the two little static beams that are always attached to
	// Hun-garr's bitmap. If the user has hardware acceleration, we'll
	// draw the beams pulsating. If not, we'll skip it since it's time consuming
	// and doesn't hurt the game any. It'd be worse to drop the framrate by a few FPS
	// for this effect.

	int w = IMAGE_HUNGARR_HORIZ->GetWidth() / 2;

	if (gSexyAppBase->Is3DAccelerated())
	{
		g->DrawImageF(IMAGE_HUNGARR_BEAM_LEFT, mLine1X, mLine1Y, 
			Rect(0, 0, w, IMAGE_HUNGARR_BEAM_LEFT->GetHeight()));

		g->DrawImageF(IMAGE_HUNGARR_BEAM_RIGHT, mLine2X, mLine2Y, 
			Rect(IMAGE_HUNGARR_BEAM_RIGHT->GetWidth() - w, 0, w, IMAGE_HUNGARR_BEAM_RIGHT->GetHeight()));
	
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);

		g->SetColor(Color(255, 255, 255, mBeamPulseVal));
		g->DrawImageF(IMAGE_HUNGARR_BEAM_LEFT, mLine1X, mLine1Y, 
			Rect(0, 0, w, IMAGE_HUNGARR_BEAM_LEFT->GetHeight()));

		g->DrawImageF(IMAGE_HUNGARR_BEAM_RIGHT, mLine2X, mLine2Y, 
			Rect(IMAGE_HUNGARR_BEAM_RIGHT->GetWidth() - w, 0, w, IMAGE_HUNGARR_BEAM_RIGHT->GetHeight()));

		g->SetColorizeImages(false);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);

		g->DrawImage(IMAGE_HUNGARR_BEAM_LEFT, mLine1X, mLine1Y, 
			Rect(0, 0, w, IMAGE_HUNGARR_BEAM_LEFT->GetHeight()));

		g->DrawImage(IMAGE_HUNGARR_BEAM_RIGHT, mLine2X, mLine2Y, 
			Rect(IMAGE_HUNGARR_BEAM_RIGHT->GetWidth() - w, 0, w, IMAGE_HUNGARR_BEAM_RIGHT->GetHeight()));

		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::DrawHungarr(Graphics* g)
{
	// again, we use the floating point functions instead of the integer ones
	// if the user has a 3d card.

	bool is3d = gSexyAppBase->Is3DAccelerated();

	if (mHungarrIsVertical)
	{					
		DrawHungarrVertBeamsHelper(g);

		if (is3d)
			g->DrawImageF(IMAGE_HUNGARR_VERT, mHungarrX, mHungarrY);
		else
			g->DrawImage(IMAGE_HUNGARR_VERT, mHungarrX, mHungarrY);
	}
	else
	{				
		DrawHungarrHorizBeamsHelper(g);

		if (is3d)
			g->DrawImageF(IMAGE_HUNGARR_HORIZ, mHungarrX, mHungarrY);
		else
			g->DrawImage(IMAGE_HUNGARR_HORIZ, mHungarrX, mHungarrY);
	}	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::AddedToManager(WidgetManager* theWidgetManager)
{	
	Widget::AddedToManager(theWidgetManager);


	mOptionsBtn = new ButtonWidget(1, this);	
	mOptionsBtn->SetFont(FONT_DEFAULT);
	mOptionsBtn->mLabel = _S("Options");
	mOptionsBtn->SetColor(ButtonWidget::COLOR_LABEL, Color::White);
	mOptionsBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color::White);	

	mOptionsBtn->mOverImage = IMAGE_BUTTON_OVER;
	mOptionsBtn->mDownImage = IMAGE_BUTTON_DOWN;
	mOptionsBtn->mButtonImage = IMAGE_BUTTON_NORMAL;
	mOptionsBtn->mDoFinger = true;
	mOptionsBtn->Resize(gSexyAppBase->mWidth - IMAGE_BUTTON_NORMAL->GetWidth() - 10, FONT_HUNGARR->GetHeight() * 3 - 20, 
				IMAGE_BUTTON_NORMAL->GetWidth(), IMAGE_BUTTON_NORMAL->GetHeight());

	theWidgetManager->AddWidget(mOptionsBtn);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	theWidgetManager->RemoveWidget(mOptionsBtn);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::ButtonDepress(int theId)
{
	// Play a sound whenever the options button is depressed/sad.
	if (theId == mOptionsBtn->mId)
	{
		// Stop the shorting sound if it's playing, otherwise it's annoying
		mShortSound->Stop();

		mApp->PlaySample(SOUND_BUTTON);
		Pause(true);
		OptionsDialog* od = new OptionsDialog(this);
		od->Resize(mWidth / 2 - 200, mHeight / 2 - 175, 400, 350);
		mApp->AddDialog(OptionsDialog::DIALOG_ID, od);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseMove(int x, int y)
{
	UpdateHungarrPosition(x, y);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseDrag(int x, int y)
{
	UpdateHungarrPosition(x, y);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseDown(int x, int y, int theClickCount)
{		
	// if the level up effect is displaying stats, or the game over effect is too,
	// and the user clicked, then start the next phase
	if (mLevelupEffect->ShowingStats())
		mLevelupEffect->DoneViewingStats();

	if (mGameOverEffect->CanStartNewGame())
		mGameOverEffect->DoneViewingStats();

	// ignore mouse clicks when paused or an effect is on screen and the user has no reason to click
	if (mLevelupEffect->IsActive() || (mPauseLevel > 0) || (mGameOverEffect->IsActive() && !mGameOverEffect->CanStartNewGame()))
		return;

	// On a right click, if the click was within the grid bounds, switch hungarr's orientation
	if ((theClickCount < 0) && XCoordInBounds(x) && YCoordInBounds(y, mHungarrIsVertical))
		mHungarrIsVertical = !mHungarrIsVertical;
	else if ((theClickCount > 0) && mMovingLine1.mDone && mMovingLine2.mDone && !mFilling)
	{
		//left click, and there's no lines moving: drop two new lines 

		// Make sure the user didn't click on a planet which would instantly kill them
		FRect hungarrRect = FRect(mHungarrX, mHungarrY, IMAGE_HUNGARR_HORIZ->mWidth, IMAGE_HUNGARR_HORIZ->mHeight);
		for (int i = 0; i < mPlanets.size(); i++)
		{
			Planet* p = &mPlanets[i];
			FRect planetRect = FRect(p->mX, p->mY, IMAGE_PLANETS->GetCelWidth(), IMAGE_PLANETS->GetCelHeight());
			if (planetRect.Intersects(hungarrRect))
				return;
		}

		mApp->PlaySample(SOUND_MAGZAP);

		// start the electrical shorting sound
		mShortSound->Play(true, false);

		mMovingLine1.mDone = mMovingLine2.mDone = false;
		mMovingLine1.mBroken = mMovingLine2.mBroken = false;
		int midX = IMAGE_HUNGARR_HORIZ->GetWidth() / 2;
		int midY = IMAGE_HUNGARR_HORIZ->GetHeight() / 2;

		//Align the XYs of the lines to the grid, and set the target coordinates to the
		//closest normal state tile.
		if (mHungarrIsVertical)
		{
			mMovingLine1.mIsVertical = mMovingLine2.mIsVertical = true;			
			mMovingLine1.mX = mMovingLine2.mX = GetAlignedX(mHungarrX + midX);
			mMovingLine1.mY = mMovingLine2.mY = GetAlignedY(mHungarrY + midY);
			mMovingLine1.mHeight = 1;
			mMovingLine2.mHeight = 13;
			mMovingLine1.mWidth = mMovingLine2.mWidth = GRID_PIX_SIZE;
			mMovingLine1.mTargetY = mMovingLine2.mTargetY = mMovingLine1.mY;
			mMovingLine1.mTargetX = mMovingLine2.mTargetX = mMovingLine1.mX;

			// Make sure the target coords end at a tile that's normal. If not, keep moving them
			int row = GetRow(mMovingLine1.mTargetY);
			int col = GetCol(mMovingLine1.mTargetX);
			
			// Tile immediately below is not valid
			if (mGridState[row][col].mFillState != GRID_NORMAL)
				return;

			while ((row >= 0) && (mGridState[row][col].mFillState == GRID_NORMAL))
			{
				mMovingLine1.mTargetY -= GRID_PIX_SIZE;
				--row;
			}

			// Make it end on the last valid tile. The loop above makes it leave
			// on an invalid tile
			mMovingLine1.mTargetY += GRID_PIX_SIZE;

			row = GetRow(mMovingLine2.mTargetY);
			col = GetCol(mMovingLine2.mTargetX);

			while ((row < GRID_HEIGHT) && (mGridState[row][col].mFillState == GRID_NORMAL))
			{
				mMovingLine2.mTargetY += GRID_PIX_SIZE;
				++row;
			}

			if (mMovingLine1.mTargetY > mMovingLine2.mTargetY)
				mMovingLine1.mDone = mMovingLine2.mDone = true;
		}
		else
		{
			mMovingLine1.mIsVertical = mMovingLine2.mIsVertical = false;
			mMovingLine1.mX = mMovingLine2.mX = GetAlignedX(mHungarrX + midX);
			mMovingLine1.mY = mMovingLine2.mY = GetAlignedY(mHungarrY + midY);
			mMovingLine1.mWidth = 1;
			mMovingLine2.mWidth = 13;
			mMovingLine1.mHeight = mMovingLine2.mHeight = GRID_PIX_SIZE;
			mMovingLine1.mTargetX = mMovingLine2.mTargetX = mMovingLine1.mX;
			mMovingLine1.mTargetY = mMovingLine2.mTargetY = mMovingLine1.mY;

			// Make sure the target coords end at a tile that's normal. If not, keep moving them
			int row = GetRow(mMovingLine1.mTargetY);
			int col = GetCol(mMovingLine1.mTargetX);

			// Tile immediately below is not valid...?
			if (mGridState[row][col].mFillState != GRID_NORMAL)
				return;

			while ((col >= 0) && (mGridState[row][col].mFillState == GRID_NORMAL))
			{
				mMovingLine1.mTargetX -= GRID_PIX_SIZE;
				--col;
			}

			// Make it end on the last valid tile. The loop above makes it leave
			// on an invalid tile
			mMovingLine1.mTargetX += GRID_PIX_SIZE;


			row = GetRow(mMovingLine2.mTargetY);
			col = GetCol(mMovingLine2.mTargetX);
			
			while ((col < GRID_WIDTH) && (mGridState[row][col].mFillState == GRID_NORMAL))
			{
				mMovingLine2.mTargetX += GRID_PIX_SIZE;
				++col;
			}

			
			if (mMovingLine1.mTargetX > mMovingLine2.mTargetX)
				mMovingLine1.mDone = mMovingLine2.mDone = true;
		}
	}

	UpdateHungarrPosition(x, y);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::UpdateHungarrPosition(int x, int y)
{
	// Place the Hun-garr bitmap and the two lines that stick out of him
	// so that Hun-garr is centered on the mouse cursor
	int midX = IMAGE_HUNGARR_HORIZ->GetWidth() / 2;
	int midY = IMAGE_HUNGARR_HORIZ->GetHeight() / 2;

	if (YCoordInBounds(y, mHungarrIsVertical))
	{		
		mHungarrY = y - midY;
		mLine1Y = mHungarrY + (!mHungarrIsVertical ? 7 : -12);		
		mLine2Y = mHungarrY + (!mHungarrIsVertical ? 7 : 35);
	}

	if (XCoordInBounds(x))
	{
		mHungarrX = x - midX;
		mLine1X = mHungarrX + (!mHungarrIsVertical ? -13 : 8);
		mLine2X = mHungarrX + (!mHungarrIsVertical ? 36 : 9);
	}	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::CalculateFillRegions(void)
{
	int topRow, botRow, leftCol, rightCol;

	// The basic idea is this: Find the CLOSEST edge to where the user clicks that
	// meets the following conditions:
	// 1. If Hun-garr is vertical, the edge must be as tall as the column created
	//	by the two emitted lines (or more) and every grid piece must be normal
	// 2. If Hun-garr is horizontal, the edge must be as wide as the row created by
	//	the two emitted lines (or more) and every grid piece must be normal
	// 3. If either of the lines were broken, we instead just fill in the single
	//	line made by the non-broken line (the code won't execute if both are broken)


	if (mMovingLine1.mIsVertical)
	{
		topRow = GetRow(mMovingLine1.mTargetY);
		int col1 = GetCol(mMovingLine1.mTargetX);
		botRow = GetRow(mMovingLine2.mTargetY);

		if ((mMovingLine1.mBroken && !mMovingLine2.mBroken) || 
			(!mMovingLine1.mBroken && mMovingLine2.mBroken))
		{
			leftCol = col1;
			rightCol = col1 + 1;
			mFillDirection = FILL_RIGHT;
			topRow = mMovingLine1.mBroken ? GetRow(mMovingLine2.mY) : topRow;
			botRow = mMovingLine1.mBroken ? botRow : GetRow(mMovingLine2.mY);
		}
		else
		{
			int rightEdge, leftEdge;
			GetVerticalFillValues(col1, topRow, botRow, 1, &rightEdge);
			GetVerticalFillValues(col1, topRow, botRow, -1, &leftEdge);

			if ((rightEdge - col1) <= (col1 - leftEdge))
			{
				leftCol = col1;
				rightCol = rightEdge + 1;
				mFillDirection = FILL_RIGHT;			
			}
			else
			{
				leftCol = leftEdge;
				rightCol = col1 + 1;
				mFillDirection = FILL_LEFT;
			}
		}

	}
	else
	{
		leftCol = GetCol(mMovingLine1.mTargetX);
		rightCol = GetCol(mMovingLine2.mTargetX);
		int row1 = GetRow(mMovingLine1.mTargetY);

		if ((mMovingLine1.mBroken && !mMovingLine2.mBroken) || 
			(!mMovingLine1.mBroken && mMovingLine2.mBroken))
		{
			leftCol = mMovingLine1.mBroken ? GetCol(mMovingLine2.mX) : leftCol;
			rightCol = mMovingLine1.mBroken ? rightCol : GetCol(mMovingLine2.mX);
			topRow = row1;
			botRow = row1 + 1;
			mFillDirection = FILL_DOWN;
		}
		else
		{
			int topEdge, botEdge;
			GetHorizontalFillValues(row1, leftCol, rightCol, -1, &topEdge);
			GetHorizontalFillValues(row1, leftCol, rightCol, 1, &botEdge);

			if ((botEdge - row1) <= (row1 - topEdge))
			{
				topRow = row1;
				botRow = botEdge + 1;
				mFillDirection = FILL_DOWN;
			}
			else
			{
				topRow = topEdge;
				botRow = row1 + 1;
				mFillDirection = FILL_UP;
			}
		}

	}	

	//Make a rectangular fill region: every block in it will eventually be filled.
	// Then, for all grid pieces in that region, if they are in the normal state,
	// set them to the filling state and initialize their mFillRect's
	mFillRegion.mLeft = leftCol;
	mFillRegion.mRight = rightCol - 1;
	mFillRegion.mTop = topRow;
	mFillRegion.mBottom = botRow - 1;

	for (int y = topRow; y < botRow; y++)
	{
		for (int x = leftCol; x < rightCol; x++)
		{
			if (mGridState[y][x].mFillState == GRID_NORMAL)
			{
				mGridState[y][x].mFillState = GRID_FILLING;

				switch (mFillDirection)
				{
					case FILL_RIGHT: 
						mGridState[y][x].mFillRect = 
							FRect(GetColPix(x), GetRowPix(y), 0, GRID_PIX_SIZE); 
						break;

					case FILL_LEFT:
						mGridState[y][x].mFillRect = 
							FRect(GetColPix(x + 1), GetRowPix(y), 0, GRID_PIX_SIZE); 
						break;

					case FILL_UP:
						mGridState[y][x].mFillRect = 
							FRect(GetColPix(x), GetRowPix(y + 1), GRID_PIX_SIZE, 0); 
						break;

					case FILL_DOWN:
						mGridState[y][x].mFillRect = 
							FRect(GetColPix(x), GetRowPix(y), GRID_PIX_SIZE, 0); 
						break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::FillRectF(Graphics* g, FRect fr)
{
	Rect r = Rect((int)fr.mX, (int)fr.mY, (int)fr.mWidth, (int)fr.mHeight);
	g->FillRect(r);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::GetVerticalFillValues(int startCol, int topRow, int botRow, int dir, int* edge)
{
	// If dir == -1, left, if 1, right
	// See function header for algorithm description

	bool done = false;
	int col = startCol;

	while (!done)
	{
		bool found = true;
		for (int y = topRow; y < botRow; y++)
		{
			if (mGridState[y][col].mFillState != GRID_FILLED)
			{
				found = false;
				break;
			}
		}
		
		if (!found)
		{			
			if ( ((dir > 0) && (++col >= GRID_WIDTH)) ||
				((dir < 0) && (--col < 0)) )
			{
				done = true;
				*edge = col + (dir > 0 ? -1 : 1);
			}
		}
		else
		{
			*edge = col;
			done = true;
		}

	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::GetHorizontalFillValues(int startRow, int leftCol, int rightCol, int dir, int* edge)
{
	// If dir == -1, up, if 1, down
	// See function header for algorithm description

	bool done = false;
	int row = startRow;

	while (!done)
	{

		bool found = true;
		for (int x = leftCol; x < rightCol; x++)
		{
			if (mGridState[row][x].mFillState != GRID_FILLED)
			{
				found = false;
				break;
			}
		}

		if (!found)
		{
			if ( ((dir > 0) && (++row >= GRID_HEIGHT)) ||
				((dir < 0) && (--row < 0)) )
			{
				done = true;
				*edge = row + (dir > 0 ? -1 : 1);
			}
		}
		else
		{
			*edge = row;
			done = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::InitLevel(int level)
{
	mLives += level - mLevel;

	mPopulationEaten = 0;
	mPlanetSpeed += 0.15f;
	mLevel = level;
	mPlanets.clear();
	mFlashCount = 0;
	mPercentComplete = 0;
	mPlanetsEaten.clear();
	mBonusText.clear();
	mParticles.clear();

	mFilling = false;

	// reset the grid state
	int i;
	for (i = 0; i < GRID_HEIGHT; i++)
	{
		for (int x = 0; x < GRID_WIDTH; x++)
		{
			mGridState[i][x].mFillRect = FRect(x * GRID_PIX_SIZE, GRID_START_Y + i * GRID_PIX_SIZE, 0, 0);
			mGridState[i][x].mFillState = GRID_NORMAL;
		}
	}

	// Start with 2 planets. Then add 1 every other level
	int numPlanets = 2 + (mLevel / 2);
	for (i = 0; i < numPlanets; i++)
	{
		Planet p;

		// Choose a random name and export
		p.mNameIdx = Rand() % NUM_PLANET_NAMES;
		p.mExportIdx = Rand() % NUM_PLANET_EXPORTS;

		// a random number I made up for the population. Increases by a random amount each level.
		p.mPopulation = (mLevel * 133602) + 748819;

		// Position it randomly within the confines of the grid
		p.mX = GRID_START_X + 20 + (Rand() % (GRID_END_X - GRID_START_X - 20));
		p.mY = GRID_START_Y + 20 + (Rand() % (GRID_END_Y - GRID_START_Y - 20));

		// Get a random angle for the planet to travel in. It's easier to do RAND on
		// degrees, so convert them to radians after choosing an angle
		float a = (Rand() % 360) * M_PI / 180.0f;
		p.mVX = mPlanetSpeed * cosf(a);
		p.mVY = -mPlanetSpeed * sinf(a);

		// don't let the speed be too close to 0 though, it's lame if the planet
		// bounces just straight vertically or horizontally
		if ((p.mVX >= -0.1) && (p.mVX <= 0.1))
			p.mVX = 0.3f;

		if ((p.mVY >= -0.1) && (p.mVY <= 0.1))
			p.mVY = 0.3f;

		// Set a random initial rotation angle and speed to rotate at.
		// All angle manipulation is in radians.
		p.mRotationAngle = (Rand() % 360) * M_PI / 180.0f;
		p.mRotateSpeed = (float)(Rand() % 100) / 1000.0f;
		
		// Choose a random image. There's 11 images, each is just 1 frame.
		p.mImgCol = Rand() % IMAGE_PLANETS->mNumCols;

		mPlanets.push_back(p);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool Board::MovePlanet(Planet* p, float theFrac)
{
	// This is a pretty simple collision detection routine and is good enough for this
	// game. It's also a lot easier to understand than other more accurate but more
	// complex methods. The basic idea is to check the two blocks in the direction of
	// travel to see if they are solid or not, and if so, we reverse course, and
	// don't move the planet to that new coordinate.

	int w = IMAGE_PLANETS->GetCelWidth();
	int h = IMAGE_PLANETS->GetCelHeight();
	bool playSample = false;

	// Update rotation
	p->mRotationAngle += p->mRotateSpeed;

	// Don't move it yet...compute where it WOULD be if the move was valid
	float newx = p->mX + p->mVX * theFrac;
	float newy = p->mY + p->mVY * theFrac;

	// If moving right, we'll check the grid piece right of the
	// planet to see if we hit it. Otherwise, we'll just use the grid piece that maps to
	// where the new X coordinate is:	
	float checkx = p->mVX > 0 ? newx + GRID_PIX_SIZE : newx;
	int col = GetCol(checkx);

	// We're going to check both the current row and row below it:
	int row = GetRow(p->mY);
	int nextrow = ValidRow(row + 1) ? row + 1 : row;

	if (ValidCol(col) && ValidRow(row))
	{
		int state1 = mGridState[row][col].mFillState;
		int state2 = mGridState[nextrow][col].mFillState;
		
		if ((state1 == GRID_NORMAL) && (state2 == GRID_NORMAL) && (newx > GRID_START_X))
			p->mX = newx;	// valid grid space
		else if (((state1 == GRID_FILLING) || (state2 == GRID_FILLING)) && (newx > GRID_START_X))
		{
			// planet entered a grid space that is in the process of being filled, so make it explode
			p->mExploding = true;
			GivePlanetBonus(p);			
			return true;
		}
		else
		{
			// planet hit a filled in space, reverse the X velocity
			playSample = true;
			p->mVX = -p->mVX;
		}
	}
	else
	{
		// Not valid cases would be if the planet hit the edges of the board, where there
		// aren't any grid tiles. If so, just bounce it off the wall.
		playSample = true;
		p->mVX = -p->mVX;
	}
	

	// Now for the Y direction. The principal is the same as above.
	int checky = p->mVY > 0 ? newy + GRID_PIX_SIZE : newy;
	row = GetRow(checky);
	col = GetCol(p->mX);
	int nextcol = ValidCol(col + 1) ? col + 1 : col;	

	if (ValidCol(col) && ValidRow(row))
	{
		int state1 = mGridState[row][col].mFillState;
		int state2 = mGridState[row][nextcol].mFillState;

		if ((state1 == GRID_NORMAL) && (state2 == GRID_NORMAL) && (newy > GRID_START_Y))
			p->mY = newy;
		else if (((state1 == GRID_FILLING) || (state2 == GRID_FILLING)) && (newy > GRID_START_Y))
		{
			p->mExploding = true;
			GivePlanetBonus(p);
			return true;
		}
		else
		{
			playSample = true;
			p->mVY = -p->mVY;
		}
	}
	else
	{
		p->mVY = -p->mVY;
		playSample = true;
	}

	// When a planet hits a wall/filled in grid piece, bounce it and play a sound
	if (playSample)
		mApp->PlaySample(SOUND_PLANET_HIT);

	CheckPlanetBeamCollision(p);

	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::CheckPlanetBeamCollision(Planet* p)
{
	// We're just going to do a rectangular collision check on each of the beams
	// and the given planet. Because the visible "beam" part of the image is smaller
	// than the image width/height, and the same goes for the planet too, you'll notice
	// that offsets are used to constrain the collision rectangles to make a more fair algorithm.
	FRect pr = FRect(p->mX + 4, p->mY + 4, 11, 11);

	FRect beam1Rect;

	if (!mMovingLine1.mIsVertical)
		beam1Rect = FRect(mMovingLine1.mX + 7, mMovingLine1.mY + 12, mMovingLine1.mWidth - 12, 8);
	else
		beam1Rect = FRect(mMovingLine1.mX + 12, mMovingLine1.mY + 10, 9, mMovingLine1.mHeight);

	// Only allow the user to lose 1 life max: if both beams break you don't lose 2. If the beam breaks,
	// set a flag and set its target to be its current location (indicating that it's done moving)
	if (pr.Intersects(beam1Rect))
	{
		if (!mMovingLine1.mDone)
		{
			mMovingLine1.mBroken = true;
			mApp->PlaySample(SOUND_BEAM_HIT);

			if (!mMovingLine2.mBroken)
				LostLife();

			if (!mMovingLine1.mIsVertical)
				mMovingLine1.mTargetX = mMovingLine1.mX = mMovingLine1.mX + mMovingLine1.mWidth;
			else
				mMovingLine1.mTargetY = mMovingLine1.mY = mMovingLine1.mY + mMovingLine1.mHeight;
		}
		else if (!mMovingLine1.mBroken && mMovingLine1.mDone && !mMovingLine2.mDone)
		{
			// bounce off of it
			if (mMovingLine1.mIsVertical)
			{
				p->mVX *= -1.0f;
				p->mX += p->mVX;
			}
			else
			{
				p->mVY *= -1.0f;
				p->mY += p->mVY;
			}
		}
	}

	FRect beam2Rect;

	if (!mMovingLine2.mIsVertical)
		beam2Rect = FRect(mMovingLine2.mX, mMovingLine2.mY + 12, mMovingLine2.mWidth - 7, 8);
	else
		beam2Rect = FRect(mMovingLine2.mX + 12, mMovingLine2.mY, 9, mMovingLine2.mHeight);

	if (pr.Intersects(beam2Rect))
	{
		if (!mMovingLine2.mDone)
		{
			mMovingLine2.mBroken = true;
			mApp->PlaySample(SOUND_BEAM_HIT);

			if (!mMovingLine1.mBroken)
				LostLife();

			if (!mMovingLine2.mIsVertical)
				mMovingLine2.mTargetX = mMovingLine2.mX;
			else
				mMovingLine2.mTargetY = mMovingLine2.mY;
		}
		else if (!mMovingLine2.mBroken && mMovingLine2.mDone && !mMovingLine1.mDone)
		{
			// bounce off of it
			if (mMovingLine2.mIsVertical)
			{
				p->mVX *= -1.0f;
				p->mX += p->mVX;
			}
			else
			{
				p->mVY *= -1.0f;
				p->mY += p->mVY;
			}
		}
	}

	if ((mMovingLine1.mDone && mMovingLine2.mDone) ||
		(mMovingLine1.mBroken && mMovingLine2.mDone) ||
		(mMovingLine2.mBroken && mMovingLine1.mDone))
		mShortSound->Stop();
}	

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Pause(bool p)
{
	if (p)
	{
		// Since when we're paused we don't update each frame, call
		// MarkDirty here so that we ensure the "PAUSED" overlay appears
		MarkDirty();
		++mPauseLevel;

		// Don't play the looping circuit sound
		mShortSound->Stop();
	}
	else
	{
		if (--mPauseLevel == 0)
		{
			// If any of the lines are moving, re-play the shorting sound
			if (!mMovingLine1.mDone || !mMovingLine2.mDone)
				mShortSound->Play(true, false);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::KeyChar(char theChar)
{
	if (theChar == ' ')
		Pause(mPauseLevel == 0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::GivePlanetBonus(Planet* p)
{
	mTotalPopulationEaten += p->mPopulation;
	mPopulationEaten += p->mPopulation;
	mLineSpeed += BEAM_INC_SPEED;
	if (mLineSpeed > MAX_BEAM_SPEED)
		mLineSpeed = MAX_BEAM_SPEED;

	++mNumPlanetsEaten;
	
	SexyString pName = PLANET_NAME[p->mNameIdx];
	SexyString pExport = PLANET_EXPORTS[p->mExportIdx];
	int points = mLevel * 1000;
	AddBonusText(StrFormat(_S("%s: +%d"), pName.c_str(), points), p->mX, p->mY);
	mScore += points;

	mPlanetsEaten.push_back(pName);
	mPlanetsEaten.push_back(pExport.c_str());
	mPlanetsEaten.push_back(CommaSeperate(p->mPopulation));

	std::map<int, int>::iterator it = mPlanetIdxCount.find(p->mNameIdx);
	if (it == mPlanetIdxCount.end())
		mPlanetIdxCount[p->mNameIdx] = 1;
	else
		++it->second;

	it = mExportIdxCount.find(p->mExportIdx);
	if (it == mExportIdxCount.end())
		mExportIdxCount[p->mExportIdx] = 1;
	else
		++it->second;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::UpdatePercentComplete(void)
{
	if (mGameOverEffect->IsActive())
		return;

	int total = GRID_WIDTH * GRID_HEIGHT;
	int actual = 0;

	for (int y = 0; y < GRID_HEIGHT; y++)
		for (int x = 0; x < GRID_WIDTH; x++)
			if (mGridState[y][x].mFillState == GRID_FILLED)
				++actual;

	int newAmount = (int) (((float)actual / (float)total) * 100.0f);

	int pctCleared = newAmount - mPercentComplete;

	// Make the edges of the grid flash the larger the filled region was, but not
	// for more than 3 seconds.
	mFlashCount = pctCleared * 10;
	if (mFlashCount > 300)
		mFlashCount = 300;

	// Points are exponential, so the larger a fill region, the much larger the score
	int points = pctCleared * pctCleared * 20;
	mScore += points;
	if (points > 0)
	{
		mApp->PlaySample(SOUND_REGION_FILLED);
		AddBonusText(StrFormat(_S("+%d"), points));
	}

	mPercentComplete = newAmount;

	if (mPercentComplete >= LEVELUP_PERCENT)
	{
		// Time to level up, set up the stats
		LevelupStats ls;
		ls.mLevelCompleted = mLevel;
		ls.mPercentComplete = mPercentComplete;
		ls.mPopulationEaten = mPopulationEaten;
		ls.mPlanetsEaten = mPlanetsEaten;

		// Award a bonus for extra region filling action
		if (mPercentComplete >= COMPLETION_BONUS_PCT)
			mScore += COMPLETION_BONUS * mLevel;

		mOptionsBtn->SetVisible(false);
		mLevelupEffect->Activate(ls);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::AddBonusText(SexyString t)
{
	AddBonusText(t, mWidth / 2 - FONT_HUNGARR->StringWidth(t) / 2, 
				    (mHeight - GRID_START_Y) / 2 - FONT_HUNGARR->GetHeight() / 2);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::AddBonusText(SexyString t, float x, float y)
{
	BonusText bt;
	bt.mText = t;
	bt.mX = x;
	bt.mY = y;

	mBonusText.push_back(bt);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::LostLife(void)
{	

	if (--mLives <= 0)
	{
		mLives = 0;

		// Game over. Set up the stats:
		mOptionsBtn->SetVisible(false);
		EndGameStats es;
		es.mLevel = mLevel;
		es.mNumPlanetsEaten = mNumPlanetsEaten;
		es.mPopulationConsumed = mTotalPopulationEaten;
		es.mScore = mScore;
		
		// Find which planet and export were consumed the most:
		int idx = -1;
		int count = 0;
		int i;
		for (i = 0; i < NUM_PLANET_NAMES; i++)
		{
			std::map<int, int>::iterator it = mPlanetIdxCount.find(i);
			if (it != mPlanetIdxCount.end())
			{
				if (it->second > count)
				{
					count = it->second;
					idx = i;
				}
			}
		}

		if (idx != -1)
			es.mFavoritePlanet = PLANET_NAME[idx];
		else
			es.mFavoritePlanet = _S("N/A");

		idx = -1;
		count = 0;
		for (i = 0; i < NUM_PLANET_EXPORTS; i++)
		{
			std::map<int, int>::iterator it = mExportIdxCount.find(i);
			if (it != mExportIdxCount.end())
			{
				if (it->second > count)
				{
					count = it->second;
					idx = i;
				}
			}
		}

		if (idx != -1)
			es.mFavoriteExport = PLANET_EXPORTS[idx];
		else
			es.mFavoriteExport = _S("N/A");

		// Fade out the music
		mApp->mMusicInterface->FadeOut(0, true);

		mShortSound->Stop();

		mGameOverEffect->Activate(es);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::EmitSparks(void)
{
	// This basically emits sparks between various angles, depending on
	// the orientation of the line. The angle values I had to play around with
	// until I found ones that I liked, and there's no magic secret formula for 
	// coming up with them, unless you just know how to guess well. Again, 
	// the Rand() function is easier dealt with using degrees, so we convert them
	// to radians after choosing an angle. Then, using some basic math, we compute
	// the separate XY velocities for the projectiles. In some cases, projectiles use
	// diferent velocities, and again, that was just due to trying to get the nicest look
	// and involved some playing around with to figure out. The offsets are to make the sparks
	// appear to come out of the bulbous part of the line.
	if (!mMovingLine1.mDone && !mMovingLine1.mBroken)
	{
		if (mMovingLine1.mIsVertical)
		{
			// between 90 and 180 degrees for left side emission
			float angle = (90 + (Rand() % 90)) * M_PI / 180.0f;	
			float vx = cosf(angle) * 2.0f;
			float vy = -sinf(angle) * 2.0f;
			mParticles.push_back(Particle(mMovingLine1.mX + 5, mMovingLine1.mY + 8, vx, vy));

			// between 0 and 90 degrees for right side emission
			angle = (Rand() % 90) * M_PI / 180.0f;	
			vx = cosf(angle) * 2.0f;
			vy = -sinf(angle) * 2.0f;
			mParticles.push_back(Particle(mMovingLine1.mX + 5, mMovingLine1.mY + 8, vx, vy));
		}
		else
		{
			// between 280 and 320 degrees for bottom side emission
			float angle = (280 + (Rand() % 40)) * M_PI / 180.0f;	
			float vx = cosf(angle) * 4.0f;
			float vy = -sinf(angle) * 2.0f;
			mParticles.push_back(Particle(mMovingLine1.mX + 5, mMovingLine1.mY + 8, vx, vy));

			// between 50 and 90 degrees for top side emission
			angle = (50 + (Rand() % 40)) * M_PI / 180.0f;	
			vx = cosf(angle) * 4.0f;
			vy = -sinf(angle) * 3.0f;
			mParticles.push_back(Particle(mMovingLine1.mX + 5, mMovingLine1.mY + 8, vx, vy));
		}		
	}

	if (!mMovingLine2.mDone && !mMovingLine2.mBroken)
	{
		if (mMovingLine2.mIsVertical)
		{
			// between 50 and 90 degrees for left side emission
			float angle = (50 + (Rand() % 40)) * M_PI / 180.0f;	
			float vx = cosf(angle) * 3.0f;
			float vy = -sinf(angle) * 4.0f;
			mParticles.push_back(Particle(mMovingLine2.mX + 1, mMovingLine2.mY + mMovingLine2.mHeight - 17, vx, vy));

			// between 120 and 160 degrees for right side emission
			angle = (120 + (Rand() % 40)) * M_PI / 180.0f;	
			vx = cosf(angle) * 2.0f;
			vy = -sinf(angle) * 4.0f;
			mParticles.push_back(Particle(mMovingLine2.mX + 1, mMovingLine2.mY + mMovingLine2.mHeight - 17, vx, vy));
		}
		else
		{
			// between 90 and 140 degrees for top side emission
			float angle = (90 + (Rand() % 50)) * M_PI / 180.0f;	
			float vx = cosf(angle) * 4.0f;
			float vy = -sinf(angle) * 3.0f;
			mParticles.push_back(Particle(mMovingLine2.mX + mMovingLine2.mWidth - 20, mMovingLine2.mY + 2, vx, vy));

			// between 220 and 260 degrees for bottom side emission
			angle = (220 + (Rand() % 40)) * M_PI / 180.0f;	
			vx = cosf(angle) * 4.0f;
			vy = -sinf(angle) * 4.0f;
			mParticles.push_back(Particle(mMovingLine2.mX + mMovingLine2.mWidth - 20, mMovingLine2.mY + 2, vx, vy));
		}		
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::OptionsDialogDone()
{
	// If any of the lines are moving, re-play the shorting sound
	if (!mMovingLine1.mDone || !mMovingLine2.mDone)
		mShortSound->Play(true, false);

	Pause(false);

	// Give focus back to the board so that it processes keyboard input
	mApp->mWidgetManager->SetFocus(this);
}
							  