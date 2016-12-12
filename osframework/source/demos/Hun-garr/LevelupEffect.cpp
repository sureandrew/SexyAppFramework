#pragma warning(disable:4244) 
#pragma warning(disable:4018)


#include "LevelupEffect.h"
#include "Board.h"
#include "Res.h"
#include "SexyAppFramework/SexyAppBase.h"
#include "SexyAppFramework/Graphics.h"
#include "SexyAppFramework/Font.h"

using namespace Sexy;

const int STRIP_WIDTH	= 20;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
LevelupEffect::LevelupEffect()
{
	Init();
	mActive = false;
	mCoverWidth = 0;
	mStripSizeChange = 60;
	mStripHeight = 0;
	mFadeOutAlpha = 255;
	mStartNextLevel = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LevelupEffect::Init()
{
	mText.clear();

	//////////////////////////////////////////////////////////////////////////
	//	Each letter, starting with the 'L' in "LEVEL UP!", will drop, delayed
	//	by a height of FONT_HUNGARR->GetHeight() pixels. Each letter will initially
	//	drop at a rate of 5 pixels per update (500 pixels per second). Once they
	//	reach their target Y coordinate (300), the speed will decrease and reverse
	//	to make the letters bounce upward. Gravity is applied each frame, so eventually
	//	the letters return back to Y of 300, where the speed is again decreased and
	//	reversed until it drops below the threshhold speed of 0.1 pixels per update.
	//////////////////////////////////////////////////////////////////////////	

	float x = (GRID_END_X - GRID_START_X) / 2 - FONT_HUNGARR->StringWidth(_S("LEVEL UP!")) / 2;
	float y = FONT_HUNGARR->GetHeight();
	float startingY = y;
	float ydec = y;
	float speed = 5.0f;

	mText.push_back(BouncyChar(_S("L"), x, y, speed));

	x += FONT_HUNGARR->StringWidth(_S("L"));
	y -= ydec;
	mText.push_back(BouncyChar(_S("E"), x, y, speed));

	x += FONT_HUNGARR->StringWidth(_S("E"));
	y -= ydec;
	mText.push_back(BouncyChar(_S("V"), x, y, speed));

	x += FONT_HUNGARR->StringWidth(_S("V"));
	y -= ydec;
	mText.push_back(BouncyChar(_S("E"), x, y, speed));

	x += FONT_HUNGARR->StringWidth(_S("E"));
	y -= ydec;
	mText.push_back(BouncyChar(_S("L"), x, y, speed));



	x += FONT_HUNGARR->StringWidth(_S("L "));
	y -= ydec;
	mText.push_back(BouncyChar(_S("U"), x, y, speed));

	x += FONT_HUNGARR->StringWidth(_S("U"));
	y -= ydec;
	mText.push_back(BouncyChar(_S("P"), x, y, speed));

	x += FONT_HUNGARR->StringWidth(_S("P"));
	y -= ydec;
	mText.push_back(BouncyChar(_S("!"), x, y, speed));
	
	mDone = false;
	mHue = 0;
	mCurtainX = 0;
	mState = LevelupEffect::LEVELUP_TEXT;	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LevelupEffect::Activate(LevelupStats ls)
{
	Init();
	mActive = true;
	mStats = ls;

	gSexyAppBase->PlaySample(SOUND_LEVEL_UP1);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LevelupEffect::Update(float theFrac)
{

	// HSL is an alternative to specifying an RGB color format.
	// Using HSL lets us easily do the hyper blinking crazy weird
	// flashing effect commonly found in old games, such as Robotron.
	// Below, we increment the value by 7 per update. The &0xFF is an 
	// easy way to clamp the value between 0 and 255 instead of having to
	// do a separate if (mHue > 255) mHue -= 255. This lets the value
	// rollover and keep cycling.
	mHue = (mHue + 7) & 0xFF;

	if (mState == LevelupEffect::LEVELUP_TEXT)
	{
		// Move all the letters, applying 0.15 pixels per update of
		// "gravity" to pull the letters down. Once reaching Y of 300,
		// the speed of the letters decreases by 0.75 and is reversed, making
		// them "bounce" in the up direction. Once this bounce speed drops below 0.1
		// pixels per update, the letter is done bouncing. When all letters are done bouncing,
		// switch to the next state.
		bool allDone = true;
		for (int i = 0; i < mText.size(); i++)
		{
			BouncyChar* c = &mText[i];

			if (c->mDone)
				continue;

			c->mY += c->mBounceSpeed;
			
			c->mBounceSpeed += 0.15f;

			if (c->mY >= 300)
			{
				c->mY = 300;

				c->mOldBounceSpeed -= 0.750f;
				if (c->mOldBounceSpeed <= 0.1f)
				{
					c->mDone = true;
					c->mOldBounceSpeed = 0;
				}
				else
				{
					allDone = false;
					c->mBounceSpeed = -c->mOldBounceSpeed;
				}
			}
			else
				allDone = false;
		}

		if (allDone)
			mState = LevelupEffect::CURTAIN_IN;
	}
	else if (mState == LevelupEffect::CURTAIN_IN)
	{
		// Moves the black rectangles ("curtains") inward to cover up the
		// previous level. We only move the left one. The right one is logically
		// just the same width but starting from the right of the screen instead
		// of the left. The curtain moves 25 pixels per update. Once it reaches
		// the center (and thus fully covers the screen), we switch to the next state.
		if ((mCurtainX += 25) >= gSexyAppBase->mWidth / 2)
		{
			mCurtainX = gSexyAppBase->mWidth / 2;
			mState = LevelupEffect::CURTAIN_OUT;
			gSexyAppBase->PlaySample(SOUND_LEVEL_UP2);
		}
	}
	else if (mState == LevelupEffect::CURTAIN_OUT)
	{
		// Now the curtain is red and slowly moves back out to reveal the
		// stats below it. This works just like above.
		if ((mCurtainX -= 2) <= 0)
			mState = LevelupEffect::SHOWING_STATS;
	}
	else if (mState == LevelupEffect::COVERING_STATS)
	{
		// This state begins when the user clicks the mouse to dismiss the
		// stats screen. We then make these strips quickly appear. Imagine
		// rectangles of width 20 coming in from the left and right of the screen.
		// The left one starts comign from the top left of the screen, the right
		// from the bottom right. These strips quickly expand to be screen height in size
		// and then move closer inward (by their width of 20) and reverse the direction
		// that they come in at. Once the strips have traveled to the center of the screen,
		// it's fully covered and we then move to the next state. After a strip becomes
		// the height of the screen, we update mCoverWidth, which is just simply the width
		// of the region fully filled in. Again, like with the curtain effects, we only
		// keep track of the left side, since the right moves the same amount and it's
		// easy to compute the right's offsets. 
		if (mStripSizeChange > 0)
		{
			// Left strip is moving downward from Y of 0, right is moving up
			// from Y of app height
			if ((mStripHeight += mStripSizeChange) >= gSexyAppBase->mHeight)
			{
				// Left strip and right strip are now the height of the screen.
				// Reverse the direction they come in at and reset their heights.
				// Then, increment the width of the fully covered region by the
				// size of one of the strip's width.
				mStripSizeChange = -mStripSizeChange;
				mStripHeight = 0;
				if ((mCoverWidth += STRIP_WIDTH) >= gSexyAppBase->mWidth / 2)
				{
					// The screen is fully covered. Fade out.
					mStartNextLevel = true;
					mState = LevelupEffect::FADE_OUT_STATS;
				}
			}
		}
		else
		{
			if ((mStripHeight -= mStripSizeChange) >= gSexyAppBase->mHeight)
			{
				mStripSizeChange = -mStripSizeChange;
				mStripHeight = 0;
				if ((mCoverWidth += STRIP_WIDTH) >= gSexyAppBase->mWidth / 2)
				{
					mStartNextLevel = true;
					mState = LevelupEffect::FADE_OUT_STATS;
				}
			}
		}				
	}	
	else if (mState == LevelupEffect::FADE_OUT_STATS)
	{
		//Fade the screen out, with the next level appearing below it.
		if ((mFadeOutAlpha -= 2) <= 0)
		{
			mDone = true;
			mActive = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LevelupEffect::Draw(Graphics* g)
{
	g->SetFont(FONT_HUNGARR);
	if (mState == LevelupEffect::LEVELUP_TEXT)
	{		
		// This is how we convert an HSL value to an RGB value, which we have to
		// do to specify the color for the graphics object. The function HSLToRGB
		// takes as parameters: hue, saturation, luminance. We want to leave the
		// saturation at max and luminance at half for our particular example.
		// The returned value is ANDed with 0xFFFFFFFF to clamp the values for
		// the alpha, red, green, and blue to the valid region of 0 to 255. 
		g->SetColor( (gSexyAppBase->HSLToRGB(mHue, 255, 128) & 0xFFFFFFFF) );
		for (int i = 0; i < mText.size(); i++)
		{
			BouncyChar* c = &mText[i];
			g->DrawString(c->mChar, c->mX, c->mY);
		}
	}
	else if (mState == LevelupEffect::CURTAIN_IN)
	{
		// The righ tcurtain is just the same width as the left, but starts from the right
		// side instead of X of 0.
		g->SetColor(Color(0, 0, 0));
		g->FillRect(0, 0, mCurtainX, gSexyAppBase->mHeight);
		g->FillRect(gSexyAppBase->mWidth - mCurtainX, 0, mCurtainX, gSexyAppBase->mHeight);
	}
	else if ((mState == LevelupEffect::SHOWING_STATS) || (mState == LevelupEffect::CURTAIN_OUT) || 
				(mState == LevelupEffect::COVERING_STATS))
	{
		// When just showing the stats normally, fading out the red curtain (to reveal the stats
		// beneath it), or covering up the stats with the effect triggered when the user clicks
		// to begin the next level, we display info on the user's performance from the last level.
		int y = 50;
		g->SetColor(gSexyAppBase->HSLToRGB(mHue, 255, 128) & 0xFFFFFFFF);
		SexyString s = StrFormat(_S("LEVEL %d COMPLETE!"), mStats.mLevelCompleted);
		g->DrawString(s, gSexyAppBase->mWidth / 2 - FONT_HUNGARR->StringWidth(s) / 2, y);

		g->SetColor(Color::White);
		s = _S("POPULATION CONSUMED:");
		int strWidth = FONT_HUNGARR->StringWidth(s);

		int rightX = strWidth + 100;
		y += 50;
		g->DrawString(s, 100, y);
		g->SetColor(Color(255, 0, 0));
		g->DrawString(CommaSeperate(mStats.mPopulationEaten), rightX + 5, y);

		y += FONT_HUNGARR->GetHeight();
		g->SetColor(Color::White);		
		s = _S("SYSTEMS SUBJUGATED:");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(255, 0, 0));
		g->DrawString(StrFormat(_S("%d%%"), mStats.mPercentComplete), rightX + 5, y);
		
		y += FONT_HUNGARR->GetHeight();
		if (mStats.mPercentComplete >= COMPLETION_BONUS_PCT)
		{
			s = StrFormat(_S("%d%%+ BONUS:"), COMPLETION_BONUS_PCT);
			g->SetColor(Color::White);
			strWidth = FONT_HUNGARR->StringWidth(s);
			g->DrawString(s, rightX - strWidth, y);
			g->SetColor(Color(255, 0, 0));
			g->DrawString(StrFormat(_S("%d"), COMPLETION_BONUS * mStats.mLevelCompleted), 
							rightX + 5, y);
		}

		if (mStats.mPlanetsEaten.size() > 0)
		{
			y += 50;
			int third =  gSexyAppBase->mWidth / 3;
			g->SetColor(Color(255, 255, 0));

			s = _S("PLANET EATEN:");
			g->DrawString(s, third / 2 - FONT_HUNGARR->StringWidth(s) / 2, y);
			s = _S("EXPORTS:");
			g->DrawString(s, third + (third / 2 - FONT_HUNGARR->StringWidth(s) / 2), y);
			s = _S("POPULATION:");
			g->DrawString(s, third*2 + (third / 2 - FONT_HUNGARR->StringWidth(s) / 2), y);
			
			y += FONT_HUNGARR->GetHeight();

			// If the user ate too many planets to fit on screen, we'll just display "..."
			// to indicate that they ate a bunch but we just can't fit it all on screen. 
			// In reality, it'd be best to either ensure that all the planets fit on screen,
			// or that there's some sort of scrolling mechanism to allow the user to view all their
			// stats.
			bool drawDotDotDot = false;
			for (int i = 0; i < mStats.mPlanetsEaten.size(); i += 3)
			{
				if (y >= gSexyAppBase->mHeight - FONT_HUNGARR->GetHeight() * 2)
				{
					drawDotDotDot = true;
					break;
				}
				
				g->SetColor(Color(255, 255, 255));
				s = mStats.mPlanetsEaten[i];
				g->DrawString(s, third / 2 - FONT_HUNGARR->StringWidth(s) / 2, y);

				g->SetColor(Color(128, 255, 0));
				s = mStats.mPlanetsEaten[i+1];
				g->DrawString(s, third + (third / 2 - FONT_HUNGARR->StringWidth(s) / 2), y);

				g->SetColor(Color(255, 128, 0));
				s = mStats.mPlanetsEaten[i+2];
				g->DrawString(s, third*2 + (third / 2 - FONT_HUNGARR->StringWidth(s) / 2), y);
				
				y += FONT_HUNGARR->GetHeight();

			}

			if (drawDotDotDot)
			{
				g->SetColor(Color::White);
				g->DrawString(_S("..."), 5, y);
			}
		}

		g->SetColor(Color::White);
		s = _S("CLICK TO CONTINUE");
		g->DrawString(s, gSexyAppBase->mWidth / 2 - FONT_HUNGARR->StringWidth(s) / 2, gSexyAppBase->mHeight - 20);		
	}

	if (mState == LevelupEffect::CURTAIN_OUT)
	{
		// Draw a red curtain, whose alpha decreases as it withdrawls more.
		// This lets the stats display underneath the curtain while it moves.
		int alpha = mCurtainX > 255 ? 255 : mCurtainX;
		g->SetColor(Color(255, 0, 0, alpha));
		g->FillRect(0, 0, mCurtainX, gSexyAppBase->mHeight);
		g->FillRect(gSexyAppBase->mWidth - mCurtainX, 0, mCurtainX, gSexyAppBase->mHeight);
	}

	if ((mState == LevelupEffect::COVERING_STATS) || (mState == LevelupEffect::FADE_OUT_STATS))
	{
		// Both the states where we cover up the stats and fade them out are controlled here,
		// since they are functionally the same. Again, with the HSL we do the usual HSLToRGB
		// stuff. However, since the FADE_OUT_STATS state actually fades out the screen and
		// has an alpha value that decreases over time (mFadeOutAlpha), instead of always
		// setting the alpha to 255, we need to set the alpha to mFadeOutAlpha. To do this,
		// we AND with 0xFFFFFF instead of 0xFFFFFFFF. Then, we OR it with the alpha value
		// shifted left 24 bits. This then sticks the alpha value, which changes over time,
		// into our HSL color. As you may recall from previous demos, the actual color
		// structure is 32 bit, and looks like this in binary form:
		//	AAAA RRRR GGGG BBBB  Where A,R,G,B are alpha, red, green, blue. 
		//
		// We draw the totally filled in regions separately, since they're easy.
		// The strips then are drawn differently depending on if they are moving up or down.
		// The left and right ones move oppositely.
		g->SetColor( (gSexyAppBase->HSLToRGB(mHue, 255, 128) & 0xFFFFFF) | (mFadeOutAlpha << 24) );
		g->FillRect(0, 0, mCoverWidth, gSexyAppBase->mHeight);
		g->FillRect(gSexyAppBase->mWidth - mCoverWidth, 0, mCoverWidth, gSexyAppBase->mHeight);

		if (mStripSizeChange > 0)
		{
			g->FillRect(mCoverWidth, 0, STRIP_WIDTH, mStripHeight);
			g->FillRect(gSexyAppBase->mWidth - mCoverWidth - STRIP_WIDTH,
						gSexyAppBase->mHeight - mStripHeight,
						STRIP_WIDTH,
						mStripHeight);
		}
		else
		{			
			g->FillRect(mCoverWidth, gSexyAppBase->mHeight - mStripHeight, STRIP_WIDTH, mStripHeight);
			g->FillRect(gSexyAppBase->mWidth - mCoverWidth - STRIP_WIDTH, 0, STRIP_WIDTH, mStripHeight);
		}

	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LevelupEffect::DoneViewingStats()
{
	// The user clicked to dismiss the stats screen. Cover it up
	// and begin the transition to the next level.
	mState = LevelupEffect::COVERING_STATS;
	mCoverWidth = 0;
	mStripSizeChange = 90;
	mStripHeight = 0;
	mFadeOutAlpha = 255;
	mStartNextLevel = false;
	gSexyAppBase->PlaySample(SOUND_LEVEL_UP3);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool LevelupEffect::StartNextLevel()
{
	// See function header for a complete description
	if (mStartNextLevel)
	{
		mStartNextLevel = false;
		gSexyAppBase->PlaySample(SOUND_LEVEL_UP4);
		return true;
	}

	return false;
}